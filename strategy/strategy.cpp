/**
 * @file strategy.cpp
 * @author GER
 * @brief 	Implementation of game strategy. Here are calculated the robots speed based on game's
 * 			robot and ball position.
 * @version 0.1
 * @date 2020-05-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "strategy.hpp"
#include <armadillo>
#include <list>

Strategy::Strategy(VSSSBuffer<GameState> &game_buffer, int *waitkey_buf, const char *serial_port, int baud)
{
	if (serial_port)
	{
		this->serial = new SerialSender(serial_port, baud);
	}
	else
	{
		this->serial = NULL;
	}
	this->robot_control_th = std::thread(&Strategy::robot_control, this, &game_buffer, waitkey_buf);
}

Strategy::~Strategy()
{
	if ((this->robot_control_th).joinable())
	{
		(this->robot_control_th).join();
	}
	this->serial->send(0, 0, 0, 0, 0, 0);
	this->serial->serialclose();
	delete this->serial;
}

arma::vec2 to_target(Robot robot, arma::vec2 target, double distance_to_stop = -1.0);
arma::vec2 future_ball_pos(int n_frames);

list<Ball> prev_ball_state;
list<Ball> ball_history;
list<Robot> mid_history;

/**
 * @brief Calculate robots speed and send through serial.
 * 
 * @param game_buffer Game State (robots and ball position)
 * @param waitkey_buf keyboard's pressed key to stop and run the game.
 */
void Strategy::robot_control(VSSSBuffer<GameState> *game_buffer, int *waitkey_buf)
{
	vector<arma::vec2> robot_speed(3);
	const arma::vec2 friendly_goal_center_pos = {0, 65};
	const arma::vec2 enemy_goal_center_pos = {150, 65};
	bool game_running = false;
	int last_paused = 0;
	int frame_number = 0;
	
	while (*waitkey_buf != ESC_CHAR)
	{
		game_running = (*waitkey_buf) == 's' ? true : game_running;
		game_running = (*waitkey_buf) == 'p' ? false : game_running;

		game_buffer->get(this->state);
		frame_number++;
		if (!game_running)
			last_paused = frame_number;

		// Ball prediction
		prev_ball_state.push_back(this->state.ball);
		if (prev_ball_state.size() > 5)
			prev_ball_state.pop_front();

		ball_history.push_back(this->state.ball);
		if (ball_history.size() > 30)
			ball_history.pop_front();

		mid_history.push_back(this->state.robots[0]);
		if (mid_history.size() > 30)
			mid_history.pop_front();

		cout << this->state.robots[0].pos[0] << ' ' << this->state.robots[0].pos[1] << ' ';
		cout << this->state.robots[0].dir[0] << ' ' << this->state.robots[0].dir[1] << endl;
		cout << this->state.ball.pos[0] << ' ' << this->state.ball.pos[1] << endl;

		// Mid-field
		// TODO future number of frames reduced when close.
		arma::vec2 enemy_goal_to_ball = (future_ball_pos(0) - enemy_goal_center_pos);
		double scaling_factor = (sqrt(arma::norm(enemy_goal_to_ball, 2)) + 1.5) / sqrt(arma::norm(enemy_goal_to_ball, 2));

		arma::vec2 midfield_target = scaling_factor * enemy_goal_to_ball + enemy_goal_center_pos;
		// TODO verify out of bounds conditions or goalkeeper interference

		if (sqrt(arma::norm(midfield_target - midfield_target, 2)) < 3.0)
		{
			midfield_target = this->state.ball.pos;
		}

		midfield_target[0] = max(6.0, min(144.0, midfield_target[0]));
		midfield_target[1] = min(124.0, max(6.0, midfield_target[1]));

		// Stay outside goal area.
		if (midfield_target[0] < 20 && 25 < midfield_target[1] && midfield_target[1] < 105)
		{
			midfield_target[0] = 30;
			if (this->state.ball.pos[1] < 65)
				midfield_target[1] = 15;
			else
				midfield_target[1] = 115;
		}

		robot_speed[0] = to_target(this->state.robots[0], midfield_target);

		if (!mid_history.front().missing && !mid_history.back().missing && frame_number - last_paused > 60)
		{
			if (sqrt(arma::norm(mid_history.front().pos - mid_history.back().pos, 2)) < 1)
			{
				robot_speed[0] = -2 * robot_speed[0];
				robot_speed[0][(frame_number / 40) % 2] = 0;
			}
		}

		if (!ball_history.front().missing && !ball_history.back().missing && frame_number - last_paused > 60)
		{
			if (sqrt(arma::norm(ball_history.front().pos - ball_history.back().pos, 2)) < 1)
				if (sqrt(arma::norm(this->state.ball.pos - this->state.robots[0].pos, 2)) < 3)
				{
					if (this->state.robots[0].pos[1] > 65)
						robot_speed[0] = {120, -120};
					else
						robot_speed[0] = {-120, 120};
				}
		}

		// Defender
		arma::vec2 defender_target;
		defender_target[0] = 30;
		if (this->state.robots[2].pos[1] < 65)
			defender_target[1] = 80;
		else
			defender_target[1] = 50;

		robot_speed[1] = to_target(this->state.robots[1], defender_target, 2.0);

		double defender_distance_to_ball = sqrt(arma::norm(this->state.ball.pos - this->state.robots[1].pos, 2));
		if (defender_distance_to_ball < 6)
		{
			arma::vec2 goal_to_robot = this->state.robots[1].pos - friendly_goal_center_pos;
			arma::vec2 goal_to_ball = this->state.ball.pos - friendly_goal_center_pos;

			arma::vec3 c_prod = arma::cross(arma::vec3({goal_to_robot[0], goal_to_robot[1], 0.0}), arma::vec3({goal_to_ball[0], goal_to_ball[1], 0.0}));
			double c_prod_sign = c_prod[2] == 0 ? 1 : (c_prod[2] / abs(c_prod[2]));

			robot_speed[1] = {c_prod_sign * 250, c_prod_sign * -250};
		}

		// Goalkeeper
		arma::vec2 goalkeeper_target = {8, max(42.0, min(88.0, future_ball_pos(2)[1]))};
		robot_speed[2] = to_target(this->state.robots[2], goalkeeper_target, 2.0);

		double goalkeeper_distance_to_ball = sqrt(arma::norm(this->state.ball.pos - this->state.robots[2].pos, 2));
		if (goalkeeper_distance_to_ball < 4)
		{
			arma::vec2 goal_to_robot = this->state.robots[2].pos - friendly_goal_center_pos;
			arma::vec2 goal_to_ball = this->state.ball.pos - friendly_goal_center_pos;

			arma::vec3 c_prod = arma::cross(arma::vec3({goal_to_robot[0], goal_to_robot[1], 0.0}), arma::vec3({goal_to_ball[0], goal_to_ball[1], 0.0}));
			double c_prod_sign = c_prod[2] == 0 ? 1 : (c_prod[2] / abs(c_prod[2]));

			robot_speed[2] = {c_prod_sign * 120, c_prod_sign * -120};
		}

		// Missing conditions
		for (int i = 0; i < 3; i++)
			if (this->state.robots[i].missing || this->state.ball.missing || !game_running)
				robot_speed[i] = {0, 0};

		if (this->serial)
			this->serial->send(robot_speed[0][0], robot_speed[0][1], robot_speed[1][0], robot_speed[1][1], robot_speed[2][0], robot_speed[2][1]);
		else
		{
			printf("Speed: %lf %lf %lf %lf %lf %lf\n", robot_speed[0][0], robot_speed[0][1], robot_speed[1][0], robot_speed[1][1], robot_speed[2][0], robot_speed[2][1]);
		}
	}
}

/**
 * @brief Calculates robot speed based on it's position and target position.
 * 
 * @param robot Robot class that indicates its position and direction
 * @param target Target (x,y) position
 * @param distance_to_stop Distance from target that send {0,0} 
 * @return arma::vec2 {left_speed, right_speed} -> Maybe it is inversed
 */
arma::vec2 to_target(Robot robot, arma::vec2 target, double distance_to_stop)
{
	arma::vec2 dir_target = arma::normalise(target - robot.pos);

	arma::vec3 c_prod = arma::cross(arma::vec3({robot.dir[0], robot.dir[1], 0.0}), arma::vec3({dir_target[0], dir_target[1], 0.0}));
	double c_prod_sign = c_prod[2] == 0 ? 1 : (c_prod[2] / abs(c_prod[2]));

	double theta;
	double move_dir;
	if (arma::dot(dir_target, robot.dir) >= 0)
		theta = acos(arma::dot(dir_target, robot.dir)), move_dir = 1.0;
	else
		theta = acos(-1 * arma::dot(dir_target, robot.dir)), move_dir = -1.0;

	double distance_to_target = sqrt(arma::norm(target - robot.pos, 2));

	double diff = min(60.0, theta * 30); // TODO find the best parameters for both these numbers
	double fwd = min(200.0, max(60.0, 40 + 6 * distance_to_target));

	arma::vec2 retv = {move_dir * (fwd - c_prod_sign * diff), move_dir * (fwd + c_prod_sign * diff)}; // TODO maybe use {fwd, fwd + diff}

	if (distance_to_target < distance_to_stop)
		retv = {0.0, 0.0};

	return retv;
}

/**
 * @brief Predict the ball's position n frames ahead
 * 
 * @param n_frames 
 * @param global::prev_ball_state
 * @return arma::vec2 {x,y} ball position if ball is missing, its prediction if not
 * 
 */
arma::vec2 future_ball_pos(int n_frames)
{
	if (prev_ball_state.size() > 1 && !prev_ball_state.front().missing && !prev_ball_state.back().missing)
	{
		arma::vec2 diff = (prev_ball_state.back().pos - prev_ball_state.front().pos) / (prev_ball_state.size() - 1);
		return prev_ball_state.back().pos + n_frames * diff;
	}
	else
		return prev_ball_state.back().pos;
}

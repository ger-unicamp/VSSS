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
list<Ball> prev_ball_state;
arma::vec2 future_ball_pos(int n_frames);

void Strategy::robot_control(VSSSBuffer<GameState> *game_buffer, int *waitkey_buf)
{
	vector<arma::vec2> robot_speed(3);
	const arma::vec2 friendly_goal_center_pos = {0, 65};
	const arma::vec2 enemy_goal_center_pos = {150, 65};
	bool game_running = false;
	while (*waitkey_buf != ESC_CHAR)
	{
		game_running = (*waitkey_buf) == 's' ? true : game_running;
		game_running = (*waitkey_buf) == 'p' ? false : game_running;

		game_buffer->get(this->state);

		// Ball prediction
		prev_ball_state.push_back(this->state.ball);
		if (prev_ball_state.size() > 5)
			prev_ball_state.pop_front();

		cout << this->state.robots[0].pos << endl;

		// Mid-field
		// TODO future number of frames reduced when close.
		arma::vec2 enemy_goal_to_ball = (future_ball_pos(5) - enemy_goal_center_pos);
		double scaling_factor = (sqrt(arma::norm(enemy_goal_to_ball, 2)) + 5.0) / sqrt(arma::norm(enemy_goal_to_ball, 2));

		arma::vec2 midfield_target = scaling_factor * enemy_goal_to_ball + enemy_goal_center_pos;
		// TODO verify out of bounds conditions or goalkeeper interference

		if (sqrt(arma::norm(midfield_target - this->state.robots[0].pos, 2)) < 3.0)
		{
			midfield_target = this->state.ball.pos;
		}

		robot_speed[0] = to_target(this->state.robots[0], midfield_target);

		// Finisher
		robot_speed[1] = {0, 0};

		// Goalkeeper
		arma::vec2 goalkeeper_target = {17, max(47.0, min(83.0, this->state.ball.pos[1]))};
		robot_speed[2] = to_target(this->state.robots[2], goalkeeper_target, 3.0);

		double goalkeeper_distance_to_ball = sqrt(arma::norm(this->state.ball.pos - this->state.robots[2].pos, 2));
		if (goalkeeper_distance_to_ball < 5.0)
		{
			arma::vec2 goal_to_robot = this->state.robots[2].pos - friendly_goal_center_pos;
			arma::vec2 goal_to_ball = this->state.ball.pos - friendly_goal_center_pos;

			arma::vec3 c_prod = arma::cross(arma::vec3({goal_to_robot[0], goal_to_robot[1], 0.0}), arma::vec3({goal_to_ball[0], goal_to_ball[1], 0.0}));
			double c_prod_sign = c_prod[2] == 0 ? 1 : (c_prod[2] / abs(c_prod[2]));

			robot_speed[2] = {c_prod_sign * 200, c_prod_sign * -200};
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

	double diff = max(40.0, theta * 10); // TODO find the best parameters for both these numbers
	double fwd = max(100.0, min(40.0, distance_to_target));

	arma::vec2 retv = {move_dir * (fwd - c_prod_sign * diff), move_dir * (fwd + c_prod_sign * diff)}; // TODO maybe use {fwd, fwd + diff}

	if (distance_to_target < distance_to_stop)
		retv = {0.0, 0.0};

	return retv;
}

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

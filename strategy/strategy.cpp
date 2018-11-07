#include "strategy.hpp"
#include <armadillo>

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

arma::vec2 to_target(Robot robot, arma::vec2 target);

void Strategy::robot_control(VSSSBuffer<GameState> *game_buffer, int *waitkey_buf)
{
	vector<arma::vec2> robot_speed(3);

	while (*waitkey_buf != ESC_CHAR)
	{
		game_buffer->get(this->state);

		cout << this->state.robots[0].pos << endl;

		robot_speed[0] = to_target(this->state.robots[0], state.ball.pos);

		/*
		if (this->state.robots[0].pos[0] < FIELD_WIDTH / 2.0)
		{
			robot_speed[0] = {50, 50};
		}
		else
		{
			robot_speed[0] = {-50, -50};
		}
*/
		robot_speed[1] = robot_speed[2] = {0, 0};

		for (int i = 0; i < 3; i++)
			if (this->state.robots[i].missing || this->state.ball.missing)
				robot_speed[i] = {0, 0};

		if (this->serial)
			this->serial->send(robot_speed[0][0], robot_speed[0][1], robot_speed[1][0], robot_speed[1][1], robot_speed[2][0], robot_speed[2][1]);
		else
		{
			printf("Speed: %lf %lf %lf %lf %lf %lf\n", robot_speed[0][0], robot_speed[0][1], robot_speed[1][0], robot_speed[1][1], robot_speed[2][0], robot_speed[2][1]);
		}
	}
}

arma::vec2 to_target(Robot robot, arma::vec2 target)
{
	arma::vec2 dir_target = arma::normalise(target - robot.pos);

	arma::vec3 c_prod = arma::cross(arma::vec3({robot.dir[0], robot.dir[1], 0.0}), arma::vec3({dir_target[0], dir_target[1], 0.0}));
	double c_prod_sign = c_prod[2] == 0 ? 1 : (c_prod[2] / abs(c_prod[2]));

	double theta = acos(arma::dot(dir_target, robot.dir));

	double diff = max(40.0, theta * 10); // TODO find the best parameters for both these numbers
	double fwd = max(100.0, min(40.0, sqrt(arma::norm(target - robot.pos, 2))));

	return arma::vec2({fwd - c_prod_sign * diff, fwd + c_prod_sign * diff}); // TODO maybe use {fwd, fwd + diff}
}

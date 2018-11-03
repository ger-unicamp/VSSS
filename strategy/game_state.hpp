#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <armadillo>

/*
Install Armadillo:
	If not present already, install LAPACK, Boost and BLAS  with 
		sudo apt-get install liblapack-dev
		sudo apt-get install libblas-dev
		sudo apt-get install libboost-dev
	Install Armadillo using 
		sudo apt-get install libarmadillo-dev
*/

struct Robot
{
	arma::vec2 pos; // (x, y) robot position in centimeters.
	arma::vec2 dir; // Unit vector indicating robot direction.
	bool missing;   // True if robot is not being seen.

	Robot()
	{
		pos = {0.0, 0.0};
		dir = {0.0, 0.0};
		missing = true;
	}
};

struct Ball
{
	arma::vec2 pos; // Ball position
	bool missing;   // True if ball is not being seen.

	Ball()
	{
		pos = {0.0, 0.0};
		missing = true;
	}
};

struct GameState
{
	Ball ball;					// Ball Position (x, y) in centimeters.
	std::vector<Robot> robots;  // Friendly robots.
	std::vector<Robot> enemies; // Enemy robots.

	GameState() : ball()
	{
		this->robots = std::vector<Robot>(3, Robot());
		this->enemies = std::vector<Robot>(3, Robot());
	}
};

#endif
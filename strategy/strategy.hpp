/**
 * @file strategy.hpp
 * @author GER
 * @brief 	Definition of game strategy. Here are calculated the robots speed based on game's
 * 			robot and ball position.
 * @version 0.1
 * @date 2020-05-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "../vision/vision.h"
#include "game_state.hpp"
#include "serial/SerialSender.hpp"
#include "../manager/constants.h"

class Strategy
{
	SerialSender *serial;
	GameState state;
	std::thread robot_control_th;

  public:
	Strategy(VSSSBuffer<GameState> &game_buffer, int *waitkey_buf, const char *serial_port = NULL, int baud = 57600);
	~Strategy();
	void robot_control(VSSSBuffer<GameState> *game_buffer, int *waitkey_buf);
};

#endif
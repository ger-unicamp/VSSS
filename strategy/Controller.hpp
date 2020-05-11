/**
 * @file Controller.hpp
 * @author GER
 * @brief File to implement new robot controller model. THIS IS NOT BEING USED IN ANY CODE.
 * @version 0.1
 * @date 2020-05-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "game_state.hpp"
#include "../manager/constants.h"
#include <list>
#include <chrono>
#include <armadillo>

class code_timer
{
    std::chrono::high_resolution_clock::time_point begin;

public:
    code_timer()
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    double seconds()
    {
        return duration_cast<duration<double>>(std::chrono::high_resolution_clock::now() - begin).count();
    }
};

class Controller
{
    double kp_dist;
    double kp_angle;
    double ki_dist;
    double ki_angle;
    double dist_error;
    double angle_error;
    double dist_integral;
    double angle_integral;
    code_timer my_timer;
    list<double> time_list;

public:
    Controller(double kp_dist = 0.0, double ki_dist = 0.0, double kp_angle = 0.0, double ki_angle = 0.0) : my_timer()
    {
        this->kp_dist = kp_dist;
        this->ki_dist = ki_dist;
        this->kp_angle = kp_angle;
        this->ki_angle = ki_angle;
        this->dist_integral = 0;
        this->angle_integral = 0;
    }

    arma::vec2 to_target(Robot robot, arma::vec2 target, double distance_to_stop)
    {
        arma::vec2 robot_to_target = target - robot.pos;
        arma::vec3 cross_pos = arma::cross(arma::vec3({robot.dir[0], robot.dir[1], 0.0}), arma::vec3({robot_to_target[0], robot_to_target[1], 0.0}));
        double cos_sig = arma::dot(robot.dir, robot_to_target);
        cos_sig = cos_sig >= 0 ? 1 : -1;

        dist_error = arma::norm(robot_to_target) * cos_sig;
        angle_error = cross_pos[2] * cos_sig;

        time_list.push_back(my_timer.seconds());
        if (time_list.size() >= 2)
        {
            dist_integral += dist_error * (time_list.back() - time_list.front());
            angle_integral += angle_error * (time_list.back() - time_list.front());
            time_list.pop_front();
        }

        arma::vec2 retv = {kp_dist * dist_error - kp_angle * angle_error + ki_dist * dist_integral - ki_angle * angle_integral,
                           kp_dist * dist_error + kp_angle * angle_error + ki_dist * dist_integral + ki_angle * angle_integral};

        if (arma::norm(robot_to_target) < distance_to_stop)
            retv = {0.0, 0.0};

        return retv;
    }
};

#endif
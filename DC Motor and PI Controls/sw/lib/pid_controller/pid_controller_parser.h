/**
 * @file parser.h
 * @author Jared McArthur
 * @brief Serial input parser for the PID controller
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "lib/pid_controller/pid_controller.h"

/**
 * @brief Starts PID controller UART parser on PA1-0. Spins inifinitely so DO NOT place in an ISR.
 * 
 * @param pid_controller 
 */
void pid_controller_parser_start(pid_controller_t* pid_controller);
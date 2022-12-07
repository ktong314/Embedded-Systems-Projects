/**
 * @file tachometer.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>


void tachometer_init(void);

uint32_t get_speed(void);

void Timer1A_Handler();

void Timer2A_task();

#pragma once

#define PERIOD 40000

/** File includes. */
#include <stdint.h>
#include "lib/pid_controller/pid_controller.h"
pid_controller_t pid_controller;

void Motor_Init(void);

void update_duty(uint16_t duty); // between 0-40,000 (0-100%)

void update_duty_cycle(void);
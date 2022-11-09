
#pragma once 

#include "inc/tm4c123gh6pm.h"

void DAC_Init(uint16_t data, uint32_t period, uint8_t priority);

void DAC_Out(uint16_t value);

void DAC_change_period(uint32_t period);

void waveOutput();


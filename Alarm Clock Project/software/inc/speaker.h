
#pragma once


#include <stdint.h>


void Sound_Init(void);

void Sound_Start(uint32_t period);

void SysTick_Handler(void);
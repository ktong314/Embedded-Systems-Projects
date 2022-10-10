#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "./inc/tm4c123gh6pm.h"
extern uint32_t pause_flag, stop_flag, rewind_flag, next_flag, play1;

//this function initializes port F interrupts for both switches
void EdgeCounterPortF_Init(void);
void EdgeCounter_Init(void);
void GPIOPortF_Handler(void);
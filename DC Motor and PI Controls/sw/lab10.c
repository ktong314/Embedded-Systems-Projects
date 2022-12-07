/**
 * @file        lab10.c
 * @author      Allen Viljoen (allenviljoen@utexas.edu), Jonathan Valvano, Matthew Yu, Jared McArthur
 *              	Jared McArthur TTH 11am
 * @brief       An empty main file for running lab10.
 * @version     0.1.0
 * @date        2022-11-10
 * 
 * @copyright   Copyright (c) 2022     
 */

#define MAIN2

/*
change MAINx above to the following as desired:
MAIN0: normal main to output state over UART, everything else interrupt driven
MAIN1: cycle through PWM values (10, 30, 50, 70, 90) with a button press
MAIN2: test tachometer
*/

#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/PLL.h"
#include "inc/CortexM.h"
#include "inc/Unified_Port_Init.h"



#ifdef MAIN0

/* Add whatever else you need here! */
#include "lib/pid_controller/pid_controller.h"
#include "lib/pid_controller/pid_controller_parser.h"

pid_controller_t pid_controller;

int main(void) {
    DisableInterrupts();
    PLL_Init(Bus80MHz);

    // initialize things
    
    // initialize pid controller
    pid_controller = pid_controller_init(0, 0, 0, 0, 0, 0);

    EnableInterrupts();
    
    // start pid controller parser (never returns)
    pid_controller_parser_start(&pid_controller);
    
    // should never reach
    return 1;
}
#endif

#ifdef MAIN1

/* Add whatever else you need here! */
#include "lib/pid_controller/pid_controller.h"
#include "lib/pid_controller/pid_controller_parser.h"
#include "lib/buttons.h"

int main(void) {
    DisableInterrupts();
    PLL_Init(Bus80MHz);

    // initialize things
	sw1_debounced_init();
    
    // initialize pid controller
    pid_controller_t pid_controller = pid_controller_init(0, 0, 0, 0, 0, 0);

    EnableInterrupts();
	
    while (1) {}
    
    // should never reach
    return 1;
}
#endif

#ifdef MAIN2

/* Add whatever else you need here! */
#include "lib/pid_controller/pid_controller.h"
#include "lib/pid_controller/pid_controller_parser.h"
#include "lib/buttons.h"
#include "lib/tachometer/tachometer.h"
#include "inc/ST7735.h"


int main(void) {
    DisableInterrupts();
    PLL_Init(Bus80MHz);

    // initialize things
	sw1_debounced_init();
	tachometer_init();    
    ST7735_InitR(INITR_REDTAB);
    // initialize pid controller
    pid_controller_t pid_controller = pid_controller_init(0, 0, 0, 0, 0, 0);

    EnableInterrupts();
	
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("RPS:");
	uint32_t speed = 0;
    while (1) {
		speed = get_speed();
		if (speed != -1){
			ST7735_SetCursor(0,1);
			ST7735_OutUDec(speed);
			ST7735_OutString("       ");
		}
		volatile int delay = 500000;
		while (delay) {
			delay--;
		}
	}
    
    // should never reach
    return 1;
}
#endif
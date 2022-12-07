/**
 * @file tachometer.c
 * @author Allen Viljoen (allenviljoen@utexas.edu)
 * @brief 
	uses timers 1 and 2
	
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
 
#include "tachometer.h"
#include "/inc/Timer2A.h"
#include "inc/CortexM.h"
#include "inc/tm4c123gh6pm.h"


int32_t period;
int32_t rps;
static int32_t first;
static uint8_t done;

void tachometer_init(void){ 
	
	
	SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
	SYSCTL_RCGCTIMER_R     |= 0x02;            // activate timer 1
	first = 0;
	done = 0;
	// ----------------- Initialize PB4 as Timer Capture input (T1CCP0) ---------
	GPIO_PORTB_DIR_R      &= ~0x10;           // make PB4 input
	GPIO_PORTB_AFSEL_R    |= 0x10;            // enable alt funct on PB4
	GPIO_PORTB_DEN_R      |= 0x10;            // enable digital I/O on PB4
												// configure PB4 (T1CCP0)
	GPIO_PORTB_PCTL_R     = (GPIO_PORTB_PCTL_R 
							&  0xFFF0FFFF)
							+  0x00070000;
	GPIO_PORTB_AMSEL_R    &= ~0x10;           // disable analog functionality on PB4
	TIMER1_CTL_R &= ~(0x1);	// disable during setup
	TIMER1_CFG_R = 0x4; 		// 16 bit timer operation
	TIMER1_TAMR_R = 0x07;	// input capture mode
	TIMER1_CTL_R &= ~(0xC); // Rising edge
	TIMER1_TAILR_R = 0x0000FFFF;
	TIMER1_TAPR_R = 0xFF;
	TIMER1_IMR_R |= 0x4; // enable capture match input
	TIMER1_ICR_R = 0x4; // clear timer1 capture match flag
	TIMER1_CTL_R |= 0x1;	// timer 1A 24 bit mode, rising edge, interrupts enabled
	NVIC_PRI5_R = (NVIC_PRI5_R & 0xffff00ff)|(4<<13); // priority 4
	NVIC_EN0_R = 1<<21;	// enable interrupt 21
	Timer2A_Init(&Timer2A_task, 800000, 5); // every 10ms check to see if the RPS is 0
}

static uint8_t count = 0;

void Timer1A_Handler(){
	TIMER1_ICR_R = 0x04;
	period = 0xFFFFFF&(first - (TIMER1_TAR_R));	// 12.5ns resolution
	if ((count < 3) && (period) && (period >= 0x14000)){	// roughly the shortest period the motor can generate, anything faster is noise
		rps = 6666666/period;	// 12 pulses per rotation
	}
	count = 0;
	first = TIMER1_TAR_R;		//setup for next
	done = 1;
}

uint32_t get_speed(void){
	if (done) {
		done = 0;
		return rps;
	} else {
		return -1;
	}
}

void Timer2A_task(){
	count++;
	if (count >= 3) {
		rps = 0;
		done = 1;
	}
}
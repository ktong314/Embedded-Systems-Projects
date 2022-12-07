// Switch.c

// PF4 connected to a negative logic switch using internal pull-up (trigger on both edges)
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "Switches.h"
#include "inc/Timer1A.h"

#define PF4                     (*((volatile uint32_t *)0x40025040))

uint32_t debounce_flag;
uint32_t Last;
extern uint32_t desired_speed;
uint32_t speed_array[5] = {10, 30, 50, 70, 90};
uint32_t array_counter;
	
void EdgeCounterPortF_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  //FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_DIR_R |=  0x0E;    // output on PF3,2,1 
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R = 0x11;    //     PF4 is both edges
  //GPIO_PORTF_IEV_R &= ~0x11;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
	
	debounce_flag = 0;
	array_counter = 0;
}

void GPIOPortF_Handler(void){
	Timer1A_Init(&debounce_check, 16000, 1);
	Last = PF4;
	if(debounce_flag){
		array_counter++;
		desired_speed = speed_array[array_counter];

		debounce_flag = 0;
	}
}

void debounce_check(void){
	uint32_t debounce = PF4;
	if(debounce == Last){
		debounce_flag = 1;
		GPIO_PORTF_ICR_R = 0x11;
		Timer1A_Stop();
	}
}



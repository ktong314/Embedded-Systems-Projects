#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "Switch.h"
#include "./inc/tm4c123gh6pm.h"



#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
uint32_t Last4;
uint32_t Last0;

uint32_t pause_flag = 0;
uint32_t stop_flag = 0;
uint32_t rewind_flag = 0;
uint32_t next_flag = 0;
uint32_t play1 = 0;

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
	Last4 = GPIO_PORTF_DATA_R & 0x00000010;
	Last0 = GPIO_PORTF_DATA_R & 0x00000001;
	
	
}

void EdgeCounter_Init(void){       
	GPIO_PORTE_DIR_R  &= ~0x02;
	GPIO_PORTE_DEN_R  |=0x02;
  GPIO_PORTE_IS_R &= ~0x2;     
  GPIO_PORTE_IBE_R &= ~0x2;  
  GPIO_PORTE_IEV_R &= ~0x2;   
  GPIO_PORTE_ICR_R = 0x2;     
  GPIO_PORTE_IM_R |= 0x2;     
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF0F)|0x00000020;
  NVIC_EN0_R = 0x00000010;            
  
}

void GPIOPortE_Handler(void){
		GPIO_PORTE_ICR_R=0x2;
		pause_flag ^= 1;
		PF3^=0xffff;
		play1=1;
}


void GPIOPortF_Handler(void){
	uint32_t current4, change4, current0, change0;
	current4 = GPIO_PORTF_DATA_R & 0x00000010;
	current0 = GPIO_PORTF_DATA_R & 0x00000001;
	change4 = current4^Last4;
	change0 = current0^Last0;
	if(current0 == 0){
		
		rewind_flag = !rewind_flag;
	}
	
	if(current4 == 0){
		next_flag = 1;
	}

	
  GPIO_PORTF_ICR_R = 0x11;      // acknowledge flag4
  //FallingEdges = FallingEdges + 1;
  //PF1 ^= 0x02; // profile
}
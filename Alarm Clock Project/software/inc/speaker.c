//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the timer interrupt.
// It starts the sound, and the timer ISR does the output
// feel free to change the parameters
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
#include <stdint.h>
#include "/inc/tm4c123gh6pm.h"
#include "./inc/Unified_Port_Init.h"


//#define PB0   (*((volatile uint32_t *)0x4005A004)) //PB0




void Sound_Init(void){
	NVIC_ST_CTRL_R = 0; //disable SysTick during setup
	NVIC_ST_CURRENT_R = 0; //clears current time in SysTick
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | (0x20000000); 
	
	
	
//// write this
//    // initialize a 11kHz Timer0A, and the DAC
//	DAC_Init();
//	//SYSTICK IS INTITALIZED
//	NVIC_ST_CTRL_R=0; //CONTROL SET EQUAL TO 0
//	NVIC_ST_RELOAD_R=0x00ffffff; //RELOAD SET EQUAL TO THE MAX
//	//NVIC_ST_CURRENT_R=0; //SET CURRENT TO 0
//	NVIC_ST_CTRL_R=7; //CONTROL SET EQUAL TO 7
};



void Sound_Start(uint32_t period){
	if(period == 0){
		NVIC_ST_CTRL_R = 0; //disable Systick until new period/frequency/input is given
		return;
	}
	NVIC_ST_RELOAD_R = period-1; //start countdown with period value
	NVIC_ST_CTRL_R = 7; //activate Systick
	
	
//// write this
//	if(period==1){
//		sound=1;
//	}
//	else{
//		sound=0;
//	}
//	NVIC_ST_RELOAD_R=7256/2;
////	NVIC_ST_CURRENT_R=0; //SET CURRENT TO 0
//	NVIC_ST_CTRL_R |= 2; //TURN ON THE SECOND BIT TURNS ON INTERRUPTS
}



void SysTick_Handler(void){
	PB0 ^= 0x01;
	PC6 ^= -1;
	
	
//	if (sound==1){
//	//DAC_Out(highpitch[idx]>>2);
//	}
//	else{
//		//DAC_Out(explosion[idx]>>2);
//	}
//	idx++;
//	if (idx==2000){
//		NVIC_ST_CTRL_R &=0xFFFFFFFD; //CLEAR BIT 1 (2 in hex)
//		idx=0;
//	}
}


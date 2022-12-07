//this is the c file that contains code for the speaker
//gets and outputs data to the speaker one value at a time
//testing with sin wave
//input - DAC values from Wifi or RF

#include "speaker.h"
#include "/inc/tm4c123gh6pm.h"
#include "/inc/Timer1A.h"
#include "inc/Unified_Port_Init.h"

uint16_t sinwave[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
}; 

static int sinwave_i = 0;


void tlv5618_init(){
	SYSCTL_RCGCSSI_R |= 0x04;       // 1) activate SSI2
	SYSCTL_RCGCGPIO_R |= 0x02;      // 2) activate port B
	volatile int delay = 1;
	delay ++;
	GPIO_PORTB_AFSEL_R |= 0xB0;     // 3) enable alt funct 
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R // 4) set function: SSI
                     & 0x0F00FFFF)
                     + 0x20220000;
	GPIO_PORTB_DEN_R |= 0xF0;       // 5) enable digital I/O pb 4-7
//	GPIO_PORTB_DIR_R |= 0x20;		// 6) pb5 out (csb)
	
	SSI2_CR1_R = 0x00000000;        // 1) 2) disable SSI, master mode
	SSI2_CC_R = 0;					// 3) system clock
	SSI2_CPSR_R = 0x08;             // 4) 80MHz/8 = 10 MHz SSIClk (should work up to 20 MHz)
	SSI2_CR0_R &= ~(0x0000FFFF);    // 5) SCR = 0, SPH = 0, SPO = 0
	SSI2_CR0_R |= 0x0F;             // 	  DSS = 16-bit data
									// 6) skip optional
	SSI2_CR1_R |= 0x00000002;       // 7) enable SSI
//	PB5 = 0x20;
}

//call this function 
int tlv5618_output(uint16_t data){
	
//	PB5 = 0;
	data = (data&0x00000fff)+0x4000;
	while((SSI2_SR_R&0x2)==0){};
	//data output 
	SSI2_DR_R=data;																				
	while((SSI2_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
//	PB5 = 0x20;
	return 1; //if successful
}



void sine_output(void){
	sinwave_i = (sinwave_i+1)%64;
	tlv5618_output(sinwave[sinwave_i]);
}


void tlv5618_test(int period){
	Timer1A_Init(&sine_output, period, 2);
}


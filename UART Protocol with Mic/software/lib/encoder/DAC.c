


#include "DAC.h"
#include "./inc/tm4c123gh6pm.h"
#include "inc/Timer1A.h"
#include "TLV5616/TLV5616.h"


#define HIGH_PER 80000
#define LOW_PER 80000*4
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define VALUES_PER_WAVEFORM 64	// the number of values in the waveform array per period
	

const uint16_t sinTable[64] = {1023,1123,1222,1319,1414,1505,1591,1671,1746,1813,1873,1925,1968,2001,2026,2041,2046,2041,2026,2001,1968,1925,1873,1813,1746,1671,1591,1505,1414,1319,1222,1123,1023,923,824,727,632,541,455,375,300,233,173,121,78,45,20,5,0,5,20,45,78,121,173,233,300,375,455,541,632,727,824,923};
	
void DisableInterrupts();
void EnableInterrupts();	

int period; 
int square; 

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

void DAC_Init(uint16_t data, uint32_t period, uint8_t priority){
  Timer1A_Init(&waveOutput, period/VALUES_PER_WAVEFORM, priority);
  SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI1
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  
  while((SYSCTL_PRGPIO_R&0x08) == 0){}; // ready?
  SSI1_CR1_R = 0x00000000;        // disable SSI, master mode
  GPIO_PORTD_AFSEL_R |= 0x0B;     // enable alt funct on PD3,1,0
  GPIO_PORTD_DEN_R |= 0x0B;       // enable digital I/O on PD3,1,0
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R
                     & 0xFFFF0F00)
                     + 0x00002022;
  GPIO_PORTD_AMSEL_R &= ~0x0B;    // disable analog functionality on PD
  SSI1_CPSR_R = 0x08;             // 80MHz/8 = 10 MHz SSIClk (should work up to 20 MHz)
  SSI1_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 1 Freescale
  SSI1_CR0_R += 0x40;             // SPO = 1
  SSI1_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI1_DR_R = data;               // load 'data' into transmit FIFO
  SSI1_CR1_R |= 0x00000002;       // enable SSI
}

// outputs the given value to the DAC
void DAC_Out(uint16_t value){
  while((SSI1_SR_R&0x00000002)==0){};	// wait until room in FIFO
  SSI1_DR_R = value; 					// data out
}

//void DAC_Init(){
//	period = 800000;
//	square = 0;
//	Timer1A_Init(&timer1Task, period, 3);
//	tlv5616_init();
//	//TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
//}


// currently setup for square wave
static uint8_t sinTable_i = 0;
void waveOutput(){
//// uncomment these lines to make a square wave and change VALUES_PER_WAVEFORM to 2
//	square ^= 1; 
//	if(square){
//		DAC_Out(2047);
//	}
//	else{
//		DAC_Out(0); 
//	}
// uncomment these lines to make a 64 value sine wave and change VALUES_PER_WAVEFORM to 64
	DAC_Out(sinTable[sinTable_i]);
	sinTable_i = (sinTable_i+1)%VALUES_PER_WAVEFORM;
}

void DAC_change_period(uint32_t period){
	if (period >= LOW_PER) {
		PF1 = 0;
	} else if (period >= HIGH_PER){
		PF1 = 0x2;
	}
	DisableInterrupts();
	TIMER1_TAILR_R = (period/VALUES_PER_WAVEFORM)-1;    // 4) reload value
	EnableInterrupts();
}	


// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 5/2/2021 
// Ethan Litchauer and Kevin Tong
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(uint32_t sac){ 
	SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x02;    // 2) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};  // 3 for stabilization
  GPIO_PORTB_DIR_R &= ~0x10;    // 4) make PB4 input
  GPIO_PORTB_AFSEL_R |= 0x10;   // 5) enable alternate function on PB4
  GPIO_PORTB_DEN_R &= ~0x10;    // 6) disable digital I/O on PB4
  GPIO_PORTB_AMSEL_R |= 0x10;   // 7) enable analog functionality on PB4
// while((SYSCTL_PRADC_R&0x0001) != 0x0001){}; // good code, but not implemented in simulator
  ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 10;           // 12) set channel
  ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
  ADC0_SAC_R = sac;							// 16) improves signal to noise ratio
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
  uint32_t data;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&0x08)==0){};   
	data = ADC0_SSFIFO3_R&0xFFF; 
	ADC0_ISC_R = 0x0008; 
	return data;
}



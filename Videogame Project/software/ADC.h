// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 5/2/2021 
// Ethan Litchauer and Kevin Tong
// Last modification date: change this to the last modification date or look very silly
#ifndef ADC_H
#define ADC_H
#include <stdint.h>

// ADC initialization function 
// Input: none
// Output: none
// measures from PB4, analog channel 10
void ADC_Init(uint32_t sac);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 10
uint32_t ADC_In(void);
#endif

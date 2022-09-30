#include <stdint.h>
#include "/inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x02; // Enable clock for PORT B
	for(uint32_t i; i<50; i++){ //Wait for clock to start
	}
	GPIO_PORTB_DEN_R  |= 0x3F; //enable pins 5-0
	GPIO_PORTB_DIR_R  |= 0x3F; //output pins 5-0
	GPIO_PORTB_DR8R_R |= 0x3F; //more current to DAC
}

// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63 
// Input=n is converted to n*3.3V/63
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & 0xFFFFFFC0) + data; 
}

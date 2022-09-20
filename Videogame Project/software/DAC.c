// dac.c
// This software configures DAC output
// Lab 6 requires a minimum of 4 bits for the DAC, but you could have 5 or 6 bits
// Runs on TM4C123
// Ethan Litchauer and Kevin Tong
// Date Created: 3/6/17 
// Last Modified: 5/2/2021 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10; //activate clock for Port E
	int wait = SYSCTL_RCGCGPIO_R; //wait for clock to stabilize
	GPIO_PORTE_DIR_R |= 0x0F; //Ports 0-3 are outputs
	GPIO_PORTE_DEN_R |= 0x0F; //Enable data read for ports 0-3
	GPIO_PORTE_AMSEL_R |= ~(0x0F); //no analog
	GPIO_PORTE_AFSEL_R |= ~(0x0F); //no alt function
	GPIO_PORTE_PCTL_R |= ~(0x0000FFFF); //regular function
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// or     6-bit data, 0 to 63
// Input=n is converted to n*3.3V/15
// or Input=n is converted to n*3.3V/63
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTE_DATA_R = data; //write data to port output
}

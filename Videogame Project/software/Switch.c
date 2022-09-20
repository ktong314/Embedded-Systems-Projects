// Switch.c
// This software to input from switches or buttons
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/14/21
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

void Switch_Init(void){
	GPIO_PORTB_DIR_R |= 0x00; //Ports 1-3 are inputs (only outputs get ones)
	GPIO_PORTB_DEN_R |= 0x0E; //Enable data read for ports 1-3
	GPIO_PORTB_AMSEL_R |= ~(0x0E); //no analog
	GPIO_PORTB_AFSEL_R |= ~(0x0E); //no alt function
	GPIO_PORTB_PCTL_R |= ~(0x0000FFFF); //regular function
}

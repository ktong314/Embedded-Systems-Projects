//this is the header file for keyboard.c

#include <stdint.h>
#include "/inc/tm4c123gh6pm.h"

#define SLAVE_ADDRESS	0x5F

//will probably need an ascii character reference table

void keyboard_init();

char recieve_keyboard();

char send_char();


//Timer for keyboard interrupts 
void Timer();

//Handler for timer interrupts 
void Timer_Handler();



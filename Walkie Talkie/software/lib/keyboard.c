//this is the c file that contains code for the keyboard

#include "keyboard.h"
#include "inc/I2C0.h"


void keyboard_init(){
	I2C_Init();
}

char recieve_keyboard(){
	char c = I2C_Recv2(SLAVE_ADDRESS);
	return c;
}

char send_char(){
	char a; 
	return a;
}


//Timer for keyboard interrupts 
void Timer();

//Handler for timer interrupts 
void Timer_Handler();
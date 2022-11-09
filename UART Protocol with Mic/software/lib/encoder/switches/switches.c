/**
 * @file switches.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdint.h>

#include "switches.h"
#include "inc/UART.h"
#include "inc/ST7735.h"

 
 char output[64] = {0};
 int cursor;
 
 //initializes the cursor 
 void cursor_init(){
	 cursor = 0;
 }
 
 //takes in an keyboard input and outputs onto screen
 void input_and_display(){
	 UART_InString(output, 64);
	 ST7735_OutString(output);
	 cursor++;
	 if (cursor > 15){
		 cursor = 0;
	 }
	 ST7735_SetCursor(0, cursor); 
 }
 
 //gets an input from keyboard press
 void set_input(){
	 
	 UART_InString(output, 64);
 }
 
 //display on screen 
 void display_input(){
	 ST7735_OutString(output);
	 cursor++;
	 if (cursor > 15){
		 cursor = 0;
	 }
	 ST7735_SetCursor(0, cursor); 
 }
 
 //pass referece to array
 char* get_input(){
	 return output;
 }
 

 
 



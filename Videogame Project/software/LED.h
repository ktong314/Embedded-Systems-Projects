//LED_and_Switch.h
//Header file for LED.c
//Ethan Litchauer and Kevin Tong

#ifndef LED_and_Switch_H
#define LED_and_Switch_H
#include <stdint.h>

//LED and Switch Initialization Function
void LED_and_Switch_Init(void);

void LED_on(void);

//Breathe function for LED
void LED_breathe(int flag);

#endif

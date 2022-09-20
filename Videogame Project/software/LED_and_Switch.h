//LED_and_Switch.h
//Header file for LED.c
//Ethan Litchauer and Kevin Tong
// Last Modified: 5/2/2021 
#ifndef LED_and_Switch_H
#define LED_and_Switch_H
#include <stdint.h>

//LED and Switch Initialization Function
void LED_and_Switch_Init(void);

void LED_on(void);

void LED_off(void);

//Breathe function for LED
void LED_breathe(void);

int FirstSwitchToggle(void);

int SecondSwitchToggle(void);

int ThirdSwitchToggle(void);

void GPIOPortB_Handler(void);

#endif

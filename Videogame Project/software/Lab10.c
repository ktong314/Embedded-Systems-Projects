// Lab10.c
// Jonathan Valvano and Daniel Valvano
// Ethan Litchauer and Kevin Tong
// This is a starter project for the EE319K Lab 10

// Last Modified: 5/2/2021 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

//************WARNING***********
// The LaunchPad has PB7 connected to PD1, PB6 connected to PD0
// Option 1) do not use PB7 and PB6
// Option 2) remove 0-ohm resistors R9 R10 on LaunchPad
//******************************

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "SSD1306.h"
#include "SSD1306_2.h"
#include "Print.h"
#include "Random.h"
#include "ADC.h"
#include "aithon_image.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "TExaS.h"
#include "LED_and_Switch.h"
#include "DAC.h"
//********************************************************************************
// debuging profile, pick up to 7 unused bits and send to Logic Analyzer
#define PB54                  (*((volatile uint32_t *)0x400050C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// use for debugging profile
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PB5       (*((volatile uint32_t *)0x40005080)) 
#define PB4       (*((volatile uint32_t *)0x40005040)) 
// TExaSdisplay logic analyzer shows 7 bits 0,PB5,PB4,PF3,PF2,PF1,0 
// edit this to output which pins you use for profiling
// you can output up to 7 pins
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PB54; // sends at 10kHz
}
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// edit this to initialize which pins you use for profiling
void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x22;      // activate port B,F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DEN_R |=  0x0E;   // enable digital I/O on PF3,2,1
  GPIO_PORTB_DIR_R |=  0x30;   // output on PB4 PB5
  GPIO_PORTB_DEN_R |=  0x30;   // enable on PB4 PB5  
}
//********************************************************************************

void Delay100ms(uint32_t count); // time delay in 0.1 seconds


//****Both static and global variables used in the game****//

static int flag = 2;				//sets arrow for language selection
static int final = 0;				//confirms selection of language via third switch
static int drawn = 0;				//determines if the arrow is drawn or not
static int language = 2;		//flag for language selection (0 for English, 1 for Spanish)
static int NextPercent = 0;	//used to increment/decrement the progress bar
static int GameStop = 0;		//time for gameplay (1 is 2 seconds to a maximum of 100 or 200 seconds to finish)
static int Pause = 0;				//flag for whether the game was paused or not (toggled by third switch)

int lose = 0;								//flag for lose or win screen
int Position = 30;					//position or x-variable for ship given by ADC
int move1 = 128;						//y-variable for 1st asteroid
int move2 = 128;						//y-variable for 2nd asteroid
int move3 = 128;						//y-variable for 3rd asteroid
int move4 = 128;						//y-variable for 4th asteroid
int chance1 = 0;						//x-variable for 1st asteroid
int chance2 = 0;						//x-variable for 2nd asteroid
int chance3 = 0;						//x-variable for 3rd asteroid
int chance4 = 0;						//x-variable for 4th asteroid
int MailStatus;							//flag used for Systick
int MailValue;							//data imported from ADC for Systick
int send = 0;								//determines when to send out obstacles
int win = 5;								//y-variable for ship on win screen

void SysTick_Init(unsigned long period){	//For ADC
	NVIC_ST_CTRL_R = 0; //disable SysTick during setup
	NVIC_ST_CURRENT_R = 0; //clears current time in SysTick
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | (0x50000000); //allows SysTick to be interrupted by the microcontroller (priority 3)
	NVIC_ST_RELOAD_R = period-1; //start countdown with period value
	NVIC_ST_CTRL_R = 7; //activate Systick
}
void SysTick_Handler(void){ // every 100 ms
  MailValue = ADC_In();
	MailStatus = 1;
}

int Menu_Select_Language(void){	//Sets language for the game
	if(FirstSwitchToggle() == 1 || SecondSwitchToggle() == 1){
		drawn = 0;
		if(FirstSwitchToggle() == 1){
			flag = 1;
		}
		if(SecondSwitchToggle() == 1){
			flag = 0;
		}
		if(flag == 1){	//arrow to English with Elon Musk face on other screen
			SSD1306_OutClear();
			SSD1306_DrawBMP(0,46,Title_Screen,14,SSD1306_WHITE);
			SSD1306_OutBuffer();
			SSD1306_DrawBMP(6, 60, Arrow, 14, SSD1306_WHITE);
			SSD1306_OutBuffer();
			SSD1306_SetCursor(3, 7);
			SSD1306_OutString("English");
			SSD1306_SetCursor(13, 7);
			SSD1306_OutString("Espanol");
			SSD1306_2_OutClear();
			SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
		if(flag == 0){	//arrow to Spanish with Astronaut on other screen
			SSD1306_OutClear();
			SSD1306_DrawBMP(0,46,Title_Screen,14,SSD1306_WHITE);
			SSD1306_OutBuffer();
			SSD1306_DrawBMP(66, 60, Arrow, 14, SSD1306_WHITE);
			SSD1306_OutBuffer();
			SSD1306_SetCursor(3, 7);
			SSD1306_OutString("English");
			SSD1306_SetCursor(13, 7);
			SSD1306_OutString("Espanol");
			SSD1306_2_OutClear();
			SSD1306_2_DrawBMP(0, 64, Astronaut, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
	}
	if(ThirdSwitchToggle() == 1){
		final = 1;
	}
	if(flag == 1 && final == 1 && drawn == 0){
		SSD1306_2_OutClear();
		SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
		drawn = 1;
		return 0;	//draws an Elon Musk face and returns 0 to language for English
	}
	if(flag == 0 && final == 1 & drawn == 0){
		SSD1306_2_OutClear();
		SSD1306_2_DrawBMP(0, 64, Astronaut, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
		drawn = 1;
		return 1; //draws an Astronaut and returns 1 to language for Spanish
	}
	if(ThirdSwitchToggle() == 0){
		final = 0;
	}
	return 2;
}

void EnglishStory(void){	//Dialogue in English
//	SSD1306_2_OutClear();
//	SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
//	SSD1306_2_OutBuffer();
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Elon Musk: Hello,    pilot. I've heard a  lot of great things  about you!"); //each line is 21 characters long
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
//	SSD1306_2_OutClear();
//	SSD1306_2_DrawBMP(0, 64, Astronaut, 14, SSD1306_2_WHITE);
//	SSD1306_2_OutBuffer();
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Player: Pleasure to  finally meet you,    boss. What do you    need me to do?"); //each line is 21 characters long
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);	
	SSD1306_2_OutClear();
	SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: SpaceX is");
	SSD1306_SetCursor(0,1);
	SSD1306_OutString("finally ready to");
	SSD1306_SetCursor(0,2);
	SSD1306_OutString("begin our journey");
	SSD1306_SetCursor(0,3);
	SSD1306_OutString("to Mars.");
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: And I needyou to get us there.");
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_2_OutClear();
	SSD1306_2_DrawBMP(0, 64, Astronaut, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Player: No worries!  I've trained over 12 years for this day."); //each line is 21 characters long
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Player: What does shelook like?"); //each line is 21 characters long
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
//	SSD1306_2_OutClear();
//	SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
//	SSD1306_2_OutBuffer();
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Elon Musk: Our       engineers have been  working nonstop      for many years to    build her.");
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
	SSD1306_2_OutClear();

	SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
	SSD1306_2_DrawBMP(0, 54, Rocket_big, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: I present to you, the Aithon.");	
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Player: Whoa.        She's beautiful!");
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
//	SSD1306_2_OutClear();
//	SSD1306_2_DrawBMP(0, 64, Elon_Musk_General_1, 14, SSD1306_2_WHITE);
//	SSD1306_2_OutBuffer();
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Elon Musk: Yes, we   are all very excited,but the expedition   is very dangerous."); //each line is 21 characters long
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
//	SSD1306_OutClear();
//	SSD1306_SetCursor(0,0);
//	SSD1306_OutString("Elon Musk: Along the way, you will face   many obstacles. The  path is filled with  asteroids and debris."); //each line is 21 characters long
//	Delay100ms(5);
//	while(ThirdSwitchToggle() == 0);
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: You must  use the slide pot to dodge the obstacles. Pressing button1 willactivate a shield."); //each line is 21 characters long
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_2_OutClear();
	SSD1306_2_DrawBMP(0, 64, Astronaut, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Player: Alright!"); //each line is 21 characters long
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_2_OutClear();

	SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
	SSD1306_2_DrawBMP(0, 54, Rocket_big, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Player: Buckled in   and ready to make    history!");
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	playsound(Rocket);
	for(int i = 0; i <= 100; i += 10){	//Rocket Animation
		SSD1306_2_OutClear();

		SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
		SSD1306_2_DrawBMP(i, 54, Rocket_big, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
		Delay100ms(3);
	}
	SSD1306_OutClear();
	SSD1306_2_OutClear();
}

void SpanishStory(void){	//Dialogue in Spanish
	SSD1306_2_OutClear();
	SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
	SSD1306_2_DrawBMP(0, 54, Rocket_big, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: estoy     presente, la Aithon.");	
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Elon Musk: Debes     esquivar obstaculos  con el deslizante.   Presionar el boton 1 activara un escudo."); //each line is 21 characters long
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	SSD1306_2_OutClear();
	SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
	SSD1306_2_DrawBMP(0, 54, Rocket_big, 14, SSD1306_2_WHITE);
	SSD1306_2_OutBuffer();
	SSD1306_OutClear();
	SSD1306_SetCursor(0,0);
	SSD1306_OutString("Jugador: Despegar!");
	Delay100ms(5);
	while(ThirdSwitchToggle() == 0);
	playsound(Rocket);
	for(int i = 0; i <= 100; i += 10){	//Rocket Animation
		SSD1306_2_OutClear();
		SSD1306_2_DrawBMP(0, 15, Tower, 14, SSD1306_2_WHITE);
		SSD1306_2_DrawBMP(i, 54, Rocket_big, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
		Delay100ms(3);
	}
	SSD1306_OutClear();
	SSD1306_2_OutClear();
}

void TravelBar(int i){	//Moves the bar to the right every two seconds based on interrupt
	SSD1306_OutClear();
	SSD1306_DrawBMP(13, 40, BarPercent, 14, SSD1306_WHITE);
	SSD1306_OutBuffer();
	if(language == 0){
		SSD1306_OutString("Travelled: ");
	}
	else if(language == 1){
		SSD1306_OutString("Recorrida: ");
	}
	BarPercent[i + 171] = 0xF0;	//Specific markers that change the pixels to fit the percent status
	BarPercent[i + 223] = 0xF0;
	BarPercent[i + 275] = 0xF0;
	BarPercent[i + 327] = 0xF0;
	BarPercent[i + 379] = 0xF0;
	BarPercent[i + 431] = 0xF0;
	SSD1306_DrawBMP(13, 40, BarPercent, 14, SSD1306_WHITE);
	SSD1306_OutBuffer();
	SSD1306_SetCursor(0,0);
	if(language == 0){
		SSD1306_OutString("Travelled: ");
	}
	else if(language == 1){
		SSD1306_OutString("Recorrida: ");
	}
	if(i < 50){
		SSD1306_SetCursor(12,0);
		SSD1306_OutUDec2(i*2);
	}
	else{
		SSD1306_SetCursor(11,0);
		SSD1306_OutString("100");
	}
	SSD1306_SetCursor(14,0);
	SSD1306_OutString("%");
	BarPercent[i + 171] = 0xFF;
	BarPercent[i + 223] = 0xFF;
	BarPercent[i + 275] = 0xFF;
	BarPercent[i + 327] = 0xFF;
	BarPercent[i + 379] = 0xFF;
	BarPercent[i + 431] = 0xFF;
	SSD1306_DrawBMP(13, 40, BarPercent, 14, SSD1306_WHITE);
	SSD1306_OutBuffer();
	SSD1306_SetCursor(0,0);
	if(language == 0){
		SSD1306_OutString("Travelled: ");
	}
	else if(language == 1){
		SSD1306_OutString("Recorrida: ");
	}
	if(i < 50){
		SSD1306_SetCursor(12,0);
		SSD1306_OutUDec2(i*2);
	}
	else{
		SSD1306_SetCursor(11,0);
		SSD1306_OutString("100");
	}
	SSD1306_SetCursor(14,0);
	SSD1306_OutString("%");
}

void TravelBarReset(int i){	//Resets TravelBar back to 0%
	while(i != 0){	//Takes NextPercent and counts down to 0, reversing and resetting the progress bar
		BarPercent[i + 171] = 0x00;
		BarPercent[i + 223] = 0x00;
		BarPercent[i + 275] = 0x00;
		BarPercent[i + 327] = 0x00;
		BarPercent[i + 379] = 0x00;
		BarPercent[i + 431] = 0x00;
		i--;
	}
}
void BigCheck(void){	//Explosion centered on ship when asteroid collides
		Timer1A_Stop();
		Timer0A_Stop();
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(5, 64 - Position, Explosion, 14, SSD1306_2_WHITE); //Position determined by ADC
		SSD1306_2_OutBuffer();
		
		lose = 1;
		GameStop = 101;
}

void Asteroid_Animate1(void){	//Not called; antiquated function for asteroid 1 trails
	if(move1 % 4 == 0){
		SSD1306_2_DrawBMP(move1 - 10, chance1, Asteroid_Animation_1, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move1 % 4 == 1){
		SSD1306_2_DrawBMP(move1 - 10, chance1, Asteroid_Animation_2, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move1 % 4 == 2){
		SSD1306_2_DrawBMP(move1 - 10, chance1, Asteroid_Animation_3, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move1 % 4 == 3){
		SSD1306_2_DrawBMP(move1 - 10, chance1, Asteroid_Animation_4, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
}

void Asteroid_Animate2(void){	//Not called; antiquated function for asteroid 2 trails
	if(move2 % 4 == 0){
		SSD1306_2_DrawBMP(move2 - 10, chance1, Asteroid_Animation_1, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move2 % 4 == 1){
		SSD1306_2_DrawBMP(move2 - 10, chance1, Asteroid_Animation_2, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move2 % 4 == 2){
		SSD1306_2_DrawBMP(move2 - 10, chance1, Asteroid_Animation_3, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move2 % 4 == 3){
		SSD1306_2_DrawBMP(move2 - 10, chance1, Asteroid_Animation_4, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
}

void Asteroid_Animate3(void){	//Not called; antiquated function for asteroid 3 trails
	if(move3 % 4 == 0){
		SSD1306_2_DrawBMP(move3 - 10, chance1, Asteroid_Animation_1, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move3 % 4 == 1){
		SSD1306_2_DrawBMP(move3 - 10, chance1, Asteroid_Animation_2, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move3 % 4 == 2){
		SSD1306_2_DrawBMP(move3 - 10, chance1, Asteroid_Animation_3, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move3 % 4 == 3){
		SSD1306_2_DrawBMP(move3 - 10, chance1, Asteroid_Animation_4, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
}

void Asteroid_Animate4(void){	//Not called; antiquated function for asteroid 4 trails
	if(move4 % 4 == 0){
		SSD1306_2_DrawBMP(move4 - 10, chance1, Asteroid_Animation_1, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move4 % 4 == 1){
		SSD1306_2_DrawBMP(move4 - 10, chance1, Asteroid_Animation_2, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move4 % 4 == 2){
		SSD1306_2_DrawBMP(move4 - 10, chance1, Asteroid_Animation_3, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
	if(move4 % 4 == 3){
		SSD1306_2_DrawBMP(move4 - 10, chance1, Asteroid_Animation_4, 14, SSD1306_2_WHITE);
		SSD1306_2_OutBuffer();
	}
}

void ShieldCheck1(void){	//Destroys asteroid 1 upon collision with shield
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(move1, chance1, Explosion, 14, SSD1306_2_WHITE); //Position determined by ADC
		SSD1306_2_OutBuffer();
		playsound(Fire);
		move1 = -1;
}

void ShieldCheck2(void){	//Destroys asteroid 2 upon collision with shield
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(move2, chance2, Explosion, 14, SSD1306_2_WHITE); //Position determined by ADC
		SSD1306_2_OutBuffer();
		playsound(Fire);
		move2 = -1;
}

void ShieldCheck3(void){	//Destroys asteroid 3 upon collision with shield
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(move3, chance3, Explosion, 14, SSD1306_2_WHITE); //Position determined by ADC
		SSD1306_2_OutBuffer();
		playsound(Fire);
		move3 = -1;
}

void ShieldCheck4(void){	//Destroys asteroid 4 upon collision with shield
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(move4, chance4, Explosion, 14, SSD1306_2_WHITE); //Position determined by ADC
		SSD1306_2_OutBuffer();
		playsound(Fire);
		move4 = -1;
}

void DebrisMove1(void){		//Moves asteroid 1 as well as checks for collision with shield or ship
	if(move1 < 0){
		move1 = 128;
		chance1 = Random();
		if(chance1 <= 8){
			chance1 = 8;
		}
	}
	if(move1 < 100 && send == 0){
		send++;	//Calls second asteroid
	}
	if(move1 < 50 && send == 1){
		send++;	//Calls third asteroid
	}
	if(move1 < 10 && send == 2){
		send++;	//Calls last asteroid
	}
//	Asteroid_Animate1();
	if((move1 >= 0) && (move1 <= 20) && (chance1 - 8 <= 64 - Position) && (chance1 + 8 >= 64 - Position) && shieldOn == 1){
		ShieldCheck1();	//Hit by shield
	}
	if((move1 >= 0) && (move1 <= 14) && (chance1 - 8 <= 64 - Position) && (chance1 + 8 >= 64 - Position) && shieldOn == 0){
		BigCheck();	//Destroys ship
	}
	move1-= 3;
}

void DebrisMove2(void){		//Moves asteroid 2 as well as checks for collision with shield or ship
	if(move2 < 0){
		move2 = 128;
		chance2 = Random();
		if(chance2 <= 8){
			chance2 = 8;
		}
	}
//	Asteroid_Animate2();
	if((move2 >= 0) && (move2 <= 20) && (chance2 - 8 <= 64 - Position) && (chance2 + 8 >= 64 - Position) && shieldOn == 1){
		ShieldCheck2();	//Hit by shield
	}
	if((move2 >= 0) && (move2 <= 14) && (chance2 - 8 <= 64 - Position) && (chance2 + 8 >= 64 - Position) && shieldOn == 0){
		BigCheck();	//Destroys ship
	}
	move2-= 3;
}

void DebrisMove3(void){		//Moves asteroid 3 as well as checks for collision with shield or ship
	if(move3 < 0){
		move3 = 128;
		chance3 = Random();
		if(chance3 <= 8){
			chance3 = 8;
		}
	}
//	Asteroid_Animate3();
	if((move3 >= 0) && (move3 <= 20) && (chance3 - 8 <= 64 - Position) && (chance3 + 8 >= 64 - Position) && shieldOn == 1){
		ShieldCheck3();	//Hit by shield
	}
	if((move3 >= 0) && (move3 <= 14) && (chance3 - 8 <= 64 - Position) && (chance3 + 8 >= 64 - Position) && shieldOn == 0){
		BigCheck();	//Destroys ship
	}
	move3-= 3;
}

void DebrisMove4(void){		//Moves asteroid 4 as well as checks for collision with shield or ship
	if(move4 < 0){
		move4 = 128;
		chance4 = 64 - Position;
		if(chance4 <= 8){
			chance4 = 8;
		}
	}
//	Asteroid_Animate4();
	if((move4 >= 0) && (move4 <= 20) && (chance4 - 8 <= 64 - Position) && (chance4 + 8 >= 64 - Position) && shieldOn == 1){
		ShieldCheck4();	//Hit by shield	
	}
	if((move4 >= 0) && (move4 <= 14) && (chance4 - 8 <= 64 - Position) && (chance4 + 8 >= 64 - Position) && shieldOn == 0){
		BigCheck();	//Destroys ship 
	}
	move4-= 3;
}

void GameplayInit(void){	//Before GamePlay, resets TravelBar, resets changed variables, and initializes timers
	activate = 1;
	lose = 0;
	win = 5;
	shieldOn = 0;
	move1 = 128;	//Set to the top and falling down
	move2 = 128;
	move3 = 128;
	move4 = 128;
	chance1 = Random();
	chance2 = Random();
	chance3 = Random();
	chance4 = Random();
	send = 0;
	if(chance1 <= 8){
		chance1 = 8;	//Not off-screen
	}
	Timer0_Init(800000);
	Timer1_Init(80000000);
	if(GameStop == 100){
		GameStop = 0;
	}
	TravelBarReset(NextPercent);
	NextPercent = 0;
	TravelBar(NextPercent);
}

void GameplayCheck(void){	//Determines win or lose screen based on GamePlay
	if(GameStop >= 100 && lose == 0){
		Timer0A_Stop();
		Timer1A_Stop();
		SSD1306_SetCursor(0, 7);
		playsound(Clear);
		if(language == 0){
			SSD1306_OutString("You Win!");
		}
		else if(language == 1){
			SSD1306_OutString("Tu Ganas!");
		}
		for(win = 5; win <= 90; win += 5){	//Win animation to Mars
			SSD1306_2_OutClear();
			SSD1306_2_DrawBMP(110, 64, Mars, 14, SSD1306_2_WHITE);
			SSD1306_2_DrawBMP(win, 40, Aithon_Ship, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
			Delay100ms(1);
		}
		Delay100ms(5);
		if(language == 0){
			SSD1306_2_OutClear();
			SSD1306_2_DrawBMP(0, 64, Win_Screen, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
		else if(language == 1){
			SSD1306_2_OutClear();
			SSD1306_2_DrawBMP(0, 64, Tu_Ganas, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
		while(ThirdSwitchToggle() == 0);
		GameStop = 0;
	}
	else if(GameStop >= 100 && lose == 1){
		Timer0A_Stop();
		Timer1A_Stop();
		SSD1306_2_OutClear();
		SSD1306_SetCursor(0, 7);
		playsound(Fire);
		if(language == 0){
			SSD1306_OutString("You Lose!");
			SSD1306_2_DrawBMP(0, 64, Lose_Screen, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
		else if(language == 1){
			SSD1306_OutString("Tu Pierdas!");
			SSD1306_2_DrawBMP(0, 64, Tu_Pierdas, 14, SSD1306_2_WHITE);
			SSD1306_2_OutBuffer();
		}
		Delay100ms(1);
		while(ThirdSwitchToggle() == 0);
		GameStop = 0;
	}
}

uint32_t Convert(uint32_t data){	//Function that converts ADC data into Position data
	int d;
	d =  (64*data)/4096;	//64 pixels
	if(d >= 56){
		d = 56;
	}
  return d; // replace this line with your Lab 8 solution
}

void Gameplay(void){	//The main game function, encompassing all interrupt changes, displays, and pause menu
	GameplayInit();
	while(GameStop <= 100){
		while(Pause == 1){	//Pause Menu
			Timer1A_Stop();
			if(language == 0){
				SSD1306_SetCursor(0, 6);
				SSD1306_OutString("1 - Restart");
				SSD1306_SetCursor(0, 7);
				SSD1306_OutString("2 - Resume");
			}
			else if(language == 1){
				SSD1306_SetCursor(0, 6);
				SSD1306_OutString("1 - Rehacer");
				SSD1306_SetCursor(0, 7);
				SSD1306_OutString("2 - Reanudar");
			}
			if(SecondSwitchToggle() == 1){
				Pause = 0;	//Resume game
				Timer1A_Start();
			}
			if(FirstSwitchToggle() == 1){
				Gameplay();	//Restart game
			}
		}
		while(MailStatus == 0);
		MailStatus = 0;
		Position = Convert(MailValue);
		SSD1306_2_ClearBuffer();
		SSD1306_2_DrawBMP(5, 64 - Position, Aithon_Ship, 14, SSD1306_2_WHITE); //Position determined by ADC
		if(GameStop <= 20){	//Turn satellite into asteroid after certain distance (time) is passed
			SSD1306_2_DrawBMP(move1, chance1, Satellite, 14, SSD1306_2_WHITE);
		}
		if(GameStop > 20){
			SSD1306_2_DrawBMP(move1, chance1, Asteroid, 14, SSD1306_2_WHITE);
		}
		SSD1306_2_DrawBMP(move2, chance2, Asteroid, 14, SSD1306_2_WHITE);
		SSD1306_2_DrawBMP(move3, chance3, Asteroid, 14, SSD1306_2_WHITE);
		SSD1306_2_DrawBMP(move4, chance4, Asteroid, 14, SSD1306_2_WHITE);
		if(shieldOn == 1){	//Draw shield above ship
			SSD1306_2_DrawBMP(16, 65 - Position, Shield, 14, SSD1306_2_WHITE);
		}
		SSD1306_2_OutBuffer();
		if(activate == 1){	//Shield is ready to be activated
			if(language == 0){
				SSD1306_SetCursor(0,2);
				SSD1306_OutString("SHIELD READY");
			}
			else if(language == 1){
				SSD1306_SetCursor(0,2);
				SSD1306_OutString("ESCUDO LISTA");
			}
			
			LED_on();
		}
		if(activate == 1 && FirstSwitchToggle() == 1){
			if(language == 0){	//Shield is activated and in use
				SSD1306_SetCursor(0,2);
				SSD1306_OutString("SHIELD CHARGING");
			}
			else if(language == 1){
				SSD1306_SetCursor(0,2);
				SSD1306_OutString("ESCUDO DE CARGA");
			}
			activate = 0;
			LED_off();
			shieldOn = 1;
			playsound(Drop);
		}
		if(spaceMove == 1){ //Interrupt ran on Timer0 every 1 ms; moves asteroids
			spaceMove = 0;
			DebrisMove1();
			if(send >= 1){
				DebrisMove2();
			}
			if(send >= 2){
				DebrisMove3();
			}
			if(send >= 3){
				DebrisMove4();
			}
		}
		if(flagBar == 1){ //Interrupt ran on Timer1 every 2 seconds; moves progress bar
			flagBar = 0;
			GameStop+= 2;
			NextPercent++;
			TravelBar(NextPercent);
		}
		Pause = ThirdSwitchToggle();
	}
	GameplayCheck();
	DisableInterrupts();
}

void Everything(void){	//GamePlay plus some initializations for the screens and variables
	while(1){
		EnableInterrupts();
		//Startup
		SSD1306_2_OutClear();
		SSD1306_2_SetCursor(0, 0);
		SSD1306_2_OutString("[:");
		//LoadingScreen();
		SSD1306_OutClear();
		SSD1306_DrawBMP(0,46,Title_Screen,14,SSD1306_2_WHITE);
		SSD1306_OutBuffer();
	
		SSD1306_SetCursor(3, 7);
		SSD1306_OutString("English");
		SSD1306_SetCursor(13, 7);
		SSD1306_OutString("Espanol");
		language = 2;
		flag = 2;
		final = 0;
		drawn = 0;
		language = 2;
		GameStop = 0;
		Pause = 0;
		while(language == 2){
			language = Menu_Select_Language();
		}
			if(language == 0){
				EnglishStory();
			}
			else if(language == 1){
				SpanishStory();
			}
			Gameplay();
	}
}

int main(void){ //Includes Everything function and all initializations for external files (save timers)
  DisableInterrupts();
  // pick one of the following three lines, all three set to 80 MHz
  PLL_Init();                   // 1) call to have no TExaS debugging
  //TExaS_Init(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_Init(&ScopeTask);       // or 3) call to activate analog scope PD2
	SSD1306_2_Init(SSD1306_2_SWITCHCAPVCC);
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear();
	SSD1306_2_OutClear();
	ADC_Init(5);
	LED_and_Switch_Init();
	SysTick_Init(8000000);
	DAC_Init();
  Random_Init(1);
	EnableInterrupts();
	Everything();
}

void Delay100ms(uint32_t count){ //100 ms for every 1 into this function
	uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

void Delay1ms(uint32_t count){	//1 ms for every 1 into this function
	uint32_t volatile time;
	while(count > 0){
		time = 7272;
		while(time){
			time--;
		}
		count--;
	}
}

//LED_and_Switch.c
//Fluctuates the LED to match the status of the shield ability
//Ethan Litchauer and Kevin Tong
// Last Modified: 5/2/2021 
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
extern int Restart;

void Delay1ms(uint32_t count);
void Delay100ms(uint32_t count);

void LED_and_Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x02; //activate clock for Port B
	int hold = SYSCTL_RCGCGPIO_R; //wait for clock to stabilize
	GPIO_PORTB_DIR_R |= 0x01; //Port 0 is output
	GPIO_PORTB_DEN_R |= 0x0F; //Enable data read for port 0
	GPIO_PORTB_AMSEL_R |= ~(0x0F); //no analog
	GPIO_PORTB_AFSEL_R |= ~(0x0F); //no alt function
	GPIO_PORTB_PCTL_R |= ~(0x00000F00); //regular function
}

int first = 0;
int second = 0;
int third = 0;
int time = 1;
int j;
int i;

int FirstSwitchToggle(void){	//returns 1 for pressed, 0 for not pressed
	first = (GPIO_PORTB_DATA_R & 0x00000002)>> 1;
	return first;
}

int SecondSwitchToggle(void){	//returns 1 for pressed, 0 for not pressed
	second = (GPIO_PORTB_DATA_R & 0x00000004)>> 2;
	return second;
}

int ThirdSwitchToggle(void){	//returns 1 for pressed, 0 for not pressed
	third = (GPIO_PORTB_DATA_R & 0x00000008)>> 3;
	return third;
}

void LED_on(void){
	GPIO_PORTB_DATA_R |= 0x01;
}

void LED_off(void){
	GPIO_PORTB_DATA_R &= ~(0x01);
}

void LED_breathe(){	//Not used; antiquated function for breathing LED
	while(ThirdSwitchToggle() == 0 && time != 10){
		for(j = 0; j < 100; j++){
			GPIO_PORTB_DATA_R |= 0x01;
			Delay1ms(time);
			GPIO_PORTB_DATA_R &= ~(0x01);
			Delay1ms(10-time);
		}
		time++;
	}
	while(ThirdSwitchToggle() == 1 && time != 0){
	for(int i = 0; i < 20; i++){
			GPIO_PORTB_DATA_R |= 0x01;
			Delay1ms(time);
			GPIO_PORTB_DATA_R &= ~(0x01);
			Delay1ms(10-time);
		}
		time--;
	}
	if(time == 10){
		GPIO_PORTB_DATA_R |= 0x01;
	}
	if(time == 0){
		GPIO_PORTB_DATA_R &= ~(0x01);
		Delay100ms(50);
	}
}

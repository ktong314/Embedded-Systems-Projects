//LED_and_Switch.c
//Fluctuates the LED to match the status of the shield ability
//Ethan Litchauer and Kevin Tong

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

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

void LED_on(void){
	GPIO_PORTB_DATA_R |= 0x01;
}

void LED_breathe(int flag){
	int time = 1;
	while(flag == 0 && time != 20){
		for(int j = 0; j < 10; j++){
			GPIO_PORTB_DATA_R |= 0x01;
			Delay100ms(time);
			GPIO_PORTB_DATA_R |= ~(0x01);
			Delay100ms(20-time);
		}
		time++;
	}
	while(flag == 1 && time != 0){
	for(int i = 0; i < 10; i++){
			GPIO_PORTB_DATA_R |= 0x01;
			Delay100ms(time);
			GPIO_PORTB_DATA_R |= ~(0x01);
			Delay100ms(20-time);
		}
		time--;
	}
}

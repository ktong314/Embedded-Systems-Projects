
#include "inc/tm4c123gh6pm.h"
#include "inc/CortexM.h"
#include "buttons.h"
#include "inc/PWM.h"
#include "inc/Unified_Port_Init.h"


uint8_t sw2_last;
uint16_t duty_periods[5] = {4000, 12000, 20000, 28000, 36000};	// corresponding to 10, 30, 50, 70, 90
static uint8_t duties_i = 0;



void sw1_debounced_init(){
	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F and port C
	
	//PF4 init
    GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in 
	GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4
    GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4   
    GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
    GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF4
    GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
    GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
    GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
    GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
    GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
    GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
    NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
	// initialize Timer3A to debounce
	SYSCTL_RCGCTIMER_R |= 1<<3;	  // 0) turn on timer 3a clock
	TIMER3_CTL_R &= ~0x0000001;   // 1) disable TIMER3A during setup
	TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER3_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
	TIMER3_TAILR_R = 1600000;      // 4) 20ms reload value
	TIMER3_TAPR_R = 0;            // 5) bus clock resolution
	TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
	TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
	NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
	NVIC_EN1_R = 1<<3;           // 9) enable IRQ 35 in NVIC
	
	sw2_last = 0;
}

void Timer3A_Handler(void){
	TIMER3_ICR_R = 0x01;	// clear the flag
	//TIMER3_IMR_R = 0x00000000;    // disarm timeout interrupt
	//if the port c 4 interupts
	if(sw2_last){
		sw2_last = 0;
		if(!(GPIO_PORTF_DATA_R & 0x10)){  // switch state
			duties_i = (duties_i + 1) % 5;
			PWM0A_Duty(duty_periods[duties_i]);
		}		 
	}
}

void GPIOPortF_Handler(){
	GPIO_PORTF_ICR_R |= 0x10;      // (e) clear flag4
	sw2_last = 1;
	TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A	(starts countdown)	
}


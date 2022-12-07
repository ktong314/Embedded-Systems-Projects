
#include "inc/tm4c123gh6pm.h"
#include "inc/CortexM.h"
#include "buttons.h"
#include "inc/PWM.h"
#include "inc/Unified_Port_Init.h"
#include "lib/transceiver.h"

uint8_t sw1_last;
uint8_t sw2_last;
static uint8_t sw1_edge = 0;
static uint8_t sw2_edge = 0;


// SW1 is PF4, SW2 is PB1 (not PF0 because I intended to leave SSI1 free in case I needed it for something)
// input: switches = 1 if sw1, 2 if sw2, 3 if both
void sw1_sw2_debounced_init(uint8_t switches){
//	Port_F_Init();
	SYSCTL_RCGCTIMER_R |= 1<<3;	  // 0) turn on timer 3a clock
	sw1_last = 0;
	sw2_last = 0;
	if (switches & 0x1) { //PF4 init
		sw1_edge = 1;
		SYSCTL_RCGCGPIO_R     |= 0x20;      // activate clock for Port F
		while((SYSCTL_PRGPIO_R & 0x20)==0){};     // allow time for clock to stabilize 
		GPIO_PORTF_LOCK_R     = 0x4C4F434B;       // unlock GPIO Port F
		GPIO_PORTF_CR_R       = 0x10;             // allow changes to PF4
		GPIO_PORTF_AMSEL_R = 0x0;       //     disable analog functionality on PF
		GPIO_PORTF_PCTL_R = 0x0; // configure PF as GPIO
		GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in
		GPIO_PORTF_AFSEL_R = 0x0;  //     disable alt funct on PF4
		GPIO_PORTF_DEN_R = 0x10;     //     enable digital I/O on PF4
		GPIO_PORTF_PUR_R = 0x10;     //     enable weak pull-up on PF4
		GPIO_PORTF_IS_R &= ~(0x10);     // (d) PF4 is edge-sensitive
		GPIO_PORTF_IBE_R &= ~(0x10);    //     PF4 is not both edges
		GPIO_PORTF_IEV_R &= ~(0x10);    //     PF4 falling edge event
		GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag 4
		GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 
		NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
		NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	}
	if (switches & 0x2) { //PB1 init
		sw2_edge = 1;
		SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
		while((SYSCTL_PRGPIO_R & 0x02) == 0){};   // Wait
		GPIO_PORTB_AMSEL_R = 0x0;       //     disable analog functionality on PB
		GPIO_PORTB_PCTL_R &= ~0x2; // configure PB1 as GPIO
		GPIO_PORTB_DIR_R &= ~0x2;    // (c) make PB1 in
		GPIO_PORTB_AFSEL_R &= ~0x2;  //     disable alt funct on PB1
		GPIO_PORTB_DEN_R |= 0x2;     //     enable digital I/O on PB1
		GPIO_PORTB_PUR_R |= 0x2;     //     enable weak pull-up on PB1
		GPIO_PORTB_IS_R &= ~(0x2);     // (d) PB1 is edge-sensitive
		GPIO_PORTB_IBE_R &= ~(0x2);    //     PB1 is not both edges
		GPIO_PORTB_IEV_R &= ~(0x2);    //     PB1 falling edge event
		GPIO_PORTB_ICR_R |= 0x2;      // (e) clear PB1 flag 
		GPIO_PORTB_IM_R |= 0x2;      // (f) arm interrupt on PB1
		NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|(5<<13); // (g) priority 5
		NVIC_EN0_R = 1<<1;      // (h) enable interrupt 1 in NVIC
	}
	
	// initialize Timer3A to debounce	
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
}

void sw1_sw2_level_init(uint8_t switches) {
	sw1_last = 0;
	sw2_last = 0;
	if (switches&0x1) { //PF4 init
		SYSCTL_RCGCGPIO_R     |= 0x20;      // activate clock for Port F
		while((SYSCTL_PRGPIO_R & 0x20)==0){};     // allow time for clock to stabilize 
		GPIO_PORTF_LOCK_R     = 0x4C4F434B;       // unlock GPIO Port F
		GPIO_PORTF_CR_R       = 0x10;             // allow changes to PF4
		GPIO_PORTF_AMSEL_R = 0x0;       //     disable analog functionality on PF
		GPIO_PORTF_PCTL_R = 0x0; // configure PF as GPIO
		GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in
		GPIO_PORTF_AFSEL_R = 0x0;  //     disable alt funct on PF4
		GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
		GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4
	}
	if (switches&0x2) { //PB1 init		
		SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
		while((SYSCTL_PRGPIO_R & 0x02) == 0){};   // Wait
		GPIO_PORTB_AMSEL_R = 0x0;       //     disable analog functionality on PB
		GPIO_PORTB_PCTL_R &= ~0x2; // configure PB1 as GPIO
		GPIO_PORTB_DIR_R &= ~0x2;    // (c) make PB1 in
		GPIO_PORTB_AFSEL_R &= ~0x2;  //     disable alt funct on PB1
		GPIO_PORTB_PUR_R |= 0x2;     //     enable weak pull-up on PB1
		GPIO_PORTB_DEN_R |= 0x2;     //     enable digital I/O on PB1
	}
}

uint8_t sw1 = 0;
uint8_t sw2 = 0;

void GPIOPortF_Handler(){	
	GPIO_PORTF_ICR_R |= 0x10;		
	if (!sw1_last && (GPIO_PORTF_DATA_R & 0x10)){			// if rising edge
		sw1_last = 1;
	} else if (sw1_last && !(GPIO_PORTF_DATA_R & 0x10)){	// else falling edge
		sw1_last = 0;
	}
	TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A	(starts countdown)	
}

void GPIOPortB_Handler(){
	GPIO_PORTB_ICR_R |= 0x2;
	if (!sw2_last && (GPIO_PORTB_DATA_R & 0x2)){			// if rising edge
		sw2_last = 1;
	} else if (sw2_last && !(GPIO_PORTB_DATA_R & 0x2)){	// else falling edge
		sw2_last = 0;
	}
	TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A	(starts countdown)	
}

void Timer3A_Handler(void){
	TIMER3_ICR_R = 0x01;	// clear the flag
	if (sw1_edge) {
		if(sw1_last && (GPIO_PORTF_DATA_R & 0x10)){	// if it was high and is still high, then the button was released
			sw1 = 0;		
		} else if (!sw1_last && !(GPIO_PORTF_DATA_R & 0x10)) {	// if it was low and is still low, then the button was pressed
			sw1 = 1;
		}
	}
	if (sw2_edge) {
		if(sw2_last && (GPIO_PORTB_DATA_R & 0x2)){	// if it was high and is still high, then the button was released
			switchRXmode();
			sw2 = 0;		
		} else if (!sw2_last && !(GPIO_PORTB_DATA_R & 0x2)) {	// if it was low and is still low, then the button was pressed
			switchTXmode();
			sw2 = 1;
		}
	}
}
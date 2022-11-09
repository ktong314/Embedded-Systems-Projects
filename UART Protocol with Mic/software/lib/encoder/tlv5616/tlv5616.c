/**
 * @file TLV5616.c
 * @author your name (you@domain.com)
 * @brief Low level driver for the TLV5616 12-bit SPI DAC.
 * @version 0.2.0
 * @date 2022-09-26
 *
 * @copyright Copyright (c) 2022
 * @note Reference datasheet:
 *     https://www.ti.com/lit/ds/symlink/tlv5616.pdf?ts=1644622732627&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTLV5616
 */

#include "tlv5616.h"
#include "inc/tm4c123gh6pm.h"

//#include "./lib/Dump.h"

#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))
	

/**
     * Unified_Port_Init in Lab5.c calls Port_D_Init, which initializes the Port
     * D GPIOs for the appropriate alternate functionality (SSI).
     *
     * According to Table 15-1. SSI Signals in the datasheet, this corresponds
     * to SSI1. The corresponding Valvanoware register defines are at L302 and
     * L2670 in inc/tm4c123gh6pm.h. Use this in combination with the datasheet
     * or any existing code to write your driver! An example of how to
     * initialize SSI is found in L741 in inc/ST7735.c.
     */
int tlv5616_init() {
	// output data on rising edge
	// SSI3SPH doesn't matter since we aren't reading data from DAC
//	
//	SYSCTL_RCGCSSI_R |= 0x01;
//	while ((SYSCTL_PRSSI_R & 0x1) == 0);
//	SYSCTL_RCGCGPIO_R |= 0x01;
//	while((SYSCTL_PRGPIO_R & 0x01) == 0){};
//	GPIO_PORTA_AFSEL_R |= 0x02C;
//	GPIO_PORTA_DEN_R |= 0x2C;
//	GPIO_PORTA_PCTL_R |= (GPIO_PORTA_PCTL_R & 0xFF0F00FF) + 0x00202200;
//	GPIO_PORTA_AMSEL_R = 0;
//	SSI0_CR1_R = 0x00000000;
//	SSI0_CPSR_R = 0x02;
//	SSI0_CR0_R &= ~(0x0000FFF0);
//	SSI0_CR0_R |= 0x0F;
//	SSI0_CR1_R |= 0x00000002;
//	
	SYSCTL_RCGCSSI_R |= 0x8;	// enable SSI3 clock
	SYSCTL_RCGCGPIO_R |= 0x8;	// enable Port D clock
	int delay = 1;
	while (delay) {delay--;}	// wait for clock to start
	GPIO_PORTD_AFSEL_R |= 0xB;	// enable alternate functions on PD0, 1, 3
	GPIO_PORTD_PCTL_R |= (GPIO_PORTD_PCTL_R&0xffff0f00)+0x00001011;		// enable SSI3 functions on PD0, 1, 3
	GPIO_PORTD_DEN_R |= 0xB;	// enable digital function on PD0, 1, 3
	GPIO_PORTD_AMSEL_R &= ~(0xB); // disable analog mode on PD0, 1, 3
	SSI3_CR1_R &= ~(0x6);		// disable SSI3, set master mode
	SSI3_CC_R &= ~(0xf);		// set clock source to system clock
	SSI3_CPSR_R |= 0x8;			// SSI3Clk set to busClk/8 = 80MHz/8 = 10MHz; ensures cheap logic analyzer compatability; allows sound freqs <= ~9765Hz
	SSI3_CR0_R = (SSI3_CR0_R&0xffff0000)+0x000f; // SCR = 0; SPH = don't care; SPO = 0 (TLV5616 reads on low CLK), FRF = 0 (freescale), 16 bit data
	SSI3_CR1_R |= 0x2;			// enable SSI3
    return 1;
}

int tlv5616_output(uint16_t data) {
	//PF1 ^= 0x02;
    //data = (data&0x00000fff)+0x4000; // add a 4-bit header to 12 bit data telling the DAC to operate in fast mode
	while((SSI3_SR_R&0x00000002) == 0) {} // wait for transmit FIFO to not be full
	SSI3_DR_R = data;
	return 1;
	//PF1 ^= 0x02;
}


//	SYSCTL_RCGCSSI_R |= 0x8;	// enable SSI3 clock
//	while ((SYSCTL_PRSSI_R & 0x8) == 0);
//	
//	SYSCTL_RCGCGPIO_R |= 0x8;	// enable Port D clock
//	while ((SYSCTL_PRGPIO_R & 0x8) == 0);
//	GPIO_PORTD_AFSEL_R |= 0xB;	// enable alternate functions on PD0, 1, 3
//	GPIO_PORTD_PCTL_R |= (GPIO_PORTD_PCTL_R&0xffff0000)+0x00001111;		// enable SSI3 functions on PD0, 1, 3
//	GPIO_PORTD_DEN_R |= 0xB;	// enable digital function on PD0, 1, 3
//	GPIO_PORTD_AMSEL_R &= ~(0xB); // disable analog mode on PD0, 1, 3
//	
//	SSI3_CR1_R &= ~0xF;		// disable SSI3, set master mode
//	SSI3_CR1_R &= ~0x1F;
//	SSI3_CC_R = 0;		// set clock source to system clock
//	SSI3_CPSR_R |= 0x8;			// SSI3Clk set to busClk/8 = 80MHz/8 = 10MHz; ensures cheap logic analyzer compatability; allows sound freqs <= ~9765Hz
//	SSI3_CR0_R &= ~0x0000FFFF;
//	SSI3_CR0_R |= 0x000f; // SCR = 0; SPH = don't care; SPO = 0 (TLV5616 reads on low CLK), FRF = 0 (freescale), 16 bit data
//	SSI3_CR1_R |= 0x2;			// enable SSI3
//    return 1;

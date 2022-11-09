/**
 * @file RX_UART.c
 * @author Allen Viljoen (ajv2648)
 *    Jared McArthur TTH 11am
 * @brief
 *    RX UART implementation for decoder.
 * 
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */
 
#define RX_FIFO_SIZE 64
 
#include "RX_UART.h"
#include "/lib/mic.h"
#include "/lib/fifo.h"
#include "/inc/Timer4A.h"
#include "inc/tm4c123gh6pm.h"
#include "./inc/CortexM.h"


extern uint16_t decoded_freq;
fifo_t RX_fifo;
uint16_t RX_array[RX_FIFO_SIZE];
uint8_t reject_message;
static uint32_t interrupt_count;
static uint32_t baud_period;
static uint16_t result = 0;
static uint8_t isReceiving = 0;

void RX_UART_init(uint32_t baud){
	baud_period = baud;
	fifoInit(&RX_fifo, RX_array, RX_FIFO_SIZE); //includes 8-bit message and parity bit
	Timer4A_Init(&RX_UART_read_message, baud_period, 3); //delay
	Timer4A_Stop();
	result = 0;		// clear the resulting message bit vector
	interrupt_count = 0;	
}


// called on the falling edge of the start bit by mic.c
void begin_convert(void){
	isReceiving = 1;
	// set timer delay to 1.5*Baud - 0.5*SampleTime (SampleTime ~= 640k clocks)
	DisableInterrupts();
	TIMER4_TAILR_R = (3*baud_period)/2 - 320000 - 1;    // 4) reload value
	NVIC_EN2_R = 0x00000040;      
	TIMER4_CTL_R = 0x00000001;    // start timer4
	EnableInterrupts();
}

// first time this is called per message is after the 1.5 baud delay, IE the middle of bit 0
static uint8_t parity_check = 0;
void RX_UART_read_message(void){
	DisableInterrupts();
	if (interrupt_count == 0) {
		TIMER4_TAILR_R = baud_period - 1; // needs to set the timer delay to the actual baud rate
	}
	EnableInterrupts();
	
	if (interrupt_count < 9) {
		result |= (decoded_freq << interrupt_count);	// set the corresponding bit in the message bit vector
		parity_check += decoded_freq;					// checks for even parity
	}
	
	// parity check
	if (interrupt_count == 8 && (parity_check%2)) {	// invalid parity
//		DisableInterrupts();
//		Timer4A_Stop();		// stop the timer
//		EnableInterrupts();
//		interrupt_count = 0;
//		parity_check = 0;
		result = 0x2a; // set to '*' if there's a parity or framing error
	} else if (interrupt_count == 9) {	
		if (!isFull(&RX_fifo)) { // valid stop bit and parity so add to the RX_FIFO
			if (decoded_freq) {
				fifoPush(&RX_fifo, result & 0xff);
			} else {
				fifoPush(&RX_fifo, '*');
			}
		}
		DisableInterrupts();
		Timer4A_Stop();
		EnableInterrupts();
		interrupt_count = 0;
		parity_check = 0;
		result = 0;
		isReceiving = 0;
		return;
	}
	interrupt_count++;
}

uint8_t RX_fifo_full(){
	return isFull(&RX_fifo);
}

uint8_t RX_fifo_empty(){
	return isEmpty(&RX_fifo);
}

uint8_t RX_fifo_pop(){
	return fifoPop(&RX_fifo);
}

uint8_t RX_UART_isReceiving() {
	return isReceiving;
}





 
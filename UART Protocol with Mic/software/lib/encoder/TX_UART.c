/**
 * @file TX_UART.c
 * @author Allen Viljoen (ajv2648)
 *    Jared McArthur TTH 11am
 * @brief
 *    TX UART implementation for encoder.
 * 
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */
#define TX_FIFO_SIZE 64
#define HIGH_PER 213333	// 375Hz
#define LOW_PER 80000	// 1kHz

#include "TX_UART.h"
#include "lib/fifo.h"
#include "inc/Timer0A.h"
#include "lib/encoder/DAC.h"


#include "inc/Unified_Port_Init.h"
#define PF1       (*((volatile uint32_t *)0x40025008))    // RED LED
#define PF2       (*((volatile uint32_t *)0x40025010))    // BLUE LED
#define PF3       (*((volatile uint32_t *)0x40025020))    // GREEN LED

static uint16_t output_array [TX_FIFO_SIZE];
fifo_t output_queue;
uint8_t output_index;
uint16_t output_encoded;


void TX_UART_init(int baud_period){
	PF1 = 0x2;
	Timer0A_Init(&update_DAC_period, baud_period, 3);
	fifoInit(&output_queue, output_array, TX_FIFO_SIZE);
	output_index = 11;
	
}

static void update_DAC_period() {
	if (!isEmpty(&output_queue) && output_index == 11) {	// if the output bitstream is empty and there's something in the output queue
		output_index = 0;
		output_encoded = fifoPop(&output_queue);
	}
	if (output_index < 11) {
		if (output_encoded & 0x1){	// encode the LSB bit as a frequency
			//TODO: put in code IAW the dac header for setting the period of the interrupt
			DAC_change_period(HIGH_PER);
			PF1 = 0x2;
		} else {
			//TODO: put in code IAW the dac header for setting the period of the interrupt
			DAC_change_period(LOW_PER);			
			PF1 = 0x0;
		}
		output_encoded >>= 1;
		output_index++;	// increment the LSB and the index counter
	} // no need to change anything since the stop bit is a high, so the DAC output will be high
}


uint8_t TX_UART_send(uint8_t data){
	if (isFull(&output_queue)) {
		return 0;
	}
	uint8_t even_parity = 0;
	for (int i = 0; i < 8; i++) {
		if (data & (1<<i)) {
			even_parity++;
		}
	}
	even_parity %= 2;
	uint16_t output_bitstream = data << 1;	// shift the data over so the first bit sent is the start bit
	output_bitstream &= ~(0x1);	// clear bit 0 to implement the start bit
	output_bitstream |= even_parity << 9;	// bit 9 becomes the parity bit
	output_bitstream |= 0xc00;			// bit 10 (and 11) is the stop bit
	fifoPush(&output_queue, output_bitstream);
	return 1;	
}
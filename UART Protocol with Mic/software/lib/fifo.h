/**
 * @file fifo.h
 * @author Allen Viljoen (ajv2648)
 *    Jared McArthur TTH 11am
 * @brief
 *    Fifo struct for lab 9. This code won't save you from yourself.
 *	  IE make sure to check emptiness/fullness before pushing or popping.
 * 	
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */
 
 #pragma once
 #include <stdint.h>
 
 typedef struct fifo{
	uint16_t* fifo_data;
	uint16_t head;
	uint16_t tail;
	uint16_t num_elements;
} fifo_t;
 
void fifoInit(fifo_t* userFifo, uint16_t* userArr, uint16_t n);

uint8_t isFull(fifo_t* userFifo);

uint8_t isEmpty(fifo_t* userFifo);

uint16_t fifoPop(fifo_t* userFifo);

void fifoPush(fifo_t* userFifo, uint16_t elem);
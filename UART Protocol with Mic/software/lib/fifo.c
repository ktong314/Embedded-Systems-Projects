/**
 * @file fifo.c
 * @author Allen Viljoen (ajv2648)
 *    Jared McArthur TTH 11am
 * @brief
 *    fifo implementation for lab 9.
 * 
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */
 
#include "fifo.h"
 
void fifoInit(fifo_t* userFifo, uint16_t* userArr, uint16_t n){
	userFifo->fifo_data = userArr;
	userFifo->head = 0;
	userFifo->tail = 0;
	userFifo->num_elements = n;
	
}

uint8_t isFull(fifo_t* userFifo){
	return (userFifo->head + 1)%userFifo->num_elements == userFifo->tail;
}

uint8_t isEmpty(fifo_t* userFifo){
	return userFifo->head == userFifo->tail;
}

uint16_t fifoPop(fifo_t* userFifo){
	int retVal = userFifo->fifo_data[userFifo->tail];
	userFifo->tail = (userFifo->tail +1)%userFifo->num_elements;
	return retVal;
}

void fifoPush(fifo_t* userFifo, uint16_t elem){
	userFifo->fifo_data[userFifo->head] = elem;
	userFifo->head = (userFifo->head + 1)%userFifo->num_elements;
}
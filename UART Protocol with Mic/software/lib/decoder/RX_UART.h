/**
 * @file RX_UART.h
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
 #pragma once
 
#include <stdint.h>

void RX_UART_init(uint32_t baud_period);

void begin_convert(void);

void RX_UART_read_message(void);

void convert_to_ascii(void);

uint8_t RX_fifo_full();

uint8_t RX_fifo_empty();

uint8_t RX_fifo_pop();

uint8_t RX_UART_isReceiving();
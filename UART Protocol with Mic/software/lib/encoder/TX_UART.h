/**
 * @file TX_UART.h
 * @author Allen Viljoen (ajv2648)
 *    Jared McArthur TTH 11am>
 * @brief
 *    TX UART implementation for encoder.
 *
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */

#pragma once
#include <stdint.h>

void TX_UART_init(int baud_period);

static void update_DAC_period();

uint8_t TX_UART_send(uint8_t data);
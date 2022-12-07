/**
 * @file transceiver.h
 * @author Allen Viljoen (allenviljoen@utexas.edu)
 *    Jared McArthur (TTH 11am)
 * @brief 
 *    Software driver for the transceiver module of the walkie talkie.
 * 	  Contains code to initialize, send and receive string messages,
 *	  and set the current communication channel.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *	  Used code examples from https://electronoobs.com/eng_arduino_tut98.php  
 *    Configuration settings generated using SmartRF Studio software by Texas Instruments.
 */


#pragma once
#include <stdint.h>

// Pin definitions (with multiple labels)
#define PD0       		 		(*((volatile uint32_t *)0x40007004))		// Clk
#define CC1101_CLK_PIN       	(*((volatile uint32_t *)0x40007004))		// Clk
#define PD1       				(*((volatile uint32_t *)0x40007008))		// CSn
#define CC1101_CSn_PIN       	(*((volatile uint32_t *)0x40007008))		// CSn
#define PD2       				(*((volatile uint32_t *)0x40007010))		// MISO
#define CC1101_MISO_PIN       	(*((volatile uint32_t *)0x40007010))		// MISO
#define PD3       				(*((volatile uint32_t *)0x40007020))		// MOSI
#define CC1101_MOSI_PIN       	(*((volatile uint32_t *)0x40007020))		// MOSI
#define PD6       				(*((volatile uint32_t *)0x40007100))		// GDO0
#define CC1101_GDO0_PIN       	(*((volatile uint32_t *)0x40007100))		// GDO0
#define PD7       				(*((volatile uint32_t *)0x40007200))		// GDO2
#define CC1101_GDO2_PIN       	(*((volatile uint32_t *)0x40007200))		// GDO2
	
// addresses
#define CC1101_GDO2_CFG_R				0x00	// IOCFG2 output pin configuration register: [7] not used; [6] inverse; [5:0] configuration setting
#define CC1101_GDO0_CFG_R				0x02	// IOCFG0 output pin configuration register: [7] not used; [6] inverse; [5:0] configuration setting
#define CC1101_CHANNR_R					0x0A	// CHANNR 8-bit channel number (256 options)
#define CC1101_TX_FIFO_R 				0x3F	// Transmit fifo, write only; single byte access
#define CC1101_RX_FIFO_R 				0xBF	// Receive fifo, read only; single byte access

// output pin configuration settings
#define CC1101_GDO_RX_THRESH_SET		0x00	// 1 when RX FIFO is filled at or above the RX FIFO threshold. 0 when RX FIFO is drained below the same threshold.
#define CC1101_GDO_RX_EMPTYn_SET		0x01	// 1 when RX FIFO is filled at or above the RX FIFO threshold or the end of packet is reached. 0 when the RX FIFO is empty.
#define CC1101_GDO_TX_THRESH_SET		0x02	// 1 when the TX FIFO is filled at or above the TX FIFO threshold. 0 when the TX FIFO is below the same threshold.
#define CC1101_GDO_TX_FULL_SET			0x03	// 1 when TX FIFO is full. 0 when the TX FIFO is drained below the TX FIFO threshold.
#define CC1101_GDO_SYNC_SET				0x06	// 1 when sync word has been sent/received, 0 at the end of the packet. 
												// In RX, 0 when a packet is discarded due to address or maximum length filtering or when the radio enters RXFIFO_OVERFLOW state. 
												// In TX, 0 if the TX FIFO underflows.
#define CC1101_GDO_CRC_OK_SET			0x07	// 1 when a packet has been received with CRC OK. 0 when the first byte is read from the RX FIFO. (Use for interrupts)

// command strobes
#define CC1101_SRES_CMD					0x30	// reset chip
#define CC1101_SFSTXON_CMD				0x31	// frequency sythesizer enable and calibrate (?)
#define CC1101_SXOFF_CMD				0x32	// turn off oscillator
#define CC1101_SCAL_CMD					0x33	// Calibrate freq sythesizer and turn off. Strobe from IDLE.
#define CC1101_SRX_CMD					0x34	// Enable RX. Calibrates first if coming from IDLE as well depending on configuration settings
#define CC1101_STX_CMD					0x35	// Enable TX. Calibrates if coming from IDLE (depending on config)
#define CC1101_SIDLE_CMD				0x36	// Exit TX/RX, turn off freq synthesizer and exit WOR mode
#define CC1101_SWOR_CMD					0x38	// Start WOR depending on configuration settings
#define CC1101_SPWD_CMD					0x39	// Power down when CSn goes high
#define CC1101_SFRX_CMD					0x3A	// Flush the RX fifo when status byte indicates IDLE (000) or RXFIFO_OVERFLOW (110)
#define CC1101_SFTX_CMD					0x3B	// Flush the TX fifo when status byte indicates IDLE (000) or TXFIFO_OVERFLOW (111)
#define CC1101_SWORRST_CMD				0x3C	// dunno
#define CC1101_SNOP_CMD					0x3D	// No operation, used to get the status byte if needed




// initilizes SPI and the CC1101.
// tx_mode specifies whether to start in tx_mode (1) or rx_mode (0)
void Transceiver_init(uint8_t tx_mode);

// reads the value from the software fifo that an interrupt will be filling
uint8_t RX_FIFO_get();

void TX_FIFO_send(uint8_t data);

// writes to a specified register
void CC1101_Write(char address, char data);

// reads data from a specified register into CC1101Data
uint8_t CC1101_Read(char address);

// reads burst data from a specified register into CC1101Data
void CC1101_Burst_Read(uint8_t address, uint8_t* RX_Array, uint8_t length);

// sends a command strobe to the CC1101
void CC1101_Command_Strobe(uint8_t command);

// sends the specified data on the specified channel while in TX mode
void CC1101_Send_Data(uint8_t data);

// sends bulk data, up to 61 bytes at once
void CC1101_Send_Bulk_Data(uint8_t* data_array, uint8_t length);

// receives the specified data on the specified channel while in RX mode
void CC1101_Receive_Data();

// receives multiple bytes (up to 61) of specified data on the specified channel while in RX mode
uint8_t CC1101_Receive_Bulk_Data(uint8_t* RX_Array);

// changes the channel
void setChannel(uint8_t channel);

// returns the current channel number
uint8_t getChannel();

void switchRXmode();

void switchTXmode();

uint8_t CC1101RXfifoHasData();
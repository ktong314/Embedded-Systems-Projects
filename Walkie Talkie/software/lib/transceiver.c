/**
 * @file transceiver.c
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
 
#include "transceiver.h"
#include "inc/tm4c123gh6pm.h"
#include "buttons.h"

uint8_t CC1101Status;	// byte to hold current status when sending headers, data, or command strobes
uint8_t CC1101Data;	// data from RX mode, may turn into a fifo buffer. Data loss not super important though	


/* Initialization settings:*/

// Address Config = No address check 
// Base Frequency = 432.999817 
// CRC Autoflush = false 
// CRC Enable = true 
// Carrier Frequency = 432.999817 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Format = Normal mode 
// Data Rate = 249.939 
// Deviation = 126.953125 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = GFSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 541.666667 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 

static const uint8_t CC1101Settings[]= 
{
    0x0002, 0x06, //iocfg0: GDO0 Output Pin Configuration
    0x0008, 0x05, //pktctrl0: Packet Automation Control
    0x000b, 0x12, //fsctrl1: Frequency Synthesizer Control
    0x000d, 0x10, //freq2: Frequency Control Word, High Byte
    0x000e, 0xa7, //freq1: Frequency Control Word, Middle Byte
    0x000f, 0x62, //freq0: Frequency Control Word, Low Byte
    0x0010, 0x2d, //mdmcfg4: Modem Configuration
    0x0011, 0x3b, //mdmcfg3: Modem Configuration
    0x0012, 0x93, //mdmcfg2: Modem Configuration
    0x0015, 0x62, //deviatn: Modem Deviation Setting
    0x0018, 0x18, //mcsm0: Main Radio Control State Machine Configuration
    0x0019, 0x1d, //foccfg: Frequency Offset Compensation Configuration
    0x001a, 0x1c, //bscfg: Bit Synchronization Configuration
    0x001b, 0xc7, //agcctrl2: AGC Control
    0x001c, 0x00, //agcctrl1: AGC Control
    0x001d, 0xb0, //agcctrl0: AGC Control
    0x0020, 0xfb, //worctrl: Wake On Radio Control
    0x0021, 0xb6, //frend1: Front End RX Configuration
    0x0023, 0xea, //fscal3: Frequency Synthesizer Calibration
    0x0024, 0x2a, //fscal2: Frequency Synthesizer Calibration
    0x0025, 0x00, //fscal1: Frequency Synthesizer Calibration
    0x0026, 0x1f, //fscal0: Frequency Synthesizer Calibration
    0x002e, 0x09, //test0: Various Test Settings
};

// initialize PD0-3 as SPI with PD1 (CSn) as digital output (since CS stays low for whole transaction per CC1101 datasheet p30)
// PD6 as GDO0 (1 if TX FIFO full/above threshold, 0 if below threshold) and PD7 as GDO2 (1 when RX FIFO above threshold; 0 if empty)
static void SPI3_Init(){
	SYSCTL_RCGCSSI_R |= 0x8;		// 1) enable SSI3 clock
	SYSCTL_RCGCGPIO_R |= 0x8;		// 2) enable Port D clock
	int delay = 1;
	while (delay) {delay--;}		// wait for clock to start
	GPIO_PORTD_AFSEL_R |= 0x0D;		// 3) enable alternate functions on PD0-3, not PD1
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xffff00f0)+0x00001101;		// 4) enable SSI3 functions on PD0-3, not PD1
	GPIO_PORTD_DEN_R |= 0xCF;		// 5) enable digital function on PD0-3, 6-7
	GPIO_PORTD_DIR_R &= ~(0xC0);	// 	 a) PD6-7 in
	GPIO_PORTD_DIR_R |= 0x2;		// 	 b) PD1 out
	GPIO_PORTD_PDR_R |= 0xC0;		// 	 c) PD6-7 pull down since the CC1101 will send a high voltage when true
	GPIO_PORTD_IS_R &= ~(0xC0);		//	 d) PD6-7 edge sensitive interrupts
	GPIO_PORTD_IBE_R |= 0xC0;		//	 e) PD6-7 interrupt both edges
	GPIO_PORTD_ICR_R |= 0xC0;		//	 f) clear PD6-7 interrupt flags
	GPIO_PORTD_IM_R |= 0xC0;		//	 g) arm PD6-7 interrupt
	NVIC_PRI0_R = (NVIC_PRI0_R & 0x00FFFFFF) | 2 << 29;	//	 h) priority 2
	NVIC_EN0_R = 1<<3;				//	 i) enable interrupt 3
	PD1 = 0x2;						// CSn high idle
	SSI3_CR1_R = 0;					// 1) 2) disable SSI3; set master mode
	SSI3_CC_R &= ~(0xf);			// 3) set clock source to system clock
	SSI3_CPSR_R |= 0x8;				// 4) SSI3Clk set to busClk/10 = 80MHz/10 = 8MHz; ensures logic analyzer compatability (the cheap one from amazon); 
	SSI3_CR0_R = (SSI3_CR0_R&0xffff0000)+0x0007; // 5) SCR = 0; SPH = 0 (read on 1st sck edge); SPO = 0 (low idle clock), FRF = 0 (freescale), DSS = 0x7 (8 bit data)
									// 6) skip (no uDMA)
	SSI3_CR1_R |= 0x2;				// 7) enable SSI3
	do {
		CC1101_Command_Strobe(CC1101_SNOP_CMD);
	} while(CC1101Status&0x80);		// don't send data until the MISO pin is low (clock has stabilized)
	if (CC1101Status >> 4 == 0x6) {	// RX fifo overflow
		CC1101_Command_Strobe(CC1101_SFRX_CMD);
	}
	if (CC1101Status >> 4 == 0x7) {	// TX fifo underflow
		CC1101_Command_Strobe(CC1101_SFTX_CMD);
	}
	//
}

static uint8_t PD6_Last = 0;
uint8_t PD6_Rising = 0;
uint8_t PD6_Falling = 0;
static uint8_t PD7_Last = 0;
uint8_t PD7_Rising = 0;
uint8_t PD7_Falling = 0;

void GPIOPortD_Handler() {
	if (GPIO_PORTD_RIS_R & 0x40) {		// PD6 interrupted
		GPIO_PORTD_ICR_R |= 0x40;		// acknowledge flag
		if (!PD6_Last && (GPIO_PORTD_DATA_R & 0x40)){			// if rising edge
			PD6_Last = 1;
			PD6_Rising = 1;
			PD6_Falling = 0;
		} else if (PD6_Last && !(GPIO_PORTD_DATA_R & 0x40)){	// else falling edge
			PD6_Last = 0;
			PD6_Rising = 0;
			PD6_Falling = 1;
		} 
	}
	if (GPIO_PORTD_RIS_R & 0x80) {
		GPIO_PORTD_ICR_R |= 0x80;		// acknowledge flag
		if (!PD7_Last && (GPIO_PORTD_DATA_R & 0x80)){			// if rising edge
			PD7_Last = 1;
			PD7_Rising = 1;
			PD7_Falling = 0;
		} else if (PD7_Last && !(GPIO_PORTD_DATA_R & 0x80)){				// else falling edge
			PD7_Last = 0;
			PD7_Rising = 0;
			PD7_Falling = 1;
		}
	}
}


// initilizes SPI and the CC1101.
// tx_mode specifies whether to start in tx_mode (1) or rx_mode (0)
void Transceiver_init(uint8_t tx_mode){
	SPI3_Init();
	for (int i = 0; i < 46; i+=2) {
		CC1101_Write(CC1101Settings[i], CC1101Settings[i+1]);
	}
	if (tx_mode) {
		switchTXmode();
	} else {
		switchRXmode();
	}
}

// reads the value from the hardware fifo
uint8_t RX_FIFO_get(){
	while((SSI3_SR_R&0x04) == 0) {}
	return SSI3_DR_R & 0x00ff;			
}

void TX_FIFO_send(uint8_t data){	
	while((SSI3_SR_R&0x02) == 0) {} 	// wait for transmit FIFO to not be full
	SSI3_DR_R = data;		// send the data over SPI
}

// transmits data on the selected channel
void CC1101_Write(char address, char data){		// R/Wn bit stays 0 for write (address stays unchanged)
	CC1101_CSn_PIN = 0;					// CSn low to start transmission
	
	TX_FIFO_send(address);				// send the address
	CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	
	TX_FIFO_send(data);					// send the data
	CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	
	CC1101_CSn_PIN = 0xFF;				// CSn high to end transmission
}

void CC1101_Burst_Write(char address, uint8_t* data_array, uint8_t length){
	address = address | 0x40;	// set the burst bit
	CC1101_CSn_PIN = 0;					// CSn low to start transmission
	
	TX_FIFO_send(address);				// send the address
	CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	for (int i = 0; i < length; i++){
		TX_FIFO_send(data_array[i]);		// send the data
		CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	}	
	
	CC1101_CSn_PIN = 0xFF;				// CSn high to end transmission
}

// reads data from a specified register into CC1101Data
uint8_t CC1101_Read(char address){	
	CC1101_CSn_PIN = 0;					// CSn low to start transmission
	
	address |= 0xC0;					// Set the R/Wn bit and the burst bit
	TX_FIFO_send(address);				// send the address with the read bit set
	CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	TX_FIFO_send(CC1101_SNOP_CMD);		// send a NOP command while receiving the data	(might be optional, idk how the SSI state machine works)
	CC1101Data = RX_FIFO_get();			// write the data byte to the global variable
	
	CC1101_CSn_PIN = 0xFF;				// CSn high to end transmission
	return CC1101Data;
}

// reads burst data from a specified register into CC1101Data
void CC1101_Burst_Read(uint8_t address, uint8_t* RX_Array, uint8_t length){	
	CC1101_CSn_PIN = 0;					// CSn low to start transmission	
	address |= 0xC0;					// Set the R/Wn bit [7] and burst bit [6]
	TX_FIFO_send(address);				// send the address with the read bit set
	CC1101Status = RX_FIFO_get();		// any time we send a header, data, or command strobe byte the CC1101 sends a status byte at the same time
	for (int i = 0; i < length; i++) {
		TX_FIFO_send(CC1101_SNOP_CMD);	// send a NOP command while receiving the data
		RX_Array[i] = RX_FIFO_get();	// put the data into the array
	}
	CC1101_CSn_PIN = 0xFF;				// CSn high to end transmission	
}

// sends a command strobe to the CC1101
void CC1101_Command_Strobe(uint8_t command){
	CC1101_CSn_PIN = 0;					// CSn low to start a transmission
	
	TX_FIFO_send(command);
	CC1101Status = RX_FIFO_get();
	
	CC1101_CSn_PIN = 0xFF;				// CSn high to end the transmission
}

// sends the specified data on the specified channel while in TX mode
void CC1101_Send_Data(uint8_t data){
	CC1101_Write(CC1101_TX_FIFO_R, 1);
	CC1101_Write(CC1101_TX_FIFO_R, data);
	CC1101_Command_Strobe(CC1101_STX_CMD);		// start TX	
	while((CC1101_GDO0_PIN & 0x40) == 0) {} 	// wait for the sync word to be sent
	while(CC1101_GDO0_PIN & 0x40) {}			// wait for the packet to finish sending
	CC1101_Command_Strobe(CC1101_SFTX_CMD);		// flush the tx fifo
}

// sends bulk data, up to 61 bytes at once
void CC1101_Send_Bulk_Data(uint8_t* data_array, uint8_t length){	
	CC1101_Write(CC1101_TX_FIFO_R, length);
	CC1101_Burst_Write(CC1101_TX_FIFO_R, data_array, length);
	CC1101_Command_Strobe(CC1101_STX_CMD);		// start TX	
	while((CC1101_GDO0_PIN & 0x40) == 0) {} 	// wait for the sync word to be sent
	
	while(CC1101_GDO0_PIN & 0x40) {}			// wait for the packet to finish sending
	CC1101_Command_Strobe(CC1101_SFTX_CMD);		// flush the tx fifo
	
}

// receives the specified data on the specified channel while in RX mode
void CC1101_Receive_Data(){
	CC1101_Command_Strobe(CC1101_SNOP_CMD|0x80);	// send a NOP command with write bit set to get the status byte with RX fifo size
	if (CC1101Status & 0xF) {		// if there's anything in the RX fifo
		CC1101_Read(CC1101_RX_FIFO_R);	// the first byte is the size byte, this sets CC1101Data to that number
		uint8_t size  = CC1101Data;	// should always be 1 due to how I wrote transmit
		for (int i = 0; i < size; i++){
			CC1101_Read(CC1101_RX_FIFO_R);	// should only run once, CC1101Data becomes the data byte that was sent
		}
	} 
	CC1101_Command_Strobe(CC1101_SFRX_CMD);		// flush the RX fifo
//	while(!(CC1101_GDO2_PIN & 0xFF)) {}	// wait for something to be in the RX fifo
//	CC1101_Read(CC1101_RX_FIFO_R);
}

// receives multiple bytes (up to 61) of specified data on the specified channel while in RX mode
uint8_t CC1101_Receive_Bulk_Data(uint8_t* RX_Array){
	CC1101_Command_Strobe(CC1101_SNOP_CMD|0x80);	// send a NOP command with write bit set to get the status byte with RX fifo size
	if (CC1101Status & 0xF) {						// If there's something in the RX fifo
		uint8_t length = CC1101_Read(CC1101_RX_FIFO_R);		// the length is the first item in the fifo
		CC1101_Burst_Read(CC1101_RX_FIFO_R, RX_Array, length);		// read the burst data into the array
		CC1101_Command_Strobe(CC1101_SFRX_CMD);						// clear the RX fifo on the CC1101
		return length;
	} else {		
		CC1101_Command_Strobe(CC1101_SFRX_CMD);						// clear the RX fifo on the CC1101
		return 0;
	}
}

// changes the channel (address 0x0A on CC1101, unsigned 8-bit channel number)
void setChannel(uint8_t channel){
	CC1101_Command_Strobe(CC1101_SIDLE_CMD);		// switch to IDLE mode command
	CC1101_Write(CC1101_CHANNR_R, channel);			// switch to the specified channel
	switchRXmode();
}

// returns the current channel number
uint8_t getChannel(){
	
	//TODO: implement
	return 0;
}

// switches GDOx pins to GDO2 (0x0000): 		GDO0 (0x0002): 
void switchRXmode(){
	CC1101_Write(CC1101_GDO0_CFG_R, CC1101_GDO_SYNC_SET);			// setup GDO0 to assert true when a sync word has been received
	CC1101_Write(CC1101_GDO2_CFG_R, CC1101_GDO_RX_EMPTYn_SET);		// setup GDO2 to assert true when the RX fifo is above a threshhold and de assert when 0
//	CC1101_Command_Strobe(CC1101_SIDLE_CMD);						// switch to IDLE mode command
//	CC1101_Command_Strobe(CC1101_SFTX_CMD);							// clears the TX buffer (unsure if needed)
	CC1101_Command_Strobe(CC1101_SRX_CMD);							// switch to RX mode command
}

void switchTXmode(){
	CC1101_Write(CC1101_GDO0_CFG_R, CC1101_GDO_SYNC_SET);			// setup GDO0 to assert true when a sync word has been sent
	CC1101_Write(CC1101_GDO2_CFG_R, CC1101_GDO_TX_FULL_SET);		// setup GDO2 to assert true when the TX fifo is full, false when below threshhold
//	CC1101_Command_Strobe(CC1101_SIDLE_CMD);						// switch to IDLE mode command
//	CC1101_Command_Strobe(CC1101_SFTX_CMD);							// clears the TX buffer (unsure if needed)
}



// Used for polling in busy wait
uint8_t CC1101RXfifoHasData() {
	return (CC1101_GDO2_PIN & 0x40) ? 1 : 0;
}



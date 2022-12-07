//this is the main file

// file LabFinal.c
// Names: 
// TA: Jared
// Section: 
// brief: This is the main file for the code in the final lab. 
 
#define MAIN6
/**	change the x in MAINx corresponding to the table below:

	MAIN0:	Test the speaker
	MAIN0a: Test the keyboard
	MAIN1:	Test the screen on the PCB
	MAIN2:	Test mic/speaker
	MAIN3:	Test the transceiver in tx mode no sound
	MAIN4: 	Test the transceiver in rx mode no sound
	MAIN5:	Test half-duplex controlled by btn2
	MAIN6:	Test half-duplex with sound controlled by btn2 (basic walkie talkie functionality)
	MAIN7:	Test the transceiver in tx mode with sound
	MAIN8:	Test display fliped
	
*/
 
 //includes for the main file

#include "./inc/tm4c123gh6pm.h"
#include "./inc/PLL.h"
#include "./inc/CortexM.h"
#include "./inc/Unified_Port_Init.h"
#include "./inc/UART.h"
#include "./inc/ST7735.h"

/** MAP Pin definitions. */
#define MIC_BUFFER_NUM 100
#define ARRLEN 32


//speaker main
#ifdef MAIN0

#include "speaker.h"
#include "mic.h"
#include "transceiver.h"
#include "keyboard.h"
#include "speaker.h"
#include "mic.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

extern int output_data1;
extern char CC1101Status;
extern char CC1101Data;
uint32_t mic_buffer[100];
int mic_counter;


int mainSpeaker(void){
	
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Unified_Port_Init();
	EnableInterrupts();

	while (1) {
		output_data1=1;
		//test();
		Timer1A_Init(&mic_out, MIC_PERIOD/2, 2);
	}
	return 1;
}

int main_mic_test(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Unified_Port_Init();
	mic_init();
	EnableInterrupts();
	mic_counter = 0;
	while(1){
		while(mic_counter < MIC_BUFFER_NUM){
			mic_buffer[mic_counter] = get_queue();
		  mic_counter++;
		}
		
	}
	return 1;
}
#endif

#ifdef MAIN0a

#include "lib/keyboard.h"

int main(void){
	
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	Unified_Port_Init();
	
	keyboard_init();
	
	EnableInterrupts();
	
	while (1){
		char c = recieve_keyboard();
	}
	
	return -1;
}

#endif

#ifdef MAIN1
	
int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Hello World");
	
	EnableInterrupts();
	
	while(1) {
		WaitForInterrupt();
	}
}
	
#endif

#ifdef MAIN2

#include "lib/speaker.h"
#include "lib/mic.h"

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Hello World");
	tlv5618_init();
//	mic_init();
	tlv5618_test(2000);
	
	EnableInterrupts();
	
	while(1) {
		WaitForInterrupt();
	}
}
#endif


#ifdef MAIN3	// Test the transceiver in tx mode


#include "lib/transceiver.h"
#include "inc/Timer4A.h"

extern uint8_t CC1101Data;
extern uint8_t CC1101Status;
static uint8_t waitFlag = 1;

void timer4task();

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Testing transmission");
	Transceiver_init(1);
	Timer4A_Init(&timer4task, 50000, 5);
	uint8_t outNum = 0;
	uint8_t outNumsArray [ARRLEN];
	
	EnableInterrupts();
		
	while(1) {
		
//		ST7735_SetCursor(0, 0);
//		ST7735_OutString("Sending: ");
//		ST7735_OutUDec(outNum);
//		ST7735_OutString(" - ");
		for (int i = 0; i < ARRLEN; i++) {
			outNumsArray[i] = outNum;		
			outNum = (outNum + 1 ) % 256;
		}
		CC1101_Send_Bulk_Data(outNumsArray, ARRLEN);
//		ST7735_OutUDec(outNum == 0 ? 255 : outNum-1);
//		ST7735_OutString("  ");
		
		while (waitFlag) {
			WaitForInterrupt();
		}
		waitFlag = 1;
	}
}

void timer4task(){
	waitFlag = 0;
}

#endif

#ifdef MAIN4	// Test the transceiver in rx mode

#include "lib/transceiver.h"

extern uint8_t CC1101Data;
extern uint8_t CC1101Status;
extern uint8_t PD6_Rising;
extern uint8_t PD6_Falling;

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	Transceiver_init(0);
	uint8_t RX_Array[61];
	uint8_t length = 0;
	uint8_t line_num = 0;
	
	EnableInterrupts();
	
	while(1) {
		if (PD6_Rising) {	// if it's receiving
			WaitForInterrupt();
			while (!PD6_Falling) {}	// wait for it to finish receiving	
			length = CC1101_Receive_Bulk_Data(RX_Array);
//			ST7735_SetCursor(0, line_num);
//			ST7735_OutString("Got: ");
//			ST7735_OutUDec(RX_Array[0]);
//			ST7735_OutString(" - ");
//			ST7735_OutUDec(RX_Array[length-1]);
//			ST7735_OutString("  ");
//			line_num = (line_num + 1) % 15;
		}
		switchRXmode();
		WaitForInterrupt();
	}
}
#endif

#ifdef MAIN5	// Test half-duplex controlled by btn2

#define RX_MODE 0
#define TX_MODE 1

#include "lib/transceiver.h"
#include "lib/buttons.h"
#include "inc/Timer4A.h"

extern uint8_t CC1101Data;
extern uint8_t CC1101Status;
extern uint8_t PD6_Rising;
extern uint8_t PD6_Falling;
static uint8_t waitFlag = 1;

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	Transceiver_init(0);
	uint8_t outNum = 0;
	uint8_t outNumsArray [ARRLEN];
	uint8_t RX_Array[61];
	uint8_t length = 0;
	uint8_t line_num = 0;
	uint8_t mode = TX_MODE;
//	Port_F_Init();
	sw1_sw2_debounced_init(2);
	
	
	EnableInterrupts();
	
	while(1) {		
		if (!(GPIO_PORTB_DATA_R&0x2)) {	// transmit mode and send chars
			if (mode == RX_MODE) {
				ST7735_SetCursor(0, 0);
				ST7735_OutString("   TX MODE   ");
				mode = TX_MODE;
			}
			for (int i = 0; i < ARRLEN; i++) {
				outNumsArray[i] = outNum;		
				outNum = (outNum + 1 ) % 256;
			}
			DisableInterrupts();
			CC1101_Send_Bulk_Data(outNumsArray, ARRLEN);	// needs to be atomic
			EnableInterrupts();
		} else {		// receive mode and receive chars
			if (mode == TX_MODE) {
				ST7735_SetCursor(0, 0);
				ST7735_OutString("   RX MODE   ");
				mode = RX_MODE;
			}
			if (PD6_Rising) {	// if it's receiving
				WaitForInterrupt();
				while (!PD6_Falling) {}	// wait for it to finish receiving	
				length = CC1101_Receive_Bulk_Data(RX_Array);
			}	
			switchRXmode();
			WaitForInterrupt();		
		}
	}
}

#endif

#ifdef MAIN6	// Test half-duplex with sound controlled by btn2

#define RX_MODE 0
#define TX_MODE 1

#include "lib/transceiver.h"
#include "lib/buttons.h"
#include "inc/Timer2A.h"
#include "inc/Timer4A.h"
#include "lib/mic.h"
#include "lib/speaker.h"


extern uint8_t CC1101Data;
extern uint8_t CC1101Status;
extern uint8_t PD6_Rising;
extern uint8_t PD6_Falling;
extern uint16_t MicOutputArray[];
extern uint8_t MicOutputFull;
static uint8_t waitFlag = 1;
uint8_t RX_Array[NUM_QUEUE_ELEMENTS];
uint8_t length = 0;

void playSoundArray();

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	Port_F_Init();
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	Transceiver_init(0);
	sw1_sw2_debounced_init(2);
	mic_init();
	tlv5618_init();
	uint8_t mode = TX_MODE;
	Timer4A_Init(&playSoundArray, 10000, 5);
	Timer4A_Stop();
	
	EnableInterrupts();
	
	while(1) {		
		if (!(GPIO_PORTB_DATA_R&0x2)) {	// transmit mode and send chars
			if (mode == RX_MODE) {
				ST7735_SetCursor(0, 0);
				ST7735_OutStringH("   TX MODE   ");
				mode = TX_MODE;
			}
			if (MicOutputFull) {
				MicOutputFull = 0;
				uint8_t copyMicArray [NUM_QUEUE_ELEMENTS];
				for (int i = 0; i < NUM_QUEUE_ELEMENTS; i++){
					copyMicArray[i] = (uint8_t) (MicOutputArray[i]>>4);
				}
				Timer2A_Start();
				CC1101_Send_Bulk_Data(copyMicArray, NUM_QUEUE_ELEMENTS);
			}
		} else {		// receive mode and receive chars
			if (mode == TX_MODE) {
				ST7735_SetCursor(0, 0);
				ST7735_OutStringH("   RX MODE   ");
				mode = RX_MODE;
			}
			if (PD6_Rising) {	// if it's receiving
				WaitForInterrupt();
				while (!PD6_Falling) {}	// wait for it to finish receiving	
				length = CC1101_Receive_Bulk_Data(RX_Array);
				Timer4A_Start();
			}	
			switchRXmode();
//			WaitForInterrupt();		
		}
	}
}

static uint8_t RX_Array_i = 0;

void playSoundArray(){
	PF1 ^= 0x2;
	PF1 ^= 0x2;
	tlv5618_output(RX_Array[RX_Array_i]<<3);
	RX_Array_i++;
	if (RX_Array_i == length) {
		Timer4A_Stop();
		RX_Array_i = 0;
	}
	PF1 ^= 0x2;
}

#endif

#ifdef MAIN7	// Test half-duplex with sound controlled by btn2

#define RX_MODE 0
#define TX_MODE 1

#include "lib/transceiver.h"
#include "lib/buttons.h"
#include "inc/Timer2A.h"
#include "inc/Timer4A.h"
#include "lib/mic.h"
#include "lib/speaker.h"


extern uint8_t CC1101Data;
extern uint8_t CC1101Status;
extern uint8_t PD6_Rising;
extern uint8_t PD6_Falling;
extern uint16_t MicOutputArray[];
extern uint8_t MicOutputFull;
static uint8_t waitFlag = 1;
uint8_t RX_Array[61];
uint8_t length = 0;

void playSoundArray();

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	Port_F_Init();
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	Transceiver_init(0);
	sw1_sw2_debounced_init(2);
	mic_init();
	tlv5618_init();
	ST7735_SetCursor(0, 0);
	ST7735_OutString("TX MODE (w/ sound)");
	
	EnableInterrupts();
	
	while(1) {
		if (MicOutputFull) {
			MicOutputFull = 0;
			uint8_t copyMicArray [NUM_QUEUE_ELEMENTS];
			for (int i = 0; i < NUM_QUEUE_ELEMENTS; i++){
				copyMicArray[i] = MicOutputArray[i]>>4;
			}
			Timer2A_Start();
			CC1101_Send_Bulk_Data(copyMicArray, NUM_QUEUE_ELEMENTS);
		}		
	}
}

#endif

#ifdef MAIN8

int main() {
	DisableInterrupts();
	
    PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	ST7735_OutStringH("Hello World A");
	ST7735_SetCursor(0, 1);
	ST7735_OutStringH("Hello World B");
	
	EnableInterrupts();
	
	while(1) {
		WaitForInterrupt();
	}
}

#endif
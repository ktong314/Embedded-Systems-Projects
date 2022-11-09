/**
 * @file Lab9.c
 * @author Allen Viljoen (allenviljoen@utexas.edu), Yuqi Pan (), Som Wakidar (), Kevin Tong () 
	Jared McArthur TTH 11am
 * @brief
 *    Possible main programs to run Lab 9. Feel free to edit this to match your
 *    specifications.
 * 
 *    For this lab, the students must implement the following communication pipeline:
 *     - Input -> Encoder -> DAC -> Speaker -> AIR GAP -> Microphone -> ADC ->
 *       Decoder -> Display.
 *     Teams are allowed to choose an encoding scheme to transmit their
 *     information over the speaker. They need to be able to discuss and profile
 *     the execution latency, bandwidth, and SNR of their implementation.
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 */

/**
 * @note Modify __MAIN__ on L41 to determine which main method is executed.
 *   __MAIN__ = 0 or ENCODER - run the encoder main
 *              1 or DECODER - run the decoder main
 */
///* Register definitions. */
//#include "./inc/tm4c123gh6pm.h"
///* Clocking. */
//#include "./inc/PLL.h"
///* Clock delay and interrupt control. */
//#include "./inc/CortexM.h"
///* Initialization of all the pins. */
//#include "./inc/Unified_Port_Init.h"
///* Talking to PC via UART. */
//#include "./inc/UART.h"
///* ST7735 display. */
//#include "./inc/ST7735.h"


#define __MAIN10	// change the 'x' in __MAINx to select a test function below:
/*
__MAIN1:	
__MAIN2:	
__MAIN3:
__MAIN4: Tests ADC and Decoder
__MAIN5: Test TX UART
__MAIN6: Test the DAC
__MAIN7: 
__MAIN8: Test the DAC with TX UART
__MAIN9: TX mode with PUTTY
__MAIN10: RX mode
__MAIN11: test SNR of speaker at different frequencies
*/

#include <stdio.h>
#include <stdint.h>
#include <math.h>

/* Register definitions. */
#include "./inc/tm4c123gh6pm.h"
/* Clocking. */
#include "./inc/PLL.h"
/* Clock delay and interrupt control. */
#include "./inc/CortexM.h"
/* Initialization of all the pins. */
#include "./inc/Unified_Port_Init.h"

void EnableInterrupts();
void DisableInterrupts();

#ifdef __MAIN1
	/**
	main1 for testing adc and decoder
	*/
	#include "lib/decoder/display/display.h"
	#include "inc/UART.h"
	#include "inc/ST7735.h"

    int main(void) {
        /* TODO: Set up the required initializations here. */

		PLL_Init(Bus80MHz);
//		Unified_Port_Init(); 
		ST7735_InitR(INITR_REDTAB);
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_SetCursor(0, 0);
		UART_Init();
		UART_OutString("starts here ");
		UART_OutString("\n");
		
        while (1) {
            /* TODO: Decoder main loop. */
			
			input_and_display();
        }
    }
	
#endif
	
#ifdef __MAIN2
    /**
     * @brief Runs the encoder. Flash this code to one system.
     */
	 /* you can include/uninclude encoder files as you see fit here */
    #include "lib/encoder/switches/switches.h"
    #include "lib/encoder/tlv5616/tlv5616.h"
    int main2(void) {
        /* TODO: Set up the required initializations here. */

        while (1) {
            /* TODO: Encoder main loop. */
        }
    }
#endif
	
#ifdef __MAIN3
    /**
     * @brief Runs the decoder. Flash this code to a different system
     * than the encoder's system.
     */
	 /* you can include/uninclude encoder files as you see fit here */
    #include "lib/decoder/adc/adc.h"
    #include "lib/decoder/display/display.h"
    #include "lib/fft/fft.h"
    int main(void) {
        /* TODO: Set up the required initializations here. */

        while (1) {
            /* TODO: Decoder main loop. */
        }
    }
#endif
	
#ifdef __MAIN4
	#include "lib/mic.h"
	int main(void){
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		
		mic_init();
		EnableInterrupts();
		
		while(1){
			
			mic_start_sample();
			
		}
	}
#endif


#ifdef __MAIN5
	#include "lib/encoder/TX_UART.h"
	int main() {
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		TX_UART_init(8000000);
		EnableInterrupts();
		char letters [] = "hello world!";
		int delay = 8000000;
		while (delay) {
			delay--;
		}
		while (1) {
			int i = 0;
			while (letters[i]) {
				while (!TX_UART_send(letters[i])) {}	// blocking write
				i++;
			}
		}		
	}	
#endif

#ifdef __MAIN6
	#include "lib/encoder/DAC.h"
	#include "inc/Timer1A.h"
	int main(void){
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		Unified_Port_Init();
		DAC_Init(2000);
		Timer1A_Init(&waveOutput, 90909, 5);
		EnableInterrupts();
		while(1){
			WaitForInterrupt();
		}
	}
#endif


#ifdef __MAIN7
	#include "lib/mic.h"
	#include "inc/Dump.h"
	
	int main(void){
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		DumpInit();
		
		mic_init();
		EnableInterrupts();
		
		while(1){
			
			mic_start_sample();
		}
	}
#endif

#ifdef __MAIN8
    #include "lib/encoder/DAC.h"
    #include "inc/Timer1A.h"
    #include "lib/encoder/TX_UART.h"
    int main() {
        DisableInterrupts();
        PLL_Init(Bus80MHz);
        //Unified_Port_Init();
        Port_F_Init();
        TX_UART_init(8000000); // 10 bps baud
        DAC_Init(2000, 90909, 5);    // initializes both the DAC and Timer1 interrupt
        EnableInterrupts();
        char letters [] = "hello world!";
        int delay = 8000000;
        while (delay) {
            delay--;
        }
        while (1) {
            int i = 0;
            while (letters[i]) {
                while (!TX_UART_send(letters[i])) {}    // blocking write
                i++;
            }
        }
    }
#endif


#ifdef __MAIN9
	#include "lib/encoder/DAC.h"
	#include "inc/Timer1A.h"
	#include "lib/encoder/TX_UART.h"
	#include "inc/UART.h"
	#include "inc/ST7735.h"
	#include "lib/encoder/switches/switches.h"
	
	
	int main() {
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		//Unified_Port_Init();
		Port_F_Init();
		ST7735_InitR(INITR_REDTAB);
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_SetCursor(0, 0);
		UART_Init();
		TX_UART_init(8000000); // 10 bps baud
		DAC_Init(2000, 90909, 5);	// initializes both the DAC and Timer1 interrupt
		//EnableInterrupts();
		//char letters [] = "hello world!";
		cursor_init();
		int delay = 8000000;
		while (delay) {
			delay--;
		}
		while (1) {
			input_and_display();
			char* letters = get_input();
			EnableInterrupts();
			int i = 0;
			while (letters[i]) {
				while (!TX_UART_send(letters[i])) {}	// blocking write
				i++;
			}
		}		
	}
#endif
	
#ifdef __MAIN10	// receiver
	#include "inc/ST7735.h"
	#include "lib/decoder/RX_UART.h"
	#include "lib/mic.h"
	
	int main() {
		DisableInterrupts();
		PLL_Init(Bus80MHz);
		Port_F_Init();		
		ST7735_InitR(INITR_REDTAB);
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_SetCursor(0, 0);
		RX_UART_init(8000000);	// 10 bps baud
		mic_init();
		
		EnableInterrupts();	
		int num_outputs = 0;
		while (1) {
			char letter = 0;
			my_decoder_fourier();
			if(!RX_fifo_empty()){
				letter = RX_fifo_pop();
			}
			if (letter) {
				ST7735_OutChar(letter);
				num_outputs++;
				letter = 0;
				if (!(num_outputs%15)) {					
					ST7735_SetCursor(0, num_outputs/15);
					num_outputs %= (15*20);
				}
			}
			
		}
	}
	
#endif
	
#ifdef __MAIN11	// constant test freq
    #include "lib/encoder/DAC.h"
    #include "inc/Timer1A.h"
    int main() {
        DisableInterrupts();
        PLL_Init(Bus80MHz);
        //Unified_Port_Init();
        Port_F_Init();
        DAC_Init(2000, 80000, 1);    // initializes both the DAC and Timer1 interrupt at 80000 for 1000Hz or 213333 for 375Hz
        EnableInterrupts();
        
        while (1) {
            
        }
    }
#endif
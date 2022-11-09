//this is the c file that contains code for the microphone


#define PF1       (*((volatile uint32_t *)0x40025008))    // RED LED
#define PF2       (*((volatile uint32_t *)0x40025010))    // BLUE LED
#define PF3       (*((volatile uint32_t *)0x40025020))    // GREEN LED
#define HIGH_PER 213333	// 375 Hz
#define LOW_PER 80000	// 1000 Hz
#define WAVEFORE_NUM_ELEMENTS 64

#include "mic.h"
#include "./lib/encoder/encoder.h"
#include "../lib/decoder/fft/fft.h"
#include "/inc/Dump.h"
#include "lib/decoder/RX_UART.h"
#include "/inc/Timer5A.h"



static uint32_t waveform_array[WAVEFORE_NUM_ELEMENTS];
static uint32_t fourier_transform[WAVEFORE_NUM_ELEMENTS];
//static int decoded_buffer[100];

static int waveform_counter;
static uint8_t array_full;
//static int buffer_counter;

uint16_t decoded_freq;
uint16_t previous_decode;
uint16_t message_start;

void mic_init(void){
	ADC0_InitSWTriggerSeq3(5);
	// 
	//DumpInit();
	waveform_counter = 0;
	array_full = 0;
	previous_decode = 1;
	message_start = 0;
	array_full = 0;
	waveform_counter = 0;
	Timer3A_Init(&sample_sound, LOW_PER / 8, 1);
}


void sample_sound(void){
	uint32_t sample = ADC0_InSeq3();
	
	if(waveform_counter == WAVEFORE_NUM_ELEMENTS){
		array_full = 1;
		return;
	}
	waveform_array[waveform_counter] = sample & 0xFFFF;
	waveform_counter++;	
}


void my_decoder_fourier(void){
	if (!array_full){
		return;
	}
	uint32_t magHigh = 0;
//	uint32_t magLow = 0;
	int loudest_i = 0;
	int16_t real;
	int16_t imag;
	
	fft_formatted_64(fourier_transform, waveform_array);
	//index = (N*frequency)/samplefreq
	//index = (64*1000)/8000 = 8
	//bottom 16 bits are real and top 16 bits are imag
	//magnitude = sqrt(real*real+imag*imag)
	for(int i = 1; i < 9; i++){ //check index 7 and 8 of the fft array
		real = (int16_t) fourier_transform[i]&0xFFFF; // bottom 16 bits
		imag = (int16_t) fourier_transform[i]>>16; // top 16 bits
		uint32_t currMagHigh = sqrt(real*real+imag*imag);
		if(currMagHigh > magHigh){
			magHigh = currMagHigh;
			loudest_i = i;
		}
	}
	
	if(loudest_i == 8)
	{ 
		PF3 = 0x8;
		PF2 = 0;
		decoded_freq = 0; //if frequency was 1
		if(previous_decode == 1 && !RX_UART_isReceiving())
		{ //check to see if the previous bit is 1
			begin_convert();
		}
	} else 
	{
		PF3 = 0;
		PF2 = 0x4;
		decoded_freq = 1; //if frequency was 1000
	}
	previous_decode = decoded_freq;
	array_full = 0;
	waveform_counter = 0;	
	
	//DumpCapture(decoded_freq);
}


	


//this is the header file for mic.c
#pragma once

#include <stdint.h>
#include <math.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ADCSWTrigger.h"
#include "../inc/Timer3A.h"


extern uint16_t decoded_freq;
extern uint16_t message_start;

void mic_init(void);

uint16_t sound_in(void);

void my_decoder(void);

void my_decoder_fourier(void);

void sample_sound(void);

void send_samples(void);

void mic_start_sample(void);
	
	
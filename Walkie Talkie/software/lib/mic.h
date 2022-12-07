//this is the header file for mic.c

#pragma once

#include <stdint.h>
#define NUM_QUEUE_ELEMENTS 16
#define MIC_PERIOD 10000

void mic_init(void);

uint16_t sound_in(void);

void decoder(uint16_t data);

void sample_sound(void);

void add_sound_to_array();

void send_samples(void);

uint32_t get_queue(void);

void play_sound(void);
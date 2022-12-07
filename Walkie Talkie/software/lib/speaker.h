//this is the header file for speaker.c
#pragma once

#include <stdint.h>


static int index = 0;
void tlv5618_init();
int tlv5618_output(uint16_t data);
void tlv5618_test(int period);
void sine_output();

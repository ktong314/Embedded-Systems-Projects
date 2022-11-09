//this is the header file for encoder.c

#include <stdint.h>
#include "/inc/tm4c123gh6pm.h"
#include "inc/Timer1A.h"
#include "lib/fifo.h"

#define PERIOD300 133333 //square wave for 300Hz
#define PERIOD1000 40000 //square wave for 1000Hz

uint32_t encoding_period = 80000000; //this relates to the baud rate
uint32_t dac_period = PERIOD1000;
int32_t enc_count; //counter for 11 bits
void encoder_init(void);
void encoder(void);

//this is the c file that contains code for the microphone

#include "mic.h"
#include "speaker.h"
#include "/inc/tm4c123gh6pm.h"
#include "/inc/ADCSWTrigger.h"
#include "/inc/Timer2A.h"
#include "inc/Unified_Port_Init.h"

static uint32_t samples[NUM_QUEUE_ELEMENTS];
static uint8_t queue_head, queue_tail;
uint16_t MicOutputArray[NUM_QUEUE_ELEMENTS];
uint8_t MicOutputFull = 0;
static uint8_t MicOutputArrayInd = 0;

uint32_t get_queue(void){
	if (queue_head == queue_tail){
		return -1; //failed
	}
	uint32_t data = samples[queue_tail];
	queue_tail = (queue_tail + 1) % NUM_QUEUE_ELEMENTS;
	return data;
}


void mic_init(void){
	ADC0_InitSWTriggerSeq3(1);
//	Timer2A_Init(&sample_sound, MIC_PERIOD, 1);
	Timer2A_Init(&add_sound_to_array, MIC_PERIOD, 3);
	
}

void add_sound_to_array() {
	PF2 ^= 0x4;
	PF2 ^= 0x4;
	MicOutputArray[MicOutputArrayInd] = ADC0_InSeq3();
	MicOutputArrayInd++;
	if (MicOutputArrayInd == NUM_QUEUE_ELEMENTS) {
		MicOutputArrayInd = 0;
		MicOutputFull = 1;
		Timer2A_Stop();
	}
	PF2 ^= 0x4;
}

void sample_sound(void){
	
	uint32_t sample = ADC0_InSeq3();
	
	if (queue_head == (queue_tail + NUM_QUEUE_ELEMENTS - 1) % NUM_QUEUE_ELEMENTS) { //return if fifo is full
        return;
  }
	
	samples[queue_head] = sample;
	queue_head = (queue_head + 1) % NUM_QUEUE_ELEMENTS;

}



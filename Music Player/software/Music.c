#include <stdint.h>
#include "Music.h"
#include "/inc/tm4c123gh6pm.h"
#include "/inc/Timer0A.h"
#include "/inc/Timer1A.h"
#include "/inc/Timer2A.h"
#include "./inc/Unified_Port_Init.h"
#include "DAC.h"
#include "Switch.h"

#define TWINKLESIZE 84
#define RICKSIZE 50
uint16_t sinwave[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
}; 
uint16_t nothing[64] = {  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}; 


note twinkle_twinkle[TWINKLESIZE] = {
{C5, 7, sinwave}, {0, 1, nothing},
{C5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{A5, 7, sinwave}, {0, 1, nothing},
{A5, 7, sinwave}, {0, 1, nothing},
{G5, 13, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{D5, 7, sinwave}, {0, 1, nothing},
{D5, 7, sinwave}, {0, 1, nothing},
{C5, 13, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{D5, 13, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{D5, 13, sinwave}, {0, 1, nothing},
{C5, 7, sinwave}, {0, 1, nothing},
{C5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{A5, 7, sinwave}, {0, 1, nothing},
{A5, 7, sinwave}, {0, 1, nothing},
{G5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{F5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{E5, 7, sinwave}, {0, 1, nothing},
{D5, 7, sinwave}, {0, 1, nothing},
{D5, 7, sinwave}, {0, 1, nothing},
{C5, 13, sinwave}, {0, 1, nothing}
};


note rick_roll[RICKSIZE] = {
	{C5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{F5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{A5, 13, sinwave}, {0, 1, nothing},
	{A5, 7, sinwave}, {0, 1, nothing},
	{G5, 23, sinwave}, {0, 1, nothing},
	{C5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{F5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{G5, 13, sinwave}, {0, 1, nothing},
	{G5, 7, sinwave}, {0, 1, nothing},
	{F5, 23, sinwave}, {0, 1, nothing},
	{C5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{F5, 7, sinwave}, {0, 1, nothing},
	{D5, 7, sinwave}, {0, 1, nothing},
	{F5, 13, sinwave}, {0, 1, nothing},
	{G5, 7, sinwave}, {0, 1, nothing},
	{E5, 13, sinwave}, {0, 1, nothing},
	{D5, 13, sinwave}, {0, 1, nothing},
	{C5, 7, sinwave}, {0, 1, nothing},
	{G5, 13, sinwave}, {0, 1, nothing},
	{F5, 23, sinwave}, {0, 1, nothing}
};

const song Twinkle = {twinkle_twinkle, TWINKLESIZE, 8000000};
const song Rick = {rick_roll, RICKSIZE, 2500000};
song chosen;


int note_counter;
uint32_t beat_counter;
int envelope=25;
uint8_t wave_step1;
uint8_t wave_step = 0;

void playSongInit(uint32_t songIndex){
	song chosen_song;
	if(songIndex == 1){
		chosen = Twinkle;
	}
	else if(songIndex == 2){
		chosen = Rick;
	}
	DAC_Init(0);
	Timer0A_Init(&note_play, chosen.tempo, 3); //interrupt every second, plays a note every second
	note_counter = 0;
	beat_counter = 0;
}


void note_play(void){
	PF2 ^= -1;
	if(pause_flag){
		TIMER1_CTL_R = 0x00000000;
		PF2 ^= -1;
		return;
	}
	else if(stop_flag){
		note_counter = 0;
		beat_counter = 0;
	}
	else if(rewind_flag){
		Timer1A_Init(&sound_play, 80000000/(64*chosen.notes[note_counter].freq), 4);
		Timer2A_Init(&melody, 4000, 5);
		beat_counter++;
		envelope--;
		envelope--;
		if(beat_counter == chosen.notes[note_counter].duration){
			beat_counter = 0;
			envelope=25;
			note_counter--;
			if(note_counter < 0){
				note_counter = chosen.songSize - 1;
			}
		}
	}
	else{
		Timer1A_Init(&sound_play, 80000000/(64*chosen.notes[note_counter].freq), 4);
		Timer2A_Init(&melody, 4000, 5);
		beat_counter++;
		envelope--;
		envelope--;
		if(beat_counter == chosen.notes[note_counter].duration){
			beat_counter = 0;
			envelope=25;
			note_counter = (note_counter + 1) % chosen.songSize;
		}
	}
	PF2^= -1;
}

void do_nothing(void){
};

void melody(void) {
	wave_step1 = (wave_step1+1)%64;
}

void sound_play(void){
	PF1 ^= -1;
	DAC_Out(envelope*(chosen.notes[note_counter].wave[wave_step]+chosen.notes[note_counter].wave[wave_step1])/25);
	wave_step = (wave_step+1)%64; 
	PF1 ^= -1;
}
#include <stdbool.h>
#include <stdint.h>



#define G2 98
#define C3 131
#define D3 147
#define E3 165
#define F3 175
#define G3 196
#define A3 220
#define B3 247
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440 //2800
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 988
#define C6 1047
#define D6 1175
#define E6 1319
#define F6 1397
#define G6 1568
#define A6 1760
#define B6 1976
#define C7 2093




typedef struct note{
	uint32_t freq;
	uint32_t duration;
	uint16_t *wave;
}note;

typedef struct song{
	note *notes;
	uint32_t songSize;
	uint32_t tempo;
}song;

void playSongInit(uint32_t songIndex);
void sound_play(void);
void note_play(void);
void do_nothing(void);
void melody(void);

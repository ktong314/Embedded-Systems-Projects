#include <stdbool.h>
#include <stdint.h>

#define C2 523
#define B 494
#define Bflat 466
#define A 440
#define Aflat 415
#define G 392
#define Gflat 370
#define F 349
#define E 330
#define Eflat 311
#define D 294
#define Dflat 277
#define C1 262


typedef struct note{
	uint32_t freq;
	uint32_t duration;
	uint32_t *wave;
}note;

typedef struct song{
	uint32_t melody_index;
	uint32_t harmony_index;
	note *melody;
	note *harmony;
}song;


#include <stdint.h>
#include "Music.h"
#include "/inc/tm4c123gh6pm.h"

uint32_t sinwave[] = {1,2,3,4,5};


note random_song_melody[] =  {
{C1, 4, sinwave},
{D, 4, sinwave},
{E, 4, sinwave},
{F, 4, sinwave},
{G, 4, sinwave},
{A, 4, sinwave},
{B, 4, sinwave},
{C2, 4, sinwave}
};

note random_song_harmony[] =  {
{C1, 4, sinwave},
{D, 4, sinwave},
{E, 4, sinwave},
{F, 4, sinwave},
{G, 4, sinwave},
{A, 4, sinwave},
{B, 4, sinwave},
{C2, 4, sinwave}
};

const song random_song = {0, 0, random_song_melody, random_song_harmony};
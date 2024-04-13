//header only file for help with composing music
#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <stdint.h>

//the really cool thing is these are all you really need to make any 12-tone note
//since multiplying frequency by 2 raises a note by an octave, dividing the period by 2 achieves the same effect
//bit shifting is the same as multiplying/dividing by powers of 2
//if I want high C, I can write C4 >> 1, which easily reads as raising by 1 octave
//the same can  be achieves for C3 = C4 << 1, which is lowering by 1 octave
#define G4   2551050
#define Fsh4 2702744
#define Gfl4 Fsh4
#define F4   2863457
#define E4   3033726
#define Efl4 3214121
#define Dsh4 Efl4
#define D4   3405243
#define Csh4 3607730
#define Dfl4 Csh4
#define C4   3822256
#define B3   4049538
#define Bfl3 4290337
#define Ash3 Bfl3
#define A3   4504545
#define Gsh3 4815742
#define Afl3 Gsh3

#define MS_PER_SECOND 1000
#define SECOND_PER_MINUTE 60

#define MEASURE_LEN 1440U //a single 4/4 measure is 1440 units
#define BEAT_LEN (MEASURE_LEN / 4U)
#define MEASURE_TO_MS(beat_delay, bpm) ((beat_delay) * SECOND_PER_MINUTE * MS_PER_SECOND / BEAT_LEN / (bpm))

//standard 4/4 note lengths, NOTE_2 is a half note, NOTE_4 is a quarter note, etc.
#define NOTE_1 MEASURE_LEN
#define NOTE_2 (MEASURE_LEN / 2U)
#define NOTE_4 (MEASURE_LEN / 4U)
#define NOTE_8 (MEASURE_LEN / 8U)
#define NOTE_16 (MEASURE_LEN / 16U)
#define NOTE_32 (MEASURE_LEN / 32U)

struct Note {
    uint64_t period;
    uint16_t duration;
};

#endif
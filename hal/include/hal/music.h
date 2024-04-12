#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <stdint.h>


#define G4   2551050
#define F4   2863457
#define E4   3033726
#define D4   3405243
#define C4   3822256
#define B3   4049538
#define A3   4504545
#define Afl3 4815742

#define MS_PER_SECOND 1000
#define SECOND_PER_MINUTE 60

#define MAX_SOUNDS 3
#define MEASURE_LEN 1440U //a single 4/4 measure is 1440 units
#define BEAT_LEN (MEASURE_LEN / 4U)
#define MEASURE_TO_MS(beat_delay, bpm) ((beat_delay) * SECOND_PER_MINUTE * MS_PER_SECOND / BEAT_LEN / (bpm))

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
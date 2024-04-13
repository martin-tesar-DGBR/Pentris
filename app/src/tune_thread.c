#include "hal/buzzer.h"

#include <stdatomic.h>
#include <pthread.h>

#include "music.h"
#include "hal/util.h"

//https://musescore.com/user/28837378/scores/5144713
struct Note melody[] = {
    {E4, NOTE_4}, {B3, NOTE_8}, {C4, NOTE_8}, {D4, NOTE_4}, {C4, NOTE_8}, {B3, NOTE_8},
    {A3, NOTE_4}, {A3, NOTE_8}, {C4, NOTE_8}, {E4, NOTE_4}, {D4, NOTE_8}, {C4, NOTE_8},
    {B3, NOTE_4 + NOTE_8}, {C4, NOTE_8}, {D4, NOTE_4}, {E4, NOTE_4},
    {C4, NOTE_4}, {A3, NOTE_4}, {A3, NOTE_4}, {0, NOTE_8},
    {0, NOTE_8}, {D4, NOTE_4}, {F4, NOTE_8}, {A3 >> 1, NOTE_4}, {G4, NOTE_8}, {F4, NOTE_8},
    {E4, NOTE_4 + NOTE_8}, {C4, NOTE_8}, {E4, NOTE_4}, {D4, NOTE_8}, {C4, NOTE_8},
    {B3, NOTE_4 + NOTE_8}, {C4, NOTE_8}, {D4, NOTE_4}, {E4, NOTE_4},
    {C4, NOTE_4}, {A3, NOTE_4}, {A3, NOTE_4}, {0, NOTE_4},
    {E4, NOTE_2}, {C4, NOTE_2},
    {D4, NOTE_2}, {B3, NOTE_2},
    {C4, NOTE_2}, {A3, NOTE_2},
    {Gsh3, NOTE_1 - NOTE_4}, {0, NOTE_4},
    {E4, NOTE_2}, {C4, NOTE_2},
    {D4, NOTE_2}, {B3, NOTE_2},
    {C4, NOTE_4}, {E4, NOTE_4}, {A3 >> 1, NOTE_4}, {A3 >> 1, NOTE_4},
    {Gsh3 >> 1, NOTE_1 - NOTE_4}, {0, NOTE_4}
};

#define NUM_NOTES (sizeof(melody) / sizeof(*melody))
#define BPM 180

static _Atomic int cont = 1;
static pthread_t tuneThread;

static void *tuneLoop(void *args){
    int curr_note = 0;
    buzzer_enable();
    while(cont == 1){
        struct Note note = melody[curr_note];
        buzzer_set_period(note.period >> 1); //raised an octave
        curr_note = (curr_note + 1) % NUM_NOTES;
        sleep_ms(MEASURE_TO_MS(note.duration, BPM));
    }

    return args;
}

void tune_cleanup(void){
    cont = 0;
    pthread_join(tuneThread, NULL);

    buzzer_disable();
}

void tune_init(void){
    pthread_create(&tuneThread, NULL, tuneLoop, NULL);
}
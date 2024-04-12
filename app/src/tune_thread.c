#include "buzzer.h"
#include <stdatomic.h>
#include <pthread.h>

char** periods = {"1000000", "0", "1000000", "0", "1250000", "1500000", "1250000", "1000000", 
                    "900000", "800000", "0", "1250000", "1000000", "2000000", "3000000", "4000000", 
                    "3000000", "2000000"};

char** cycles = {"500000", "0", "500000", "0", "625000", "750000", "625000", "500000", "450000", 
                "400000", "0", "625000", "500000", "1000000", "1500000", "2000000", "1500000", "1000000"};


static _Atomic int cont = 1;
static pthread_t tuneThread;

void *tuneLoop(void){
    BUZZER_H.enable();
    while(cont == 1){
        for(int i = 0; i < 18 && cont == 1; i++){
            BUZZER_H.setPeriod(periods[i]);
            BUZZER_H.setDutyCycle(cycles[i])
            BUZZER_H.sleepForMs(300);
        }
    }

    BUUZER_H.disable();
}

void endLoop(void){
    cont = 0;
}

void startThread(void){
    pthread_create(&tuneThread, NULL, &tuneLoop, NULL);
}
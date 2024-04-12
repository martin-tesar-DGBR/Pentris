#ifndef BUZZER_C
#define BUZZER_C

#include "hal/buzzer.h"
#include "hal/util.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

//provided by prof
/*
static void sleepForMs(long long delayInMs){
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}
*/

void setPeriod(char* newPeriod){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/period", "w");
    fprintf(buzzFile, newPeriod);
    fclose(buzzFile);
}

void setDutyCycle(char* newCycle){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
    fprintf(buzzFile, newCycle);
    fclose(buzzFile);

}

void enable(void){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/enable", "w");
    fprintf(buzzFile, "1");
    fclose(buzzFile);
}

void disable(void){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/enable", "w");
    fprintf(buzzFile, "0");
    fclose(buzzFile);
}

void buzzerInit(void){
    run_command("config-pin p9_22 pwm");
}


#endif
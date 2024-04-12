#ifndef BUZZER_C
#define BUZZER_C

#include "hal/buzzer.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "hal/util.h"

void buzzer_set_period(uint64_t period) {
    if (period == 0) {
        buzzer_disable();
        return;
    }
    
    FILE *period_file = fopen("/dev/bone/pwm/0/a/period", "w");
    FILE *dc_file = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
    fprintf(dc_file, "0");
    sleep_ms(5);
    fprintf(period_file, "%llu", period);
    fprintf(dc_file, "%llu", period / 2);
    buzzer_enable();
    fclose(period_file);
    fclose(dc_file);
}

void buzzer_enable(void){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/enable", "w");
    fprintf(buzzFile, "1");
    fclose(buzzFile);
}

void buzzer_disable(void){
    FILE *buzzFile = fopen("/dev/bone/pwm/0/a/enable", "w");
    fprintf(buzzFile, "0");
    fclose(buzzFile);
}

void buzzer_init(void){
    run_command("config-pin p9_22 pwm");
}


#endif
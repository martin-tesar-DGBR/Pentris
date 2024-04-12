#ifndef BUZZER_H
#define BUZZER_H
//provided by prof
static void sleepForMs(long long delayInMs);

void setPeriod(char* newPeriod);

void setDutyCycle(char* newCycle);

void enable(void);

void disable(void);

void buzzerInit(void);

#endif
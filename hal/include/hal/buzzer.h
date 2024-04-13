//interface for using buzzer
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void buzzer_init(void);

void buzzer_set_period(uint64_t period);
void buzzer_enable(void);
void buzzer_disable(void);

#endif
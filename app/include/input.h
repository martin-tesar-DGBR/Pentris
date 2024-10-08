//module for smooth input handling
#ifndef _INPUT_H_
#define _INPUT_H_

#include "pentris.h"

void input_init();

void input_update(enum PentrisInput input, long long delta_time, int is_pressed);

#endif
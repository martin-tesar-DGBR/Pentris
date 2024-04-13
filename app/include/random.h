//module for random number generation (to pay homage to NES Tetris)
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>

uint32_t LFSR(uint32_t state);

uint32_t shuffle(void *data, int data_size, int num_elements, uint32_t state);

#endif
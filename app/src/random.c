#include "random.h"

#include "hal/util.h"

//linear feedback shift register, used in the original NES version of Tetris
//(not the same implementation since the NES uses 16 bit state)
//https://www.ijera.com/papers/Vol4_issue6/Version%206/P0460699102.pdf
//LFSRs have a number of deficiencies [citation needed], but the most egregious one comes not
//from the math itself but a bug in the NES version where they don't get as many random states as they should
//We have chosen not to replicate the bug
uint32_t LFSR(uint32_t state) {
    uint32_t carry = ((state >> 31) ^ (state >> 30) ^ (state >> 10) ^ (state)) & 1;
    return (state >> 1) | (carry << 31);
}

//https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
//Fisher-Yates asserts that the output is shuffled uniformly if the RNG used is uniformly distributed
//LFSRs are not uniformly distributed
uint32_t shuffle(void *data, int data_size, int num_elements, uint32_t state) {
    for (int i = num_elements - 1; i > 0; i--) {
        state = LFSR(state);
        int j = state % (i + 1);
        swap((uint8_t *) data + data_size * i, (uint8_t *) data + data_size * j, data_size);
    }

    return state;
}
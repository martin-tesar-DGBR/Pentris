#ifndef _DISPLAY_LINUX_H_
#define _DISPLAY_LINUX_H_

#include <stdint.h>

//returns back buffer to draw on
volatile uint8_t *display_init();
void display_cleanup();

volatile uint8_t *display_buffer_swap();

#endif
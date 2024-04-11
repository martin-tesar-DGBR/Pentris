#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <stdint.h>

void renderer_draw(volatile uint8_t *buffer);
void renderer_draw_gameover(volatile uint8_t *buffer, int ms_elapsed);

#endif
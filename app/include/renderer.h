//module for rendering the game to the screen
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <stdint.h>

//it may be possible to combine these two functions into one using a struct
//argument type thing but it seems overkill for this application
void renderer_draw(volatile uint8_t *buffer);
void renderer_draw_gameover(volatile uint8_t *buffer, int ms_elapsed);

#endif
//interface for analog joystick
#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

void joystick_init();
void joystick_cleanup();

//returns 0 for center, -1 for one direction and 1 for the other
//+x axis is right, +y axis is down
void joystick_read(int *x, int *y);

#endif
//interface for reading button inputs
#ifndef _BUTTON_H_
#define _BUTTON_H_

//depends on gpio_init() to be called
void button_init();

void button_read(int *button1, int *button2);

#endif
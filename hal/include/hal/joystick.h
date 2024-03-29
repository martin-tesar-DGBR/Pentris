/*
interface for raw joystick input
*/

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#define JSUP_PIN "/sys/class/gpio/gpio26/"
#define JSRT_PIN "/sys/class/gpio/gpio47/"
#define JSDN_PIN "/sys/class/gpio/gpio46/"
#define JSLFT_PIN "/sys/class/gpio/gpio65/"
#define JSPB_PIN "/sys/class/gpio/gpio27/"

typedef enum JoystickInput {
    NO_DIRECTION = 0x00,
    JS_UP = 0x01,
    JS_RIGHT = 0x02,
    JS_DOWN = 0x04,
    JS_LEFT = 0x08,
    JS_PUSHED = 0x10
} JoystickInput;

void joystick_init();
JoystickInput joystick_get_input();
void joystick_cleanup();

#endif
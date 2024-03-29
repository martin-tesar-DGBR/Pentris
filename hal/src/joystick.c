#include "hal/joystick.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "hal/util.h"

static void joystick_write() {
    file_write(JSUP_PIN "direction", "in");
    file_write(JSRT_PIN "direction", "in");
    file_write(JSDN_PIN "direction", "in");
    file_write(JSLFT_PIN "direction", "in");
    file_write(JSPB_PIN "direction", "in");

    file_write(JSUP_PIN "active_low", "0");
    file_write(JSRT_PIN "active_low", "0");
    file_write(JSDN_PIN "active_low", "0");
    file_write(JSLFT_PIN "active_low", "0");
    file_write(JSPB_PIN "active_low", "0");
}

void joystick_init() {
    //setting joystick pins to GPIO using config-pin
    run_command("config-pin p8_14 gpio");
    run_command("config-pin p8_15 gpio");
    run_command("config-pin p8_16 gpio");
    run_command("config-pin p8_18 gpio");
    run_command("config-pin p8_17 gpio");

    joystick_write();
    sleep_ms(300);
}

JoystickInput joystick_get_input() {
    JoystickInput acc = NO_DIRECTION;

    const int MAX_LENGTH = 16;
    char buff[MAX_LENGTH];
    file_read(JSUP_PIN "value", buff, MAX_LENGTH);
    if (strncmp(buff, "0", 1) == 0) {
        acc |= JS_UP;
    }

    file_read(JSRT_PIN "value", buff, MAX_LENGTH);
    if (strncmp(buff, "0", 1) == 0) {
        acc |= JS_RIGHT;
    }

    file_read(JSDN_PIN "value", buff, MAX_LENGTH);
    if (strncmp(buff, "0", 1) == 0) {
        acc |= JS_DOWN;
    }

    file_read(JSLFT_PIN "value", buff, MAX_LENGTH);
    if (strncmp(buff, "0", 1) == 0) {
        acc |= JS_LEFT;
    }

    file_read(JSPB_PIN "value", buff, MAX_LENGTH);
    if (strncmp(buff, "0", 1) == 0) {
        acc |= JS_PUSHED;
    }

    return acc;
}

void joystick_cleanup() {

}
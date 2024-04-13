//interface for interacting with GPIO pins
#ifndef _GPIO_H_
#define _GPIO_H_

//pins found using command "gpioinfo | less"
//the reason it looks wack is because it is wack
//it's like the manufacturer randomly assigned the numbers to their corresponding pin

enum GPIOPin {
    P9_22 = 2,
    P9_21 = 3,
    P9_18 = 4,
    P9_17 = 5,
    P9_42 = 7,
    P8_35 = 8,
    P8_33 = 9,
    P8_31 = 10,
    P8_32 = 11,
    P9_20 = 12,
    P9_19 = 13,
    P9_26 = 14,
    P9_24 = 15,
    P9_41 = 20,
    P8_19 = 22,
    P8_13 = 23,
    P8_14 = 26,
    P8_17 = 27,
    P9_11 = 30,
    P9_13 = 31,

    P8_25 = 32,
    P8_24 = 33,
    P8_05 = 34,
    P8_06 = 35,
    P8_23 = 36,
    P8_22 = 37,
    P8_03 = 38,
    P8_04 = 39,
    P8_12 = 44,
    P8_11 = 45,
    P8_16 = 46,
    P8_15 = 47,
    P9_15 = 48,
    P9_23 = 49,
    P9_14 = 50,
    P9_16 = 51,
    BBG_GREEN_USR0 = 53,
    BBG_GREEN_USR1 = 54,
    BBG_GREEN_USR2 = 55,
    BBG_GREEN_USR3 = 56,
    P9_12 = 60,
    P8_26 = 61,
    P8_21 = 62,
    P8_20 = 63,

    P8_18 = 65,
    P8_07 = 66,
    P8_08 = 67,
    P8_10 = 68,
    P8_09 = 69,
    P8_45 = 70,
    P8_46 = 71,
    P8_43 = 72,
    P8_44 = 73,
    P8_41 = 74,
    P8_42 = 75,
    P8_39 = 76,
    P8_40 = 77,
    P8_37 = 78,
    P8_38 = 79,
    P8_36 = 80,
    P8_34 = 81,
    P8_27 = 86,
    P8_29 = 87,
    P8_28 = 88,
    P8_30 = 89,

    P9_31 = 110,
    P9_29 = 111,
    P9_30 = 112,
    P9_28 = 113,
    P9_92 = 114,
    P9_27 = 115,
    P9_91 = 116,
    P9_25 = 117
};

enum GPIODirection {
    GPIO_in,
    GPIO_out
};

//the caller must set up the pins themselves like so:
//run_command("config-pin <pin name> gpio");
void gpio_init();
void gpio_cleanup();

#define gpio_config_pin(pin) run_command("config-pin " #pin " gpio")

void gpio_set_direction(enum GPIOPin pin, enum GPIODirection direction);
int gpio_read(enum GPIOPin pin);
void gpio_write(enum GPIOPin pin, int value);

#endif
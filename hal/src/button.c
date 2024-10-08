#include "hal/button.h"

#include <stdlib.h>

#include "hal/gpio.h"
#include "hal/util.h"

void button_init() {
    gpio_config_pin(P9_15);
    gpio_config_pin(P9_16);
    gpio_config_pin(P9_42);

    gpio_set_direction(P9_42, GPIO_out);
    gpio_set_direction(P9_15, GPIO_in);
    gpio_set_direction(P9_16, GPIO_in);
}

void button_read(int *button1, int *button2) {
    gpio_write(P9_42, 1);
    if (button1 != NULL) {
        *button1 = gpio_read(P9_15);
    }
    if (button2 != NULL) {
        *button2 = gpio_read(P9_16);
    }
    gpio_write(P9_42, 0);
}
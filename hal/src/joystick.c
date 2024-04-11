#include "hal/joystick.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

#include "hal/util.h"

//memory mapping ADC is significantly more complicated so we stick with this
#define A2D_FILE_VOLTAGE5 "/sys/bus/iio/devices/iio:device0/in_voltage5_raw"
#define A2D_FILE_VOLTAGE6 "/sys/bus/iio/devices/iio:device0/in_voltage6_raw"

#define JOYSTICK_CENTER_VALUE 1624
#define JOYSTICK_THRESHOLD 600

static atomic_int joystick_thread_running;
static pthread_t joystick_thread;
static atomic_int joystick_x = 0;
static atomic_int joystick_y = 0;

static FILE *js_x_fd;
static FILE *js_y_fd;

static void *joystick_thread_fn(void *args) {
    #define BUFFER_SIZE 32
    char buffer[BUFFER_SIZE];
    while (joystick_thread_running) {
        fseek(js_x_fd, 0, SEEK_SET);
        int num_read = fread(buffer, sizeof(char), BUFFER_SIZE, js_x_fd);
        buffer[num_read] = '\0';
        joystick_x = atoi(buffer);

        fseek(js_y_fd, 0, SEEK_SET);
        num_read = fread(buffer, sizeof(char), BUFFER_SIZE, js_y_fd);
        buffer[num_read] = '\0';
        joystick_y = atoi(buffer);
        sleep_ms(10);
    }
    return args;
}

void joystick_init() {
    js_x_fd = fopen(A2D_FILE_VOLTAGE5, "r");
    if (!js_x_fd) {
        perror("Could not open file for joystick x");
        exit(1);
    }

    js_y_fd = fopen(A2D_FILE_VOLTAGE6, "r");
    if (!js_y_fd) {
        perror("Could not open file for joystick y");
        exit(1);
    }

    joystick_thread_running = 1;
    pthread_create(&joystick_thread, NULL, joystick_thread_fn, NULL);

    sleep_ms(5);
}

void joystick_cleanup() {
    joystick_thread_running = 0;
    pthread_join(joystick_thread, NULL);

    fclose(js_x_fd);
    fclose(js_y_fd);
}

void joystick_read(int *x, int *y) {
    int js_x = joystick_x;
    int js_y = joystick_y;
    if (x != NULL) {
        if (js_x - JOYSTICK_CENTER_VALUE > JOYSTICK_THRESHOLD) {
            *x = 1;
        }
        else if (js_x - JOYSTICK_CENTER_VALUE < -JOYSTICK_THRESHOLD) {
            *x = -1;
        }
        else {
            *x = 0;
        }
    }
    if (y != NULL) {
        if (js_y - JOYSTICK_CENTER_VALUE > JOYSTICK_THRESHOLD) {
            *y = 1;
        }
        else if (js_y - JOYSTICK_CENTER_VALUE < -JOYSTICK_THRESHOLD) {
            *y = -1;
        }
        else {
            *y = 0;
        }
    }
}
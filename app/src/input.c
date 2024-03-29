#include "input.h"

#include <stdio.h>

#define HOLD_INPUT(acc_label, action, delta_time, das_interval, arr_interval, is_pressed) \
do {\
    if (!is_pressed) {\
        acc_label = 0;\
    }\
    else {\
        if (acc_label == 0) {\
            pentris_input(action);\
        }\
        acc_label += delta_time;\
        if (acc_label >= (das_interval)) {\
            while (acc_label >= (das_interval) + (arr_interval)) {\
                pentris_input(action);\
                acc_label -= (arr_interval);\
            }\
        }\
    }\
} while(0)

#define BUTTON_INPUT(acc_label, action, is_pressed) \
do {\
    if (acc_label == 0 && (is_pressed)) {\
        pentris_input(action);\
        acc_label = 1;\
    }\
    else if (!is_pressed) {\
        acc_label = 0;\
    }\
} while(0)

//DAS: delayed auto shift
//ARR: auto repeat rate
#define MOVEMENT_DAS 150
#define MOVEMENT_ARR 30

static long long acc_left = 0;
static long long acc_right = 0;
static int ccw_pressed = 0;
static int cw_pressed = 0;
static int soft_drop_pressed = 0;
static int hard_drop_pressed = 0;

void input_init() {
    acc_left = 0;
    acc_right = 0;
    ccw_pressed = 0;
    cw_pressed = 0;
    soft_drop_pressed = 0;
    hard_drop_pressed = 0;
}

void input_cleanup() {

}

void input_update(enum PentrisInput input, long long delta_time, int is_pressed) {
    switch(input) {
        case P_LEFT:
        HOLD_INPUT(acc_left, P_LEFT, delta_time, MOVEMENT_DAS, MOVEMENT_ARR, is_pressed);
        break;
        case P_RIGHT:
        HOLD_INPUT(acc_right, P_RIGHT, delta_time, MOVEMENT_DAS, MOVEMENT_ARR, is_pressed);
        break;
        case P_ROTATE_CCW:
        BUTTON_INPUT(ccw_pressed, P_ROTATE_CCW, is_pressed);
        break;
        case P_ROTATE_CW:
        BUTTON_INPUT(cw_pressed, P_ROTATE_CW, is_pressed);
        break;
        case P_SOFT_DROP:
        HOLD_INPUT(soft_drop_pressed, P_SOFT_DROP, delta_time, 50, 50, is_pressed);
        break;
        case P_HARD_DROP:
        BUTTON_INPUT(hard_drop_pressed, P_HARD_DROP, is_pressed);
        break;
    }
}
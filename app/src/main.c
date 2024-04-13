#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pentris.h"
#include "input.h"
#include "score.h"
#include "renderer.h"
#include "tune_thread.h"

#include "hal/gpio.h"
#include "hal/button.h"
#include "hal/joystick.h"
#include "hal/display_linux.h"
#include "hal/util.h"
#include "hal/buzzer.h"

//#define PLAY_MUSIC

#define GAMEOVER_WAIT_INPUT 3000

int main() {
    volatile uint8_t *buffer = display_init();
    gpio_init();
    button_init();
    joystick_init();
    pentris_init();
    input_init();
    score_init();
    #ifdef PLAY_MUSIC
    buzzer_init();
    tune_init(); // starts background tune
    #endif

    long long prev_time = get_time_ms();
    long long acc = 0;

    int is_running = 1;
    while (is_running) {
        long long current_time = get_time_ms();
        long long delta_time = current_time - prev_time;
        acc += delta_time;

        int btn_left, btn_right;
        int joystick_x, joystick_y;
        button_read(&btn_left, &btn_right);
        joystick_read(&joystick_x, &joystick_y);

        if (pentris_is_gameover()) {
            renderer_draw_gameover(buffer, acc);
            //exit
            if (acc > GAMEOVER_WAIT_INPUT && (joystick_x || joystick_y)) {
                break;
            }
            //reset
            if (acc > GAMEOVER_WAIT_INPUT && (btn_left || btn_right)) {
                pentris_init();
                input_init();
                score_init();
                acc = 0;
                sleep_ms(300);
            }
        }
        else {
            input_update(P_ROTATE_CCW, delta_time, btn_left);
            input_update(P_ROTATE_CW, delta_time, btn_right);
            input_update(P_LEFT, delta_time, joystick_x < 0);
            input_update(P_RIGHT, delta_time, joystick_x > 0);
            input_update(P_SOFT_DROP, delta_time, joystick_y > 0);
            input_update(P_HARD_DROP, delta_time, joystick_y < 0);

            while (acc >= 50) {
                int lines_cleared;
                pentris_tick(&lines_cleared);
                score_line_clear(lines_cleared);
                acc -= 50;
            }
            
            renderer_draw(buffer);
        }

        buffer = display_buffer_swap();
        prev_time = current_time;
        sleep_ms(3);
    }
    #ifdef PLAY_MUSIC
    tune_cleanup();
    #endif
    joystick_cleanup();
    gpio_cleanup();
    display_cleanup();
    return 0;
}
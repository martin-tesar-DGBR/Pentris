#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pentris.h"
#include "input.h"
#include "score.h"
#include "renderer.h"

#include "hal/joystick.h"
#include "hal/display_linux.h"
#include "hal/pru_shared.h"
#include "hal/util.h"

int main() {
    joystick_init();
    volatile uint8_t *buffer = display_init();
    pentris_init();
    input_init();
    score_init();

    long long prev_time = get_time_ms();
    long long acc = 0;

    int is_running = 1;
    while (is_running) {
        long long current_time = get_time_ms();
        long long delta_time = current_time - prev_time;
        acc += delta_time;

        //TODO: replace with actual inputs
        enum JoystickInput input = joystick_get_input();
        input_update(P_ROTATE_CCW, delta_time, input & JS_UP);
        input_update(P_RIGHT, delta_time, input & JS_RIGHT);
        input_update(P_LEFT, delta_time, input & JS_LEFT);
        input_update(P_HARD_DROP, delta_time, input & JS_DOWN);
        if (input & JS_PUSHED) {
            is_running = 0;
        }

        while (acc >= 50) {
            int lines_cleared, game_over;
            pentris_tick(&lines_cleared, &game_over);
            score_line_clear(lines_cleared);
            if (game_over) {
                //TODO: get a better game over
                //(show game over screen, prompt input to reset/exit?)
                //for now break out of the game loop
                is_running = 0;
                continue;
            }
            acc -= 50;
        }
        
        renderer_draw(buffer);
        buffer = display_buffer_swap();

        prev_time = current_time;
        sleep_ms(3);
    }
    
    input_cleanup();
    pentris_cleanup();
    display_cleanup();
    joystick_cleanup();
    return 0;
}
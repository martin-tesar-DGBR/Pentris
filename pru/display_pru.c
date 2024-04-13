#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "pru_shared.h"

//MUST RUN ON PRU1

// GPIO Configuration
// ----------------------------------------------------------
volatile register uint32_t __R30;   // output GPIO register
volatile register uint32_t __R31;   // input GPIO register

#define R1_MASK  (1 << 11)
#define G1_MASK  (1 << 10)
#define B1_MASK  (1 << 9)
#define R2_MASK  (1 << 8)
#define G2_MASK  (1 << 7)
#define B2_MASK  (1 << 6)
#define A_MASK   (1 << 5)
#define B_MASK   (1 << 4)
#define C_MASK   (1 << 3)
#define CLK_MASK (1 << 2)
#define LAT_MASK (1 << 1)
#define OE_MASK  (1 << 0)

#define CYCLE_WAIT (300 / 5) //300ns, used for pulsing clock
#define BRIGHTNESS_WAIT (15000/ 5) //15000ns, how long to keep the lights on until next frame (longer = brighter)

//used for colour 0x25, corresponding to 12.5% red, 12.5% green, 25% blue
//we wanted a way to make a darkish gray (as dark as an LED allows at least) so this colour is reserved
#define DARK_COLOUR 0x25
#define DARK_FREQUENCY 16

#define NUM_BITS_RED 3
#define NUM_BITS_GREEN 3
#define NUM_BITS_BLUE 2

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM       0x00000         // Address of DRAM
#define OFFSET              0x200           // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void *) THIS_PRU_DRAM_USABLE;

static int frame = 0;

static inline void set_mask(uint32_t mask, uint8_t value) {
    if (value) {
        __R30 |= mask;
    }
    else {
        __R30 &= ~mask;
    }
}

static void set_row(int rowNum) {
    // Convert rowNum single bits from int
    char a = rowNum & 0x01 ? 1 : 0;
    char b = rowNum & 0x02 ? 1 : 0;
    char c = rowNum & 0x04 ? 1 : 0;

    // Write on the row pins
    set_mask(A_MASK, a);
    set_mask(B_MASK, b);
    set_mask(C_MASK, c);
}

/**
 *  set_colour_top
 *  Set the colour of the top part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void set_colour_top(uint8_t colour, int frame) {
    uint8_t red_col = (colour & RED_MASK) >> 5;
    uint8_t green_col = (colour & GREEN_MASK) >> 2;
    uint8_t blue_col = (colour & BLUE_MASK);
    char red_mod, green_mod, blue_mod;
    if (colour == DARK_COLOUR) {
        red_mod = DARK_FREQUENCY;
        green_mod = DARK_FREQUENCY;
        blue_mod = DARK_FREQUENCY;
    }
    else {
        red_mod = (1 << NUM_BITS_RED);
        green_mod = (1 << NUM_BITS_GREEN);
        blue_mod = (1 << NUM_BITS_BLUE);
    }
    char red_val = frame % red_mod < red_col ? 1 : 0;
    char green_val = frame % green_mod < green_col ? 1 : 0;
    char blue_val = frame % blue_mod < blue_col ? 1 : 0;

    // Write on the colour pins
    set_mask(R1_MASK, red_val);
    set_mask(G1_MASK, green_val);
    set_mask(B1_MASK, blue_val);
}

/**
 *  set_colour_bottom
 *  Set the colour of the bottom part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void set_colour_bottom(uint8_t colour, int frame) {
    uint8_t red_col = (colour & RED_MASK) >> 5;
    uint8_t green_col = (colour & GREEN_MASK) >> 2;
    uint8_t blue_col = (colour & BLUE_MASK);
    char red_mod, green_mod, blue_mod;
    if (colour == DARK_COLOUR) {
        red_mod = DARK_FREQUENCY;
        green_mod = DARK_FREQUENCY;
        blue_mod = DARK_FREQUENCY;
    }
    else {
        red_mod = (1 << NUM_BITS_RED);
        green_mod = (1 << NUM_BITS_GREEN);
        blue_mod = (1 << NUM_BITS_BLUE);
    }
    char red_val = frame % red_mod < red_col ? 1 : 0;
    char green_val = frame % green_mod < green_col ? 1 : 0;
    char blue_val = frame % blue_mod < blue_col ? 1 : 0;

    // Write on the colour pins
    set_mask(R2_MASK, red_val);
    set_mask(G2_MASK, green_val);
    set_mask(B2_MASK, blue_val);
}

//draws all 0s to display when buffer is NULL (so you don't get blinded when the program shuts down)
static void display_draw(volatile uint8_t *buffer) {
    for (int rowNum = 0; rowNum < DISPLAY_ROWS / 2; rowNum++) {
        __R30 |= OE_MASK;
        __delay_cycles(CYCLE_WAIT);
        set_row(rowNum);
        __delay_cycles(CYCLE_WAIT);
        for (int colNum = 0; colNum < DISPLAY_COLS; colNum++) {
            uint8_t colour_top = buffer ? buffer[colNum + (rowNum * DISPLAY_COLS)] : 0;
            uint8_t colour_bottom = buffer ? buffer[colNum + ((rowNum + (DISPLAY_ROWS / 2)) * DISPLAY_COLS)] : 0;
            set_colour_top(colour_top, frame);
            set_colour_bottom(colour_bottom, frame);
            __delay_cycles(CYCLE_WAIT);
            __R30 |= CLK_MASK;
            __delay_cycles(CYCLE_WAIT);
            __R30 &= ~CLK_MASK;
            __delay_cycles(CYCLE_WAIT);
        }
        __R30 |= LAT_MASK;
        __delay_cycles(CYCLE_WAIT);
        __R30 &= ~LAT_MASK;
        __delay_cycles(CYCLE_WAIT);
        __R30 &= ~OE_MASK;
        __delay_cycles(BRIGHTNESS_WAIT);
    }

    frame = (frame + 1) % 16;
}

void main(void) {
    // Initialize:
    pSharedMemStruct->pru_buffer_use = 0;
    pSharedMemStruct->linux_display_running = true;
    pSharedMemStruct->pru_screen_used = true;
    pSharedMemStruct->linux_screen_available = false;
    //draw a nice gradient on init to make sure the PRU is running
    for (int i = 0; i < DISPLAY_ROWS * DISPLAY_COLS; i++) {
        int row = i / DISPLAY_COLS;
        int col = i % DISPLAY_COLS;
        int red = col % 8;
        int green = row % 8;
        int blue = col / 8;
        int colour = (red << 5) | (green << 2) | blue;
        pSharedMemStruct->buf[0][i] = colour;
        pSharedMemStruct->buf[1][i] = colour;
    }

    while (pSharedMemStruct->linux_display_running) {
        uint8_t buffer_use = pSharedMemStruct->pru_buffer_use;
        display_draw(pSharedMemStruct->buf[buffer_use]);
        
        //swap if a new frame is available
        if (pSharedMemStruct->linux_screen_available) {
            pSharedMemStruct->linux_screen_available = false;
            pSharedMemStruct->pru_buffer_use = 1 - buffer_use;
            pSharedMemStruct->pru_screen_used = false;
        }
        else {
            pSharedMemStruct->pru_screen_used = true;
        }
    }

    display_draw(NULL);
}

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

#define CYCLE_WAIT (500 / 5) //500ns, used for pulsing clock
#define BRIGHTNESS_WAIT (15000/ 5) //2000ns, how long to keep the lights on until next frame (longer = brighter)

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

static void ledMatrix_setRow(int rowNum) {
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
 *  ledMatrix_setColourTop
 *  Set the colour of the top part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void ledMatrix_setColourTop(uint8_t colour, int frame) {
    uint8_t red_col = (colour & RED_MASK) >> 5;
    uint8_t green_col = (colour & GREEN_MASK) >> 2;
    uint8_t blue_col = (colour & BLUE_MASK);
    char red_val = frame % 8 < red_col ? 1 : 0;
    char green_val = frame % 8 < green_col ? 1 : 0;
    char blue_val = frame % 4 < blue_col ? 1 : 0;

    // Write on the colour pins
    set_mask(R1_MASK, red_val);
    set_mask(G1_MASK, green_val);
    set_mask(B1_MASK, blue_val);
}

/**
 *  ledMatrix_setColourBottom
 *  Set the colour of the bottom part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void ledMatrix_setColourBottom(uint8_t colour, int frame) {
    uint8_t red_col = (colour & RED_MASK) >> 5;
    uint8_t green_col = (colour & GREEN_MASK) >> 2;
    uint8_t blue_col = (colour & BLUE_MASK);
    char red_val = frame % 8 < red_col ? 1 : 0;
    char green_val = frame % 8 < green_col ? 1 : 0;
    char blue_val = frame % 4 < blue_col ? 1 : 0;

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
        ledMatrix_setRow(rowNum);
        __delay_cycles(CYCLE_WAIT);
        for (int colNum = 0; colNum < DISPLAY_COLS; colNum++) {
            uint8_t colour_top = buffer ? buffer[colNum + (rowNum * DISPLAY_COLS)] : 0;
            uint8_t colour_bottom = buffer ? buffer[colNum + ((rowNum + (DISPLAY_ROWS / 2)) * DISPLAY_COLS)] : 0;
            ledMatrix_setColourTop(colour_top, frame);
            ledMatrix_setColourBottom(colour_bottom, frame);
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

    frame = (frame + 1) % 8;
}

void main(void) {
    // Initialize:
    pSharedMemStruct->pru_buffer_use = 0;
    pSharedMemStruct->linux_display_running = true;
    pSharedMemStruct->pru_screen_used = true;
    pSharedMemStruct->screen_available = false;
    for (int i = 0; i < DISPLAY_ROWS * DISPLAY_COLS; i++) {
        pSharedMemStruct->buf[0][i] = 0xFF;
        pSharedMemStruct->buf[1][i] = 0xFF;
    }

    while (pSharedMemStruct->linux_display_running) {
        uint8_t buffer_use = pSharedMemStruct->pru_buffer_use;
        display_draw(pSharedMemStruct->buf[buffer_use]);
        
        //swap if a new frame is available
        if (pSharedMemStruct->screen_available) {
            pSharedMemStruct->screen_available = false;
            pSharedMemStruct->pru_buffer_use = 1 - buffer_use;
            pSharedMemStruct->pru_screen_used = false;
        }
        else {
            pSharedMemStruct->pru_screen_used = true;
        }
    }

    display_draw(NULL);
}

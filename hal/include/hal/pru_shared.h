#ifndef _PRU_SHARED_H_
#define _PRU_SHARED_H_

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_ROWS 16
#define DISPLAY_COLS 32

#define RED_MASK 0xE0
#define GREEN_MASK 0x1C
#define BLUE_MASK 0x03

#define PRU_ADDR      0x4A300000   // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN       0x80000      // Length of PRU memory
#define PRU0_DRAM     0x00000      // Offset to DRAM
#define PRU1_DRAM     0x02000
#define PRU_SHAREDMEM 0x10000      // Offset to shared memory
#define PRU_MEM_RESERVED 0x200     // Amount used by stack and heap

// Convert base address to each memory section
//#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)

//yes the first one does nothing, as specified
#define BUFFER_USE_TO_PRU(buffer_use) (buffer_use)
#define BUFFER_USE_TO_LINUX(buffer_use) (1 - (buffer_use))

//naming convention: for the sake of synchronization anything prefixed by pru or
//linux will only be writable by the named unit, and the other can only read it
//only applies during main program (e.g. outside initialization)
typedef struct {
    uint8_t buf[2][DISPLAY_ROWS * DISPLAY_COLS];
    //buffer_use: index for which buffer is accessible by which unit
    //0 -> pru uses buf[0], linux uses buf[1]
    //1 -> pru uses buf[1], linux uses buf[0]
    uint8_t pru_buffer_use; 
    bool linux_display_running;
    bool pru_screen_used; //signal when PRU is finished drawing the front buffer
    bool linux_screen_available; //signal from linux when done drawing on the back buffer, reset by PRU
} sharedMemStruct_t;

#endif

#include "hal/display_linux.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

#include "hal/util.h"
#include "hal/pru_shared.h"

volatile void *pPru_base;
volatile sharedMemStruct_t *pSharedMemStruct;

static void export_pins() {
    run_command("config-pin P8_46 pruout");
    run_command("config-pin P8_45 pruout");
    run_command("config-pin P8_44 pruout");
    run_command("config-pin P8_43 pruout");
    run_command("config-pin P8_42 pruout");
    run_command("config-pin P8_41 pruout");
    run_command("config-pin P8_40 pruout");
    run_command("config-pin P8_39 pruout");
    run_command("config-pin P8_30 pruout");
    run_command("config-pin P8_29 pruout");
    run_command("config-pin P8_28 pruout");
    run_command("config-pin P8_27 pruout");
}

//passes a handle to a frame that the application can draw on
volatile uint8_t *display_init() {
    export_pins();

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Could not open /dev/mem");
        exit(1);
    }
    pPru_base = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPru_base == MAP_FAILED) {
        exit(1);
    }
    close(fd);

    pSharedMemStruct = (volatile sharedMemStruct_t *) PRU1_MEM_FROM_BASE((volatile uint8_t *) pPru_base);
    return pSharedMemStruct->buf[BUFFER_USE_TO_LINUX(pSharedMemStruct->pru_buffer_use)];
}

void display_cleanup() {
    pSharedMemStruct->linux_display_running = false;

    if (munmap((void*) pPru_base, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(1);
    }
}

//swap out the drawn frame given by either this function or init for a new one
volatile uint8_t *display_buffer_swap() {
    pSharedMemStruct->linux_screen_available = true;
    //wait for the PRU to finish with the current frame
    while (!pSharedMemStruct->pru_screen_used);
    return pSharedMemStruct->buf[BUFFER_USE_TO_LINUX(pSharedMemStruct->pru_buffer_use)];
}
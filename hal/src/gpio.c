#include "hal/gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//see https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf tables 2-3 and 25-5
#define GPIO0_BASE_ADDRESS 0x44E07000
#define GPIO1_BASE_ADDRESS 0x4804C000
#define GPIO2_BASE_ADDRESS 0x481AC000
#define GPIO3_BASE_ADDRESS 0x481AE000

//included for completion, only GPIO0_SIZE is used
#define GPIO0_END 0x44E07FFF
#define GPIO1_END 0x4804CFFF
#define GPIO2_END 0x481ACFFF
#define GPIO3_END 0x481AEFFF

#define GPIO_SIZE (GPIO3_END - GPIO0_BASE_ADDRESS)

/*
#define GPIO0_SIZE (GPIO0_END - GPIO0_BASE_ADDRESS)
#define GPIO1_SIZE (GPIO1_END - GPIO1_BASE_ADDRESS)
#define GPIO2_SIZE (GPIO2_END - GPIO2_BASE_ADDRESS)
#define GPIO3_SIZE (GPIO3_END - GPIO3_BASE_ADDRESS)
*/

#define GPIO_OE           0x134
#define GPIO_DATAIN       0x138
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT   0x194

volatile void *gpio_addr[4];

static volatile void *gpio_get_addr(enum GPIOPin pin) {
    int idx = pin / 32;
    return gpio_addr[idx];
}

static int gpio_get_index(enum GPIOPin pin) {
    return pin % 32;
}

void gpio_init() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Could not open /dev/mem");
        exit(1);
    }

    gpio_addr[0] = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO0_BASE_ADDRESS);
    if (gpio_addr[0] == MAP_FAILED) {
        perror("GPIO0 mmap failed");
        exit(1);
    }

    gpio_addr[1] = (void *) ((uint8_t *) gpio_addr[0] + (GPIO1_BASE_ADDRESS - GPIO0_BASE_ADDRESS));
    gpio_addr[2] = (void *) ((uint8_t *) gpio_addr[0] + (GPIO2_BASE_ADDRESS - GPIO0_BASE_ADDRESS));
    gpio_addr[3] = (void *) ((uint8_t *) gpio_addr[0] + (GPIO3_BASE_ADDRESS - GPIO0_BASE_ADDRESS));

    close(fd);
}

void gpio_cleanup() {
    if (munmap((void *) gpio_addr[0], GPIO_SIZE)) {
        perror("GPIO0 munmap failed");
    }
}

void gpio_set_direction(enum GPIOPin pin, enum GPIODirection direction) {
    volatile void *addr = gpio_get_addr(pin);
    uint32_t write_mask = 0x01U << gpio_get_index(pin);
    switch (direction) {
        case GPIO_in:
        *((uint32_t *) (((uint8_t *) addr) + GPIO_OE)) |= write_mask;
        break;
        case GPIO_out:
        *((uint32_t *) (((uint8_t *) addr) + GPIO_OE)) &= ~write_mask;
        break;
    }
}

int gpio_read(enum GPIOPin pin) {
    volatile void *addr = gpio_get_addr(pin);
    uint32_t read_mask = 0x01U << gpio_get_index(pin);
    int result = *((uint32_t *) (((uint8_t *) addr) + GPIO_DATAIN)) & read_mask;
    return result != 0;
}

void gpio_write(enum GPIOPin pin, int value) {
    volatile void *addr = gpio_get_addr(pin);
    uint32_t write_mask = 0x01U << gpio_get_index(pin);
    if (value) {
        *((uint32_t *) (((uint8_t *) addr) + GPIO_SETDATAOUT)) = write_mask;
    }
    else {
        *((uint32_t *) (((uint8_t *) addr) + GPIO_CLEARDATAOUT)) = write_mask;
    }
}
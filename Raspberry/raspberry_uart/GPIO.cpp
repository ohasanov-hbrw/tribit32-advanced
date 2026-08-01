#include "GPIO.h"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

// I AM NOT USING THIS ANYWAY

GPIO::GPIO() {
    //mapGPIO();
}

GPIO::~GPIO() {
    //unmapGPIO();
}

void GPIO::mapGPIO() {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        throw std::runtime_error("Cannot open /dev/mem");
    }

    void* gpio_map = mmap(
        nullptr,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE
    );

    close(mem_fd);

    if (gpio_map == MAP_FAILED) {
        throw std::runtime_error("mmap error");
    }

    gpio = static_cast<volatile uint32_t*>(gpio_map);
}

void GPIO::unmapGPIO() {
    munmap(reinterpret_cast<void*>(const_cast<uint32_t*>(gpio)), BLOCK_SIZE);
}

void GPIO::checkPin(int pin) {
    if (pin < 0 || pin > 53) {
        throw std::out_of_range("GPIO pin number out of range");
    }
}

void GPIO::INP_GPIO(int pin) {
    *(gpio + (pin / 10)) &= ~(7 << ((pin % 10) * 3));
}

void GPIO::OUT_GPIO(int pin) {
    *(gpio + (pin / 10)) |= (1 << ((pin % 10) * 3));
}

void GPIO::SET_GPIO(int pin) {
    *(gpio + 7) = 1 << pin;
}

void GPIO::CLR_GPIO(int pin) {
    *(gpio + 10) = 1 << pin;
}

int GPIO::GET_GPIO(int pin) {
    return (*(gpio + 13) & (1 << pin)) ? 1 : 0;
}

int GPIO::GET_PIN_DIRECTION(int pin) {
    int reg = *(gpio + (pin / 10));
    int shift = (pin % 10) * 3;
    return (reg >> shift) & 7;
}

void GPIO::setPinDirection(int pin, bool isOutput) {
    checkPin(pin);
    INP_GPIO(pin); // Set pin as input first
    if (isOutput) {
        OUT_GPIO(pin); // Set pin as output
    }
}

void GPIO::writePin(int pin, int value) {
    checkPin(pin);
    if (value > 0) {
        SET_GPIO(pin); // Set pin high
    } else {
        CLR_GPIO(pin); // Set pin low
    }
}

int GPIO::readPin(int pin) {
    checkPin(pin);
    return GET_GPIO(pin); // Read pin state
}

bool GPIO::getPinDirection(int pin) {
    checkPin(pin);
    return GET_PIN_DIRECTION(pin) == 1; // 1 indicates output, other values indicate input or alternate functions
}

void GPIO::setPullUpDown(int pin, int pud) {
    checkPin(pin);
    volatile uint32_t* pud_reg = gpio + 37; // GPPUD register
    volatile uint32_t* pud_clk_reg = gpio + 38 + (pin / 32); // GPPUDCLK0/1 register

    *pud_reg = pud; // Set the pull-up/down value
    usleep(1); // Wait 150 cycles
    *pud_clk_reg = (1 << (pin % 32)); // Assert clock on the pin
    usleep(1); // Wait 150 cycles
    *pud_reg = 0; // Remove the control signal
    *pud_clk_reg = 0; // Remove the clock
}

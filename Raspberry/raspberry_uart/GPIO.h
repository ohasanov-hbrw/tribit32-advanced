#ifndef GPIO_H
#define GPIO_H

#include <cstdint>
#include <cstddef>


#define PUD_OFF    0
#define PUD_DOWN   1
#define PUD_UP     2

static const int HEADER_IO_PINS[40] = {
	-2,  2,  3,  4, -1, 17, 27, 22, -2, 10,  9, 11, -1, -4,  5,  6, 13, 19, 26, -1,
	-3, -3, -1, 14, 15, 18, -1, 23, 24, -1, 25,  8,  7, -4, -1, 12, -1, 16, 20, 21};

class GPIO {
public:
    GPIO();
    ~GPIO();

    void setPinDirection(int pin, bool isOutput);
    void writePin(int pin, int value);
    int readPin(int pin);
    bool getPinDirection(int pin);
    void setPullUpDown(int pin, int pud); // PUD_OFF, PUD_DOWN, PUD_UP

private:
    static const int IO_PINS[40];
    volatile uint32_t* gpio;

    void mapGPIO();
    void unmapGPIO();
    void checkPin(int pin);
    void INP_GPIO(int pin);
    void OUT_GPIO(int pin);
    void SET_GPIO(int pin);
    void CLR_GPIO(int pin);
    int GET_GPIO(int pin);
    int GET_PIN_DIRECTION(int pin);

    static constexpr uintptr_t GPIO_BASE = 0x3F200000; // For RPi 2/3. Adjust for other models.
    static constexpr size_t BLOCK_SIZE = 4096;
};

#endif // GPIO_H

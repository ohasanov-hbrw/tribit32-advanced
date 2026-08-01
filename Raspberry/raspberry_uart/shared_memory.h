#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <cstdint>

const int SCREEN_WIDTH = 320; // Example width, replace with actual
const int SCREEN_HEIGHT = 240; // Example height, replace with actual

const key_t SHM_KEY_UPDATE = 1022; // Shared memory key for color buffer
const key_t SHM_KEY_COLOR = 1023; // Shared memory key for color buffer
const key_t SHM_KEY_TRANSPARENCY = SHM_KEY_COLOR + 20; // Shared memory key for transparency buffer

struct ColorBuffer {
    uint16_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT]; // 16-bit color depth per pixel
};

struct TransparencyBuffer {
    uint8_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT]; // 8-bit transparency per pixel
};

struct Updater{
	bool update;
};

#endif // SHARED_MEMORY_H

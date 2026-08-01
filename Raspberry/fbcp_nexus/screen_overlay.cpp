#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>
#include "screen_overlay.h"
#include "shared_memory.h"

static ColorBuffer* colorBuffer = nullptr;
static TransparencyBuffer* transparencyBuffer = nullptr;
static Updater* updater = nullptr;

void InitScreenOverlay() {
    // Create shared memory segments
	int shmidUpdater = shmget(SHM_KEY_UPDATE, sizeof(Updater), 0666 | IPC_CREAT);
    if (shmidUpdater == -1) {
        perror("shmget (update)");
        return;
    }
	
    int shmidColor = shmget(SHM_KEY_COLOR, sizeof(ColorBuffer), 0666 | IPC_CREAT);
    if (shmidColor == -1) {
        perror("shmget (color)");
        return;
    }

    int shmidTransparency = shmget(SHM_KEY_TRANSPARENCY, sizeof(TransparencyBuffer), 0666 | IPC_CREAT);
    if (shmidTransparency == -1) {
        perror("shmget (transparency)");
        return;
    }

    // Attach to the shared memory segments
	updater = (Updater*)shmat(shmidUpdater, nullptr, 0);
    if (updater == (void*)-1) {
        perror("shmat (updater)");
        updater = nullptr;
        return;
    }
	
    colorBuffer = (ColorBuffer*)shmat(shmidColor, nullptr, 0);
    if (colorBuffer == (void*)-1) {
        perror("shmat (color)");
        colorBuffer = nullptr;
        return;
    }

    transparencyBuffer = (TransparencyBuffer*)shmat(shmidTransparency, nullptr, 0);
    if (transparencyBuffer == (void*)-1) {
        perror("shmat (transparency)");
        transparencyBuffer = nullptr;
        return;
    }
	
}

bool PollOverlayUpdate(){
	return updater->update == true;
}

void OverlayUpdateReset(){
	updater->update = false;
}

void DrawFullScreenImage(uint16_t* framebuffer) {
    if (colorBuffer == nullptr || transparencyBuffer == nullptr) {
        std::cerr << "Screen buffers not initialized. Call InitScreenOverlay first." << std::endl;
        return;
    }

    for (int y = 0; y < screenHeight; ++y) {
        for (int x = 0; x < screenWidth; ++x) {
            uint16_t color = colorBuffer->buffer[y * screenWidth + x];
            uint8_t transparency = transparencyBuffer->buffer[y * screenWidth + x];

            if (transparency != 0) { // 0 means fully transparent
                uint16_t* framebufferPixel = &framebuffer[x * screenHeight + y];
                uint16_t existingColor = *framebufferPixel;

                if (transparency == 255) { // Fully opaque, directly copy the color
                    *framebufferPixel = color;
                } else {
                    // Decompose colors into RGB components
                    uint16_t existingRed = (existingColor >> 11) & 0x1F;
                    uint16_t existingGreen = (existingColor >> 5) & 0x3F;
                    uint16_t existingBlue = existingColor & 0x1F;

                    uint16_t newRed = (color >> 11) & 0x1F;
                    uint16_t newGreen = (color >> 5) & 0x3F;
                    uint16_t newBlue = color & 0x1F;

                    // Blend the colors using integer arithmetic
                    uint16_t finalRed = ((newRed * transparency) + (existingRed * (255 - transparency))) >> 8;
                    uint16_t finalGreen = ((newGreen * transparency) + (existingGreen * (255 - transparency))) >> 8;
                    uint16_t finalBlue = ((newBlue * transparency) + (existingBlue * (255 - transparency))) >> 8;

                    // Recompose the final color
                    uint16_t finalColor = (finalRed << 11) | (finalGreen << 5) | (finalBlue);

                    // Write the blended color back to the framebuffer
                    *framebufferPixel = finalColor;
                }
            }
        }
    }
}

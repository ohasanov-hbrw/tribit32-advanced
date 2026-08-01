#pragma once

#include <inttypes.h>


#define screenHeight 240
#define screenWidth 320

// Initializes the shared memory segment for drawing the full-screen image.
void InitScreenOverlay();

// Draws the full-screen image from the shared memory segment onto the given framebuffer.
void DrawFullScreenImage(uint16_t *framebuffer);
bool PollOverlayUpdate();
void OverlayUpdateReset();

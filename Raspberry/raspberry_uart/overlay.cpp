#include "overlay.h"

#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include "font5x7.h"
#include "symbols5x7.h"
#include "lodepng.h"

static Updater* updater = nullptr;
static ColorBuffer* colorBufferLink = nullptr;
static TransparencyBuffer* transparencyBufferLink = nullptr;

OverlayManager::OverlayManager() {
	int shmidUpdater = shmget(SHM_KEY_UPDATE, sizeof(Updater), 0666 | IPC_CREAT);
    if(shmidUpdater == -1) {
        perror("shmget (update)");
        exit(0);
    }
	
    int shmidColor = shmget(SHM_KEY_COLOR, sizeof(ColorBuffer), 0666 | IPC_CREAT);
    if(shmidColor == -1) {
        perror("shmget (color)");
        exit(0);
    }

    int shmidTransparency = shmget(SHM_KEY_TRANSPARENCY, sizeof(TransparencyBuffer), 0666 | IPC_CREAT);
    if(shmidTransparency == -1) {
        perror("shmget (transparency)");
        exit(0);
    }

    // Attach to the shared memory segments
	updater = (Updater*)shmat(shmidUpdater, nullptr, 0);
    if(updater == (void*)-1) {
        perror("shmat (updater)");
        exit(0);
    }
	
    colorBufferLink = (ColorBuffer*)shmat(shmidColor, nullptr, 0);
    if(colorBufferLink == (void*)-1) {
        perror("shmat (color)");
        exit(0);
    }

    transparencyBufferLink = (TransparencyBuffer*)shmat(shmidTransparency, nullptr, 0);
    if(transparencyBufferLink == (void*)-1) {
        perror("shmat (transparency)");
        exit(0);
    }
	
	
    initializeBuffers();
}
OverlayManager::~OverlayManager() {
	if(shmdt(colorBufferLink) == -1) {
        perror("shmdt (color)");
        exit(0);
    }

    if(shmdt(transparencyBufferLink) == -1) {
        perror("shmdt (transparency)");
        exit(0);
    }
	
	if(shmdt(updater) == -1) {
        perror("shmdt (update)");
        exit(0);
    }
}

// Function to initialize the buffers with default values
void OverlayManager::initializeBuffers() {
    const uint16_t black = 0x0000;
    const uint8_t transparent = 0;
    for(int y = 0; y < SCREEN_HEIGHT; ++y) {
        for(int x = 0; x < SCREEN_WIDTH; ++x) {
            colorBuffer.buffer[y * SCREEN_WIDTH + x] = black;
            transparencyBuffer.buffer[y * SCREEN_WIDTH + x] = transparent;
        }
    }
	commit();
}

void OverlayManager::commit() {
	memcpy(transparencyBufferLink->buffer, transparencyBuffer.buffer, sizeof(transparencyBuffer.buffer));
	memcpy(colorBufferLink->buffer, colorBuffer.buffer, sizeof(colorBuffer.buffer));
	
	updater->update = true;
}

void OverlayManager::clearScreen() {
	fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
}

void OverlayManager::drawLine(int x0, int y0, int x1, int y1, uint16_t color, uint8_t transparency) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while(true) {
        if(x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT) {
            colorBuffer.buffer[y0 * SCREEN_WIDTH + x0] = color;
            transparencyBuffer.buffer[y0 * SCREEN_WIDTH + x0] = transparency;
        }

        if(x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
	
}

void OverlayManager::drawRect(int x, int y, int width, int height, uint16_t color, uint8_t transparency) {
    // Draw top and bottom
    for(int i = 0; i < width; ++i) {
        if(x + i < SCREEN_WIDTH) {
            if(y >= 0 && y < SCREEN_HEIGHT) {
                colorBuffer.buffer[y * SCREEN_WIDTH + (x + i)] = color;
                transparencyBuffer.buffer[y * SCREEN_WIDTH + (x + i)] = transparency;
            }
            if(y + height - 1 >= 0 && y + height - 1 < SCREEN_HEIGHT) {
                colorBuffer.buffer[(y + height - 1) * SCREEN_WIDTH + (x + i)] = color;
                transparencyBuffer.buffer[(y + height - 1) * SCREEN_WIDTH + (x + i)] = transparency;
            }
        }
    }

    // Draw sides
    for(int i = 0; i < height; ++i) {
        if(y + i < SCREEN_HEIGHT) {
            if(x >= 0 && x < SCREEN_WIDTH) {
                colorBuffer.buffer[(y + i) * SCREEN_WIDTH + x] = color;
                transparencyBuffer.buffer[(y + i) * SCREEN_WIDTH + x] = transparency;
            }
            if(x + width - 1 >= 0 && x + width - 1 < SCREEN_WIDTH) {
                colorBuffer.buffer[(y + i) * SCREEN_WIDTH + (x + width - 1)] = color;
                transparencyBuffer.buffer[(y + i) * SCREEN_WIDTH + (x + width - 1)] = transparency;
            }
        }
    }
}

void OverlayManager::fillRect(int x, int y, int width, int height, uint16_t color, uint8_t transparency) {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            if(x + j < SCREEN_WIDTH && y + i < SCREEN_HEIGHT) {
                colorBuffer.buffer[(y + i) * SCREEN_WIDTH + (x + j)] = color;
                transparencyBuffer.buffer[(y + i) * SCREEN_WIDTH + (x + j)] = transparency;
            }
        }
    }
}

void OverlayManager::drawCircle(int centerX, int centerY, int radius, uint16_t color, uint8_t transparency) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x; // Decision criterion divided by 2 evaluated at x=r, y=0

    while(y <= x) {
        // Draw 8 octants
        if(centerX + x >= 0 && centerX + x < SCREEN_WIDTH) {
            if(centerY + y >= 0 && centerY + y < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY + y) * SCREEN_WIDTH + (centerX + x)] = color;
                transparencyBuffer.buffer[(centerY + y) * SCREEN_WIDTH + (centerX + x)] = transparency;
            }
            if(centerY - y >= 0 && centerY - y < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY - y) * SCREEN_WIDTH + (centerX + x)] = color;
                transparencyBuffer.buffer[(centerY - y) * SCREEN_WIDTH + (centerX + x)] = transparency;
            }
        }
        if(centerX - x >= 0 && centerX - x < SCREEN_WIDTH) {
            if(centerY + y >= 0 && centerY + y < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY + y) * SCREEN_WIDTH + (centerX - x)] = color;
                transparencyBuffer.buffer[(centerY + y) * SCREEN_WIDTH + (centerX - x)] = transparency;
            }
            if(centerY - y >= 0 && centerY - y < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY - y) * SCREEN_WIDTH + (centerX - x)] = color;
                transparencyBuffer.buffer[(centerY - y) * SCREEN_WIDTH + (centerX - x)] = transparency;
            }
        }
        if(centerX + y >= 0 && centerX + y < SCREEN_WIDTH) {
            if(centerY + x >= 0 && centerY + x < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY + x) * SCREEN_WIDTH + (centerX + y)] = color;
                transparencyBuffer.buffer[(centerY + x) * SCREEN_WIDTH + (centerX + y)] = transparency;
            }
            if(centerY - x >= 0 && centerY - x < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY - x) * SCREEN_WIDTH + (centerX + y)] = color;
                transparencyBuffer.buffer[(centerY - x) * SCREEN_WIDTH + (centerX + y)] = transparency;
            }
        }
        if(centerX - y >= 0 && centerX - y < SCREEN_WIDTH) {
            if(centerY + x >= 0 && centerY + x < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY + x) * SCREEN_WIDTH + (centerX - y)] = color;
                transparencyBuffer.buffer[(centerY + x) * SCREEN_WIDTH + (centerX - y)] = transparency;
            }
            if(centerY - x >= 0 && centerY - x < SCREEN_HEIGHT) {
                colorBuffer.buffer[(centerY - x) * SCREEN_WIDTH + (centerX - y)] = color;
                transparencyBuffer.buffer[(centerY - x) * SCREEN_WIDTH + (centerX - y)] = transparency;
            }
        }
        y++;
        if(decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;
        }
    }
}

void OverlayManager::fillCircle(int centerX, int centerY, int radius, uint16_t color, uint8_t transparency) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x; // Decision criterion divided by 2 evaluated at x=r, y=0

    while(y <= x) {
        for(int i = centerX - x; i <= centerX + x; ++i) {
            if(i >= 0 && i < SCREEN_WIDTH) {
                if(centerY + y >= 0 && centerY + y < SCREEN_HEIGHT) {
                    colorBuffer.buffer[(centerY + y) * SCREEN_WIDTH + i] = color;
                    transparencyBuffer.buffer[(centerY + y) * SCREEN_WIDTH + i] = transparency;
                }
                if(centerY - y >= 0 && centerY - y < SCREEN_HEIGHT) {
                    colorBuffer.buffer[(centerY - y) * SCREEN_WIDTH + i] = color;
                    transparencyBuffer.buffer[(centerY - y) * SCREEN_WIDTH + i] = transparency;
                }
            }
        }
        for(int i = centerX - y; i <= centerX + y; ++i) {
            if(i >= 0 && i < SCREEN_WIDTH) {
                if(centerY + x >= 0 && centerY + x < SCREEN_HEIGHT) {
                    colorBuffer.buffer[(centerY + x) * SCREEN_WIDTH + i] = color;
                    transparencyBuffer.buffer[(centerY + x) * SCREEN_WIDTH + i] = transparency;
                }
                if(centerY - x >= 0 && centerY - x < SCREEN_HEIGHT) {
                    colorBuffer.buffer[(centerY - x) * SCREEN_WIDTH + i] = color;
                    transparencyBuffer.buffer[(centerY - x) * SCREEN_WIDTH + i] = transparency;
                }
            }
        }
        y++;
        if(decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;
        }
    }
}

void OverlayManager::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, uint8_t transparency) {
    drawLine(x0, y0, x1, y1, color, transparency);
    drawLine(x1, y1, x2, y2, color, transparency);
    drawLine(x2, y2, x0, y0, color, transparency);
}

void OverlayManager::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, uint8_t transparency) {
    auto swap = [](int &a, int &b) { int t = a; a = b; b = t; };
    if(y0 > y1) { swap(y0, y1); swap(x0, x1); }
    if(y1 > y2) { swap(y1, y2); swap(x1, x2); }
    if(y0 > y1) { swap(y0, y1); swap(x0, x1); }

    int total_height = y2 - y0;
    for(int i = 0; i < total_height; i++) {
        bool second_half = i > y1 - y0 || y1 == y0;
        int segment_height = second_half ? y2 - y1 : y1 - y0;
        float alpha = (float)i / total_height;
        float beta  = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;
        int A = x0 + (x2 - x0) * alpha;
        int B = second_half ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta;
        if(A > B) swap(A, B);
        for(int j = A; j <= B; j++) {
            if(j >= 0 && j < SCREEN_WIDTH && y0 + i >= 0 && y0 + i < SCREEN_HEIGHT) {
                colorBuffer.buffer[(y0 + i) * SCREEN_WIDTH + j] = color;
                transparencyBuffer.buffer[(y0 + i) * SCREEN_WIDTH + j] = transparency;
            }
        }
    }
}

void OverlayManager::drawChar(char c, int x, int y, uint16_t color, uint8_t transparency) {
    if(c < 32 || c > 126) return; // Only support ASCII 32 to 126

    for(int i = 0; i < 5; ++i) { // 5 columns
        uint8_t col = font5x7[c - 32][i];
        for(int j = 0; j < 7; ++j) { // 7 rows
            if(col & (1 << j)) {
                int drawX = x + i;
                int drawY = y + j;
                if(drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                    colorBuffer.buffer[drawY * SCREEN_WIDTH + drawX] = color;
                    transparencyBuffer.buffer[drawY * SCREEN_WIDTH + drawX] = transparency;
                }
            }
        }
    }
}

void OverlayManager::drawSymbol(int s, int x, int y, uint16_t color, uint8_t transparency) {
    if(s > 10) return; // Only support ASCII 32 to 126

    for(int i = 0; i < 5; ++i) { // 5 columns
        uint8_t col = symbol5x7[s - 32][i];
        for(int j = 0; j < 7; ++j) { // 7 rows
            if(col & (1 << j)) {
                int drawX = x + i;
                int drawY = y + j;
                if(drawX >= 0 && drawX < SCREEN_WIDTH && drawY >= 0 && drawY < SCREEN_HEIGHT) {
                    colorBuffer.buffer[drawY * SCREEN_WIDTH + drawX] = color;
                    transparencyBuffer.buffer[drawY * SCREEN_WIDTH + drawX] = transparency;
                }
            }
        }
    }
}

void OverlayManager::drawString(const char* str, int x, int y, uint16_t color, uint8_t transparency) {
    int startX = x;
    while(*str) {
        if(*str == '\n') {
            x = startX;
            y += 8; // Move to the next line
        } else {
            drawChar(*str, x, y, color, transparency);
            x += 6; // Move to the next character position (5 pixels + 1 pixel space)
        }
        ++str;
    }
}

int OverlayManager::getStringWidth(const char* str) {
	int startX = 0;
	int maxX = 0;
    while(*str) {
        if(*str == '\n') {
			if(startX > maxX) maxX = startX;
            startX = 0;
        } else {
            startX += 6;
        }
        ++str;
    }
	if(startX > maxX) maxX = startX;
	
	return maxX;
}

void OverlayManager::drawPNG(const char* filename, int posX, int posY) {
    std::vector<unsigned char> image; // The raw pixels
    unsigned width, height;

    // Decode the PNG
    lodepng::decode(image, width, height, filename);
	drawPNG(filename, posX, posY, width, height);
}

void OverlayManager::drawPNG(const char* filename, int posX, int posY, int targetWidth, int targetHeight) {
    std::vector<unsigned char> image; // The raw pixels
    unsigned width, height;

    // Decode the PNG
    unsigned error = lodepng::decode(image, width, height, filename);

    // Check for errors
    if(error) {
        std::cerr << "Error decoding PNG: " << lodepng_error_text(error) << std::endl;
        return;
    }

    // Draw the image with scaling
    for(int y = 0; y < targetHeight; ++y) {
        for(int x = 0; x < targetWidth; ++x) {
            int srcX = x * width / targetWidth;
            int srcY = y * height / targetHeight;

            if(posX + x < SCREEN_WIDTH && posY + y < SCREEN_HEIGHT) {
                unsigned idx = 4 * (srcY * width + srcX);
                uint8_t r = image[idx];
                uint8_t g = image[idx + 1];
                uint8_t b = image[idx + 2];
                uint8_t a = image[idx + 3];
				
				if (a != 0) { // 0 means fully transparent
					uint16_t newColor = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3); // Convert to RGB565
					uint16_t existingColor = colorBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)];

					if (a == 255) { // Fully opaque, directly copy the color
						colorBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)] = newColor;
						transparencyBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)] = 255;
					} else {
						// Decompose colors into RGB components
						uint16_t existingRed = (existingColor >> 11) & 0x1F;
						uint16_t existingGreen = (existingColor >> 5) & 0x3F;
						uint16_t existingBlue = existingColor & 0x1F;
						uint8_t  existingAlpha = transparencyBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)];

						uint16_t newRed = (newColor >> 11) & 0x1F;
						uint16_t newGreen = (newColor >> 5) & 0x3F;
						uint16_t newBlue = newColor & 0x1F;

						// Blend the colors using integer arithmetic
						uint16_t finalRed = ((newRed * a) + (existingRed * (255 - existingAlpha))) >> 8;
						uint16_t finalGreen = ((newGreen * a) + (existingGreen * (255 - existingAlpha))) >> 8;
						uint16_t finalBlue = ((newBlue * a) + (existingBlue * (255 - existingAlpha))) >> 8;
					
						uint16_t calcAlpha = transparencyBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)] + a;
						uint8_t  finalAlpha = calcAlpha > 255 ? 255 : calcAlpha;

						// Recompose the final color
						uint16_t finalColor = (finalRed << 11) | (finalGreen << 5) | (finalBlue);

						// Write the blended color back to the framebuffer
						colorBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)] = finalColor;
						transparencyBuffer.buffer[(posY + y) * SCREEN_WIDTH + (posX + x)] = finalAlpha;
					}
				}
            }
        }
    }
}

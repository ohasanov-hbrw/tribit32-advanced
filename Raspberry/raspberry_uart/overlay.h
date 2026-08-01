#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared_memory.h"

class OverlayManager {
public:
    OverlayManager();
    ~OverlayManager();
	
	void commit();
	void clearScreen();
	void drawLine(int x0, int y0, int x1, int y1, uint16_t color, uint8_t transparency);
	void drawRect(int x, int y, int width, int height, uint16_t color, uint8_t transparency);
	void fillRect(int x, int y, int width, int height, uint16_t color, uint8_t transparency);
	void drawCircle(int centerX, int centerY, int radius, uint16_t color, uint8_t transparency);
	void fillCircle(int centerX, int centerY, int radius, uint16_t color, uint8_t transparency);
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, uint8_t transparency);
	void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, uint8_t transparency);
	void drawChar(char c, int x, int y, uint16_t color, uint8_t transparency);
	void drawSymbol(int s, int x, int y, uint16_t color, uint8_t transparency);
	void drawString(const char* str, int x, int y, uint16_t color, uint8_t transparency);
	void drawPNG(const char* filename, int posX, int posY);
	void drawPNG(const char* filename, int posX, int posY, int targetWidth, int targetHeight);
	
	
	int getStringWidth(const char* str);
	
private:
	void initializeBuffers();
	
	ColorBuffer colorBuffer;
    TransparencyBuffer transparencyBuffer;
};

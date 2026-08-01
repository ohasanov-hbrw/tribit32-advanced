#ifndef PICO_MONITOR_H
#define PICO_MONITOR_H

#include "monitor.h"
#include <string>

class PicoMonitor : public Monitor{
public:	
	PicoMonitor(std::string port);
	~PicoMonitor();
	
	typedef uint8_t byte;
	
	bool init() override;
	void update() override;
	void request(byte func, unsigned int value) override;
	bool hasFeatures(int features) override;
	
private:
	int serial_fd, update_counter;
    byte buffer[2048];  // Buffer to store incomplete messages
    size_t buffer_len = 0;
	std::string port;
	
	void statusUpdate();
	int openSerialPort(const char* portName);
	uint16_t crc16_ccitt_xmodem(const uint8_t* data, size_t length);
	
    void readSerial();
	void processBuffer();
}; 

#endif // PICO_MONITOR_H
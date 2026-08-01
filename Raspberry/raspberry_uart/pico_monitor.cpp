#include "pico_monitor.h"
#include <iostream>
#include <cstdio>
#include <ostream>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define DEBUG false
#define ONE_LINE false

#define STATUS_TIMEOUT 1000

PicoMonitor::PicoMonitor(std::string port) : Monitor() {
	this->port = port;
}
PicoMonitor::~PicoMonitor() {
    close(serial_fd);
}
	
bool PicoMonitor::init() {
	serial_fd = openSerialPort(port.c_str());
	if(serial_fd == -1) {
        return false;
    }
	
	statusUpdate();
	return true;
}

void PicoMonitor::update() {
	readSerial();
	if(update_counter == 0) statusUpdate(); 
		
	update_counter++;
	if(update_counter > STATUS_TIMEOUT) update_counter = 0;
}

void PicoMonitor::readSerial() {
    byte temp_buffer[512];
    ssize_t len;
    while((len = read(serial_fd, temp_buffer, sizeof(temp_buffer))) > 0) {
        if(buffer_len + len > sizeof(buffer)) {
            // Handle overflow
            std::cerr << "Buffer overflow" << std::endl;
            buffer_len = 0;
            continue;
        }
        memcpy(buffer + buffer_len, temp_buffer, len);
        buffer_len += len;
        processBuffer();
    }
}

void PicoMonitor::processBuffer() {
    size_t i = 0;
    while(i < buffer_len) {
        // Find the start of a message
        if(buffer[i] != '{') {
            if(i < 64) {
                ++i;
                continue;
            }
            // Shift the buffer to remove processed part
            buffer_len -= i;
            memmove(buffer, buffer + i, buffer_len);
            i = 0;
            continue;
        }

        // Check if there is enough space for at least the header
        size_t message_start = i + 1;
        if(message_start + 2 >= buffer_len) break;

        byte button_count = buffer[message_start + 1];
        size_t button_value_count = (button_count + 7) / 8;
        size_t axis_count_pos = message_start + 2 + button_value_count;

        // Check if there is enough space for the button values and axis count
        if(axis_count_pos >= buffer_len) break;

        byte axis_count = buffer[axis_count_pos];
        size_t message_end = axis_count_pos + 1 + axis_count * 4;

        // Check if there is enough space for the entire message and if it ends with '}' with a 2 byte crc
        if(message_end + 2 >= buffer_len) break;
		if(buffer[message_end] != '}') {
			buffer_len -= 3;
            memmove(buffer, buffer + 3, buffer_len);
            continue;
		}

		uint16_t calc_crc = crc16_ccitt_xmodem(buffer + message_start - 1, message_end + 2 - message_start);
		uint16_t sent_crc = buffer[message_end + 1] + (buffer[message_end + 2] << 8);
		
		if(calc_crc != sent_crc) {
			buffer_len -=1;
			memmove(buffer, buffer + 1, buffer_len);
		}

        // Valid message found, extract and process it
        size_t message_len = message_end - message_start;
        byte* message = buffer + message_start;

        if(false) {
            //if(true) std::cout << "\033[A";
            std::cout << "\rExtracted message: ";
            for(size_t j = 0; j < message_len; ++j) {
                std::cout << std::hex << static_cast<int>(message[j]) << " " << std::dec;
            }
            std::cout << std::endl;
        }

        // Ensure there's enough data for a valid message
        if(message_len >= 3) {
            byte controller_index = message[0];
            byte button_count = message[1];
            size_t button_value_count = (button_count + 7) / 8;
            byte* button_values = message + 2;
            byte axis_count = message[2 + button_value_count];

            if(message_len >= 2 + button_value_count + 1 + axis_count * 4) {
                int16_t axis_values[axis_count * 2];
                for(size_t j = 0; j < axis_count; ++j) {
                    axis_values[j * 2] = static_cast<int16_t>((message[3 + button_value_count + j * 4] << 8) | message[4 + button_value_count + j * 4]);
                    axis_values[j * 2 + 1] = static_cast<int16_t>((message[5 + button_value_count + j * 4] << 8) | message[6 + button_value_count + j * 4]);
                }
                callback(controller_index, button_count, button_values, axis_count, axis_values);
            }
        }

        // Shift remaining buffer
        buffer_len -= (message_end + 1);
        memmove(buffer, buffer + message_end + 1, buffer_len);
        i = 0;
    }
}

uint16_t PicoMonitor::crc16_ccitt_xmodem(const uint8_t* data, size_t length) {
    uint16_t crc = 0x0000; // Initial value
    const uint16_t polynomial = 0x1021; // Polynomial used in CRC-16-CCITT

    for(size_t i = 0; i < length; ++i) {
        crc ^= (data[i] << 8); // XOR byte into the high order byte of CRC

        for(uint8_t j = 0; j < 8; ++j) {
            if(crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

void PicoMonitor::request(byte func, unsigned int value){
	byte buffer[11];
	buffer[0] = '{';
	buffer[1] = (byte)(func & 0xFF);
	buffer[2] = (byte)((value >> 0) & 0xFF);
	buffer[3] = (byte)((value >> 8) & 0xFF);
	buffer[4] = (byte)((value >> 16) & 0xFF);
	buffer[5] = (byte)((value >> 24) & 0xFF);
	buffer[6] = 'a';
    buffer[7] = 'a';
    buffer[8] = 'a';
    buffer[9] = 'a';
	buffer[10] = '}';
	write(serial_fd, buffer, sizeof(buffer));
}

void PicoMonitor::statusUpdate() {
	uint8_t write_data[11];
	write_data[0] = '{';
	write_data[1] = 2; //Read Battery Value
	write_data[2] = 'a';
	write_data[3] = 'a';
	write_data[4] = 'a';
	write_data[5] = 'a';
    write_data[6] = 'a';
    write_data[7] = 'a';
    write_data[8] = 'a';
    write_data[9] = 'a';
	write_data[10] = '}';
	write(serial_fd, write_data, sizeof(write_data));
	
	write_data[1] = 4; //Read Fan Value
	write(serial_fd, write_data, sizeof(write_data));
	
	write_data[1] = 6; //Read Backlight Value
	write(serial_fd, write_data, sizeof(write_data));
	
    //std::cout << "StatusUpdate\n";
	//for(int i = 0; i < 4; i++) {
		request(1, 1); //Read Controller States
	//}
}

int PicoMonitor::openSerialPort(const char* portName) {
    int fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1) {
        perror("openSerialPort: Unable to open port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    // Set baud rate to 115200
    cfsetispeed(&options, B57600);
    cfsetospeed(&options, B57600);

    // Set 8 data bits, no parity, 1 stop bit
    options.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    options.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit (most common)
    options.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    options.c_cflag |= CS8; // 8 bits per byte (most common)
    options.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    options.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    // Set raw input
    cfmakeraw(&options);

    // Set the new options for the port
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

bool PicoMonitor::hasFeatures(int features){
	const int have = FEATURE_BATTERY | FEATURE_BACKLIGHT;
	features &= ~have;
	
	return features == 0;
}

#pragma once
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>
#include <unistd.h>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <linux/uinput.h>

#include "monitor.h"

// Define DEBUG as a boolean
#define DEBUG false
#define ONE_LINE false


#define EVENT_BUTTON  1
#define EVENT_AXIS    2
#define EVENT_SPECIAL 3

#define SPECIAL_BATTERY    1
#define SPECIAL_VOLUME     2
#define SPECIAL_VOLUME_ADJ 3

#define SPECIAL_FAN        4
#define SPECIAL_FAN_ADJ 5

#define SPECIAL_BACK       6
#define SPECIAL_BACK_ADJ   7

#define SAMPLE_SIZE 100

extern int SELECT_BUTTON, START_BUTTON, L_BUTTON, R_BUTTON, A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON, EXTRA_BUTTON, OPTIONS_BUTTON , DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT;

class ControllerManager {
	
public:
    int batteryValue, fanValue, backlightValue;
	bool pluggedIn;
	
	typedef bool (*JoyCallback)(ControllerData[],int);
	typedef uint8_t byte;
	JoyCallback callback;
	Monitor* monitor = nullptr;
	ControllerData controllers[4];

    ControllerManager(JoyCallback callback);
    ~ControllerManager();
	
	
	void setMonitor(Monitor* monitor);
	int initMonitor();
	void loop();
	void monitorRequest(byte func, unsigned int value);
	
	float getBatteryAverage();
	
private:
    int fds[4];  // File descriptors for the virtual joysticks
    int fd_count = 0;
	
	bool controlMode = false;
	
	int sample_buffer[SAMPLE_SIZE];
	int sample_index = 0;
	int sample_count = 0;
	long sample_sum = 0;

    void setup_uinput_device(int joystick_id);
	void addSample(int newSample);
	
    void emulateJoystick(byte controller_index, byte button_count, const byte* button_values, byte axis_count, const int16_t* axis_values);
    void monitorJoystick(byte controller_index, byte button_count, const byte* button_values, byte axis_count, const int16_t* axis_values);
    void sendEvent(int fd, int type, int code, int16_t value);
};


#endif // CONTROLLER_H

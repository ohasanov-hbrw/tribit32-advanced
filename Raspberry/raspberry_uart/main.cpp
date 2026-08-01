#include <iostream>
#include <cstdio>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <cstring>
#include <string>
#include <sys/reboot.h>

#include "overlay.h"
#include "pico_monitor.h"
#include "gpio_monitor.h"
#include "controller.h"
#include "GPIO.h"
#include "properties.h"

#define VERSION "0.1"

#define OVERLAY_TIMEOUT 5
#define STATUS_TIMEOUT 10
#define FAN_MIN_VALUE 125

#define FAN_STEP_SIZE 5
#define BACK_STEP_SIZE 5

#define AXIS_DEADZONE 100
#define RESISTOR_RATIO 2
#define VREF_ADC 3.3
#define VOFF 0.1

#define OVERLAY_BATTERY 0
#define OVERLAY_BRIGHTNESS 4
#define OVERLAY_FAN 2
#define OVERLAY_VOLUME 1
#define OVERLAY_CLOSED -1

#define DEBUG_GPIO_OVERLAY false

using String = std::string;

int VOLUME_STEP_SIZE = 1, VOLUME_MIN = 0, VOLUME_MAX = 1;
int SELECT_BUTTON, START_BUTTON, L_BUTTON, R_BUTTON, A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON, EXTRA_BUTTON, OPTIONS_BUTTON , DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT;
bool joyCheck(ControllerData[], int);
std::string getLocalFile(const std::string& filename);

Monitor* monitor = nullptr;
ControllerManager manager(joyCheck);
OverlayManager overlay;
GPIO gpio;

volatile int overlay_counter = 0, fanCounter = 0, special_counter = 0;
int last_vol, overlay_id = -1, overlay_dir;
std::string audDevice;

void controllerLoop() {
    manager.loop();
}

void picoProcess(int data) {
	std::cout << "Battery Value: " << manager.batteryValue << std::endl;
}

float batt_diffs[11] = {4.05, 4.00, 3.95, 3.92, 3.87, 3.82, 3.79, 3.75, 3.72, 3.65, 3.20};
int   batt_percs[11] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 1};

int getBatteryPercentage(float voltage) {
	int perc = 0;
	int diff_index = 0;
    for(int i = 0; i < 11; i++) {
		if(voltage > batt_diffs[i]) {
			perc = batt_percs[i];
			diff_index = i;
			break;
		}
	}
	
	if(perc == 0 || perc == 100) return perc;
	
	float delta = (voltage - batt_diffs[diff_index]) / (batt_diffs[diff_index - 1] - batt_diffs[diff_index]);
	perc += (int)(delta * 10);
	
	return perc;
}

void drawVolumeOverlay(int dir, int value) {
	overlay.drawPNG(getLocalFile("assets/overlay_rp.png").c_str(), 0, 0);
	
	float ratio = (float)(value - VOLUME_MIN) / (VOLUME_MAX - VOLUME_MIN);
	float percent = ratio * 100;
	
	if(percent > 70){
		overlay.drawPNG(getLocalFile("assets/volume_full.png").c_str(), 289, 39);
	} else if(percent > 40){
		overlay.drawPNG(getLocalFile("assets/volume_med.png").c_str(), 289, 39);
	}  else if(percent > 1){
		overlay.drawPNG(getLocalFile("assets/volume_low.png").c_str(), 289, 39);
	} else {
		overlay.drawPNG(getLocalFile("assets/volume_mute.png").c_str(), 289, 39);
	}
	
	std::string dStr = std::to_string((int)(percent));
	int xOff = 293;
	
	int strLen = dStr.length();
	if(strLen == 1) xOff += 5;
	if(strLen == 2) xOff += 2;
	
	overlay.drawString(dStr.c_str(), xOff, 190, 0xFFFF, 0xF0);
	
	int h = (118 * ratio);
	
	overlay.drawRect(290, 65, 22, 120, 0xFFFF, 0xFF);
	overlay.fillRect(291, 66 + (118 - h), 20, h, 0xFFFF, 0xFF);
}

void drawFanOverlay(int value, int change) {
	overlay.drawPNG(getLocalFile("assets/overlay_rp.png").c_str(), 0, 0);
	
	fanCounter = (fanCounter+1) % 8;
	int fanFrame = fanCounter;
	
	std::string path = getLocalFile("assets/fan_" + std::to_string(fanFrame) + ".png");
	overlay.drawPNG(path.c_str(), 289, 39);
	
	value -= FAN_MIN_VALUE;
	if(value < 0) value = 0;
	
	float ratio = (float)value / (255 - FAN_MIN_VALUE);
	float percent = ratio * 100;
	
	std::string dStr = std::to_string((int)(percent));
	int xOff = 293;
	
	int strLen = dStr.length();
	if(strLen == 1) xOff += 5;
	if(strLen == 2) xOff += 2;
	
	overlay.drawString(dStr.c_str(), xOff, 190, 0xFFFF, 0xF0);
	
	int h = (118 * ratio);
	
	overlay.drawRect(290, 65, 22, 120, 0xFFFF, 0xFF);
	overlay.fillRect(291, 66 + (118 - h), 20, h, 0xFFFF, 0xFF);
}

void drawBacklightOverlay(int value, int change) {
	overlay.drawPNG(getLocalFile("assets/overlay_rp.png").c_str(), 0, 0);
	
	if(value < 0) value = 0;
	
	float ratio = (float)value / 255;
	float percent = ratio * 100;
	
	if(percent > 75){
		overlay.drawPNG(getLocalFile("assets/brightness_full.png").c_str(), 289, 39);
	} else if(percent > 25){
		overlay.drawPNG(getLocalFile("assets/brightness_half.png").c_str(), 289, 39);
	} else {
		overlay.drawPNG(getLocalFile("assets/brightness_none.png").c_str(), 289, 39);
	}
	
	std::string dStr = std::to_string((int)(percent));
	int xOff = 293;
	
	int strLen = dStr.length();
	if(strLen == 1) xOff += 5;
	if(strLen == 2) xOff += 2;
	
	overlay.drawString(dStr.c_str(), xOff, 190, 0xFFFF, 0xF0);
	
	int h = (118 * ratio);
	
	overlay.drawRect(290, 65, 22, 120, 0xFFFF, 0xFF);
	overlay.fillRect(291, 66 + (118 - h), 20, h, 0xFFFF, 0xFF);
}

float batteryVoltage = 0;

int adcSamples[20] = {0};
int adcSampleIndex = 0;
int adcSampleCount = 0;
int adcTotalSum = 0;

void drawBatteryOverlay(int value) {
	overlay.drawPNG(getLocalFile("assets/battery_overlay.png").c_str(), 0, 0);

        adcTotalSum -= adcSamples[adcSampleIndex];
        adcSamples[adcSampleIndex] = value;
        adcTotalSum += value;

        adcSampleIndex = (adcSampleIndex + 1) % 20;
        if (adcSampleCount < 20) {
            adcSampleCount++;
        }

        float averageValue = (float)adcTotalSum / adcSampleCount;
        float voltage = (((averageValue / (float)1023) * VREF_ADC) + VOFF) * RESISTOR_RATIO;


	//float voltage = (((value / (float)1023) * VREF_ADC) + VOFF) * RESISTOR_RATIO;
	int percent = getBatteryPercentage(voltage);
	batteryVoltage = voltage;
	char buffer[20];
    sprintf(buffer, "%d%% (%.3fv)", percent, voltage);
	
	std::string dStr = std::string(buffer) + (voltage > 4.3 ? " Charging" : " Battery");
	int xOff = (320 - overlay.getStringWidth(dStr.c_str())) / 2;
	//xOff += (3 - dStr.length()) * 3;
	overlay.drawString(dStr.c_str(), xOff, 24, 0xFFFF, 0xFF);
	
	
	int width = (float)232 / 100 * percent;
	overlay.drawRect(43, 2, 234, 16, 0xFFFF, 0xFF);
	overlay.fillRect(44 + (232 - width), 3, width, 14, 0xFFFF, 0xFF);
}

void render_pin_states(){
	int size = 10;
	int xOff = 75;
	int yOff = 20;
	
	overlay.fillRect(320 - (size * 2 + xOff), yOff, size * 2, size * 20, 0xFFFF, 0xFF);
	for(int i = 0; i < 40; i++){
		int x = i < 20 ? 320 - (size * 2 + xOff) : 320 - (size + xOff);
		int y = (i % 20) * size + yOff;
		
		int textX = i < 20 ? 320 - ((size * 8) + xOff - 6) : 320 - (xOff - 1);
		
		bool filled = false;
		bool dir = false;
		bool ioPin = false;
		
		std::string func = "";
		if(HEADER_IO_PINS[i] > 0){
			filled = gpio.readPin(HEADER_IO_PINS[i]);
			dir = gpio.getPinDirection(HEADER_IO_PINS[i]);
		}
		
		int color = 0;
		switch(HEADER_IO_PINS[i]){
			case -4: //DNC Pin
			color = 0xFC00;
			func = "DNC";
			break;
			
			case -3: //5v Pin
			color = 0xFA00;
			func = "5V";
			break;
			
			case -2: //3.3v Pin
			color = 0xF800;
			func = "3V3";
			break;
			
			case -1: //GND Pin
			color = 0x0000;
			func = "GND";
			break;
			
			default: //GND Pin
			color = 0x07E0;
			func = std::to_string(HEADER_IO_PINS[i]);
			ioPin = true;
			break;
		}
		
		if(filled){
			overlay.fillRect(x, y, size, size, color, 0xFF);
		} else {
			overlay.drawRect(x, y, size, size, color, 0xFF);
		}
		while(func.length() < 3) {
			if(i < 20){
				func.insert(0, " ");
			} else {
				func.append(" ");
			}
		}
		
		std::string pinData;
		
		if(ioPin){
			pinData.append(dir ? "Out:" : "In: ");
			pinData.append(filled ? "1" : "0");
		} else {
			pinData.append("     ");
		}
		
		if(i < 20){
			func.append(" ");
			func.append(pinData);
		} else {
			func.insert(0, " ");
			func.insert(0, pinData);
		}
		
		overlay.drawString(func.c_str(), textX, y, 0xFFFF, 0xFF);
	}
}

void controlBrightness(int value) {
	if(value > AXIS_DEADZONE) { //Adjust Down
		manager.monitorRequest(7, manager.backlightValue - BACK_STEP_SIZE);
	} else if(value < -AXIS_DEADZONE) { //Adjust Up
		manager.monitorRequest(7, manager.backlightValue + BACK_STEP_SIZE);
	}
}

void controlFan(int value) {
	if(value > AXIS_DEADZONE) { //Adjust Down
		manager.monitorRequest(5, manager.fanValue - FAN_STEP_SIZE);
	} else if(value < -AXIS_DEADZONE) { //Adjust Up
		manager.monitorRequest(5, manager.fanValue + FAN_STEP_SIZE);
	}
}

std::string removeSubstring(const std::string& str, const std::string& sub) {
    std::string result = str;
    size_t pos;

    // Find the substring in the string and erase it
    while ((pos = result.find(sub)) != std::string::npos) {
        result.erase(pos, sub.length());
    }

    return result;
}

void controlVolume(int value) {
    std::string command;
    if(value < -AXIS_DEADZONE) {
        command = "amixer sset '" + audDevice + "',0 " + std::to_string(VOLUME_STEP_SIZE) + "+";
    } else if(value > AXIS_DEADZONE) {
        command = "amixer sset '" + audDevice + "',0 " + std::to_string(VOLUME_STEP_SIZE) + "-";
    } else {
        command = "amixer sset '" + audDevice + "',0 0+";
    }

    // Execute the command and handle the output
    FILE* pipe = popen(command.c_str(), "r");
    if(!pipe) return;

    char buffer[128];
    std::string result = "";
    while(fgets(buffer, 128, pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

	result = removeSubstring(result, "Playback");

	size_t pos = result.find("Limits:");
	if(pos != std::string::npos) {
        std::sscanf(result.c_str() + pos, "Limits: %d - %d", &VOLUME_MIN, &VOLUME_MAX);
    } else {
		
	}
	VOLUME_STEP_SIZE = (VOLUME_MAX - VOLUME_MIN) / 100;
	
    // Parse the output to find the current volume level
    pos = result.find("Front Left:");
    int currVol = 0;
    if(pos != std::string::npos) {
        std::sscanf(result.c_str() + pos, "Front Left: %d", &currVol);
    } else {
		pos = result.find("Mono:");
		if(pos != std::string::npos) {
			std::sscanf(result.c_str() + pos, "Mono: %d", &currVol);
        }
	}
	last_vol = currVol;
}

bool joyCheck(ControllerData controllers[], int c) {
	bool selectPressed = controllers[c].button_states[SELECT_BUTTON];
    bool startPressed  = controllers[c].button_states[START_BUTTON];

	bool optionsPressed = controllers[c].button_states[OPTIONS_BUTTON]; 
	
    bool rPressed      = controllers[c].button_states[R_BUTTON];
    bool lPressed      = controllers[c].button_states[L_BUTTON];
    bool yPressed      = controllers[c].button_states[Y_BUTTON];
    bool xPressed      = controllers[c].button_states[X_BUTTON];
    bool bPressed      = controllers[c].button_states[B_BUTTON];
    bool aPressed      = controllers[c].button_states[A_BUTTON];
	
    int axisValue = 0;
	if(controllers[c].button_states[DPAD_UP]){
		axisValue = -128;
	}
	if(controllers[c].button_states[DPAD_DOWN]){
		axisValue = 128;
	}
	
	
	if(optionsPressed && aPressed && rPressed && lPressed) {
		special_counter++;
		//std::cout << special_counter << std::endl;
		if(special_counter >= 1000) {
			//REBOOT
		    sync(); // Flush filesystem buffers
            std::string command;
			command = "reboot";

			// Execute the command and handle the output
			FILE* pipe = popen(command.c_str(), "r");
			if(pipe){
				char buffer[128];
				while(fgets(buffer, 128, pipe) != NULL) {
				}
				pclose(pipe);
			}
			//reboot(RB_AUTOBOOT);
			exit(0);
		}
		return true;
	} else if(optionsPressed && bPressed && rPressed && lPressed) {
		special_counter++;
		if(special_counter >= 1000) {
	        sync(); // Flush filesystem buffers
            std::string command;
			command = "shutdown -h now";

			// Execute the command and handle the output
			FILE* pipe = popen(command.c_str(), "r");
			if(pipe){
				char buffer[128];
				while(fgets(buffer, 128, pipe) != NULL) {
				}
				pclose(pipe);
			}
			//reboot(RB_POWER_OFF);
			exit(0);
		}
        return true;
	}
	else if(optionsPressed && xPressed && rPressed && lPressed) {
		special_counter++;
		if(special_counter == 1000) {
	        std::string command;
			command = "systemctl restart fbcp";

			// Execute the command and handle the output
			FILE* pipe = popen(command.c_str(), "r");
			if(pipe){
				char buffer[128];
				while(fgets(buffer, 128, pipe) != NULL) {
				}
				pclose(pipe);
			}
		}
        return true;
	}

	
	if(optionsPressed && bPressed) {
		if(special_counter == 0) {
			std::cout << "OVERLAY BATTERY " << manager.getBatteryAverage() << std::endl;
			overlay_id = OVERLAY_BATTERY;
			overlay_counter = 0;
		    special_counter++;
		}
        return true;
	} 
	//else if(selectPressed && aPressed && axisValue != 0) {
	//	int start_delay = 1000;
	//	int cont_delay = 100;
	//	if(special_counter == 1) {
	//		controlFan(axisValue);
	//		overlay_id = 2;
	//		overlay_counter = 0;
	//	}
	//	special_counter++;
	//	if(special_counter == start_delay) {
	//		controlFan(axisValue);
	//		overlay_id = 2;
	//		overlay_counter = 0;
	//	}
	//	if(special_counter >= (start_delay + cont_delay)) {
	//		special_counter = start_delay - 1;
	//	}
    //    return true;
	//} //not using a fan!
	//else if(selectPressed && aPressed) {
	//	if(special_counter == 0) {
	//		overlay_id = 2;
	//		overlay_counter = 0;
	//	}
	//	special_counter = 1;
    //    return true;
	//} 
	else if(optionsPressed && xPressed && axisValue != 0) {
		int start_delay = 1000;
		int cont_delay = 100;
		if(special_counter == 1) {
			std::cout << "OVERLAY VOLUME " << axisValue << std::endl;
			controlVolume(axisValue);
			overlay_id = OVERLAY_VOLUME;
			overlay_counter = 0;
		}
		special_counter++;
		if(special_counter == start_delay) {
			std::cout << "OVERLAY VOLUME " << axisValue << std::endl;
			controlVolume(axisValue);
			overlay_id = OVERLAY_VOLUME;
			overlay_counter = 0;
		}
		if(special_counter >= (start_delay + cont_delay)) {
			special_counter = start_delay - 1;
		}
        return true;
    } else if(optionsPressed && xPressed) {
		if(special_counter == 0) {
			std::cout << "OVERLAY VOLUME " << axisValue << std::endl;
			overlay_id = OVERLAY_VOLUME;
			overlay_counter = 0;
		}
		special_counter = 1;
        return true;
	} else if(optionsPressed && yPressed && axisValue != 0) {
		int start_delay = 1000;
		int cont_delay = 100;
		if(special_counter == 1) {
			std::cout << "OVERLAY BRIGHTNESS " << axisValue << std::endl;
			controlBrightness(axisValue);
			overlay_id = OVERLAY_BRIGHTNESS;
			overlay_counter = 0;
		}
		special_counter++;
		if(special_counter == start_delay) {
			std::cout << "OVERLAY BRIGHTNESS " << axisValue << std::endl;
			controlBrightness(axisValue);
			overlay_id = OVERLAY_BRIGHTNESS;
			overlay_counter = 0;
		}
		if(special_counter >= (start_delay + cont_delay)) {
			special_counter = start_delay - 1;
		}
        return true;
    } else if(optionsPressed && yPressed) {
		if(special_counter == 0) {
			std::cout << "OVERLAY BRIGHTNESS " << axisValue << std::endl;
			overlay_id = OVERLAY_BRIGHTNESS;
			overlay_counter = 0;
		}
		special_counter = 1;
        return true;
	}
	
	special_counter = 0;
	if(optionsPressed){
		return true;
	}
	return false;
}

#define DRAW_LOW_VOLTAGE 1

void drawOverlay(){
	overlay.clearScreen();
	switch(overlay_id){
		case OVERLAY_BATTERY: //Battery
			drawBatteryOverlay(manager.getBatteryAverage());
		    break;
		case OVERLAY_VOLUME: //Volume
			drawVolumeOverlay(0, last_vol);
		    break;
		case OVERLAY_FAN: //Fan
			drawFanOverlay(manager.fanValue, 0);
		    break;
		case OVERLAY_BRIGHTNESS: //Brightness
			drawBacklightOverlay(manager.backlightValue, 0);
		    break;
	}
	if(DRAW_LOW_VOLTAGE && batteryVoltage < 3.4){
		overlay.drawString("LOW VOLTAGE, PLEASE CHARGE", 4, 4, 0x8800, 0xF0);
	}

	if(DEBUG_GPIO_OVERLAY){
		render_pin_states();
	}
	overlay.commit();
}

int strcmp(std::string a, std::string b){
	return strcmp(a.c_str(), b.c_str());
}

#define HOLD_DELAY 100
int holdCounter = 0;
int getHeldPin(int pins[]){
	bool pinStates[25];
	bool search = true;
	int pin = 0, pinIndex = 0;
	holdCounter = 0;
	
	for(int i = 0; i < 25; i++){ //Get initial states
		if(pins[i] == 0) break;
		pinStates[i] = gpio.readPin(pins[i]);
	}
	
	while(search){
		for(int i = 0; i < 25; i++){
			if(pins[i] == 0) break;
			if(pinStates[i] != gpio.readPin(pins[i])){
				pinIndex = i;
				holdCounter++;
				if(holdCounter > HOLD_DELAY){
					pin = pins[i];
					search = false;
				}
			}
		}
		if(pinIndex > -1 && (pinStates[pinIndex] == gpio.readPin(pins[pinIndex]))){
			holdCounter = 0;
			pinIndex = -1;
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	std::cout << pin << "\n" << "Let Go to Continue" << std::flush;
	while(pinStates[pinIndex] != gpio.readPin(pins[pinIndex])){}
	
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	return pin;
}

int getHeldButton(){
	bool buttonStates[32];
	bool search = true;
	int buttonIndex = -1;
	holdCounter = 0;
	
	for(int i = 0; i < 32; i++){ //Get initial states
		buttonStates[i] = manager.controllers[0].button_states[i];
	}
	
	while(search){
		for(int i = 0; i < 32; i++){
			if(buttonStates[i] != manager.controllers[0].button_states[i]){
				buttonIndex = i;
				holdCounter++;
				if(holdCounter > HOLD_DELAY){
					search = false;
				}
			}
		}
		if(buttonIndex > -1 && (buttonStates[buttonIndex] == manager.controllers[0].button_states[buttonIndex])){
			holdCounter = 0;
			buttonIndex = -1;
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	std::cout << buttonIndex << "\n" << "Let Go to Continue" << std::flush;
	while(buttonStates[buttonIndex] != manager.controllers[0].button_states[buttonIndex]){}
	
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	return buttonIndex;
}

bool configureGPIOMonitor(Properties* config){
	std::string pinsUsed = config->get("buttonPins", "2,3,4,5,6,7,12,13,16,17,18,19,20,21,23,24,26");
	int pullUpDown       = config->getInt("pullUpDown", 1);  //0 for disable, 1 for up, 2 for down
	
	std::cout << "The pins for buttons are not currently configured for GPIO Monitor, would you like to do this now? [y/n]: ";
	std::string response;
	std::getline(std::cin, response);
	if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
		std::cout << "Buttons must be setup before running gpio monitor. Exiting..." << std::endl;
		return false;
	}
	
	int pins[25] = {0};
	bool retry = true;
	int axisCount = 0;
	bool dpad = true;
	
	while(retry){
		std::cout << "Specify what bcm pins, seperated by commas, should be used to detect button presses. Leave blank for all but 8, 9, 10, 11, 14, 15, 22, 25, 27 (SPI Display Pins and Serial): ";
		std::getline(std::cin, pinsUsed);
		
		if(strcmp(pinsUsed, "") == 0){
			pinsUsed = "2,3,4,5,6,7,12,13,16,17,18,19,20,21,23,24,26";
		}
		
		try{
			int lastIndex = 0, index = 0, pinIndex = 0;
			while(index < pinsUsed.length()){
				index = pinsUsed.find(",", lastIndex);
				int pin = std::stoi(pinsUsed.substr(lastIndex, index - lastIndex));
			
				if(pin < 2 || pin > 27){
					std::cout << "Invalid Pin Number. ";
					throw std::exception();
				}
				if(index == -1){
					index = pinsUsed.length();
					pins[pinIndex] = pin;
					
					break;
				} else {
					pins[pinIndex] = pin;
					lastIndex = index + 1;
				}
				pinIndex++;
				if(pinIndex >= 25){
					std::cout << "Too many pins. ";
					throw std::exception();
				}
			}
			
			
			if(pinIndex < 5){
				std::cout << "Not enough Pins. Need (DPad or 1 Axis) + 1 Button. ";
				throw std::exception();
			}
			
			retry = false;
		} catch(...){
			std::cout << "Invalid input, Try Again? [y/n]: ";
			std::getline(std::cin, response);
			if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
				retry = false;
				return false;
			}
		}
	}
	
	retry = true;
	while(retry){
		std::cout << "How many axis do you have? (Don't include DPad) [0-4]: ";
		std::getline(std::cin, response);
		
		try{
			axisCount = std::stoi(response);
			retry = false;
		} catch(...){
			std::cout << "Invalid input, Try Again? [y/n]: ";
			std::getline(std::cin, response);
			if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
				retry = false;
				return false;
			}
		}
	}
	retry = true;
	
	std::cout << "Do you have a DPad? [y/n]: ";
	std::getline(std::cin, response);
	if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
		dpad = false;
	}
	
	if(!dpad && axisCount == 0){
		std::cout << "You need some sort of directional input to use picotroller. Exiting...";
		return false;
	}
	
	while(retry){
		std::cout << "Do you want pull downs (1), pull ups (2), or nothing (0)? [0-2]: ";
		std::getline(std::cin, response);
		
		try{
			pullUpDown = std::stoi(response);
			if(pullUpDown < 0 || pullUpDown > 2){
				throw std::exception();
			}
			retry = false;
		} catch(...){
			std::cout << "Invalid input, Try Again? [y/n]: ";
			std::getline(std::cin, response);
			if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
				retry = false;
			}
		}
	}
	retry = true;
	
	std::cout << "Recording Pins" << std::endl;
	for(int i = 0; i < 25; i++){
		gpio.setPinDirection(pins[i], false);
		gpio.setPullUpDown(pins[i], pullUpDown);
	}
		
	if(axisCount > 0){
		//TODO Implement Axis Setup
	}
		
	if(dpad){
		std::cout << "\33[2K\rHold Up: " << std::flush;
		int pin = getHeldPin(pins);
		config->setInt("PIN_UP", pin);
		
		std::cout << "\33[2K\rHold Down: " << std::flush;
		pin = getHeldPin(pins);
		config->setInt("PIN_DOWN", pin);
		
		std::cout << "\33[2K\rHold Left: " << std::flush;
		pin = getHeldPin(pins);
		config->setInt("PIN_LEFT", pin);
		
		std::cout << "\33[2K\rHold Right: " << std::flush;
		pin = getHeldPin(pins);
		config->setInt("PIN_RIGHT", pin);
	}
	
	std::cout << "\33[2K\rHold A: " << std::flush;
	int pin = getHeldPin(pins);
	config->setInt("PIN_A", pin);
	
	std::cout << "\33[2K\rHold B: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_B", pin);
	
	std::cout << "\33[2K\rHold X: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_X", pin);
	
	std::cout << "\33[2K\rHold Y: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_Y", pin);
	
	std::cout << "\33[2K\rHold R: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_R", pin);
	
	std::cout << "\33[2K\rHold L: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_L", pin);
	
	std::cout << "\33[2K\rHold Start: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_START", pin);
	
	std::cout << "\33[2K\rHold Select: " << std::flush;
	pin = getHeldPin(pins);
	config->setInt("PIN_SELECT", pin);
	
	
	config->flush();
	std::cout << "\n\nGPIO Monitor configured! Run again to start picotroller." << std::flush;
	
	return true;
}

bool configurePicotroller(Properties* config){
	audDevice = config->get("audioDevice", "");
	
	std::cout << "The menu buttons are not currently configured for PicoTroller, would you like to do this now? [y/n]: ";
	std::string response;
	std::getline(std::cin, response);
	if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
		std::cout << "Buttons must be setup before running picotroller. Exiting..." << std::endl;
		return false;
	}
	
	bool hasFan = monitor->hasFeatures(FEATURE_FAN);
	bool hasBat = monitor->hasFeatures(FEATURE_BATTERY);
	bool hasBak = monitor->hasFeatures(FEATURE_BACKLIGHT);
	
	           std::cout << "Your configured monitor has the following features: \n";
	           std::cout << "     Volume Control\n";
	if(hasBat) std::cout << "     Battery Voltage Reading\n";
	if(hasFan) std::cout << "     Dynamic Fan Control\n";
	if(hasBak) std::cout << "     Dynamic Backlight Control\n";
	
	std::cout << "\33[2K\rPress any button on the controller, then press enter." << std::flush;
	std::getline(std::cin, response);
	
	std::cout << "\33[2K\rHold Start: " << std::flush;
	START_BUTTON = getHeldButton();
	config->setInt("BUTTON_START", START_BUTTON);
	
	std::cout << "\33[2K\rHold Select: " << std::flush;
	SELECT_BUTTON = getHeldButton();
	config->setInt("BUTTON_SELECT", SELECT_BUTTON);
	
	std::cout << "\33[2K\rHold R: " << std::flush;
	R_BUTTON = getHeldButton();
	config->setInt("BUTTON_R", R_BUTTON);
	
	std::cout << "\33[2K\rHold L: " << std::flush;
	L_BUTTON = getHeldButton();
	config->setInt("BUTTON_L", L_BUTTON);
	
	//change to actual controller
	std::cout << "\33[2K\rHold X: " << std::flush;
	X_BUTTON = getHeldButton();
	config->setInt("BUTTON_X", X_BUTTON);
	
	std::cout << "\33[2K\rHold Y" << std::flush;
	Y_BUTTON = getHeldButton();
	config->setInt("BUTTON_Y", Y_BUTTON);

	std::cout << "\33[2K\rHold B" << std::flush;
	B_BUTTON = getHeldButton();
	config->setInt("BUTTON_B", B_BUTTON);

	std::cout << "\33[2K\rHold A" << std::flush;
	A_BUTTON = getHeldButton();
	config->setInt("BUTTON_A", A_BUTTON);

	std::cout << "\33[2K\rHold EXTRA" << std::flush;
	EXTRA_BUTTON = getHeldButton();
	config->setInt("BUTTON_EXTRA", EXTRA_BUTTON);

	std::cout << "\33[2K\rHold OPTIONS" << std::flush;
	OPTIONS_BUTTON = getHeldButton();
	config->setInt("BUTTON_OPTIONS", OPTIONS_BUTTON);

	std::cout << "\33[2K\rHold DPAD UP" << std::flush;
	DPAD_UP = getHeldButton();
	config->setInt("DPAD_UP", DPAD_UP);

	std::cout << "\33[2K\rHold DPAD DOWN" << std::flush;
	DPAD_DOWN = getHeldButton();
	config->setInt("DPAD_DOWN", DPAD_DOWN);

	std::cout << "\33[2K\rHold DPAD LEFT" << std::flush;
	DPAD_LEFT = getHeldButton();
	config->setInt("DPAD_LEFT", DPAD_LEFT);

	std::cout << "\33[2K\rHold DPAD RIGHT" << std::flush;
	DPAD_RIGHT = getHeldButton();
	config->setInt("DPAD_RIGHT", DPAD_RIGHT);


	
	config->flush();
	std::cout << "\n\nPicoTroller button config has been saved!\n" << std::flush;
	
	return true;
}

bool configureAudio(Properties* config){
	std::cout << "The audio device is not currently configured for PicoTroller, would you like to do this now? [y/n]: ";
	std::string response;
	std::getline(std::cin, response);
	if(strcmp(response, "y") != 0 && strcmp(response, "Y") != 0){
		std::cout << "Continuing without audio control..." << std::endl;
		return false;
	}
	
	std::cout << "Enter the name of the audio device. Could be Headphone, or PCM: ";
	std::getline(std::cin, response);
	
	if(strcmp(response, "") == 0){
		std::cout << "Continuing without audio control..." << std::endl;
		return false;
	}
	
	audDevice = response;
	config->set("audioDevice", audDevice); //Could be PCM, Headphone, or maybe something else
	config->flush();

	return true;
}

int main(int argc, char* argv[]) {
    std::cout << "Xemplar PicoTroller v" << VERSION << std::endl;
	
	Properties config;
	config.load(getLocalFile("config.prop"));
	config.addWhenMissing(true);
	audDevice               = config.get("audioDevice", ""); //Could be PCM, Headphone, or maybe something else
	bool initialized        = config.getBool("initialized", false);
	std::string monitorType = config.get("monitor", ""); //0 for unset, 1 for pico, 2 for gpio
    std::string interface   = config.get("interface", "/dev/serial0"); // normally /dev/serial0
	
	SELECT_BUTTON = config.getInt("BUTTON_SELECT", -1);
	START_BUTTON  = config.getInt("BUTTON_START", -1);
	L_BUTTON = config.getInt("BUTTON_L", -1);
	R_BUTTON = config.getInt("BUTTON_R", -1);

	// proper controllerrr
	A_BUTTON = config.getInt("BUTTON_A", -1);
	B_BUTTON = config.getInt("BUTTON_B", -1);
	X_BUTTON = config.getInt("BUTTON_X", -1);
	Y_BUTTON = config.getInt("BUTTON_Y", -1);

	DPAD_UP = config.getInt("DPAD_UP", -1);
	DPAD_DOWN = config.getInt("DPAD_DOWN", -1);
	DPAD_LEFT = config.getInt("DPAD_LEFT", -1);
	DPAD_RIGHT = config.getInt("DPAD_RIGHT", -1);

	OPTIONS_BUTTON = config.getInt("BUTTON_OPTIONS", -1);
	EXTRA_BUTTON = config.getInt("BUTTON_EXTRA", -1);
	
	if(strcmp(monitorType, "") == 0){
		std::cout << "No monitor has been selected, please enter one of the following: gpio, pico (or leave blank to exit)" << std::endl;
		std::getline(std::cin, monitorType);
		
		if(strcmp(monitorType, "gpio") == 0 || strcmp(monitorType, "pico") == 0){
			config.set("monitor", monitorType); 
		} else {
			if(strcmp(monitorType, "") == 0){
				std::cout << "Exiting..." << std::endl;
				return 0;
			} else {
				std::cout << "Invalid option, exiting..." << std::endl;
				return 0;
			}
		}
	}
	
	if(strcmp(monitorType, "gpio") == 0 && 
				(config.getInt("PIN_SELECT", -1) == -1 || 
				 config.getInt("PIN_START",  -1) == -1 || 
				 config.getInt("PIN_UP",     -1) == -1 || 
				 config.getInt("PIN_DOWN",   -1) == -1 || 
				 config.getInt("PIN_LEFT",   -1) == -1 || 
				 config.getInt("PIN_RIGHT",  -1) == -1 || 
				 config.getInt("PIN_L",      -1) == -1 || 
				 config.getInt("PIN_R",      -1) == -1 || 
				 config.getInt("PIN_A",      -1) == -1 || 
				 config.getInt("PIN_B",      -1) == -1 || 
				 config.getInt("PIN_X",      -1) == -1 || 
				 config.getInt("PIN_Y",      -1) == -1)){
		
		
		if(!configureGPIOMonitor(&config)) return 0;
	}
	
	if(strcmp(monitorType, "gpio") == 0){
		monitor = new GpioMonitor(config);
	} else if(strcmp(monitorType, "pico") == 0){
		if(strcmp(interface, "") == 0){
			std::cout << "Serial interface must be defined before running pico monitor. Exiting..." << std::endl;
			return 0;
		}
		monitor = new PicoMonitor(interface);
	}
	
	//remove unnecessary write
	if(!initialized){
		config.setBool("initialized", true);
		config.flush();
	}
	
	manager.setMonitor(monitor);
	manager.initMonitor();

    std::thread controllerThread(controllerLoop);
	
	if(SELECT_BUTTON == -1 || 
		START_BUTTON == -1 || 
		    L_BUTTON == -1 || 
		    R_BUTTON == -1 || 
		    X_BUTTON == -1){
		
		if(!configurePicotroller(&config)) return 0;
	}
	
	if(strcmp(audDevice, "") == 0){
		configureAudio(&config);
	}
	
	controlVolume(0); //Obtains initial volume
	
	overlay.clearScreen();
	overlay.commit();

    // Main loop
    while(true) {
		if(overlay_counter > -1) overlay_counter++;
		if(overlay_counter >= OVERLAY_TIMEOUT) {
			overlay_id = -1;
			overlay_counter = -1;
			overlay.clearScreen();
			overlay.commit();
		}
		
		drawOverlay();
		//10 fps should be enough for overlay, no?
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    controllerThread.join();  // Join the thread before exiting (not reached in this example)
    return 0;
}

std::string getLocalFile(const std::string& filename) {
    // Buffer to store the path of the executable
    char exePath[1024];
    ssize_t count = readlink("/proc/self/exe", exePath, sizeof(exePath));
    if (count == -1) {
        perror("readlink");
        return "";
    }
    exePath[count] = '\0'; // Null-terminate the path

    // Get the directory name of the executable
    std::string dirPath = dirname(exePath);

    // Construct the full file path
    std::string filePath = dirPath + "/" + filename;

    return filePath;
}

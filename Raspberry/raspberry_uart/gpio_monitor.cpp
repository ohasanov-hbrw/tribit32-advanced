#include "gpio_monitor.h"
#include <chrono>
#include <cstdlib>

#define UPDATE_TIME 15000

#define UP_BUTTON 8
#define DOWN_BUTTON 9
#define LEFT_BUTTON 10
#define RIGHT_BUTTON 11

GpioMonitor::GpioMonitor() : GpioMonitor(Properties("gpio.prop")){
	
}

GpioMonitor::GpioMonitor(Properties conf) : Monitor(){
	configs = conf;
	configs.addWhenMissing(true);
	PULL_UP = configs.getBool("PULLUP", PULL_UP);
	
	buttons[0].pin = configs.getInt("PIN_A",      buttons[0].pin);
	buttons[1].pin = configs.getInt("PIN_B",      buttons[1].pin);
	buttons[2].pin = configs.getInt("PIN_X",      buttons[2].pin);
	buttons[3].pin = configs.getInt("PIN_Y",      buttons[3].pin);
	buttons[4].pin = configs.getInt("PIN_R",      buttons[4].pin);
	buttons[5].pin = configs.getInt("PIN_L",      buttons[5].pin);
	buttons[6].pin = configs.getInt("PIN_START",  buttons[6].pin);
	buttons[7].pin = configs.getInt("PIN_SELECT", buttons[7].pin);
	
	buttons[8].pin = configs.getInt("PIN_UP",     buttons[8].pin);
	buttons[9].pin = configs.getInt("PIN_DOWN",   buttons[9].pin);
	buttons[10].pin = configs.getInt("PIN_LEFT",  buttons[10].pin);
	buttons[11].pin = configs.getInt("PIN_RIGHT", buttons[11].pin);
	configs.flush();
}

GpioMonitor::~GpioMonitor(){
	for(int i = 0; i < GPIO_BUTTON_COUNT; i++){
		if(buttons[i].pin == -1) continue;
		gpio.setPullUpDown(buttons[i].pin, PUD_OFF);
	}
}
	
bool GpioMonitor::init(){
	for(int i = 0; i < GPIO_BUTTON_COUNT; i++){
		if(buttons[i].pin == -1) continue;
		gpio.setPullUpDown(buttons[i].pin, PULL_UP ? PUD_UP : PUD_DOWN);
	}
	
	return true;
}

uint64_t GpioMonitor::micros() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return duration;
}

void GpioMonitor::update(){
	if(abs(micros() - updateCounter) > UPDATE_TIME){ //Calc Diffs
		bool needUpdate = false;
		uint8_t button_values[4];
		int16_t axis_values[8];
		
		axis_values[0] = 0;
		axis_values[1] = 0;
		
		for(int i = 0; i < GPIO_BUTTON_COUNT; i++){
			if(buttons[i].pin == -1) continue;
			
			int bit_index = i % 8;
			
			buttons[i].prevState = buttons[i].state;
			
			buttons[i].state = buttons[i].onCount < buttons[i].offCount; //0 is pressed, 1 is not pressed;
			buttons[i].onCount = 0;
			buttons[i].offCount = 0;
			
			button_values[i / 8] |= (buttons[i].state ? 1 : 0) << bit_index;
			
			if(i == UP_BUTTON    && buttons[i].state) {
				axis_values[1] -= 32000;
			}
			if(i == DOWN_BUTTON  && buttons[i].state) axis_values[1] += 32000;
			
			
			if(i == LEFT_BUTTON  && buttons[i].state) {
				axis_values[0] -= 32000;
			}
			if(i == RIGHT_BUTTON && buttons[i].state) axis_values[0] += 32000;
			
			if(buttons[i].prevState != buttons[i].state) needUpdate = true;
		}
		
		if(needUpdate){
			callback(1, 32, button_values, 4, axis_values);
		}
	}
	for(int i = 0; i < GPIO_BUTTON_COUNT; i++){
		if(buttons[i].pin == -1) continue;
		buttons[i].prevState = buttons[i].state;
		
		if(gpio.readPin(buttons[i].pin)){
			buttons[i].onCount++;
		} else {
			buttons[i].offCount++;
		}
			
		gpio.writePin(buttons[i].pin, buttons[i].state ? 1 : 0);
	}
}

void GpioMonitor::request(byte func, unsigned int value){
	
}

bool GpioMonitor::hasFeatures(int features){
	const int have = FEATURE_BACKLIGHT;
	features &= ~have;
	
	return features == 0;
}
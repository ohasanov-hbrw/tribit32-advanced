#ifndef GPIO_MONITOR_H
#define GPIO_MONITOR_H

#include "monitor.h"
#include "properties.h"
#include "GPIO.h"

#define GPIO_BUTTON_COUNT 32

class GpioMonitor : public Monitor{
public:	
	GpioMonitor();
	GpioMonitor(Properties conf);
	~GpioMonitor();
	
	bool init() override;
	void update() override;
	void request(byte func, unsigned int value) override;
	bool hasFeatures(int features) override;
	
private:
	struct Button {
		int index = -1, pin, onCount, offCount;
		bool state, prevState;
	};
	uint64_t updateCounter;

	Button buttons[GPIO_BUTTON_COUNT];
	
	bool PULL_UP = true;
	
	Properties configs;
	GPIO gpio;
	
	
	uint64_t micros();
};

#endif // GPIO_MONITOR_H
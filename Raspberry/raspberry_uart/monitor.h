#ifndef MONITOR_H
#define MONITOR_H

#include <cstdint>
#include <functional>

#define FEATURE_FAN         0x02
#define FEATURE_BATTERY     0x04
#define FEATURE_BACKLIGHT   0x08

using MonitorCallback = std::function<void(unsigned char, unsigned char, const unsigned char*, unsigned char, const short int*)>;
typedef uint8_t byte;

struct AxisData {
  int16_t x, y;
};

struct ControllerData {
  int id;
  int button_count;
  int axis_count;
  bool plugged_in;

  bool button_states[32];
  AxisData axis[4];
};

class Monitor{
public:
	Monitor();
	virtual ~Monitor();
	virtual bool init() = 0;
	virtual void update() = 0;
	virtual void request(byte func, unsigned int value) = 0;
	virtual bool hasFeatures(int features) = 0;
	MonitorCallback callback;
private:
};

#endif // MONITOR_H
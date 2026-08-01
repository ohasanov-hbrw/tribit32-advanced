#define AXIS_DUD A0
#define AXIS_DRL A1
#define BUTTON_B 6
#define BUTTON_START 5
#define BUTTON_SELECT 4
#define BUTTON_A 2
#define BUTTON_MENU 7
#define BUTTON_Y 9
#define BUTTON_X 8
#define BUTTON_VOLUME 10
#define BUTTON_R 11
#define BUTTON_L 12
#define MODE_SWITCH 13
#define BACKLIGHT_PIN 3
#define BATTERY_PIN A2

struct Axis {
  int pins[4];
  int digital_times_on[4];
  int digital_times_off[4];
  boolean analog = false;
  int x, y;
  int prevX, prevY;
};

struct Controller {
  int id;
  int button_count;
  int axis_count;
  boolean plugged_in;

  boolean prev_states[16];
  boolean button_states[16];
  int button_times_on[16];
  int button_times_off[16];
  int button_pins[16];
  Axis axis[4];
};

volatile Controller controllers[2];
volatile int battVal, bacVal = 255;
volatile boolean charging, systemOn;

//#define TOTAL_SHUTDOWN A4
//#define CHARGE_PIN A5

#define REG_3V3 A5
#define REG_5V A4

#define LED_MAX_VALUE 64

#define DEBOUNCE_TIME 20000

#define DEBUG true

long long last_update = -DEBOUNCE_TIME;

#define LED_POWER_ON 1
#define LED_POWER_OFF 2
#define LED_BATTERYLOW 3

#define LED_CHARGING10 4
#define LED_CHARGING25 5
#define LED_CHARGING50 6
#define LED_CHARGING75 7
#define LED_CHARGINGFULL 8

#define BATTERY_FULL 643
#define BATTERY_EMPTY 496

volatile int ledState = LED_BATTERYLOW;
volatile int counter = 0, systemOn_counter;
unsigned long counter_stamp, alive_stamp;

#define counter_delay 1000

byte readBuffer[64];
int readIndex = 0;
boolean reading = false;
long startTime = 0;

#define SERIAL_READ_TIMEOUT 10000
#define ALIVE_TIMEOUT 20000


void calcDebouce();
void readControllers();
float getBatteryPercent();
int getBatteryRaw();
float getBatteryPercent();
void processCommand(int func, int value);
void readSerial();
void sendFullStateRaw(int c);
void add_crc(uint8_t* message, size_t length);
uint16_t crc16_ccitt_xmodem(const uint8_t* data, size_t length);
void sendFullState(int c);
void sendButtonEvent(int controller, int button, boolean state);
void setBrightness(int brightness);

bool enableRaspi;


void setup() {
  enableRaspi = true;

  pinMode(REG_3V3, OUTPUT);
  pinMode(REG_5V, OUTPUT);
  digitalWrite(REG_3V3, LOW);
  digitalWrite(REG_5V, LOW);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(MODE_SWITCH, OUTPUT);
  digitalWrite(MODE_SWITCH, LOW);
  //Setup Controller 1 (0)
  controllers[1].plugged_in = true;
  controllers[1].axis_count = 0;

  int c0_buttons[14] = { AXIS_DUD, AXIS_DUD, AXIS_DRL, AXIS_DRL, BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y, BUTTON_START, BUTTON_SELECT, BUTTON_MENU, BUTTON_VOLUME, BUTTON_R, BUTTON_L };
  controllers[1].button_count = 14;
  for (int i = 0; i < controllers[1].button_count; i++) {
    controllers[1].button_pins[i] = c0_buttons[i];
  }

  controllers[0].button_count = 8;
  controllers[0].axis_count = 1;
  controllers[0].button_states[0] = false;

  //Setup Controller Pins
  for (int c = 0; c < 2; c++) {
    controllers[c].id = c + 1;
    for (int i = 0; i < controllers[c].button_count; i++) {
      if (c != 0)
        pinMode(controllers[c].button_pins[i], INPUT);
    }
  }

  //pinMode(LED_PIN, OUTPUT);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  //pinMode(CHARGE_PIN, INPUT);

  systemOn = true;
  setBrightness(255);

  //Serial1.setRX(0);
  //Serial1.setTX(1); //???
  Serial.begin(57600);

  


  pinMode(BACKLIGHT_PIN, OUTPUT);

  // TCCR2A: Fast PWM, non-inverting on OC2B (Pin 3)
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);

  // TCCR2B: Prescaler set to 32
  // CS21 and CS20 = 1
  TCCR2B = _BV(CS21) | _BV(CS20);
  //Serial.begin(9600);
  delay(100);
  if (!digitalRead(BUTTON_A) || !digitalRead(BUTTON_B) || !digitalRead(BUTTON_X) || !digitalRead(BUTTON_Y)) {
    
    digitalWrite(MODE_SWITCH, LOW);
    digitalWrite(REG_3V3, LOW);
    digitalWrite(REG_5V, HIGH);
    
  }
  else{
    digitalWrite(MODE_SWITCH, HIGH);
    digitalWrite(REG_3V3, HIGH);
    digitalWrite(REG_5V, LOW);
    enableRaspi = false;
  }
}


void loop() {
  if (enableRaspi) {
    digitalWrite(MODE_SWITCH, LOW);
    digitalWrite(REG_3V3, LOW);
    digitalWrite(REG_5V, HIGH);
    readSerial();
    systemOn = (abs((long)(millis() - alive_stamp)) < ALIVE_TIMEOUT);
    setBrightness(bacVal);

    if (charging) {
      if (getBatteryPercent() > 0.9) {
        ledState = LED_CHARGINGFULL;
      } else if (getBatteryPercent() > 0.75) {
        ledState = LED_CHARGING75;
      } else if (getBatteryPercent() > 0.50) {
        ledState = LED_CHARGING50;
      } else if (getBatteryPercent() > 0.25) {
        ledState = LED_CHARGING25;
      } else {
        ledState = LED_CHARGING10;
      }
    } else {
      if (getBatteryPercent() > 0.25) {
        ledState = systemOn ? LED_POWER_ON : LED_POWER_OFF;
      } else {
        ledState = LED_BATTERYLOW;
      }
    }

    int batt = getBatteryRaw();

    if (ledState == LED_POWER_ON) {
      //analogWrite(LED_PIN, LED_MAX_VALUE);
      counter = 0;
    }
    if (ledState == LED_POWER_OFF) {
      //digitalWrite(LED_PIN, LOW);
      counter = 0;
    }
    if (ledState == LED_CHARGING10) {
      float value = 0;
      if (counter < 600) {
        value = 0.0;
      } else if (counter < 600) {
        value = (counter - 800) / (float)200;
      } else {
        value = 1.0 - ((counter - 800) / (float)200);
      }

      //analogWrite(LED_PIN, (int)(value * LED_MAX_VALUE));
      if (counter >= 1000) counter = 0;
    }
    if (ledState == LED_CHARGING25) {
      float value = 0;
      if (counter < 600) {
        value = 0.0;
      } else if (counter < 700) {
        value = (counter - 600) / (float)100;
      } else if (counter < 800) {
        value = 1.0;
      } else {
        value = 1.0 - ((counter - 800) / (float)200);
      }

      //analogWrite(LED_PIN, (int)(value * LED_MAX_VALUE));
      if (counter >= 1000) counter = 0;
    }
    if (ledState == LED_CHARGING50) {
      float value = 0;
      if (counter < 400) {
        value = 0.0;
      } else if (counter < 600) {
        value = (counter - 400) / (float)200;
      } else if (counter < 800) {
        value = 1.0;
      } else {
        value = 1.0 - ((counter - 800) / (float)200);
      }

      //analogWrite(LED_PIN, (int)(value * LED_MAX_VALUE));
      if (counter >= 1000) counter = 0;
    }
    if (ledState == LED_CHARGING75) {
      float value = 0;
      if (counter < 200) {
        value = 0.0;
      } else if (counter < 400) {
        value = (counter - 200) / (float)200;
      } else if (counter < 600) {
        value = 1.0;
      } else {
        value = 1.0 - ((counter - 800) / (float)200);
      }

      //analogWrite(LED_PIN, (int)(value * LED_MAX_VALUE));
      if (counter >= 1000) counter = 0;
    }
    if (ledState == LED_CHARGINGFULL) {
      float value = 0;
      if (counter < 200) {
        value = counter / (float)200;
      } else if (counter < 800) {
        value = 1.0;
      } else {
        value = 1.0 - ((counter - 800) / (float)200);
      }

      //analogWrite(LED_PIN, (int)(value * LED_MAX_VALUE));
      if (counter >= 1000) counter = 0;
    }
    if (ledState == LED_BATTERYLOW) {
      float ratio = 4095 / 3.3;
      float voltage = 4095 * ratio + 1.0;  //getBatteryValue() * ratio + 1F;

      if (batt > BATTERY_FULL) batt = BATTERY_FULL;  //Maximum that should be read
      batt -= BATTERY_EMPTY;                         //Minimum voltage that should be read
      if (batt < 100) batt = 100;

      float percent = (float)batt / (BATTERY_FULL - BATTERY_EMPTY);

      int delay = percent * percent * 2000;  //Scale delay down so that it has more impact

      if (counter < delay) {
        //digitalWrite(LED_PIN, LOW);
      }
      if (counter > delay) {
        //digitalWrite(LED_PIN, HIGH);
      }
      if (counter > (delay << 1)) counter = 0;
    }

    if (abs((long long)(micros() - counter_stamp)) < counter_delay) {
      //return;
    }
    counter_stamp = micros();
    counter++;



    battVal = analogRead(BATTERY_PIN);
    charging = battVal > 680 ? true : false;

    readControllers();
    if (abs((micros() - last_update)) > DEBOUNCE_TIME) {
      last_update = micros();
      //Serial.println("calc debounce");
      calcDebouce();
    }
  }
  else{
    digitalWrite(MODE_SWITCH, HIGH);
    digitalWrite(REG_3V3, HIGH);
    digitalWrite(REG_5V, LOW);
    delay(100);
  }
}

void readControllers() {
  int c = 1;
  for (int i = 0; i < controllers[c].button_count; i++) {
    boolean state = !digitalRead(controllers[c].button_pins[i]);
    if (i == 0) {
      state = (analogRead(controllers[c].button_pins[i]) > 700) ? 1 : 0;
    } else if (i == 1) {
      state = (analogRead(controllers[c].button_pins[i]) > 250 && analogRead(controllers[c].button_pins[i]) < 600) ? 1 : 0;
    } else if (i == 3) {
      state = (analogRead(controllers[c].button_pins[i]) > 700) ? 1 : 0;
    } else if (i == 2) {
      state = (analogRead(controllers[c].button_pins[i]) > 250 && analogRead(controllers[c].button_pins[i]) < 600) ? 1 : 0;
    }

    if (state) {  //Read Button States
      controllers[c].button_times_on[i]++;
    } else {
      controllers[c].button_times_off[i]++;
    }
  }
}

void calcDebouce() {
  for (int c = 0; c < 2; c++) {
    boolean changed = false;

    for (int i = 0; i < controllers[c].button_count; i++) {
      controllers[c].button_states[i] = controllers[c].button_times_on[i] > controllers[c].button_times_off[i];
      //if(i == 2) {Serial.print(controllers[c].button_times_on[i]); Serial.print(" "); Serial.println(controllers[c].button_times_off[i]);}
      controllers[c].button_times_on[i] = 0;
      controllers[c].button_times_off[i] = 0;
      changed |= (controllers[c].button_states[i] != controllers[c].prev_states[i]);
      controllers[c].prev_states[i] = controllers[c].button_states[i];
    }
    if (changed) sendFullState(c);
  }
}


void sendButtonEvent(int controller, int button, boolean state) {
  if (DEBUG) {
    //Serial.print("Joystick ");
    //Serial.print(controller);
    //Serial.print(": Button ");
    //Serial.print(button);
    //Serial.print(" was ");
    //Serial.println(state ? "pressed." : "released.");/**/
  }
}

void sendFullState(int c) {
  sendFullStateRaw(c);
  if (true) return;

  int count = controllers[c].button_count;


  //Serial.print("{");
  //Serial.print(c);
  //Serial.print(" ");

  byte data;
  data = count & 0xFF;
  //Serial.print(data);
  //Serial.print(" ");

  data = 0;
  for (int i = 0; i < count; i++) {
    if (i % 8 == 0) data = 0;
    data += ((controllers[c].button_states[i] ? 1 : 0) << 7) >> (i % 8);

    if (i % 8 == 7 || i == count - 1) {
      //if(data >> 4 == 0) Serial.print('_');
      //Serial.print(data, HEX);
    }
    //Serial.print(controllers[c].button_states[i] ? 1 : 0);
  }

  count = controllers[c].axis_count;
  data = count & 0xFF;
  //Serial.print("  ");
  //Serial.print(data);
  //Serial.print(" ");

  for (int i = 0; i < count; i++) {
    byte up, low;
    up = (controllers[c].axis[i].x >> 8) & 0xFF;
    low = (controllers[c].axis[i].x) & 0xFF;

    //if(up < 0x10) Serial.print("0");
    //Serial.print(up, HEX);
    //if(low < 0x10) Serial.print("0");
    //Serial.print(low, HEX);


    up = (controllers[c].axis[i].y >> 8) & 0xFF;
    low = (controllers[c].axis[i].y) & 0xFF;

    //if(up < 0x10) Serial.print("0");
    //Serial.print(up, HEX);
    //if(low < 0x10) Serial.print("0");
    //Serial.print(low, HEX);
  }

  //Serial.println("}");
}

uint16_t crc16_ccitt_xmodem(const uint8_t* data, size_t length) {
  uint16_t crc = 0x0000;               // Initial value
  const uint16_t polynomial = 0x1021;  // Polynomial used in CRC-16-CCITT

  for (size_t i = 0; i < length; ++i) {
    crc ^= (data[i] << 8);  // XOR byte into the high order byte of CRC

    for (uint8_t j = 0; j < 8; ++j) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ polynomial;
      } else {
        crc = crc << 1;
      }
    }
  }

  return crc;
}

void add_crc(uint8_t* message, size_t length) {
  uint16_t crc = crc16_ccitt_xmodem(message, length);
  message[length] = crc & 0xFF;             // Low byte of CRC
  message[length + 1] = (crc >> 8) & 0xFF;  // High byte of CRC
}

void sendFullStateRaw(int c) {
  int count = controllers[c].button_count;
  byte msg[32];
  int msg_len = 0;

  msg[msg_len++] = '{';
  msg[msg_len++] = c;


  byte data;
  data = count & 0xFF;
  msg[msg_len++] = data;

  msg_len = 3;

  data = 0;
  for (int i = 0; i < count; i++) {
    if (i % 8 == 0) data = 0;
    data += (controllers[c].button_states[i] ? 1 : 0) << (i % 8);

    if (i % 8 == 7 || i == count - 1) {
      msg[msg_len++] = data;
    }
  }

  count = controllers[c].axis_count;
  data = count & 0xFF;
  msg[msg_len++] = data;

  for (int i = 0; i < count; i++) {
    byte up, low;
    up = (controllers[c].axis[i].x >> 8) & 0xFF;
    low = (controllers[c].axis[i].x) & 0xFF;

    msg[msg_len++] = up;
    msg[msg_len++] = low;

    up = (controllers[c].axis[i].y >> 8) & 0xFF;
    low = (controllers[c].axis[i].y) & 0xFF;

    msg[msg_len++] = up;
    msg[msg_len++] = low;
  }
  msg[msg_len++] = '}';
  add_crc(msg, msg_len);

  Serial.write(msg, msg_len + 2);  //Print and add crc length;
}



void readSerial() {
  if (abs((int)(micros() - startTime)) > SERIAL_READ_TIMEOUT) {
    reading = false;
  }
  //digitalWrite(BACKLIGHT_PIN, reading);
  while (Serial.available() > 0) {
    alive_stamp = millis();
    byte b = 0;
    int r = Serial.read();
    b = (byte)r;
    //Serial.println((char)b);
    if (r == -1) return;


    if (!reading && b == '{') {
      //digitalWrite(BACKLIGHT_PIN, true);
      reading = true;
      readIndex = 0;
      startTime = micros();
    }

    if (reading) {
      readBuffer[readIndex] = b;
      readIndex++;

      startTime = micros();
    }

    if (reading && b == '}' && readIndex > 5) {
      reading = false;
      int func = readBuffer[1];
      int data = readBuffer[2];
      data |= readBuffer[3] << 8;
      data |= readBuffer[4] << 16;
      data |= readBuffer[5] << 24;
      //Serial.print(readBuffer[0]);
      //Serial.print(readBuffer[1]);
      //Serial.print(readBuffer[2]);
      //Serial.print(readBuffer[3]);
      //Serial.print(readBuffer[4]);
      //Serial.print(readBuffer[5]);
      processCommand(func, data);
    }
  }
}

void processCommand(int func, int value) {
  switch (func) {
    case 1:  //Read Controller value, index is supplied number + 1;
      sendFullState(1);
      break;
    case 2:  //Read Battery Value
      controllers[0].button_states[0] = charging;
      controllers[0].button_states[1] = false;
      controllers[0].button_states[2] = false;
      controllers[0].axis[0].x = controllers[0].axis[0].y = getBatteryRaw();
      sendFullState(0);
      break;

    case 4:  //Read Fan Value
      controllers[0].button_states[0] = charging;
      controllers[0].button_states[1] = true;
      controllers[0].button_states[2] = false;
      controllers[0].axis[0].x = controllers[0].axis[0].y = 0;
      sendFullState(0);
      break;
    case 5:  //Write Fan Value
      controllers[0].button_states[0] = charging;
      controllers[0].button_states[1] = true;
      controllers[0].button_states[2] = false;
      controllers[0].axis[0].x = controllers[0].axis[0].y = 0;
      sendFullState(0);
      break;

    case 6:  //Read Backlight Value
      controllers[0].button_states[0] = charging;
      controllers[0].button_states[1] = false;
      controllers[0].button_states[2] = true;
      controllers[0].axis[0].x = controllers[0].axis[0].y = bacVal;
      sendFullState(0);
      break;
    case 7:  //Write Backlight Value
      setBrightness(value);
      controllers[0].button_states[0] = charging;
      controllers[0].button_states[1] = false;
      controllers[0].button_states[2] = true;
      controllers[0].axis[0].x = controllers[0].axis[0].y = bacVal;
      sendFullState(0);
      break;
  }
}

int getBatteryRaw() {
  return battVal;  //battval
}

float getBatteryPercent() {
  int batt = getBatteryRaw();
  if (batt > BATTERY_FULL) batt = BATTERY_FULL;  //Maximum that should be read
  batt -= BATTERY_EMPTY;                         //Minimum voltage that should be read
  if (batt < 1) batt = 1;

  return (float)batt / (BATTERY_FULL - BATTERY_EMPTY);
}


void setBrightness(int brightness) {
  if (!systemOn) {
    setLCDBacklight(3);
    return;
  }

  if (brightness >= 255) {
    bacVal = 255;
    setLCDBacklight(bacVal);
  } else {
    bacVal = brightness;
    if (bacVal < 2) bacVal = 2;
    //digitalWrite(BACKLIGHT_PIN, 0);
    //analogWrite(BACKLIGHT_PIN, brightness);
    setLCDBacklight(bacVal);
  }
}

void setLCDBacklight(uint8_t duty) {


  // Set brightness (0-255)
  OCR2B = duty;
}

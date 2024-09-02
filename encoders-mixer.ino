#include <Joystick.h>  // https://github.com/MHeironimus/ArduinoJoystickLibrary

#define CCW 0b010010  // knob turn CCW pattern
#define CW 0b100001   // knob turn CW pattern
#define DEF 0b11      // default / base value of encoder signal

struct Encoder {
  int keyPin;
  int s2Pin;
  int s1Pin;
  int encBtnJoy;
  int encCCWJoy;
  int encCWJoy;
  byte prevPattern;
  byte pattern;
  bool keyPushed;
};

const int numEncoders = 4;
Encoder encoders[numEncoders] = {
  { 9, 8, 10, 0, 1, 2, 0, 0, false },    // Encoder 4
  { 14, 15, 16, 3, 4, 5, 0, 0, false },  // Encoder 3
  { 7, 5, 6, 6, 7, 8, 0, 0, false },     // Encoder 2
  { 4, 2, 3, 9, 10, 11, 0, 0, false }    // Encoder 1
};

bool VOLUM_MODE = true;
int SWITCH_MODE_INDEX = 3;

//JOYSTICK_TYPE_JOYSTICK
//JOYSTICK_TYPE_GAMEPAD
//JOYSTICK_TYPE_MULTI_AXIS
Joystick_ joystick(0x03, JOYSTICK_TYPE_JOYSTICK, 12, 0, false, false, false, false, false, false, false, false, false, false, false);

int VOLUMS[4] = { 500, 500, 500, 500 };

void setup() {
  for (int i = 0; i < numEncoders; i++) {
    pinMode(encoders[i].keyPin, INPUT);
    pinMode(encoders[i].s2Pin, INPUT);
    pinMode(encoders[i].s1Pin, INPUT);
  }

  Serial.begin(9600);
  joystick.begin(true);
}

void loop() {
  for (int i = 0; i < numEncoders; i++) {
    readEncoder(i);
  }
  sendSliderValues();
  // Serial.println(String((int)VOLUMS[0])+"|"+String((int)VOLUMS[1])+"|"+String((int)VOLUMS[2])+"|"+String((int)VOLUMS[3]));
}


void pushJoyButton(int buttonId, int encoderIndex) {
  joystick.pressButton(buttonId);
  delay(30);
  joystick.releaseButton(buttonId);
}

void readEncoder(int encoderIndex) {
  Encoder &encoder = encoders[encoderIndex];

  byte aktS1 = digitalRead(encoder.s1Pin);
  byte aktS2 = digitalRead(encoder.s2Pin);
  byte aktKey = !digitalRead(encoder.keyPin);

  if (encoder.keyPushed != aktKey) {
    encoder.keyPushed = aktKey;
    // Serial.println(encoder.keyPushed ? "Key push, Encoder:" + String(encoderIndex) : "Key release, Encoder:" + String(encoderIndex));
    if (encoder.keyPushed) {
      joystick.pressButton(encoder.encBtnJoy);
    } else {
      joystick.releaseButton(encoder.encBtnJoy);
      if (encoderIndex == SWITCH_MODE_INDEX) {
        VOLUM_MODE = !VOLUM_MODE;
      }
    }
  }

  byte aktPattern = (aktS1 << 1) | aktS2;
  if (aktPattern != encoder.prevPattern) {
    encoder.prevPattern = aktPattern;
    if (aktPattern != DEF) {
      encoder.pattern = (encoder.pattern << 2) | aktPattern;
    } else {
      if (encoder.pattern == CW) {
        // Serial.println("Turn right, Encoder:" + String(encoderIndex));
        if (VOLUM_MODE) {
          volumeUp(encoderIndex);
        } else {
          pushJoyButton(encoder.encCWJoy, encoderIndex);
        }
      } else if (encoder.pattern == CCW) {
        // Serial.println("Turn left, Encoder:" + String(encoderIndex));
        if (VOLUM_MODE) {
          volumeDown(encoderIndex);
        } else {
          pushJoyButton(encoder.encCCWJoy, encoderIndex);
        }
      }
      encoder.pattern = 0;
    }
  }
}

void volumeUp(int ind) {
  VOLUMS[ind] += 50;
  if (VOLUMS[ind] > 1000) {
    VOLUMS[ind] = 1000;
  }
}

void volumeDown(int ind) {
  VOLUMS[ind] -= 50;
  if (VOLUMS[ind] < 0) {
    VOLUMS[ind] = 0;
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < numEncoders; i++) {
    builtString += String((int)VOLUMS[i]);

    if (i < numEncoders - 1) {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
}

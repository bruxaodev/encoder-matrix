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
  { 2, 3, 4, 0, 1, 2, 0, 0, false },  // Encoder 1
  { 5, 6, 7, 0, 1, 2, 0, 0, false },   // Encoder 2
  { 15, 16, 14, 0, 1, 2, 0, 0, false },   // Encoder 3
  { 9, 8, 10, 0, 1, 2, 0, 0, false }   // Encoder 4
};

//JOYSTICK_TYPE_JOYSTICK
//JOYSTICK_TYPE_GAMEPAD
//JOYSTICK_TYPE_MULTI_AXIS
Joystick_ joystick[numEncoders] = {
  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, 3, 0, false, false, false, false, false, false, false, false, false, false, false),
  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD, 3, 0, false, false, false, false, false, false, false, false, false, false, false),
  Joystick_(0x05, JOYSTICK_TYPE_GAMEPAD, 3, 0, false, false, false, false, false, false, false, false, false, false, false),
  Joystick_(0x06, JOYSTICK_TYPE_GAMEPAD, 3, 0, false, false, false, false, false, false, false, false, false, false, false),
};

void setup() {
  for (int i = 0; i < numEncoders; i++) {
    pinMode(encoders[i].keyPin, INPUT);
    pinMode(encoders[i].s2Pin, INPUT);
    pinMode(encoders[i].s1Pin, INPUT);
  }

  Serial.begin(9600);
  for (int i = 0; i < numEncoders; i++) {
    joystick[i].begin(true);
  }
}

void loop() {
  for (int i = 0; i < numEncoders; i++) {
    readEncoder(i);
  }
}


void pushJoyButton(int buttonId, int encoderIndex) {
  joystick[encoderIndex].pressButton(buttonId);
  delay(100);
  joystick[encoderIndex].releaseButton(buttonId);
}

void readEncoder(int encoderIndex) {
  Encoder &encoder = encoders[encoderIndex];

  byte aktS1 = digitalRead(encoder.s1Pin);
  byte aktS2 = digitalRead(encoder.s2Pin);
  byte aktKey = !digitalRead(encoder.keyPin);

  if (encoder.keyPushed != aktKey) {
    encoder.keyPushed = aktKey;
    Serial.println(encoder.keyPushed ? "Key push, Encoder:" + String(encoderIndex) : "Key release, Encoder:" + String(encoderIndex));
    if (encoder.keyPushed) {
      joystick[encoderIndex].pressButton(encoder.encBtnJoy);
    } else {
      joystick[encoderIndex].releaseButton(encoder.encBtnJoy);
    }
  }

  byte aktPattern = (aktS1 << 1) | aktS2;
  if (aktPattern != encoder.prevPattern) {
    encoder.prevPattern = aktPattern;
    if (aktPattern != DEF) {
      encoder.pattern = (encoder.pattern << 2) | aktPattern;
    } else {
      if (encoder.pattern == CW) {
        Serial.println("Turn right, Encoder:" + String(encoderIndex));
        pushJoyButton(encoder.encCWJoy, encoderIndex);
      } else if (encoder.pattern == CCW) {
        Serial.println("Turn left, Encoder:" + String(encoderIndex));
        pushJoyButton(encoder.encCCWJoy, encoderIndex);
      }
      encoder.pattern = 0;
    }
  }
}
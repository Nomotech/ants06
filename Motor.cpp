#include <Arduino.h>
#include "Motor.hpp"

Motor::Motor(int mA, int mB, int pwm, int cha, volatile int& en) : enc(en){
  this->motorA = mA;
  this->motorB = mB;
  this->pwmPin = pwm;
  this->channel = cha;
  this->enc = en;
}

void Motor::setup() {
  pinMode(pwmPin, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  ledcSetup(channel, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(pwmPin, channel);
}

// value -1.0 ~ 1.0
void Motor::rotate(float value) {
  // Serial.printf("motor : %f\n", value);
  if (value == 0.0) {
    // digitalWrite(motorA, LOW);
    // digitalWrite(motorB, LOW);
    stop();
  } else if (value > 0.0) {
    ledcWrite(channel, (int)(value * VALUE_MAX));
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  } else if (value < 0.0 ){
    ledcWrite(channel, (int)(-value * VALUE_MAX));
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
  }
}

void Motor::stop() {
  ledcWrite(channel, 255);
  digitalWrite(motorA, HIGH);
  digitalWrite(motorB, HIGH);
}

void Motor::encoderReset() {
  enc = 0;
};

int Motor::count() {
  return enc;
}

bool Motor::setAngle(int ang) {
  if (enc > ang + margin) {
    rotate(-1.0);
    delay(1);
  } else if (enc < ang - margin) {
    rotate(1.0);
    delay(1);
  } else {
    stop();
    return true;
  }
  return false;
}

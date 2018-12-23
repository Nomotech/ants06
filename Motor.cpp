#include <Arduino.h>
#include "Motor.hpp"

Motor::Motor(int mA, int mB, int pwm, int cha) {
  this->motorA = mA;
  this->motorB = mB;
  this->channel = cha;
  this->pwmPin = pwm;
}

void Motor::setup() {
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  ledcSetup(channel, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(pwmPin, channel);
}

// value -1.0 ~ 1.0
void Motor::rotate(float value) {
  Serial.printf("motor : %f\n", value);
  if (value == 0.0) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);
  }else if (value > 0.0) {
    ledcWrite(channel, (int)(value * VALUE_MAX));
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
  } else if (value < 0.0 ){
    ledcWrite(channel, (int)(-value * VALUE_MAX));
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  }
}

void Motor::stop() {
  ledcWrite(channel, 255);
  digitalWrite(motorA, HIGH);
  digitalWrite(motorB, HIGH);
}

#include "Arduino.h"

void IRAM_ATTR encAHandle();
void IRAM_ATTR encBHandle();
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
volatile int enc1A;
volatile int enc1B;
volatile int enc2A;
volatile int enc2B;
volatile int enc3A;
volatile int enc3B;

void IRAM_ATTR enc1AHandle() {
  int a = digitalRead(MOTOR1_EA);
  int b = digitalRead(MOTOR1_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(a != enc1A) {
    if (a == b) encCounter1+=1;
    else        encCounter1-=1;
  }
  enc1A = a;
  enc1B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("A a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void IRAM_ATTR enc1BHandle() {
  int a = digitalRead(MOTOR1_EA);
  int b = digitalRead(MOTOR1_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(b != enc1B) {
    if (b == a) encCounter1-=1;
    else        encCounter1+=1;
  }
  enc1A = a;
  enc1B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("B a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void IRAM_ATTR enc2AHandle() {
  int a = digitalRead(MOTOR2_EA);
  int b = digitalRead(MOTOR2_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(a != enc2A) {
    if (a == b) encCounter2+=1;
    else        encCounter2-=1;
  }
  enc2A = a;
  enc2B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("A a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void IRAM_ATTR enc2BHandle() {
  int a = digitalRead(MOTOR2_EA);
  int b = digitalRead(MOTOR2_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(b != enc2B) {
    if (a == b) encCounter2-=1;
    else        encCounter2+=1;
  }
  enc2A = a;
  enc2B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("A a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void IRAM_ATTR enc3AHandle() {
  int a = digitalRead(MOTOR3_EA);
  int b = digitalRead(MOTOR3_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(a != enc3A) {
    if (a == b) encCounter3+=1;
    else        encCounter3-=1;
  }
  enc3A = a;
  enc3B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("A a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void IRAM_ATTR enc3BHandle() {
  int a = digitalRead(MOTOR3_EA);
  int b = digitalRead(MOTOR3_EB);
  portENTER_CRITICAL_ISR(&mux);
  if(b != enc3B) {
    if (a == b) encCounter3-=1;
    else        encCounter3+=1;
  }
  enc3A = a;
  enc3B = b;
  portEXIT_CRITICAL_ISR(&mux);
  // Serial.printf("A a : %d, b : %d, enc1 : %d\n", enc1A, enc1B, encCounter1);
}

void encodersSetup () {
  pinMode(MOTOR1_EA, INPUT_PULLUP);
  pinMode(MOTOR1_EB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTOR1_EA), enc1AHandle, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR1_EB), enc1BHandle, CHANGE);

  pinMode(MOTOR2_EA, INPUT_PULLUP);
  pinMode(MOTOR2_EB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTOR2_EA), enc2AHandle, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR2_EB), enc2BHandle, CHANGE);

  pinMode(MOTOR3_EA, INPUT_PULLUP);
  pinMode(MOTOR3_EB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTOR3_EA), enc3AHandle, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTOR3_EB), enc3BHandle, CHANGE);
}

#include "WSLED.hpp"
#include "Motor.hpp"

#define MOTOR_1A  33
#define MOTOR_1B  25
#define MOTOR_1PWM  32
#define MOTOR_2A  2
#define MOTOR_2B  4
#define MOTOR_2PWM  15
#define MOTOR_3A  19
#define MOTOR_3B  21
#define MOTOR_3PWM  18
#define LED 23
#define LED_NUM 6

const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
const int VALUE_MAX = 255;      // PWMの最大値

WSLED wsled(LED, LED_NUM);
Motor motor1(MOTOR_1A, MOTOR_1B, MOTOR_1PWM, 1);
Motor motor2(MOTOR_2A, MOTOR_2B, MOTOR_2PWM, 2);
Motor motor3(MOTOR_3A, MOTOR_3B, MOTOR_3PWM, 3);

void setup() {
  Serial.begin(115200);
  motor1.setup();
  motor2.setup();
  motor3.setup();
}

void loop() {
  for (int i = 0; i < 360; i += 10) {
    for (int count = 0; count < LED_NUM; count++) {
      wsled.show(count, i);
      delay(10);
    }
  }
  for (int i = 0; i < 100; i++) {
    float v = i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(v);
    motor3.rotate(v);
    delay(10);
  }
  for (int i = 100; i > 0; i--) {
    float v = i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(v);
    motor3.rotate(v);
    delay(10);
  }
  for (int i = 0; i < 100; i++) {
    float v = -i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(v);
    motor3.rotate(v);
    delay(10);
  }
  for (int i = 100; i > 0; i--) {
    float v = -i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(v);
    motor3.rotate(v);
    delay(10);
  }

  for (int i = 0; i < 100; i++) {
    float v = i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(-v);
    motor3.rotate(0);
    delay(10);
  }
  for (int i = 100; i > 0; i--) {
    float v = i / 100.0f;
    motor1.rotate(v);
    motor2.rotate(-v);
    motor3.rotate(0);
    delay(10);
  }

  for (int i = 0; i < 100; i++) {
    float v = i / 100.0f;
    motor1.rotate(-v);
    motor2.rotate(0);
    motor3.rotate(v);
    delay(10);
  }
  for (int i = 100; i > 0; i--) {
    float v = i / 100.0f;
    motor1.rotate(-v);
    motor2.rotate(0);
    motor3.rotate(v);
    delay(10);
  }

  for (int i = 0; i < 100; i++) {
    float v = i / 100.0f;
    motor1.rotate(0);
    motor2.rotate(v);
    motor3.rotate(-v);
    delay(10);
  }
  for (int i = 100; i > 0; i--) {
    float v = i / 100.0f;
    motor1.rotate(0);
    motor2.rotate(v);
    motor3.rotate(-v);
    delay(10);
  }
  // for (int i = 0; i < 50; i++) {
  //   float v = 1.0;
  //   motor1.rotate(v);
  //   motor2.rotate(v);
  //   motor3.rotate(v);
  //   delay(50);
  //   v = -1.0;
  //   motor1.rotate(v);
  //   motor2.rotate(v);
  //   motor3.rotate(v);
  //   delay(50);
  // }
  // for (int i = 0; i < 50; i++) {
  //   float v = 1.0;
  //   motor1.rotate(v);
  //   motor2.rotate(v);
  //   motor3.rotate(v);
  //   delay(30);
  //   v = -1.0;
  //   motor1.rotate(v);
  //   motor2.rotate(v);
  //   motor3.rotate(v);
  //   delay(30);
  // }
  motor1.stop();
  motor2.stop();
  motor3.stop();
  delay(1000);
}

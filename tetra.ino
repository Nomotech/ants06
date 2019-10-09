#include "freertos/task.h"
#include <vector>
#include <Wire.h>
#include <FastLED.h>
#include "Motor.hpp"
#include "Icm.hpp"
#include "Buzzer.hpp"
#include "ColorSensor.hpp"

// define pin assign Ver2.0
#define MOTOR1_OA   33
#define MOTOR1_OB   12
#define MOTOR1_EA   35
#define MOTOR1_EB   34
#define MOTOR1_PWM  32

#define MOTOR2_OA   2
#define MOTOR2_OB   4
#define MOTOR2_EA   36
#define MOTOR2_EB   39
#define MOTOR2_PWM  15

#define MOTOR3_OA   19
#define MOTOR3_OB   21
#define MOTOR3_EA   18
#define MOTOR3_EB   17
#define MOTOR3_PWM  5

#define SW 22
#define BUZZER 16
#define LED 23

#define CLK 14
#define MISO 26
#define MOSI 27
#define SS 0

#define SDA 25
#define SCL 13

// define pin assign Ver1.0
// #define MOTOR1_OA   33
// #define MOTOR1_OB   25
// #define MOTOR1_EA   35
// #define MOTOR1_EB   34
// #define MOTOR1_PWM  32

// #define MOTOR2_OA   2
// #define MOTOR2_OB   4
// #define MOTOR2_EA   36
// #define MOTOR2_EB   39
// #define MOTOR2_PWM  15

// #define MOTOR3_OA   19
// #define MOTOR3_OB   21
// #define MOTOR3_EA   5
// #define MOTOR3_EB   17
// #define MOTOR3_PWM  18

// #define SW 22
// #define BUZZER 16
// #define LED 23

// #define CLK 14
// #define MISO 26
// #define MOSI 27
// #define SS 0

// #define SDA 12
// #define SCL 13

//----------------------------< global variable >----------------------------
const int LED_NUM = 6;          // WS2813 の個数
const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 1000; // 周波数(Hz)
const int VALUE_MAX = 255;      // PWMの最大値
std::vector<note_t> song;
volatile int swCount = -1;      // switch count
volatile bool swFlag = false;
volatile int encCounter1 = 0;
volatile int encCounter2 = 0;
volatile int encCounter3 = 0;

// task list
TaskHandle_t routineTaskHandle;
TaskHandle_t motorTaskHandle;

Motor motor1(MOTOR1_OA, MOTOR1_OB, MOTOR1_PWM, 1, encCounter1);
Motor motor2(MOTOR2_OA, MOTOR2_OB, MOTOR2_PWM, 2, encCounter2);
Motor motor3(MOTOR3_OA, MOTOR3_OB, MOTOR3_PWM, 3, encCounter3);
// ICM icm(CLK, MISO, MOSI, SS);
Buzzer buzzer(BUZZER, 0);
ColorSensor colorSensor;
CRGB leds[LED_NUM];

void colorCheck() {
  // color read
  colorSensor.delay();
  // センサのデータ用レジスタ(赤、緑、青、赤外)をまとめてリード
  if (colorSensor.update()) {
    int r = colorSensor.getRed();
    int g = colorSensor.getGreen();
    int b = colorSensor.getBlue();
    int ir = colorSensor.getIR();
    float coef = 100.0f / (r + g + b);
    if (swCount % 6 == 1) {
      for (int count = 0; count < LED_NUM; count++) leds[count] = CRGB(255, 255, 255);
    } else {
      for (int count = 0; count < LED_NUM; count++) leds[count] = CRGB(0, 0, 0);
    }
    // leds[0] = CRGB(r * coef, g * coef, b * coef);
    FastLED.show();
    FastLED.delay(10);
    Serial.printf("rgb : (%d, %d, %d), IR : %d  R : %f\n", r, g, b, ir, coef);
  } else {
    Serial.println("update failed");
  }
  delay(10);
}

void routineTask(void* arg) {
  while(1) {
    if (swFlag) {
      swFlag = false;
      swCount = (swCount + 1) % 6;
      Serial.printf("swCount : %d\n", swCount);
      buzzer.play(song);
      for (int count = 0; count < LED_NUM; count++) leds[count] = CRGB(0, 0, 0);
      leds[swCount] = CRGB(100, 100, 50);
      FastLED.show();
      FastLED.delay(10);
      delay(1000);
      for (int count = 0; count < LED_NUM; count++) leds[count] = CRGB(0, 0, 0);
      FastLED.show();
      FastLED.delay(10);
    }
    colorCheck();
    delay(100);
  }
}


void motorTask(void* arg) {
  vTaskSuspend(motorTaskHandle);
  float v = 0.1;
  bool flag = false;

  // while(1){
  //   for(int a = 100; a < 10000; a += 1) {
  //     motor3.rotate(1.0);
  //     delay(a);
  //     motor3.rotate(-1.0);
  //     delay(a);
  //     if(a % 1000) Serial.printf("%d\n", a);
  //   }
  // }
  while (!flag) {
    flag = motor3.setAngle(100);
  }
  flag = false;
  delay(1000);

  while (!flag) {
    flag = motor3.setAngle(-100);
  }
  flag = false;
  delay(1000);

  while (!flag) {
    flag = motor3.setAngle(200);
  }
  flag = false;
  delay(1000);

  while (!flag) {
    flag = motor3.setAngle(-200);
  }
  flag = false;
  delay(1000);
  // icm.update();
  vTaskDelete(motorTaskHandle);
}

void taskManager(void* arg) {
  while(1) {
    if (swCount == 1) {
      // Serial.printf("motorTask start\n");
      vTaskResume(motorTaskHandle);
    } else {
      vTaskSuspend(motorTaskHandle);
      // Serial.printf("motorTask stop\n");
    } 
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);

  // motor and encoder
  motor1.setup();
  motor2.setup();
  motor3.setup();
  encodersSetup();

  // switch
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SW), []{swFlag = true;}, FALLING);

  // buzzer
  pinMode(BUZZER, OUTPUT);
  ledcSetup(0, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(BUZZER, 0);
  buzzer.setup(5000, 13); // pwm周波数 5000 Hz pwm 範囲
  
  // color sensor
  colorSensor.setMode(S11059_MODE_FIXED);
  colorSensor.setGain(S11059_GAIN_HIGH);
  colorSensor.setTint(S11059_TINT3);
  if (!colorSensor.reset()) Serial.println("reset failed");
  if (!colorSensor.start()) Serial.println("start failed");

  // ws2183b
  FastLED.addLeds<NEOPIXEL, LED>(leds, LED_NUM);
  
  // 起動完了合図
  for (int i = 0; i < 360; i += 10) {
    for (int count = 0; count < LED_NUM; count++) {
      leds[count] = CHSV(i, 255, 100);
      FastLED.show();
      FastLED.delay(5);
    }
  }
  for (int count = 0; count < LED_NUM; count++) leds[count] = CRGB(0, 0, 0);
  FastLED.show();
  FastLED.delay(10);

  song.push_back(NOTE_C);
  song.push_back(NOTE_D);
  song.push_back(NOTE_E);
  buzzer.play(song);
  song.erase(song.begin(), song.end());
  song.push_back(NOTE_F);

  // xTaskCreatePinnedToCore(motorTask,   "motorTask",   4096, NULL, 2, &motorTaskHandle, 1);
  xTaskCreatePinnedToCore(routineTask, "routineTask", 4096, NULL, 3, &routineTaskHandle, 1);
  // xTaskCreatePinnedToCore(taskManager, "taskManeger", 4096, NULL, 4, NULL, 1);
}

void loop() {
  delay(1000);
  // while(1){
  //   int a = 4;
  //   for(int i = 100; i < 1000; i += 1) {
  //     motor3.rotate(0.5);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(1.0);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(0.5);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(0.0);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(-0.5);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(-1.0);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(-0.5);
  //     // ets_delay_us(a);
  //     delay(a);
  //     motor3.rotate(0.0);
  //     // ets_delay_us(a);
  //     delay(a);
  //   }
  //   // delay(1000);
  //   Serial.printf("po\n");
  // }
    // while(1){
    // int a = 10;
    // for(int i = 100; i < 1000; i += 1) {
    //   motor1.rotate(1.0);
    //   motor3.rotate(0.0);
    //   delay(4 * a);
    //   motor1.rotate(0.0);
    //   motor3.rotate(1.0);
    //   delay(a);
    //   motor1.rotate(-1.0);
    //   motor3.rotate(0.0);
    //   delay(4 * a);
    //   motor1.rotate(0.0);
    //   motor3.rotate(-1.0);
    //   delay(a);
    // }
    // delay(1000);
    // Serial.printf("po\n");
  // }
}

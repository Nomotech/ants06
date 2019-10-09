#ifndef   MOTOR_HPP
#define   MOTOR_HPP

class  Motor{
  private:
    const int LEDC_TIMER_BIT = 8;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
    const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
    const int VALUE_MAX = 255;      // PWMの最大値
    int motorA;
    int motorB;
    int pwmPin;
    int channel;
    volatile int& enc;
    int margin = 5;

  public:
    Motor(int motorA, int motorB, int pwmPin, int channel, volatile int& en);
    void setup();
    void rotate(float value);
    void stop();
    void encoderReset();
    int count();
    bool setAngle(int ang);
};

#endif

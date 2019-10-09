#ifndef   BUZZER_HPP
#define   BUZZER_HPP
#include <vector>

class  Buzzer{
  private:
    const int LEDC_TIMER_BIT = 13;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
    const int LEDC_BASE_FREQ = 5000; // 周波数(Hz)
    const int VALUE_MAX = 255;      // PWMの最大値
    int pin;
    int channel;
    
  public:
    Buzzer(int pin, int channel);
    void setup(int freq, int bit);
    void sound(note_t note, uint8_t octave);
    void stop();
    void play(std::vector<note_t> notes);
};

#endif

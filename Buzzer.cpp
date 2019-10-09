#include <Arduino.h>
#include "Buzzer.hpp"

Buzzer::Buzzer(int pin, int cha){
  this->pin = pin;
  this->channel = cha;
}

void Buzzer::setup(int freq, int bit) {
  pinMode(pin, OUTPUT);
  ledcSetup(channel, freq, bit);
  ledcAttachPin(pin, channel);
}

/* note
ド    NOTE_C
ド♯   NOTE_Cs
レ    NOTE_D
ミ♭  NOTE_Eb
ミ    NOTE_E
ファ	NOTE_F
ファ♯	NOTE_Fs
ソ    NOTE_G
ソ♯   NOTE_Gs
ラ    NOTE_A
シ♭  NOTE_Bb
シ    NOTE_B
octave 0 ~ 8
*/
void Buzzer::sound(note_t note, uint8_t octave) {
  ledcWriteNote(channel, note, octave);
}

void Buzzer::stop() {
  ledcWriteTone(channel, 0);
}

void Buzzer::play(std::vector<note_t> notes) {
  for(note_t n : notes) {
    sound(n, 8);
    delay(100);
  }
  stop();
}
#include <vector>
#include <Adafruit_NeoPixel.h>
#include "WSLED.hpp"
using namespace std;

WSLED::WSLED(int pin, int num) {
  pixels = Adafruit_NeoPixel(num, pin, NEO_GRB + NEO_KHZ800);
  pixels.begin(); // This initializes the NeoPixel library.
  show(num, 0.0, 1.0f, 0.0f);
  this->hsv[2] = 0.5;
}

vector<int> WSLED::hsv2rgb(vector<float> hsv) {
  vector<int> rgb(3);
  int     Hi;
  float   f;
  float   p;
  float   q;
  float   t;

  Hi = fmod(floor(hsv[0] / 60.0f), 6.0f);
  f = hsv[0] / 60.0f - Hi;
  p = hsv[2] * (1.0f - hsv[1]) * 255;
  q = hsv[2] * (1.0f - f * hsv[1]) * 255;
  t = hsv[2] * (1.0f - (1.0f - f) * hsv[1]) * 255;

  if (Hi == 0) {
    rgb[0] = (int)hsv[2];
    rgb[1] = (int)t;
    rgb[2] = (int)p;
  }
  if (Hi == 1) {
    rgb[0] = (int)q;
    rgb[1] = (int)hsv[2];
    rgb[2] = (int)p;
  }
  if (Hi == 2) {
    rgb[0] = (int)p;
    rgb[1] = (int)hsv[2];
    rgb[2] = (int)t;
  }
  if (Hi == 3) {
    rgb[0] = (int)p;
    rgb[1] = (int)q;
    rgb[2] = (int)hsv[2];
  }
  if (Hi == 4) {
    rgb[0] = (int)t;
    rgb[1] = (int)p;
    rgb[2] = (int)hsv[2];
  }
  if (Hi == 5) {
    rgb[0] = (int)hsv[2];
    rgb[1] = (int)p;
    rgb[2] = (int)q;
  }
  return rgb;
}

void WSLED::show(int num, float h) {
  hsv[0] = h;
  vector<int> rgb = hsv2rgb(hsv);
  Serial.printf("%d : %f : rgb ( %d, %d, %d)\n",num, h, rgb[0], rgb[1], rgb[2]);
  pixels.setPixelColor(num, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void WSLED::show(int num, float h, float s, float v) {
  hsv[0] = h;
  hsv[1] = s;
  hsv[2] = v;
  vector<int> rgb = hsv2rgb(hsv);
  pixels.setPixelColor(num, pixels.Color(rgb[0], rgb[1], rgb[2])); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

#ifndef   WSLED_HPP
#define   WSLED_HPP

#include <vector>
#include <Adafruit_NeoPixel.h>
using namespace std;

class  WSLED{
  private:
    int pin;
    int pixelNum;
    Adafruit_NeoPixel pixels;
    vector<float> hsv = {0.0f, 1.0f, 0.3f};

    vector<int> hsv2rgb(vector<float> hsv);

  public:
    WSLED(int pin, int num);
    void show(int num, float h);
    void show(int num, float h, float s, float v);
};

#endif // LED_HPP

#ifndef   ICM_HPP
#define   ICM_HPP

#include <SPI.h>
class  ICM{
  private:
    SPIClass * hspi = NULL;
    int SS;
    static const int spiClk = 1000000; // 1 MHz
    char icmData[14];
    float mpuOffset[6];
    int16_t xa_d=0,ya_d=0,za_d=0;       // 加速度変化量
    int16_t xg_d=0,yg_d=0,zg_d=0;       // ジャイロ変化量
    float xa = 0.0, ya = 0.0, za = 0.0; // 加速度積算
    float xg = 0.0, yg = 0.0, zg = 0.0; // ジャイロ積算
    float temp = 0.0;                   // 温度
    float gyroT = 0.0;      //ジャイロTheta(SI)
    
    void writeRegister(char registerAddress, char value);
    char readRegister(char registerAddress);
    void carib(int num);
    void get();
    

  public:
    ICM(int clk, int miso, int mosi, int ss);
    void update();
    void log();

};

#endif

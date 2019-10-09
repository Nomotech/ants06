#include <Arduino.h>
#include "icm.hpp"
#include <SPI.h>
#define PI 3.1415926535897932384626433832795

ICM::ICM(int clk, int miso, int mosi, int ss) {
    SS = ss;
    hspi = new SPIClass(HSPI);
    hspi->begin(clk, miso, mosi, ss);
    Serial.begin(115200);
    
    // SSをHightに
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);

    // 初期化
    writeRegister(107,0b10000000); delay(200);
    writeRegister(104,0b00000111); delay(200);
    writeRegister(106,0b00010000); delay(200);
    writeRegister( 27,0b00011000); delay(200);

    carib(2000);
}



void ICM::update(){
    get();
    //加速度
    xa_d  = ((int16_t) icmData[0] <<8)|(int16_t) icmData[1];
    ya_d  = ((int16_t) icmData[2] <<8)|(int16_t) icmData[3];
    za_d  = ((int16_t) icmData[4] <<8)|(int16_t) icmData[5];

    //温度
    temp  = ((int16_t) icmData[6] <<8)|(int16_t) icmData[7];

    //ジャイロ
    xg_d  = ((int16_t) icmData[8] <<8)|(int16_t) icmData[9];
    yg_d  = ((int16_t) icmData[10]<<8)|(int16_t) icmData[11];
    zg_d  = ((int16_t) icmData[12]<<8)|(int16_t) icmData[13];

    //加速度加算
    xa += xa_d - mpuOffset[0];
    ya += ya_d - mpuOffset[1];
    za += za_d - mpuOffset[2];

    //  xa = xa_d;
    //  ya = ya_d;
    //  za = za_d;

    xg += xg_d - mpuOffset[3];
    yg += yg_d - mpuOffset[4];
    zg += zg_d - mpuOffset[5];

    // gyroAng = (float)zg * (90.0 / 0xffff);    // 0xffff/2 * 180 (SI);
    gyroT = zg * PI / 0xffff;
}

void ICM::writeRegister(char registerAddress, char value) {
    hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
    digitalWrite(SS, LOW);
    hspi->transfer(registerAddress);
    hspi->transfer(value);
    digitalWrite(SS, HIGH);
    hspi->endTransaction();
}

char ICM::readRegister(char registerAddress) {
    char value;
    char address = 0x80 | registerAddress;      // 書き込みフラグを立てる
    hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
    digitalWrite(SS, LOW);
    hspi->transfer(address);
    digitalWrite(SS, HIGH);
    value = hspi->transfer(0x00);
    hspi->endTransaction();
    return value;
}

void ICM::carib(int num){
    for(int i = 0;i<num;i++){
        get();
        //加速度
        xa_d  = ((int16_t) icmData[0] <<8)|(int16_t) icmData[1];
        ya_d  = ((int16_t) icmData[2] <<8)|(int16_t) icmData[3];
        za_d  = ((int16_t) icmData[4] <<8)|(int16_t) icmData[5];

        //温度
        temp  = ((int16_t) icmData[6] <<8)|(int16_t) icmData[7];

        //ジャイロ
        xg_d  = ((int16_t) icmData[8] <<8)|(int16_t) icmData[9];
        yg_d  = ((int16_t) icmData[10]<<8)|(int16_t) icmData[11];
        zg_d  = ((int16_t) icmData[12]<<8)|(int16_t) icmData[13];

        //加速度加算
        xa += xa_d;
        ya += ya_d;
        za += za_d;

        xg += xg_d;
        yg += yg_d;
        zg += zg_d;
    }
    mpuOffset[0] = xa/num;
    mpuOffset[1] = ya/num;
    mpuOffset[2] = za/num;
    mpuOffset[3] = xg/num;
    mpuOffset[4] = yg/num;
    mpuOffset[5] = zg/num;
    xa = 0.0; ya = 0.0; za = 0.0;
    xg = 0.0; yg = 0.0; zg = 0.0;
    Serial.printf("who am i %d\n",readRegister(117));
}

void ICM::get(){
    icmData[0]  = readRegister(59); delayMicroseconds(100);
    icmData[1]  = readRegister(60); delayMicroseconds(100);
    icmData[2]  = readRegister(61); delayMicroseconds(100);
    icmData[3]  = readRegister(62); delayMicroseconds(100);
    icmData[4]  = readRegister(63); delayMicroseconds(100); 
    icmData[5]  = readRegister(64); delayMicroseconds(100);
    icmData[6]  = readRegister(65); delayMicroseconds(100);
    icmData[7]  = readRegister(66); delayMicroseconds(100);
    icmData[8]  = readRegister(67); delayMicroseconds(100);
    icmData[9]  = readRegister(68); delayMicroseconds(100);
    icmData[10] = readRegister(69); delayMicroseconds(100);
    icmData[11] = readRegister(70); delayMicroseconds(100);
    icmData[12] = readRegister(71); delayMicroseconds(100);
    icmData[13] = readRegister(72); delayMicroseconds(100);
    Serial.printf("po : %d\t%d\t%d\t%d\n", icmData[0], icmData[1], icmData[2], icmData[3]);
}

void ICM::log(){
    // ログ出力 
    // Serial.print("xa: ");     Serial.print(xa);   Serial.print("\t");
    // Serial.print("ya: ");     Serial.print(ya);   Serial.print("\t");
    // Serial.print("za: ");     Serial.print(za);   Serial.print("\t");
    // Serial.print("temp: ");   Serial.print(temp); Serial.print("\t");
    Serial.print("xg: ");     Serial.print(xg);   Serial.print("\t");
    Serial.print("yg: ");     Serial.print(yg);   Serial.print("\t");
    Serial.print("zg: ");     Serial.print(zg);   Serial.print("\n");
}
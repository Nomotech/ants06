#include "Arduino.h"
#include <Wire.h>
#include "ColorSensor.hpp"

// I2C address
#define I2C_ADDR    0x2A

// Register Map
#define REG_CONTROL         0x00    // 1 byte
#define REG_MANUAL_TIMING   0x01    // 2 bytes
#define REG_DATA_RED        0x03    // 2 bytes
#define REG_DATA_GREEN      0x05    // 2 bytes
#define REG_DATA_BLUE       0x07    // 2 bytes
#define REG_DATA_IR         0x09    // 2 bytes

// Control Bits
#define CONTROL_BIT_ADC_RESET                        7
#define CONTROL_BIT_STANDBY_FUNCTION                 6
#define CONTROL_BIT_STANDBY_FUNCTION_MONITOR         5
#define CONTROL_BIT_GAIN_SELECTION                   3
#define CONTROL_BIT_INTEGRATION_MODE                 2
#define CONTROL_BIT_INTEGRATION_TIME_SETTING_HIGH    1
#define CONTROL_BIT_INTEGRATION_TIME_SETTING_LOW     0

// Control Bit Masks
#define CONTROL_BIT_MASK_ADC_RESET                   0x80
#define CONTROL_BIT_MASK_STANDBY_FUNCTION            0x40
#define CONTROL_BIT_MASK_STANDBY_FUNCTION_MONITOR    0x20
#define CONTROL_BIT_MASK_GAIN_SELECTION              0x08
#define CONTROL_BIT_MASK_INTEGRATION_MODE            0x04
#define CONTROL_BIT_MASK_INTEGRATION_TIME_SETTING    0x03

// Integration Times (For Fixed period mode)
#define INTEGRATION_TIME_0_US   88      // 87.5 us
#define INTEGRATION_TIME_1_US   1400    // 1.4 ms
#define INTEGRATION_TIME_2_US   22400   // 22.4 ms
#define INTEGRATION_TIME_3_US   179200  // 179.2 ms

// Tint Times (For Manual setting mode) 
#define TINT_TIME_0_US   175     // 175 us
#define TINT_TIME_1_US   2800    // 2.8 ms
#define TINT_TIME_2_US   44800   // 44.8 ms
#define TINT_TIME_3_US   358400  // 358.4ms

ColorSensor::ColorSensor(TwoWire *wire) {
  _wire = wire;
  for (int i = 0; i < 4; i++) {
    _rgbir[i] = 0;
  }

  // default value from datasheet
  _gain = S11059_GAIN_LOW;
  _mode = S11059_MODE_MANUAL;
  _tint = S11059_TINT0;
  _manualTiming = 0x0C30;
}

void ColorSensor::setGain(uint8_t gain) {
  _gain = gain;
}

void ColorSensor::setMode(uint8_t mode) {
  _mode = mode;
}

void ColorSensor::setTint(uint8_t tint) {
  _tint = tint;
}

void ColorSensor::setManualTiming(uint16_t manualTiming) {
  _manualTiming = manualTiming;
}

void ColorSensor::delay() {
  uint32_t ms = 0;
  uint32_t us = 0;
  if (_mode == S11059_MODE_MANUAL) {
    int64_t tmp = 0;
    switch (_tint) {
      case S11059_TINT0:
        tmp = (int64_t) TINT_TIME_0_US * (int64_t) _manualTiming;
        break;

      case S11059_TINT1:
        tmp = (int64_t) TINT_TIME_1_US * (int64_t) _manualTiming;
        break;

      case S11059_TINT2:
        tmp = (int64_t) TINT_TIME_2_US * (int64_t) _manualTiming;
        break;

      case S11059_TINT3:
        tmp = (int64_t) TINT_TIME_3_US * (int64_t) _manualTiming;
        break;
    }
    ms = tmp / 1000;
    us = tmp % 1000;
  } else {
    switch (_tint) {
      case S11059_TINT0:
        us = INTEGRATION_TIME_0_US;
        break;

      case S11059_TINT1:
        us = INTEGRATION_TIME_1_US;
        break;

      case S11059_TINT2:
        us = INTEGRATION_TIME_2_US;
        break;

      case S11059_TINT3:
        us = INTEGRATION_TIME_3_US;
        break;
    }
  }

  // add careful margin
  ms += 1;

  // delay for 4 colors
  ::delay(ms * 4);
  delayMicroseconds(us * 4);
}

bool ColorSensor::reset() {
  if (_mode == S11059_MODE_MANUAL) {
    uint8_t bytes[2] = {
      (uint8_t)((_manualTiming >> 8) & 0xFF),
      (uint8_t)(_manualTiming & 0xFF)
    };
    if (!writeRegisters(REG_MANUAL_TIMING, bytes, sizeof(bytes))) {
      return false;
    }
  }

  uint8_t controlBits = getControlBits();
  controlBits |= CONTROL_BIT_MASK_ADC_RESET;
  controlBits &= ~(CONTROL_BIT_MASK_STANDBY_FUNCTION);
  return writeRegister(REG_CONTROL, controlBits);
}

bool ColorSensor::standby() {
  uint8_t controlBits = getControlBits();
  controlBits |= CONTROL_BIT_MASK_STANDBY_FUNCTION;
  return writeRegister(REG_CONTROL, controlBits);
}

bool ColorSensor::start() {
  uint8_t controlBits = getControlBits();
  controlBits &= ~(CONTROL_BIT_MASK_ADC_RESET);
  controlBits &= ~(CONTROL_BIT_MASK_STANDBY_FUNCTION);
  bool result = writeRegister(REG_CONTROL, controlBits);
  return result;
}

bool ColorSensor::update() {
  // Read all data registers (03 - 0A)
  uint8_t values[8];
  if (readRegisters(REG_DATA_RED, values, sizeof(values))) {
    // convert endian
    _rgbir[0] = (int16_t) word(values[0], values[1]);  // Red
    _rgbir[1] = (int16_t) word(values[2], values[3]);  // Green
    _rgbir[2] = (int16_t) word(values[4], values[5]);  // Blue
    _rgbir[3] = (int16_t) word(values[6], values[7]);  // IR
    return true;
  } else {
    return false;
  }
}

bool ColorSensor::isStandby() {
  uint8_t value = 0x00;
  if (readRegister(REG_CONTROL, &value)) {
    return value & CONTROL_BIT_MASK_STANDBY_FUNCTION_MONITOR;
  } else {
    // confuse i2c communication failed result with not standby state.
    return false;
  }
}

uint16_t ColorSensor::getRed() {
  return _rgbir[0];
}

uint16_t ColorSensor::getGreen() {
  return _rgbir[1];
}

uint16_t ColorSensor::getBlue() {
  return _rgbir[2];
}

uint16_t ColorSensor::getIR() {
  return _rgbir[3];
}

uint8_t ColorSensor::getControlBits() {
  uint8_t bits = 0x00;

  // Gain selection (1 bit)
  bits |= !!(_gain) << CONTROL_BIT_GAIN_SELECTION;

  // Integration mode (1 bit)
  bits |= !!(_mode) << CONTROL_BIT_INTEGRATION_MODE;

  // Integration time setting (2 bits)
  bits |= _tint & 0x03;

  return bits;
}

bool ColorSensor::write(uint8_t value) {
  uint8_t values[1] = {value};
  return write(values, 1);
}

bool ColorSensor::write(uint8_t *values, size_t size) {
  _wire->beginTransmission(I2C_ADDR);

  bool result = false;
  if (_wire->write(values, size) == size) {
    switch (_wire->endTransmission()) {
      case 0: // success
        result = true;
        break;

      case 1: // data too long to fit in transmit buffer
        result = false;
        break;

      case 2: // received NACK on transmit of address
        result = false;
        break;

      case 3: // received NACK on transmit of data
        result = false;
        break;

      case 4: // other error
        result = false;
        break;

      default: // module unknown error
        result = false;
        break;

    }
  } else {
    _wire->endTransmission();
    result = false;
  }
  delayMicroseconds(100);
  return result;
}

bool ColorSensor::read(uint8_t *values, int size) {
  _wire->requestFrom(I2C_ADDR, size);
  if (_wire->available() == size) {
    for (uint8_t i = 0; i < size; i++) {
      values[i] = _wire->read();
    }
    return true;
  } else {
    return false;
  }
  delayMicroseconds(100);
}

bool ColorSensor::readRegister(uint8_t address, uint8_t *value) {
  return readRegisters(address, value, 1);
}

bool ColorSensor::readRegisters(uint8_t address, uint8_t *values, uint8_t size) {
  if (!write(address)) {
    return false;
  }

  return read(values, size);
}

bool ColorSensor::writeRegister(uint8_t address, uint8_t value) {
  return writeRegisters(address, &value, 1);
}

bool ColorSensor::writeRegisters(uint8_t address, uint8_t *values, uint8_t size) {
  uint8_t tmp[size + 1];
  tmp[0] = address;
  for (int8_t i = 0; i < size; i++) {
    tmp[i + 1] = values[i];
  }
  return write(tmp, sizeof(tmp));
}
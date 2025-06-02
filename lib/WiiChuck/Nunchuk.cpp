/*
  Nunchuk.cpp

  Arduino library for interface to the Wiimote Nunchuk (hard-wired, not wireless)
  Based on Chad Phillips' work at http://www.windmeadow.com/node/42
  and the non-OEM initialization by crimony, http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264805255

  2012-07-30 fix a big hole in getAccel()
  2012-07-27 @machinesalem,  (cc) https://creativecommons.org/licenses/by/3.0/
*/

#include "Arduino.h"
#include <Wire.h>

#include "Nunchuk.h"

const float radToDegrees = 57.29577951308232;

// Initialization

void Nunchuk::begin()
{
  int i;

  // Override the TWI frequency
  // TWBR = ((CPU_FREQ / NUNCHUK_TWI_FREQ) - 16) / 2;

  Wire.begin();

  // Initialize
  delay(1);

  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS); // device address
  Wire.write(0xF0);                                   // 1st initialisation register
  Wire.write(0x55);                                   // 1st initialisation value
  Wire.endTransmission();
  delay(1);

  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(0xFB); // 2nd initialisation register
  Wire.write(0x00); // 2nd initialisation value
  Wire.endTransmission();
  delay(1);

  // write the crypto key (zeros), in 3 blocks of 6, 6 & 4.
  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(0xF0); // crypto key command register
  Wire.write(0xAA); // writes crypto enable notice
  Wire.endTransmission();
  delay(1);

  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(0x40); // crypto key data address
  for (i = 0; i < 6; i++)
  {
    Wire.write(0x00); // writes 1st key block (zeros)
  }
  Wire.endTransmission();
  delay(1);

  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(0x40); // writes memory address
  for (i = 6; i < 12; i++)
  {
    Wire.write(0x00); // writes 2nd key block (zeros)
  }
  Wire.endTransmission();
  delay(1);

  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(0x40); // writes memory address
  for (i = 12; i < 16; i++)
  {
    Wire.write(0x00); // writes 3rd key block (zeros)
  }
  Wire.endTransmission();
  delay(1);

  // end device init
}

// Read the current value
uint8_t Nunchuk::read()
{
  uint8_t i;
  int a;

  // Request to read the new data
  Wire.requestFrom(NUNCHUK_TWI_DEVICE_ADDRESS, NUNCHUK_TWI_BUFFER_SIZE);
  delayMicroseconds(NUNCHUK_TWI_DELAY_MICROSEC);

  // Read the new data
  if (NUNCHUK_TWI_BUFFER_SIZE == Wire.available())
  {
    _ok = 1;
    for (i = 0; i < NUNCHUK_TWI_BUFFER_SIZE; i++)
    {
      _buf[i] = _decode_byte((uint8_t)Wire.read());
    }
  }
  else
  {
    // Throw away any more data that arrives
    _ok = 0;
    while (Wire.available())
      Wire.read();
  }

  // Calculate accel and accel^2 values
  a = _buf[2] * 2 * 2;
  if ((_buf[5] >> 2) & 1)
  {
    a += 2;
  }
  if ((_buf[5] >> 3) & 1)
  {
    a += 1;
  }
  _ax = a - 511;
  _ax2 = pow(_ax, 2);

  a = _buf[3] * 2 * 2;
  if ((_buf[5] >> 4) & 1)
  {
    a += 2;
  }
  if ((_buf[5] >> 5) & 1)
  {
    a += 1;
  }
  _ay = a - 511;
  _ay2 = pow(_ay, 2);

  a = _buf[4] * 2 * 2;
  if ((_buf[5] >> 6) & 1)
  {
    a += 2;
  }
  if ((_buf[5] >> 7) & 1)
  {
    a += 1;
  }
  _az = a - 511;
  _az2 = pow(_az, 2);

  // Set up the next read
  Wire.beginTransmission(NUNCHUK_TWI_DEVICE_ADDRESS);
  Wire.write(NUNCHUK_TWI_CMD_ZERO);
  Wire.endTransmission(true);

  return _ok;
}

/* Wiimote data stream is encoded.  This decodes each byte */
uint8_t Nunchuk::_decode_byte(uint8_t x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

/* Getters */

bool Nunchuk::isOk()
{
  return (_ok == 1);
}

bool Nunchuk::getButtonZ()
{
  return !((_buf[5] >> 0) & 1);
}

bool Nunchuk::getButtonC()
{
  return !((_buf[5] >> 1) & 1);
}

int Nunchuk::getJoyX()
{
  return (int)_buf[0] - 127;
}

int Nunchuk::getJoyY()
{
  return (int)_buf[1] - 127;
}

int Nunchuk::getAccelX()
{
  return _ax;
}

int Nunchuk::getAccelY()
{
  return _ay;
}

int Nunchuk::getAccelZ()
{
  return _az;
}

float Nunchuk::getAccel()
{
  return sqrt(_ax2 + _ay2 + _az2);
}

// Tilt angles from http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

// rho
float Nunchuk::getTiltX()
{
  return atan(_ax / sqrt(_ay2 + _az2)) * radToDegrees;
}

// phi
float Nunchuk::getTiltY()
{
  return atan(_ay / sqrt(_ax2 + _az2)) * radToDegrees;
}

// theta
float Nunchuk::getTiltZ()
{
  return atan(sqrt(_ay2 + _ax2) / _az) * radToDegrees;
}

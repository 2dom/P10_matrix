/*********************************************************************
This is a library for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef _P10_MATRIX_H
#define _P10_MATRIX_H

#include "Adafruit_GFX.h"
  #include "Arduino.h"


#include <SPI.h>




class P10_MATRIX : public Adafruit_GFX {
 public:
  P10_MATRIX(uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C);

  void begin();
  void clearDisplay(void);
  void display(uint16_t show_time);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint8_t getPixel(int8_t x, int8_t y);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

 private:

  uint8_t _LATCH_PIN;
  uint8_t _OE_PIN;
  uint8_t _A_PIN;
  uint8_t _B_PIN;
  uint8_t _C_PIN;
  uint8_t _width;
  uint8_t _height;
};

#endif

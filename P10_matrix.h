/*********************************************************************
This is a library for Chinese P10 32x16 LED matrix displays

It is based on the 5110 library by Adafruit

Written by Dominic Buchstaller.
BSD license, check license.txt for more information
*********************************************************************/


#ifndef _P10_MATRIX_H
#define _P10_MATRIX_H

// This is how many color levels the display shows - the more the slower the update
#define color_depth 8
#define PATTERN4
//#define PATTERN8

#include "Adafruit_GFX.h"
  #include "Arduino.h"


#include <SPI.h>



class P10_MATRIX : public Adafruit_GFX {
 public:
  P10_MATRIX(uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C);

  void begin();
  void clearDisplay(void);
  void display(uint16_t show_time);

  void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b);
  uint8_t getPixel(int8_t x, int8_t y);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  void displayTestPattern(uint16_t showtime);
  void flushDisplay();

 private:
  uint8_t _display_color;
  uint8_t _LATCH_PIN;
  uint8_t _OE_PIN;
  uint8_t _A_PIN;
  uint8_t _B_PIN;
  uint8_t _C_PIN;
  uint8_t _width;
  uint8_t _height;
  uint32_t _test_pixel_counter;
  uint8_t _test_row_counter;
  unsigned long last_call;
  void fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b);

};

#endif

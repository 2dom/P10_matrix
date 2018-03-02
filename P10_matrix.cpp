/*********************************************************************
This is a library for Chinese P10 32x16 LED matrix displays

It is based on the 5110 library by Adafruit

Written by Dominic Buchstaller.
BSD license, check license.txt for more information
*********************************************************************/

//#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __AVR__
#include <util/delay.h>
#endif

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

#include <stdlib.h>

#include <Adafruit_GFX.h>
#include "P10_matrix.h"

#define matrix_width 32
#define matrix_height 16
#define color_step 256 / color_depth
#define color_half_step color_step / 2

#define buffer_size matrix_width * matrix_height * 3 / 8

// the memory buffer for the LCD
uint8_t P10_MATRIX_buffer[color_depth][buffer_size] = {0x00 };

#ifdef PATTERN4
uint8_t P10_MATRIX_send_buffer[buffer_size/4] = {0x00 };
#endif
#ifdef PATTERN8
uint8_t P10_MATRIX_send_buffer[buffer_size/8] = {0x00 };
#endif


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t P10_MATRIX::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

P10_MATRIX::P10_MATRIX(uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C) : Adafruit_GFX(matrix_width+10, matrix_height) {
  _LATCH_PIN = LATCH;
  _OE_PIN = OE;
  _A_PIN= A;
  _B_PIN = B;
  _C_PIN = C;
  _width=matrix_width;
  _height=matrix_height;
  _display_color=0;

  _test_last_call=0;
  _test_pixel_counter=0;
  _test_line_counter=0;

}

void P10_MATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  drawPixelRGB565( x,  y,  color);
}

void P10_MATRIX::fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b)
{
  if ((x < 0) || (x > _width) || (y < 0) || (y > _height))
  return;
  x=31-x;

  #ifdef PATTERN4
  // Shift register length: 48 bytes, one color: 16 bytes
  uint16_t base_offset=(y%4)*48+(x/8)*2;

  uint8_t total_offset_r=0;
  uint8_t total_offset_g=0;
  uint8_t total_offset_b=0;

  // Weird shit access pattern
  if (y<4)
    total_offset_r=base_offset;
  if ((y>=4) && (y<8))
    total_offset_r=base_offset+1;
  if ((y>=8) && (y<12))
    total_offset_r=base_offset+8;
  if (y>=12)
    total_offset_r=base_offset+9;

  total_offset_g=total_offset_r+16;
  total_offset_b=total_offset_g+16;

  //Color interlacing
  for (int this_color=0; this_color<color_depth; this_color++)
  {
    if (r > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[this_color][total_offset_r] |=_BV(x%8);
    else
      P10_MATRIX_buffer[this_color][total_offset_r] &= ~_BV(x%8);

    if (g > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[(this_color+3)%8][total_offset_g] |=_BV(x%8);
    else
      P10_MATRIX_buffer[(this_color+3)%8][total_offset_g] &= ~_BV(x%8);

    if (b > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[(this_color+6)%8][total_offset_b] |=_BV(x%8);
    else
      P10_MATRIX_buffer[(this_color+6)%8][total_offset_b] &= ~_BV(x%8);
  }

  #endif
  #ifdef PATTERN8
  // Shift register length: 24 bytes, one color: 8 bytes
  uint16_t base_offset=(y%8)*24+(x/8);

  uint8_t total_offset_r=0;
  uint8_t total_offset_g=0;
  uint8_t total_offset_b=0;

  // (A bit less) weird shit access pattern
  if (y<8)
    total_offset_r=base_offset;
  else
    total_offset_r=base_offset+4;

  total_offset_g=total_offset_r+8;
  total_offset_b=total_offset_g+8;

  //Simple counting up
  for (int this_color=0; this_color<color_depth; this_color++)
  {
    if (r > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[this_color][total_offset_r] |=_BV(x%8);
    else
      P10_MATRIX_buffer[this_color][total_offset_r] &= ~_BV(x%8);

    if (g > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[(this_color+3)%8][total_offset_g] |=_BV(x%8);
    else
      P10_MATRIX_buffer[(this_color+3)%8][total_offset_g] &= ~_BV(x%8);

    if (b > this_color*color_step+color_half_step)
      P10_MATRIX_buffer[(this_color+6)%8][total_offset_b] |=_BV(x%8);
    else
      P10_MATRIX_buffer[(this_color+6)%8][total_offset_b] &= ~_BV(x%8);
  }


#endif

}


void P10_MATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  fillMatrixBuffer( x,  y, r, g,b);
}


void P10_MATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) {
  fillMatrixBuffer(x, y, r, g,b);
}


// the most basic function, get a single pixel
uint8_t P10_MATRIX::getPixel(int8_t x, int8_t y) {
  return (0);//P10_MATRIX_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}


void P10_MATRIX::begin() {

  SPI.begin();

  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(20000000);

  pinMode(_OE_PIN, OUTPUT);
  pinMode(_LATCH_PIN, OUTPUT);
  pinMode(_A_PIN, OUTPUT);
  pinMode(_B_PIN, OUTPUT);
  pinMode(_C_PIN, OUTPUT);

  digitalWrite(_A_PIN, LOW);
  digitalWrite(_B_PIN, LOW);
  digitalWrite(_C_PIN, LOW);
  digitalWrite(_OE_PIN, HIGH);

}

void P10_MATRIX::display(uint16_t show_time) {
  #ifdef PATTERN4
  for (uint8_t i=0;i<4;i++)
  #endif
  #ifdef PATTERN8
  for (uint8_t i=0;i<8;i++)
  #endif

  {
    // digitalWrite(_A_PIN,HIGH);
    // digitalWrite(_B_PIN,HIGH);
    // digitalWrite(_C_PIN,LOW);
    // digitalWrite(_A_PIN,LOW);
    // digitalWrite(_B_PIN,LOW);
    // digitalWrite(_C_PIN,LOW);
    if (i & 0x01)
    digitalWrite(_A_PIN,HIGH);
    else
    digitalWrite(_A_PIN,LOW);

    if (i & 0x02)
    digitalWrite(_B_PIN,HIGH);
    else
    digitalWrite(_B_PIN,LOW);

    if (i & 0x04)
    digitalWrite(_C_PIN,HIGH);
    else
    digitalWrite(_C_PIN,LOW);

    #ifdef PATTERN4
    for (uint8_t j=0;j<48;j++)
    P10_MATRIX_send_buffer[j]= P10_MATRIX_buffer[_display_color][47-j+i*48];
    SPI.writeBytes(P10_MATRIX_send_buffer,48);
    #endif

    #ifdef PATTERN8
    for (uint8_t j=0;j<24;j++)
    P10_MATRIX_send_buffer[j]= P10_MATRIX_buffer[_display_color][23-j+i*24];
    SPI.writeBytes(P10_MATRIX_send_buffer,24);

    #endif

    digitalWrite(_LATCH_PIN,HIGH);
    //delayMicroseconds(10);
    digitalWrite(_LATCH_PIN,LOW);
    //delayMicroseconds(10);
    digitalWrite(_OE_PIN,0);
    delayMicroseconds(show_time);
    digitalWrite(_OE_PIN,1);

  }
  _display_color++;
  if (_display_color>=color_depth)
  _display_color=0;



}

void P10_MATRIX::flushDisplay(void) {
  // pinMode(13,OUTPUT);
  // pinMode(14,OUTPUT);
  // digitalWrite(13,0);
  // digitalWrite(14,0);
  #ifdef PATTERN4
  for (int ii=0;ii<48;ii++)
  SPI.write(0x00);
  #endif

  #ifdef PATTERN8
  for (int ii=0;ii<24;ii++)
  SPI.write(0x00);
  #endif

}

void P10_MATRIX::displayTestPattern(uint16_t show_time) {


  if ((millis()-_test_last_call)>100)
  {

    //digitalWrite(13,HIGH);
    //digitalWrite(15,HIGH);
    //digitalWrite(15,LOW);
    SPI.write(0xFF);


    _test_last_call=millis();
    _test_pixel_counter++;
  }
  #ifdef PATTERN4
  if (_test_pixel_counter>48)
  #endif
  #ifdef PATTERN8
  if (_test_pixel_counter>24)
  #endif
  {
    _test_pixel_counter=0;
    _test_line_counter++;
    flushDisplay();
  }

  if (_test_line_counter>8)
  _test_line_counter=0;


  digitalWrite(_A_PIN,HIGH);
  digitalWrite(_B_PIN,HIGH);
  digitalWrite(_C_PIN,LOW);
  digitalWrite(_A_PIN,LOW);
  digitalWrite(_B_PIN,LOW);
  digitalWrite(_C_PIN,LOW);

  //  digitalWrite(_C_PIN,HIGH);
  if (_test_line_counter & 0x01)
  digitalWrite(_A_PIN,HIGH);
  else
  digitalWrite(_A_PIN,LOW);


  if (_test_line_counter & 0x02)
  digitalWrite(_B_PIN,HIGH);
  else
  digitalWrite(_B_PIN,LOW);

  if (_test_line_counter & 0x04)
  digitalWrite(_C_PIN,HIGH);
  else
  digitalWrite(_C_PIN,LOW);

  digitalWrite(_LATCH_PIN,HIGH);
  digitalWrite(_LATCH_PIN,LOW);
  digitalWrite(_OE_PIN,0);
  delayMicroseconds(show_time);
  digitalWrite(_OE_PIN,1);


}

// clear everything
void P10_MATRIX::clearDisplay(void) {
  for(int this_color=0;this_color<color_depth;this_color++)
  for (int j=0;j<(buffer_size);j++)
  P10_MATRIX_buffer[this_color][j]=0;

}

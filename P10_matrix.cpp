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
#include "P10_MATRIX.h"

#define matrix_width 32
#define matrix_height 16
#define buffer_size matrix_width * matrix_height * 3 / 8

// the memory buffer for the LCD
uint8_t P10_MATRIX_buffer[buffer_size] = {0x00 };




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
  P10_MATRIX_buffer[0]=0xFF;
}

//pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= _BV(y%8);
// the most basic function, set a single pixel
void P10_MATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return;
  x=31-x;
  uint8_t rgb_color[3];
  rgb_color[0] = (color & 0xF800) >> 8;
  rgb_color[1] = (color & 0x07E0) >> 3;
  rgb_color[2]= (color & 0x1F) << 3;


  for (int col=0; col<3; col++)
  {
    // Weird shit access pattern
    uint16_t total_offset=0;
    if (y<4)
      total_offset=(y%4)*48+16*col+(x/8)*2;
    if ((y>=4) && (y<8))
      total_offset=(y%4)*48+16*col+(x/8)*2+1;
    if ((y>=8) && (y<12))
        total_offset=(y%4)*48+16*col+(x/8)*2+8;
    if (y>=12)
      total_offset=(y%4)*48+16*col+(x/8)*2+9;

    //Serial.println(total_offset);
    //total_offset=0;
    if (rgb_color[col]>128)
        P10_MATRIX_buffer[total_offset] |=_BV(x%8);
    else
        P10_MATRIX_buffer[total_offset] &= ~_BV(x%8);
  }
}


// the most basic function, get a single pixel
uint8_t P10_MATRIX::getPixel(int8_t x, int8_t y) {
  return (0);//P10_MATRIX_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}


void P10_MATRIX::begin() {


  SPI.begin();

  SPI.setDataMode(SPI_MODE0);
  //SPI.setBitOrder(LSBFIRST);




  pinMode(_OE_PIN, OUTPUT);
  pinMode(_LATCH_PIN, OUTPUT);
  pinMode(_A_PIN, OUTPUT);
  pinMode(_B_PIN, OUTPUT);
  pinMode(_C_PIN, OUTPUT);
  SPI.begin();


  digitalWrite(_A_PIN, LOW);
  digitalWrite(_B_PIN, LOW);
  digitalWrite(_C_PIN, LOW);
  digitalWrite(_OE_PIN, LOW);

}

void P10_MATRIX::display(uint16_t show_time) {


  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(20000000);
  for (uint8_t i=0;i<4;i++)
  {
    // if (i & B001)
    //   digitalWrite(_A_PIN,HIGH);
    // else
    //   digitalWrite(_A_PIN,LOW);
    //
    // if (i & B010)
    //     digitalWrite(_B_PIN,HIGH);
    // else
    //     digitalWrite(_B_PIN,LOW);
    // if (i & B100)
    //     digitalWrite(_C_PIN,HIGH);
    // else
    //     digitalWrite(_C_PIN,LOW);


    if (i ==0)
    {
      //digitalWrite(_A_PIN,HIGH);
      digitalWrite(_A_PIN,LOW);
      digitalWrite(_B_PIN,LOW);
      digitalWrite(_C_PIN,LOW);

     }

     if (i ==1)
     {
       digitalWrite(_A_PIN,HIGH);
       digitalWrite(_B_PIN,LOW);
       digitalWrite(_C_PIN,LOW);

      }
      if (i ==2)
      {
        digitalWrite(_A_PIN,HIGH);
        digitalWrite(_A_PIN,LOW);
        digitalWrite(_B_PIN,HIGH);
        digitalWrite(_C_PIN,HIGH);

       }
       if (i ==3)
       {
         digitalWrite(_A_PIN,HIGH);
         digitalWrite(_B_PIN,HIGH);
         digitalWrite(_C_PIN,LOW);

        }



    //SPI.writeBytes(P10_MATRIX_buffer+i*24,24);
    for (uint8_t j=0;j<48;j++)
      SPI.write(P10_MATRIX_buffer[47-j+i*48]);
    digitalWrite(_LATCH_PIN,HIGH);
    digitalWrite(_LATCH_PIN,LOW);
    digitalWrite(_OE_PIN,0);
    delayMicroseconds(show_time);
    digitalWrite(_OE_PIN,1);

  }

}

// clear everything
void P10_MATRIX::clearDisplay(void) {
    for (int j=0;j<(buffer_size);j++)
      P10_MATRIX_buffer[j]=0;

}

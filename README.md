## Overview

This is a Adafruit GFX compatible driver for a single chinese 32x16 RGB LED module.

![P10](/images/P10_matrix.jpg)

They are about EUR 15-20 a piece and commonly used for large-scale LED
displays, e.g. see [aliexpress](https://www.aliexpress.com/item/outdoor-320-160mm-32-16pixels-3in1-SMD-1-2-scan-RGB-P10-full-color-LED-module/32707982524.html?spm=a2g0s.9042311.0.0.OMzudS).

This driver currently works with ESP8266 microcontrollers, however, it should be rather straightforward to port it to ESP32 or Atmel-based Arduinos. There are different panels out there - this driver only works with panels using a 4 row-step or 8 row-step pattern (details below).

## Display structure

The display basically consists of 6 large shift register. On the input connector you will find the inputs to the shift register (two for each color), Rx,Gx,Bx, a 3 bit latch address input (A,B,C), a latch enable input (LAT/STB), a clock input (CLK) and the output enable input (OE).

There are two basic layouts/patterns: 4 row-step (4RS) and 8 row-step (8RS). You can enable the correct pattern for your display in the header file via #define PATTERN4 or #define PATTERN8.

Each of the shift register is 64(4RS)/32(8RS) bits long. R1 and R2 will together therefore cover 128(4RS)/64(8RS) bits or 4(4RS)/2(8RS) lines respectively. The rows are, however, not next to each other but have a spacing of 4(4RS)/8(8RS). In case of 4RS adjacent bytes also alternate between lines n and n+4.

Setting Rx to high, cycling CLK 64(4RS)/32(8RS) times, setting (A,B,C) to low and setting LAT/STB to low will light up rows 0,4,8,12(4RS)/0,8(8RS). Repeating the same experiment with A high, B and C low will light up rows 1,5,8,13(4RS)/1,9(8RS) and so forth. The same principle applies to the other colors. As the row spacing for 4RS is 4 we only need A and B for the latch address - C has no function. Only 8RS requires C.

The P10 LED matrix is usually used as a sub-module for larger displays and therefore features an output connector for daisy chaining. On the output connector you will find the identical signals to the input connector where A,B,C,LAT,CLK are simply routed through and (R,G,B) pins are the outputs of the shift registers on the module.

## Set-up and cabling

When driving a long chain of panels in a row, splitting the data lines makes a lot of sense since it reduces the data rate. But since we are only driving a single module here, we really don't need that. We can therefore use jumper wires between input connector (PI) and output connector (PO) to chain all shift registers together and create one big shift register. This reduces the number of required GPIO pins on the ESP. The connector labeling differs from board to board:

* Green PCB (usually 4 row-step)

  ![P10_conn_A](/images/P10_conn_A.jpg)

  Connect PI and PO as follows:

  PI | PO
  ---|---
  R2 | R1
  G1 | R2
  G2 | G1
  B1 | G2
  B2 | B1

  Connect panel input (PI) to the NodeMCU/ESP8266 (ESP) as follows:

  PI  | ESP
  ----|----
  LAT |  16 - (D0)
  A   |  05 - (D1)
  B   |  04 - (D2)
  C   |  15 - (D8)
  P_OE|  02 - (D4)
  CLK |  14 - (D5)
  R1  |  13 - (D7)

* Black PCB (usually 8 row-step)

  ![P10_conn_B](/images/P10_conn_B.jpg)

  Connect PI and PO as follows:

  PI | PO
  ---|---
  R1 | R0
  G0 | R1
  G1 | G0
  B0 | G1
  B1 | B0

  Connect panel input (PI) to the NodeMCU/ESP8266 (ESP) as follows:

  PI  | ESP
  ----|----
  STB |  16 - (D0)
  A   |  05 - (D1)
  B   |  04 - (D2)
  C   |  15 - (D8)
  P_OE|  02 - (D4)
  CLK |  14 - (D5)
  R0  |  13 - (D7)


You should end up with something like this (VCC/supply not nonnected here yet):

![Cabling](/images/P10_cables.jpg)

## Colors
The number of color levels can be selected in the header file. The default (8 color levels per primary RGB color) works well with hardly any flickering. Note that the number of color levels determines the achievable display refresh rate. Hence, the more color levels are selected, the more flickering is to be expected. If you run into problems with flickering it is a good idea to increase the CPU frequency to 160MHz. This way the processor has more headroom to compute the display updates and refresh the display in time.

## Troubleshooting

  * Check you cabeling with a multimeter. You can just measure between the exposed SMD pads/legs on the input and the ouput panel connector.
  * Your display may have a different scanning pattern. Make sure that you have selected the correct scanning pattern in the header file (#define PATTERN4 or #define PATTERN8)
  * Run the "P10_pattern_test.ino" and check if the scanning pattern is appearing ok. For a 8 row-step display it should look like this (red then yellow then white line progressing):
  ![8step](/images/8step.gif)

  * It is possible that the LED multiplex chip is defective (this was the case with one of my modules). You can verify that by selecting a bit pattern on the A,B,C inputs and measuring that the corresponing row outputs are low , e.g. a 4 row-step display typically uses a ([PR4538](/docs/pr4538.pdf)) chip. Setting (A=1,B=0) should give you (LINE0=1,LINE1=0,LINE2=1,LINE3=1).  This chip can easily be replaced. Spare part available [here](https://www.aliexpress.com/item/Free-shipping-10pcs-lot-PR4538DW-SOP-20-original-authentic/32594044891.html?spm=a2g0s.9042311.0.0.bjr5BY).


## Examples

Test sketch with 16 levels per primary color

![Colors](/images/P10_color_scroll.gif)

Animated weather clock from [here](https://2dom.github.io/PixelTime/)

![Example](/images/PixelTime_small.jpg)

Animated picture frame (work in progress) -  pixels from [eboy](http://hello.eboy.com/eboy/category/everything/explore/animations/).

![Example](/images/sea.gif)

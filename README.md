## Overview

This is a Adafruit GFX compatible driver for a single chinese 32x16 RGB LED module.

![P10](/images/P10_matrix.jpg)

They are about EUR 15-20 a piece and commonly used for large-scale LED
displays, e.g. see [aliexpress](https://www.aliexpress.com/item/Freeshipping-SMD3535-outdoor-rgb-led-matrix-p10-module-32x16-wateproof-led-sign-xxx-video-led-wall/32671579710.html?spm=2114.search0104.3.1.CS5xAQ&ws_ab_test=searchweb0_0,searchweb201602_2_10320_10152_10321_10065_10151_10344_10068_10345_10342_10547_10343_10322_10340_10341_10548_10193_10194_10541_10562_10084_10083_10304_10307_10177_10302_10539_10180_10312_10059_10313_10314_10184_10534_10533_100031_10319_10604_10603_10103_10186_10594_10557_10596_10595_10142_10107,searchweb201603_14,ppcSwitch_5&btsid=4c102134-b41b-43b9-bc7b-d7f073a5052e&algo_expid=41b2585a-3330-46a3-8879-ca765ce011fb-0&algo_pvid=41b2585a-3330-46a3-8879-ca765ce011fb&rmStoreLevelAB=2).

This driver corrently only works with ESP8266 microcontrollers. However, it should be rather straightforward to port it to ESP32 or Atmel based Arduinos.

## Display structure

The display is basically a large shift register with a few latch inputs. On the input connector you will find two pins each for Red, Greed and Blue (R1,R2,G1,G2,B1,B2), a 3 bit latch address input (A,B,C), a latch enable input (LAT), a clock input (CLK) and the output enable input (OE).

Each shift register is 64 bits long. Since the matrix is row-organized, R1 and R2 will together cover 128 bits or 4 rows of red. To make it a bit more interesting, the rows are not next to each other but have a spacing of 4. Hence setting R1 and R2 to high, cycling CLK 64 times, setting (A,B) to low and setting LAT to low will light up rows 0,4,8,12. Repeating the same experiment with A low and B high will light up rows 1,5,8,13 and so forth. The same principle applies to the other colors. As the row spacing is 4 we only need A and B for the latch address - C appears to be of no function.

The P10 LED matrix is usually used as a sub-module for larger displays and therefore features a output connector for daisy chaining. On the output connector you will find the identical signals to the input connector where A,B,C,LAT,CLK are simply routed through and R1,R2,G1,G2,B1,B2 are the outputs of the shift registers on the module.

## Set-up and cabelling

When driving a long chain of panels in a row, splitting the data lines into R1,R2,G1,G2,B1,B2 makes a lot of sense since it reduces the data rate. But since we are only driving a single module here, we really don't need that.
So in order to reduce the number of inputs we can use jumper wires between input connector (PI) and output connector (PO) as follows:

PI | PO
---|---
R2 | R1
G1 | R2
G2 | G1
B1 | G2
B2 | B1

This creates once big shift register with 6x64 bits.

Connecting the panel input (PI) to the ESP8266 (ESP) then becomes rather simple:

PI  | ESP
----|----
LAT |  16 - (D0)
A   |  05 - (D1)
B   |  04 - (D2)
C (no function but connected for now)  |  15 - (D8)
P_OE|  02 - (D4)
CLK |  14 - (D5)
R1  |  13 - (D7)


## Colors
The number of color levels can be selected in the header file. The default (8 color levels per primary RGB color) works well with hardly any flickering. Note that the number of color levels determines the achievable display refresh rate. Hence, the more color levels are selected, the more flickering is to be expected. If you run into problems with flickering it is a good idea to increase the CPU frequency to 160MHz. This way the processor has more headroom to compute the display updates and refresh the display in time.

Here is an example with 16 levels.

![Colors](/images/P10_color_scroll.gif)

Now you can let all your pixel dreams come true - like this weather clock ...

![Example](/images/PixelTime_small.jpg)

... or an animated picture frame -  pixels from [eboy](http://hello.eboy.com/eboy/).

![Example](/images/sea.gif)

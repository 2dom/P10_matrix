#include <P10_matrix.h>
#include <Ticker.h>

// Pins for LED MATRIX
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
//uncomment for P5 32x64
//#define P_D 12
//for P10 16x32
P10_MATRIX display( P_LAT, P_OE,P_A,P_B,P_C);
//uncomment for P5 32x64
//P10_MATRIX display( P_LAT, P_OE,P_A,P_B,P_C,P_D);
Ticker display_ticker;

void display_updater()
{
   //display.display(70);
   display.displayTestPattern(70);
}
uint16_t myCYAN = display.color565(0, 255, 255);
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  display.begin();
  display.flushDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("Pixel");
  Serial.println("hello");
  display_ticker.attach(0.001, display_updater);
  delay(1000);
}




void loop() {

 delay(100);

}

#include <gpio_MCP23SXX.h>
#include <SPI.h>
/*
   In this basic example the MCP GPIO Chip is used as OUT
*/

/*
  --- ESP8266
  MOSI(13) MISO(12) SCLK(14) CS(5,4,0,2)
  ESP12E Dev Board 0.9, 1.0
  MOSI: D7
  MISO: D6
  SCLK: D5
  CS:   D1,D2,D3,D4

*/
//define CS pin and using HAEN or not
//to use HAEN, address should be 0x20 to 0x27 for MCP23S17
gpio_MCP23SXX mcp1(MCP23S17, D1, 0x20);

void setup() {
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000));
  Serial.println("started");
  mcp1.begin();//this initialize SPI bus as well
  mcp1.gpioPinMode(OUTPUT);//set every GPIO port as OUT
  mcp1.gpioPort(0xFFFF);//Set all port as '1'
}


void loop() {
  //ports() return the number of ports of the choosed GPIO chip
  for (int i = 0; i < mcp1.ports(); i++) {
    //gpioDigitalInvert reads internal tracked GPIO port value
    //and invert it, then update GPIO.
    mcp1.gpioDigitalInvert(i);
    delay(10);
  }

  for (int i = 0; i < mcp1.ports(); i++) {
    mcp1.gpioDigitalInvert(i);
    delay(10);
  }
}
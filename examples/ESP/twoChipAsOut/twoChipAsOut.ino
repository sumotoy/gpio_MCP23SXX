#include <gpio_MCP23SXX.h>
#include <SPI.h>
/*
   In this basic example we using 2 GPIO chip as out,
   one MCP23S17 and one MCP23S08, sharing the same SPI bus
   including CS pin, thanks Microchip proprietary HAEN.
   To do this, you need to assign different address to
   BOTH chip!
   Tested and worked.
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
//to use HAEN, address should be 0x20 (range:0x20->0x27)
gpio_MCP23SXX mcp1(MCP23S17, D1, 0x20);
//this use MCP23S08 assigned to address 0x21 (range:0x20->0x22)
gpio_MCP23SXX mcp2(MCP23S08, D1, 0x21);

void setup() {
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000));
  Serial.println("started");
  mcp1.begin();                     //Init first GPIO, this initialize SPI bus as well
  mcp2.begin();                     //Init second GPIO
  mcp1.gpioPinMode(OUTPUT);         //set every GPIO1 port as OUT
  mcp1.gpioPort(0xFFFF);            //Set all port as '1'
  mcp2.gpioPinMode(OUTPUT);         //set every GPIO2 port as OUT
  mcp2.gpioPort(0xFF);              //Set all port as '1'
}


void loop() {
  //this is the classic digital write, GPIO version. mcp1.ports() contain the number of
  //port handled by chip
  for (int i = 0; i < mcp1.ports(); i++) {
    mcp1.gpioDigitalWrite(i, 0);
    delay(20);
    mcp1.gpioDigitalWrite(i, 1);
    delay(20);
  }
  for (int i = 0; i < mcp2.ports(); i++) {
    mcp2.gpioDigitalWrite(i, 0);
    delay(20);
    mcp2.gpioDigitalWrite(i, 1);
    delay(20);
  }
  /*
    for (int i = 0; i < mcp1.ports(); i++) {
    //gpioDigitalInvert reads internal tracked GPIO port value
    //and invert it, then update GPIO.
    mcp1.gpioDigitalInvert(i);
    delay(10);
    }

    for (int i = 0; i < mcp2.ports(); i++) {
    mcp2.gpioDigitalInvert(i);
    delay(10);
    }

    for (int i = 0; i < mcp1.ports(); i++) {
    mcp1.gpioDigitalInvert(i);
    delay(10);
    }

    for (int i = 0; i < mcp2.ports(); i++) {
    mcp2.gpioDigitalInvert(i);
    delay(10);
    }
  */
}
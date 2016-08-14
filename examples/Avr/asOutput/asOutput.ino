#include <gpio_MCP23SXX.h>
#include <SPI.h>
/*
 * In this basic example the MCP GPIO Chip is used as OUT
 */

/*
  --- UNO
  MOSI(11) MISO(12) SCLK(13) CS(any)
*/
//define CS pin and using HAEN or not
//to use HAEN, address should be 0x20 to 0x27 for MCP23S17
gpio_MCP23SXX mcp1(MCP23S17, 10, 0x20);

void setup() {
  Serial.begin(38400);
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
/*
  In this example we will use a MCP23S17 with 16 push buttons.
  The example show how to deal with registers to set-up the GPIO chip in different mode than default.
  It use a CPU interrupt to trig the key decode, all code massively commented!
*/
#include <SPI.h>
#include <gpio_MCP23SXX.h>

/*
  --- ESP8266
  MOSI(13) MISO(12) SCLK(14) CS(5,4,0,2)
  ESP12E Dev Board 0.9, 1.0
  MOSI: D7
  MISO: D6
  SCLK: D5
  CS:   D1,D2,D3,D4
*/

#define GPIO_ADRS1         0x20
#define GPIO_CS            D1//5
#define GPIO_INTPIN        D2//4

volatile boolean keyPressed     = false;

gpio_MCP23SXX mcp1(MCP23S17, GPIO_CS, GPIO_ADRS1);


void setup() {
  pinMode(GPIO_INTPIN, INPUT_PULLUP);
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000));
  mcp1.begin();//it will initialize SPI as well
  //Change a bit how the gpio has been configured by default
  /*In brief, we mirror the INT pin, enable Sequence Operation, use HAEN*/
  if (!mcp1.gpioSetup(_MIRROR | _SEQOP | _HAEN)) {
    Serial.println("\ncaution, some feature requested not available for this chip!");
  }
  else {
    Serial.println("\nMCP1 setup OK");
  }
  //Once setup the 
  mcp1.gpioPinMode(INPUT);                                //bank A & B as INPUT
  mcp1.gpioPortPullup(0xFFFF);                            //enable pullup internal resistor for both banks
  mcp1.gpioPortInvert(0xFFFF);                            //invert the polarity for both banks
  mcp1.gpioPortInterrupts(0xFFFF);                        //enable internal INT for both banks
  mcp1.gpioReadInterrupts();                              //read interrupt capture port A&B (here used to clear interrupts)

  int intNumber = mcp1.getInterruptNumber(GPIO_INTPIN);   //get CPU int number correspond to pin
  if (intNumber < 255) {                                  //pin choosed is legal, proceed
    attachInterrupt(intNumber, keypress, FALLING);        //attack interrupt
    //mcp1.usingInterrupt(intNumber);                     //inform SPI that we are using an INT (optional but better use), not available for ESP
    Serial.println("INT enabled!");
  } else {
    Serial.println("sorry, pin cannot be used for INT!");
  }

}

int onKeypress() {
  delay(10);                                              //debounce
  int result = -1;                                        //we use negative as default = nothing pressed
  uint16_t keyValue = mcp1.gpioInterruptOccourred();      //read data and clear interrupts
  if (keyValue) {                                         //positive? Something was pressed!
    for (uint8_t i = 0; i < mcp1.ports(); i++) {          //now detect wich pin was low by looping bits
      if (keyValue & (1 << i)) {                          //a little trick to get the 1 inside word
        result = i;                                       //return the position of the bit inside word
        break;                                            //this will exit loop
                                                          //if you want you can continue and find the other pushed button
                                                          //by removing break and add your own method to collect pushed keys
      }
    }
  }
  keyPressed = false;                                     //time to reset keypress
  return result;
}

void loop() {
  if (keyPressed) {
    int key = onKeypress();
    if (key >= 0) {
      Serial.println(key);
    }
  }
}

static void keypress() {
  keyPressed = true;
}
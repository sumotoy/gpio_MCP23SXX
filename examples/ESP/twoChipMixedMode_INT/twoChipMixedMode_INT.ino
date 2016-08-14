/*
  This is a bit more complex example, 2 different GPIO chip will be used:
  MCP23S08 as GPIO2, only as OUTPUT
  MCP23S17 as GPIO1, bank A as INPUT, bank B as OUTPUT so you can test it by short any bank A GPIO pin
  An INT will be used for MCP23S17 to detect a push button.
  Any button pressed on GPIO1/bank A will trigger LOW the same pin on GPIO2!
  NOTE about INT pin:
  Even if ESP8266 it's an ARM and any pin can be virtually used as INT this in real life is not possible since
  some pin should be pullup/pulldown for correct operations, in brief, check your ESP module for a pin that is
  free from this restrictions. In this case I've used an ESP12 and ESP12E.
  Tested and worked.
*/
#include <SPI.h>
#include <gpio_MCP23SXX.h>   // import library

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
#define GPIO_ADRS2         0x21
#define GPIO_CS            D1
#define GPIO_INTPIN        D2//You need to pullup this one at 3v3 trough a 10K resistor!

volatile boolean keyPressed     = false;

gpio_MCP23SXX mcp1(MCP23S17, GPIO_CS, GPIO_ADRS1);
gpio_MCP23SXX mcp2(MCP23S08, GPIO_CS, GPIO_ADRS2); //see? same CS pin! We use HAEN!


void setup() {
  pinMode(GPIO_INTPIN, INPUT);
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000));
  mcp1.begin();                                       //it will initialize SPI as well
  mcp2.begin();                                       //thanks instance counter this one will NOT init SPI
  //Change a bit how the gpio has been configured by default
  /*In brief, we mirror the INT pin, enable Sequence Operation, use HAEN*/
  //if (!mcp1.gpioSetup(_MIRROR | _SEQOP | _HAEN | _ODR)) {//for some reason I've got some freezeup with ESP by using ODR.
  if (!mcp1.gpioSetup(_MIRROR | _SEQOP | _HAEN)) {
    Serial.println("\ncaution, some feature requested not available for this chip!");
  }
  else {
    Serial.println("\nAll features available for MCP1");
  }

  mcp2.gpioPinMode(OUTPUT);                             //MCP23S08 as OUTPUT only
  mcp2.gpioPort(0xFF);                                  //Set MCP23S08 port all at 1
  //Now we configure the GPIO1 to work as mixed IN/OUT mode and set internal INT
  mcp1.gpioPinMode(0b0000000011111111);                 //bank B as OUTPUT, bank A as INPUT
  mcp1.gpioPort(0b1010101100000000);                    //just for fun, we set at 1 some pin of the bank B
  mcp1.gpioPortPullup(0b0000000011111111);              //enable pullup internal resistor for bank A
  mcp1.gpioPortInvert(0b0000000011111111);              //invert the polarity of the bank A
  mcp1.gpioPortInterrupts(0b0000000011111111);          //enable internal INT on the entire bank A
  mcp1.gpioReadInterrupts();                            //read interrupt capture port A&B (it clear interrupts), bank B is automatically ignored

  int intNumber = mcp1.getInterruptNumber(GPIO_INTPIN); //get CPU int number correspond to pin (on ESP this is very rough)
  if (intNumber < 255) {                                //pin choosed is legal, proceed
    attachInterrupt(intNumber, keypress, FALLING);      //attack interrupt
    //mcp1.usingInterrupt(intNumber);                   //inform SPI that we are using an INT (optional but better use), not available on ESP
    Serial.println("INT enabled!");
  } else {
    Serial.println("sorry, pin cannot be used as INT");
  }

}

int onKeypress() {
  delay(10);//debounce
  int result = -1;                                      //we use negative as default = nothing pressed
  uint16_t keyValue = mcp1.gpioInterruptOccourred();    //read data and clear interrupts
  if (keyValue) {                                       //positive? Something was pressed!
    for (uint8_t i = 0; i < mcp1.ports(); i++) {        //now detect wich pin was low by looping bits
      if (keyValue & (1 << i)) {                        //a little trick to get the 1 inside word
        result = i;                                     //return the position of the bit inside word
        break;                                          //this will exit loop
        //if you want you can continue and find the other pushed button
        //by removing break and add your own method to collect pushed keys
      }
    }
  }
  keyPressed = false;                                   //time to reset keypress
  return result;
}

void loop() {
  if (keyPressed) {
    int key = onKeypress();
    if (key >= 0) {
      Serial.println(key);
      mcp2.gpioDigitalInvert(key);
    }
  }
}

static void keypress() {
  keyPressed = true;
}
/*
							   _
 ___  _   _  _ __ ___    ___  | |_  ___   _   _
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/

	gpio_MCP23SXX - A complete library for Microchip MCP23SXX GPIO series, compatible with many MCU's.

model:			company:		pins:		protocol:		Special Features:
---------------------------------------------------------------------------------------------------------------------
mcp23s08		Microchip		 8			SPI					INT/HAEN
mcp23s09		Microchip		 8			SPI					INT/OPEN DRAIN
mcp23s17		Microchip		 16			SPI					INT/HAEN
mcp23s18		Microchip		 16			SPI					INT/OPEN DRAIN
---------------------------------------------------------------------------------------------------------------------
Version history:
1.1: A third reincarnation, this time an attempt to a definitive one
---------------------------------------------------------------------------------------------------------------------
		Copyright (c) 2013-2016, s.u.m.o.t.o.y [sumotoy(at)gmail.com]
---------------------------------------------------------------------------------------------------------------------
Dependancies:
To simplify programming and ensure the best performances and compatibility I use separated shared libraries for each CPU family,
so I created specialized SPI fast libraries.
These libraries are included during compilation time so you have to download and put inside your libraries folder.
	For Teensy 3.0,3.1,3.2,3.5,3.6 this library use SPI_FIFO library for SPI https://github.com/sumotoy/SPI_FIFO_t3
	For Teensy LC this library use SPI_LC library for SPI				 	 https://github.com/sumotoy/SPI_LC
	For ESP8266 this library uses SPI_ESP for SPI.							 https://github.com/sumotoy/SPI_ESP
	For Arduino 8bit this library uses SPI_AVR for SPI						 https://github.com/sumotoy/SPI_AVR
	For Arduino DUE this library uses SPI_DUE for SPI						 https://github.com/sumotoy/SPI_DUE
	In settings file there's an option to force force use the 'legacy SPI', in that case all needed libraries are not needed.
---------------------------------------------------------------------------------------------------------------------
Licence:
	Licensed as GNU General Public License.
    gpio_MCP23SXX Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gpio_MCP23SXX Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _GPIO_MCP23SXX_H_
#define _GPIO_MCP23SXX_H_

#include "Arduino.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <stdio.h>
#include <stdlib.h>

#include "_includes/_cpuCommons.h"

#include <SPI.h>

//Include registers and settings
#include "_includes/MCP23XXX_registers.h"
#include "_settings/gpio_MCP23SXX_settings.h"

enum gpio_MCP23SXX_chip:uint8_t {MCP23S08=0,MCP23S09=1,MCP23S17=2,MCP23S18=3,MCPNONE=255};

class gpio_MCP23SXX {
	
public:
	enum gpio_MCP23SXX_reg:uint8_t {IODIR=0,IPOL=1,GPINTEN=2,DEFVAL=3,INTCON=4,IOCON=5,GPPU=6,INTF=7,INTCAP=8,GPIO=9,OLAT=10};
	static uint8_t gpio_MCP23SXX_instance;//used to keep track of the instances
	#if defined(__AVR__) || defined(ESP8266)
		gpio_MCP23SXX(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs);
		void		postInstance(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs);
	#elif (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))
		gpio_MCP23SXX(enum gpio_MCP23SXX_chip  chip,const uint8_t csPin,const uint8_t haenAdrs=0x20,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);//any pin,0x20....0x27 //enum gpio_MCP23SXX_chip
		void		postInstance(enum gpio_MCP23SXX_chip  chip,const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin);
	#endif
	gpio_MCP23SXX();//For include inside other libraries
	void 			begin(bool avoidSpiInit=false);
	bool 			gpioSetup(uint8_t data);						//setup the entire chip
	void 			gpioPinMode(uint16_t mode);						//OUTPUT=all out,INPUT=all in,0xxxx=you choose
	void 			gpioPinMode(uint8_t pin, bool mode);			//set a unique pin as IN(1) or OUT (0)
	void 			gpioPort(uint16_t value);						//HIGH=all Hi, LOW=all Low,0xxxx=you choose witch low or hi
	void			gpioPort(uint8_t bankA, uint8_t bankB);			//same as abowe but uses 2 separate bytes
	void 			gpioDigitalWrite(uint8_t pin, bool state);  	//(ok)write data to one pin and update chip
	bool			gpioDigitalWriteFast(uint8_t pin, bool state);  //write data to one pin but NOT send to chip
	void			gpioPortUpdate(void);							//update chip
	void			gpioPortPullup(uint16_t data);					//HIGH=all pullup, LOW=all pulldown,0xxxx=you choose witch
	void			gpioPortInvert(uint16_t data);					//Invert port reading bit for each port
	void			gpioPortInterrupts(uint16_t data);				//enable interrupt for each port
	void 			gpioPortIntControl(uint16_t data);
	void 			gpioPortLatches(uint16_t data);					//set output latches, 1 cause port as OUT
	void 			gpioPortDefValues(uint16_t data);
	void			gpioDigitalInvert(uint8_t pin);					//invert the state of a pin (it reads internal reg and perform invert)
	bool 			gpioDigitalReadFast(uint8_t pin);				//read pin state inside lib (not from chip)
	uint16_t 		gpioReadPortFast(void);							//read port state inside lib (not from chip)
	uint16_t 		gpioReadRegisterBoth(byte reg);					//read both register bank
	uint8_t 		gpioReadRegister(byte reg);						//read byte register
	uint16_t 		gpioReadPorts(void);							//read the state of the pins (all)
	uint8_t			gpioReadPort(uint8_t bank);						//read state of port a(0) or b(1)
	bool 			gpioDigitalRead(uint8_t pin);					//read pin state from chip directly
	uint8_t			gpioReadInterrupts(uint8_t bank);
	uint16_t 		gpioReadInterrupts(void);
	uint8_t 		gpioInterruptOccourred(uint8_t bank);
	uint16_t 		gpioInterruptOccourred(void);
	int 			getInterruptNumber(uint8_t pin);				//get the interrupt number corrisponding pin (CPU based)
	uint8_t			getSPIbus(void);
	uint8_t			ports(void);
	byte 			reg(gpio_MCP23SXX_reg reg);//enum gpio_MCP23SXX_reg
	#if defined(SPI_HAS_TRANSACTION)
	void			setSpiSettings(SPISettings settings);
	//void			usingInterrupt(uint8_t n);
	#endif
	//void 			printPacket(word data, uint8_t count=16);

protected:

	volatile uint16_t			_gpioDirection;
	volatile uint16_t			_gpioState;
	uint8_t						_IOCON_setup;
	enum gpio_MCP23SXX_chip		_chip;
	uint8_t						_regIndex;

	uint16_t _reverseWordOrder(uint16_t data) __attribute__((always_inline)) {
		return ((((data) & 0xFF) << 8) | ((data) >> 8));
	}
	
	#if !defined(SPI_LEGACY_METHOD)
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			SPI_FIFO_t3			_spi = SPI_FIFO_t3();
		#elif defined(ESP8266)
			SPI_ESP				_spi = SPI_ESP();
		#endif
	#else
		uint8_t					_spiBus;
	#endif

private:
	#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))
		uint8_t 		_cs, _miso, _mosi, _sclk;
	#elif defined(__AVR__)
		uint8_t 		_cs;
	#elif defined(ESP8266)
		uint32_t 		_cs;
	#endif
	uint8_t			_ports;
	byte 			_readCmd;
	byte 			_writeCmd;
	uint8_t			_initError;
	uint8_t 		_adrs;
	uint8_t 		_useHaen;
	void 			_GPIOwriteByte(byte reg, byte data);
	//void 			_GPIOwriteWord(byte reg, uint16_t data);
	void 			_getChipFeatures(enum gpio_MCP23SXX_chip chip,uint8_t adrs);//enum gpio_MCP23SXX_chip
	void			_sendData(byte reg,uint16_t data,bool forceByte=false);
	#if defined(SPI_LEGACY_METHOD)
		void			enableCS(void);
		void			disableCS(void);
		uint8_t 		beginSpi(bool avoidSpiInit=false);
		void 			startTransaction(void);
		void 			endTransaction(void);
		void 			writeByte_cont(byte val);
		void 			writeByte_last(byte val);
		void 			writeWord_cont(uint16_t val);
		void 			writeWord_last(uint16_t val);
		uint8_t 		readByte_cont(void);
		uint16_t 		readWord_cont(void);
	#endif
};
#endif
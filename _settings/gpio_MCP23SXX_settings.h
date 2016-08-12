/*=========================================================================================
	Part of gpio_MCP23SXX library
    Copyright (c) 2016, .S.U.M.O.T.O.Y., coded by Max MC Costa.

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
	---------------------------------------------------------------------------------------
	User settings
===========================================================================================*/
#ifndef _GPIOMCP23SXX_USETT_H_
#define _GPIOMCP23SXX_USETT_H_
#include <stdio.h>
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
									USER SETTINGS
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*--------------------------------------------------------------------------------
- Max SPI speed supported -
This is the max spi speed supported by those chip, remember that datasheet
spot limit to 10Mhz! Better do not change this to higher value, if you have
problems you can slow down a little.
----------------------------------------------------------------------------------*/
static const uint32_t	_maxGpioSPIspeed	= 24000000;
/*--------------------------------------------------------------------------------
- ESP8266 Faster SPI -
This force library to use the SPI.write method instead the legacy SPI.transfer.
As result is much faster. (Thanks Reaper7)
Default:uncommented
----------------------------------------------------------------------------------*/
#if defined(ESP8266)
	#define _ESP8266_SPIFAST
#endif
/*--------------------------------------------------------------------------------
- ESP8266 Compatibility mode -
This force library to use an alternative way to trigger ESP8266 GPIO, if you uncomment
the line it will use the standard digitaWrite wich is slow, this help debugging.
NOTE: uncomment this, code is MUCH slower!
Default:commented
----------------------------------------------------------------------------------*/
#if defined(ESP8266)
	//#define _ESP8266_STANDARDMODE
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//			CONSTANT
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	static const uint8_t			_BANKOP				= 	0x80;// The registers associated with each port are separated into different banks
	static const uint8_t			_MIRROR				= 	0x40;// The INT pins are internally connected
	static const uint8_t			_SEQOP				= 	0x20;// Sequential operation disabled, address pointer does not increment
	static const uint8_t			_DISSLW				= 	0x10;// Slew Rate control bit for SDA output, only I2C
	static const uint8_t			_HAEN				= 	0x08;// Enables the MCP23S17 address pins (only SPI)
	static const uint8_t			_ODR				= 	0x04;// Open-drain output (overrides the INTPOL bit).
	static const uint8_t			_INTPOL				= 	0x02;// polarity of the INT output pin HIGH
	static const uint8_t			_INTCC				= 	0x01;// Interrupt Clearing Control

#endif

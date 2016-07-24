#ifndef __MCP23XXXREG_H
	#define __MCP23XXXREG_H
	
	#include <stdio.h>
	//----------------------------- REGISTERS ------------- BANK 0 --- BANK 1
	static const byte			MCP23XXX_IODIR[2]		= 	{0x00,0x00};// Controls the direction of the data I/O.
	static const byte			MCP23XXX_IPOL[2]		= 	{0x02,0x01};// Configure the polarity on the corresponding GPIO port bits.
	static const byte			MCP23XXX_GPINTEN[2]		=   {0x04,0x02};// Controls the interrupt-on-change feature for each pin.
	static const byte			MCP23XXX_DEFVAL[2] 		= 	{0x06,0x03};// Default comparison value for compare to
	static const byte			MCP23XXX_INTCON[2] 		= 	{0x08,0x04};// Controls how the associated pin value is compared for the interrupt-on-change feature.
	static const byte			MCP23XXX_IOCON[2]		= 	{0x0A,0x05};// Contains several bits for configuring the device.
	static const byte			MCP23XXX_GPPU[2]		= 	{0x0C,0x06};// Controls the pull-up resistors for the port  pins.
	static const byte			MCP23XXX_INTF[2]		= 	{0x0E,0x07};// Reflects the interrupt condition on the port pins. If a bit is set, an Int occourred
	static const byte			MCP23XXX_INTCAP[2] 		= 	{0x10,0x08};// Captures the GPIO port value at thetimetheinterrupt occurred.Cleared by INTCAP or GPIO
	static const byte			MCP23XXX_GPIO[2]		= 	{0x12,0x09};// Reflects the value on the port.
	static const byte			MCP23XXX_OLAT[2]		= 	{0x14,0x0A};// provides access to the output latches. Set as 1 configure port as OUT.
/*
The IOCON register!
MCP23S08----------------------------------------------------------------
Ports:8	Address:0x20...0x23		HAEN:Yes
------------------------------------------------------------------------
7: [NA] *****
6: [NA] *****
5: SEQOP: (Sequential Operation mode bit)
	1 Sequential operation disabled, address pointer does not increment
	0 Sequential operation enabled, address pointer increments.
4: DISSLW: (Slew Rate control bit for SDA output, only I2C)
3: HAEN: (Hardware Address Enable bit, SPI only)
	1 Enables the MCP23S17 address pins
	0 Disables the MCP23S17 address pins
2: ODR: (This bit configures the INT pin as an open-drain output)
	1 Open-drain output (overrides the INTPOL bit).
	0 Active driver output (INTPOL bit sets the polarity).
1: INTPOL: (This bit sets the polarity of the INT output pin)
	1 Active high
	0 Active low
0: [NA] *****

MCP23S09----------------------------------------------------------------	
Ports:8	Address:0x20			HAEN:No
------------------------------------------------------------------------
7: [NA] *****
6: [NA] *****
5: SEQOP: (Sequential Operation mode bit)
	1 Sequential operation disabled, address pointer does not increment
	0 Sequential operation enabled, address pointer increments.
4: [NA] *****
3: [NA] *****
2: ODR: (This bit configures the INT pin as an open-drain output)
	1 Open-drain output (overrides the INTPOL bit).
	0 Active driver output (INTPOL bit sets the polarity).
1: INTPOL: (This bit sets the polarity of the INT output pin)
	1 Active high
	0 Active low
0: INTCC: (Interrupt Clearing Control)
	1 Reading INTCAP register clears the interrupt
	0 Reading GPIO register clears the interrupt

MCP23S17----------------------------------------------------------------
Ports:16	Address:0x20...0x27	HAEN:Yes
------------------------------------------------------------------------
7: BANK: (Controls how the registers are addressed)
	1 The registers associated with each port are separated into different banks
	0 The registers are in the same bank (addresses are sequential)
6: MIRROR: (INT Pins Mirror bit)
	1 The INT pins are internally connected
	0 The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB
5: SEQOP: (Sequential Operation mode bit)
	1 Sequential operation disabled, address pointer does not increment
	0 Sequential operation enabled, address pointer increments.
4: DISSLW: (Slew Rate control bit for SDA output, only I2C)
3: HAEN: (Hardware Address Enable bit, SPI only)
	1 Enables the MCP23S17 address pins
	0 Disables the MCP23S17 address pins
2: ODR: (This bit configures the INT pin as an open-drain output)
	1 Open-drain output (overrides the INTPOL bit).
	0 Active driver output (INTPOL bit sets the polarity).
1: INTPOL: (This bit sets the polarity of the INT output pin)
	1 Active high
	0 Active low
0: [NA] *****
	
MCP23S18----------------------------------------------------------------	
Ports:16	Address:0x20	  HAEN:No
------------------------------------------------------------------------
7: BANK: (Controls how the registers are addressed)
	1 The registers associated with each port are separated into different banks
	0 The registers are in the same bank (addresses are sequential)
6: MIRROR: (INT Pins Mirror bit)
	1 The INT pins are internally connected
	0 The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB
5: SEQOP: (Sequential Operation mode bit)
	1 Sequential operation disabled, address pointer does not increment
	0 Sequential operation enabled, address pointer increments.
4: [NA] *****
3: [NA] *****
2: ODR: (This bit configures the INT pin as an open-drain output)
	1 Open-drain output (overrides the INTPOL bit).
	0 Active driver output (INTPOL bit sets the polarity).
1: INTPOL: (This bit sets the polarity of the INT output pin)
	1 Active high
	0 Active low
0: INTCC: (Interrupt Clearing Control)
	1 Reading INTCAP register clears the interrupt
	0 Reading GPIO register clears the interrupt
*/

	
#endif

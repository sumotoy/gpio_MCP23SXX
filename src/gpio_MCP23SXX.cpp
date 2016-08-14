#include "gpio_MCP23SXX.h"

//instances counter
uint8_t gpio_MCP23SXX::gpio_MCP23SXX_instance = 0;

//main instance for include
gpio_MCP23SXX::gpio_MCP23SXX(){}

//normal instances
#if defined(__AVR__) || defined(ESP8266)
	gpio_MCP23SXX::gpio_MCP23SXX(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs)
	{
		postInstance(chip,csPin,haenAdrs);
	}

	void gpio_MCP23SXX::postInstance(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs)//enum gpio_MCP23SXX_chip
	{
		_cs = csPin;
		_getChipFeatures(chip,haenAdrs);
	}
#elif (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))
	gpio_MCP23SXX::gpio_MCP23SXX(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
	{
		postInstance(chip,csPin,haenAdrs,mosi_pin,sclk_pin,miso_pin);
	}

	void gpio_MCP23SXX::postInstance(enum gpio_MCP23SXX_chip chip,const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
	{
		_mosi = mosi_pin;
		_miso = miso_pin;
		_sclk = sclk_pin;
		_cs = csPin;
		_getChipFeatures(chip,haenAdrs);
	}
#endif

/*
  Init the GPIO chip. It also init SPI bus.
	There's a lot of stuff running inside this function, it checks
	for legal pins and SPI bus, can handle alternative SPI bus and pins,
	init correctly SPI bus and only once (in case of multiple instances).
	Parameters
	avoidSpiInit: if you use this library after any other SPI devices
	that already have SPI inited, you may use this option that will avoid SPI.begin().
	Normally you better don't use at all this option...
*/
void gpio_MCP23SXX::begin(bool avoidSpiInit)
{
	if (_chip == MCPNONE) return;//unknow chip
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
	_gpioDirection	= 0xFFFF;//all inputs
	_gpioState		= 0x0000;//bogus
	gpio_MCP23SXX_instance += 1;//instance counter
	if (gpio_MCP23SXX_instance > 1) avoidSpiInit = true;//do not init SPI again
	#if defined(SPI_LEGACY_METHOD)
	//using internal SPI methods
		if (beginSpi(avoidSpiInit) != 0xFF) return;//if != 0xFF cannot continue
		//set SPI speed...
		#if defined(SPI_HAS_TRANSACTION)
			setSpiSettings(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0));
		#else
			//pre - SPI transaction era...
			SPI.setClockDivider(SPI_CLOCK_DIV4);
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE0);
		#endif
	#else
	//using high speed external SPI libraries
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__))
			_spi.postInstance(_cs,255,_mosi,_sclk,_miso);
		#elif (defined(__AVR__) || defined(ESP8266))
			_spi.postInstance(_cs,255);
		#endif
		//begin SPI and set SPI speed...
		#if defined(SPI_HAS_TRANSACTION)
			_spi.begin(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0),avoidSpiInit);
		#else
			_spi.begin(avoidSpiInit);
		#endif
	#endif
/*
The IOCON register!
                 7     6     5   	4      3    2    1      0
MCP23S08 IOCON = NA   NA     SEQOP DISSLW HAEN ODR INTPOL  NA
MCP23S09 IOCON = NA   NA     SEQOP NA     NA   ODR INTPOL  INTCC
MCP23S17 IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL  NA
MCP23S18 IOCON = BANK MIRROR SEQOP NA     NA   ODR INTPOL  INTCC
*/
	if (_chip == MCP23S09 || _chip == MCP23S18){
		//these chip doesn't use HAEN
		gpioSetup(_SEQOP | _INTCC);//enable INTCC always
	} else {
		_useHaen == 1 ? gpioSetup(_SEQOP | _HAEN) : gpioSetup(_SEQOP);
	}
	//as default, GPIO init as High-Impedance input
	gpioPinMode(INPUT);
}

//--------------------------------- LOW LEVEL SPI ROUTINES --------------------------------------------------------
// Send a byte to a chip register...
void gpio_MCP23SXX::_GPIOwriteByte(byte reg, byte data)
{
	#if defined(SPI_LEGACY_METHOD)
		startTransaction();
			writeByte_cont(_writeCmd);
			writeByte_cont(reg);
		writeByte_last(data);
		endTransaction();
	#else
		_spi.startTransaction();
			_spi.writeByte_cont(_writeCmd);
			_spi.writeByte_cont(reg);
		_spi.writeByte_last(data);
		_spi.endTransaction();
	#endif
}


/*
  Send data to a register of a GPIO chip, 
	this is the main function that send data to the GPIO chip once is inited.
	Parameters
	reg: a legal 8bit MCP23Sxx register.
	data: 16 bit data to transfer.
	forceByte: not used till now, experimental. Maybe I will use in future.
*/
void gpio_MCP23SXX::_sendData(byte reg,uint16_t data,bool forceByte)
{
	#if defined(SPI_LEGACY_METHOD)
		startTransaction();
			writeByte_cont(_writeCmd);
			writeByte_cont(reg);
			if (_ports < 16 || (forceByte == true)){
				uint8_t temp = (uint8_t)(data & 0xFF);
				writeByte_last(temp);
			} else {
				data = _reverseWordOrder(data);
			writeWord_last(data);
			}
			endTransaction();
	#else
		_spi.startTransaction();//cs low
			_spi.writeByte_cont(_writeCmd);
			_spi.writeByte_cont(reg);
			if (_ports < 16 || forceByte){
				uint8_t temp = (uint8_t)(data & 0xFF);
				_spi.writeByte_last(temp);
			} else {
				data = _reverseWordOrder(data);
			_spi.writeWord_last(data);
			}
			_spi.endTransaction();
	#endif
}

/*
  Return register content from bank A & B of a 16 bit GPIO chip,
	combined in a 16 bit data. An 8 Bit chip will return always bank A.
	Parameters
	reg: a legal 8bit MCP23Sxx register.
*/
uint16_t gpio_MCP23SXX::gpioReadRegisterBoth(byte reg)
{
	if (_ports < 16){
		return gpioReadRegister(reg);
	} else {
		uint16_t result = 0;
	#if defined(SPI_LEGACY_METHOD)
		startTransaction();
		writeByte_cont(_readCmd);
		writeByte_cont(reg);
		#if defined(SPI_HAS_TRANSACTION)
			setSpiSettings(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			result = readWord_cont();
			setSpiSettings(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0));
		#else
			result = readWord_cont();
		#endif
		disableCS();
		endTransaction();
	#else
		_spi.startTransaction();
		_spi.writeByte_cont(_readCmd);
		_spi.writeByte_cont(reg);
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			_spi.waitTransmitComplete();
		#endif
		#if defined(SPI_HAS_TRANSACTION)
			_spi.setSpiSettings(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			result = _spi.readWord_cont(false);//command mode
			_spi.setSpiSettings(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0));
		#else
			result = _spi.readWord_cont(false);//command mode
		#endif
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			_spi.writeByte_last(0xFF);//NOP?
		#else
			_spi.disableCS();
		#endif
		_spi.endTransaction();
	#endif
		return result;
	}
}

/*
  Returns 8 bit register content from a GPIO chip.
	Parameters
	reg: a legal 8bit MCP23Sxx register.
*/
uint8_t gpio_MCP23SXX::gpioReadRegister(byte reg)
{
	uint8_t result = 0;
	#if defined(SPI_LEGACY_METHOD)
		startTransaction();
		writeByte_cont(_readCmd);
		writeByte_cont(reg);
		#if defined(SPI_HAS_TRANSACTION)
			setSpiSettings(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			result = readByte_cont();
			setSpiSettings(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0));
		#else
			result = readByte_cont();
		#endif
		disableCS();
		endTransaction();
	#else
		_spi.startTransaction();
		_spi.writeByte_cont(_readCmd);
		_spi.writeByte_cont(reg);
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			_spi.waitTransmitComplete();
		#endif
		#if defined(SPI_HAS_TRANSACTION)
			_spi.setSpiSettings(SPISettings(10000000, MSBFIRST, SPI_MODE0));
			result = _spi.readByte_cont(false);//command mode
			_spi.setSpiSettings(SPISettings(_maxGpioSPIspeed, MSBFIRST, SPI_MODE0));
		#else
			result = _spi.readByte_cont(false);//command mode
		#endif
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			_spi.writeByte_last(0xFF);//NOP?
		#else
			_spi.disableCS();
		#endif
		_spi.endTransaction();
	#endif
	return result;
}

#if !defined(SPI_LEGACY_METHOD)
int gpio_MCP23SXX::getInterruptNumber(uint8_t pin)
{
	return _spi.getInterruptNumber(pin);
}

uint8_t gpio_MCP23SXX::getSPIbus(void)
{
	return _spi.getSPIbus();
}
#endif

/*
  Returns the number of ports handled by current GPIO chip.
	Parameters
	none
*/
uint8_t gpio_MCP23SXX::ports(void)
{
	return _ports;
}

//----------------------------------------------------------------------------------------------------
//------------------------- CHIP ROUTINES ------------------------------------------------------------
/* Set ALL or individual chip pin port as IN/OUT
	Parameters
	mode:16 bit data or INPUT or OUTPUT
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPinMode(uint16_t mode)
{
	if (mode == INPUT){
		_gpioDirection = 0xFFFF;
	} else if (mode == OUTPUT){
		_gpioDirection = 0x0000;
		_gpioState = 0x0000;
	} else {
		_gpioDirection = mode;
	}
	_sendData(MCP23XXX_IODIR[_regIndex],_gpioDirection);
}

/* Set individual chip pin port as IN/OUT
	Parameters
	pin:0...7/15
	mode:0(out), 1(in)
*/
void gpio_MCP23SXX::gpioPinMode(uint8_t pin, bool mode)
{
	if (pin < _ports){//0...7/15
		mode == INPUT ? _gpioDirection |= (1 << pin) :_gpioDirection &= ~(1 << pin);
		_sendData(MCP23XXX_IODIR[_regIndex],_gpioDirection);
	}
}


/* Set ALL ports or individually as High or Low
	Parameters
	value:16 bit data or HIGH or LOW
	NOTE:
	writing GPIO actually = OLAT, in a pin is conf as input
	it will goes as high impedance
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPort(uint16_t value)
{
	if (value == HIGH){
		value = 0xFFFF;
	} else if (value == LOW){
		value = 0x0000;
	}
	_gpioState = value & ~_gpioDirection;//only pins with OUT, IN ignored
	//_gpioState = value;
	_sendData(MCP23XXX_GPIO[_regIndex],_gpioState);
}


/* Set ALL ports as High or Low by using separate banks.
	Parameters
	bankA:8 bit data for port A
	bankB:8 bit data for port B
	NOTE:
	writing GPIO actually = OLAT, in a pin is conf as input
	it will goes as high impedance
	GPIO at 8bit uses only bankA!
*/
void gpio_MCP23SXX::gpioPort(uint8_t bankA, uint8_t bankB)
{
	_gpioState = (bankB << 8) | bankA;
	_gpioState = _gpioState & ~_gpioDirection;//only pins with OUT, IN ignored
	_sendData(MCP23XXX_GPIO[_regIndex],_gpioState);
}

/* Send internal _gpioState register to chip
	Parameters
	none
	NOTE:
	writing GPIO actually = OLAT, in a pin is conf as input
	it will goes as high impedance
*/
void gpio_MCP23SXX::gpioPortUpdate(void)
{
	_sendData(MCP23XXX_GPIO[_regIndex],_gpioState);
}

/*
	NOTE:
	writing GPIO actually = OLAT, in a pin is conf as input
	it will goes as high impedance
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortLatches(uint16_t data)
{
	if (data == HIGH){//TODO
		data = 0xFFFF;
	} else if (data == LOW){
		data = 0x0000;
	}
	data = data & ~_gpioDirection;//only pins with OUT, IN ignored
	_sendData(MCP23XXX_OLAT[_regIndex],data);
}

/*

	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortDefValues(uint16_t data)
{
	if (data == HIGH){//TODO
		data = 0xFFFF;
	} else if (data == LOW){
		data = 0x0000;
	}
	data = data & _gpioDirection;//only pins with IN, OUT ignored
	_sendData(MCP23XXX_DEFVAL[_regIndex],data);
}

/* Set individual pin as High or Low and update immediately chip
	Parameters
	pin:0...7/15
	value: 0(low),1(high)
*/
void gpio_MCP23SXX::gpioDigitalWrite(uint8_t pin, bool state)
{
	if (gpioDigitalWriteFast(pin,state)) gpioPortUpdate();
}

/* Set individual pin as High or Low in the _gpioState register
	but not send to chip. You have to use gpioPortUpdate at the end.
	Return 1 if success or 0 if not.
	Parameters
	pin:0...7/15
	value: 0(low),1(high)
*/
bool gpio_MCP23SXX::gpioDigitalWriteFast(uint8_t pin, bool state)
{
	if (pin < _ports) {
		//TODO: check if pin is an input first.
		if (bitRead(_gpioDirection,pin)) return 0;//try to write to an INPUT?
		state == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
		return 1;
	} else {
		return 0;
	}
}

/* Set ALL or individual chip pin port in pullUp
	Parameters
	mode:16 bit data or HIGH or LOW
	Pullup works when pin are IN or OUT
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortPullup(uint16_t data)
{
	if (data == HIGH){
		_gpioState = 0xFFFF;
	} else if (data == LOW){
		_gpioState = 0x0000;
	} else {
		_gpioState = data;
	}
	_sendData(MCP23XXX_GPPU[_regIndex], _gpioState);
}

/* Set ALL or individual chip pin port inverted reading
	Parameters
	mode:16 bit data or HIGH or LOW
	If bit is 1, the correspondant polarity of the pin will invert
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortInvert(uint16_t data)
{
	if (data == HIGH){
		data = 0xFFFF;
	} else if (data == LOW){
		data = 0x0000;
	}
	//TODO: IN and OUT?
	_sendData(MCP23XXX_IPOL[_regIndex], data);
}

/*
	This works only for inputs!
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortIntControl(uint16_t data)
{
	data = data & _gpioDirection;//only pins with IN, OUT ignored
	_sendData(MCP23XXX_INTCON[_regIndex], data);
}

/* Set individual chip pin port generate interrupts
	Parameters
	mode:16 bit data corresponding individual pins, 0 no int, 1 int
	GPIO at 8bit uses only the first 8 bit!
*/
void gpio_MCP23SXX::gpioPortInterrupts(uint16_t data)
{
	data = data & _gpioDirection;//only pins with IN, OUT ignored
	_sendData(MCP23XXX_GPINTEN[_regIndex], data);
}

/* Setup the entire chip
The IOCON register!
				 7     6     5   	4      3    2    1      0
MCP23S08 IOCON = NA   NA     SEQOP DISSLW HAEN ODR INTPOL  NA
MCP23S09 IOCON = NA   NA     SEQOP NA     NA   ODR INTPOL  INTCC
MCP23S17 IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL  NA
MCP23S18 IOCON = BANK MIRROR SEQOP NA     NA   ODR INTPOL  INTCC

-----------------------------------------------------------------------
0b01101100
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
0: INTCC: (Interrupt Clearing Control)
	1 Reading INTCAP register clears the interrupt
	0 Reading GPIO register clears the interrupt
--------------------------------------------------------------------------
	Parameter
	data:8bit or you can use _BANKOP | _HAEN (etc)
	Note. Each time gpioSetup is called, it reset the IOCON
	register first!
*/
bool gpio_MCP23SXX::gpioSetup(uint8_t data)
{
	bool result = 1;
	if (_ports < 16){
		_regIndex = 1;
	} else {
		_regIndex = 0;
	}
	_IOCON_setup = data;
	switch(_chip){
		case MCP23S08:
			//need to clear something?
			if (bitRead(_IOCON_setup,7)) {_IOCON_setup &= ~(1 << 7); result = 0;}//NA
			if (bitRead(_IOCON_setup,6)) {_IOCON_setup &= ~(1 << 6); result = 0;}//NA
			if (bitRead(_IOCON_setup,0)) {_IOCON_setup &= ~(1 << 0); result = 0;}//NA
		break;
		case MCP23S09:
			if (bitRead(_IOCON_setup,7)) {_IOCON_setup &= ~(1 << 7); result = 0;}//NA
			if (bitRead(_IOCON_setup,6)) {_IOCON_setup &= ~(1 << 6); result = 0;}//NA
			if (bitRead(_IOCON_setup,4)) {_IOCON_setup &= ~(1 << 4); result = 0;}//NA
			if (bitRead(_IOCON_setup,3)) {_IOCON_setup &= ~(1 << 3); result = 0;}//NA
			_IOCON_setup |= (1 << 0);//always enable INTCC
		break;
		case MCP23S17:
			if (bitRead(_IOCON_setup,7)) _regIndex = 1;//1
			if (bitRead(_IOCON_setup,0)) {_IOCON_setup &= ~(1 << 0); result = 0;}//NA
		break;
		case MCP23S18:
			if (bitRead(_IOCON_setup,7)) _regIndex = 1;//1
			if (bitRead(_IOCON_setup,4)) {_IOCON_setup &= ~(1 << 4); result = 0;}//NA
			if (bitRead(_IOCON_setup,3)) {_IOCON_setup &= ~(1 << 3); result = 0;}//NA
			_IOCON_setup |= (1 << 0);//always enable INTCC
		break;
		case MCPNONE:
			return 0;
		break;
	}
	_IOCON_setup |= (1 << 5);//TODO: SEQOP always enabled in this library!?
	//CHECKTHIS!
	if (_ports < 16){
		_GPIOwriteByte(MCP23XXX_IOCON[1], _IOCON_setup);
		//_sendData(MCP23XXX_IOCON[1], _IOCON_setup,true);
	} else {
		_GPIOwriteByte(MCP23XXX_IOCON[0], _IOCON_setup);
		//_sendData(MCP23XXX_IOCON[0], _IOCON_setup,true);
	}
	return result;
}

/* Read the state of an individual pin by reading the internal library
	register, NOT the chip so take care! This is extremely fast and
	not use any SPI calls.
	Parameters
	pin:0...7/15
	Returns 0 or 1
*/
bool gpio_MCP23SXX::gpioDigitalReadFast(uint8_t pin)
{
	bool temp = 0;
	if (pin < _ports) temp = bitRead(_gpioState,pin);
	return temp;
}

/* It reads the internal _gpioState and perform inversion
	Parameters
	pin:0...7/15
*/
void gpio_MCP23SXX::gpioDigitalInvert(uint8_t pin)
{
	bool temp = gpioDigitalReadFast(pin);
	gpioDigitalWrite(pin,!temp);
}

/* It reads the _gpioState registrer, NOT from chip
	Parameters
	none
	Returns: 16bit register
*/
uint16_t gpio_MCP23SXX::gpioReadPortFast(void)
{
	return _gpioState;
}

/* It reads both ports from chip
	Parameters
	none
	Returns: 16bit register
*/
uint16_t gpio_MCP23SXX::gpioReadPorts(void)
{
	return gpioReadRegisterBoth(MCP23XXX_GPIO[_regIndex]);
}

/* It reads one bank from chip
	Parameters
	bank:0(bank A), 1(bankB)
	Returns: 8bit register
*/
uint8_t gpio_MCP23SXX::gpioReadPort(uint8_t bank)
{
	if (_ports < 16 || bank == 0){
		return gpioReadRegister(MCP23XXX_GPIO[_regIndex]);
	} else {
		if (_regIndex == 0){
			return gpioReadRegister(MCP23XXX_GPIO[_regIndex] + 1);
		} else {
			return gpioReadRegister(MCP23XXX_GPIO[_regIndex] + 0x10);
		}
	}
}

/* It reads one pin from chip
	Parameters
	pin:0...7/15
	Returns: 0 or 1
*/
bool gpio_MCP23SXX::gpioDigitalRead(uint8_t pin)
{
	if (pin < _ports) return (gpioReadRegisterBoth(MCP23XXX_GPIO[_regIndex]) & 1 << pin);
	return 0;
}

/* Returns one bank interrupts (and clear them)
	Parameters
	bank:0(bankA), 1(bankB)
	Returns: 8bit
*/
uint8_t	gpio_MCP23SXX::gpioReadInterrupts(uint8_t bank)
{
	if (_ports < 16 || bank == 0){
		return gpioReadRegister(MCP23XXX_INTCAP[_regIndex]);
	} else {
		if (_regIndex == 0){
			return gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 1);
		} else {
			return gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 0x10);
		}
	}
}

/* Returns both banks interrupts (and clear them)
	Parameters
	none
	Returns: 16bit
*/
uint16_t gpio_MCP23SXX::gpioReadInterrupts(void)
{
	uint8_t a = 0;
	uint8_t b = 0;
	b = gpioReadRegister(MCP23XXX_INTCAP[_regIndex]);
	if (_ports < 16) return b;
	if (_regIndex == 0){
		a = gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 1);
	} else {
		a = gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 0x10);
	}
	//CHECKTHIS
	#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
		return ((b << 8) | a);
	#else
		return ((a << 8) | b);
	#endif
}

/* Returns bank data to check if there was an interrupt and witch pin
	Parameters
	bank:0(bank A), 1(bank B)
	Returns: 8bit
	On 8 port GPIO the bank is always ignored, put any value
	or use gpioInterruptOccourred()
*/
uint8_t gpio_MCP23SXX::gpioInterruptOccourred(uint8_t bank)
{
	if (_ports < 16 || bank == 0){
		return gpioReadRegister(MCP23XXX_INTF[_regIndex]);
	} else {
		if (_regIndex == 0){
			return gpioReadRegister(MCP23XXX_INTF[_regIndex] + 1);
		} else {
			return gpioReadRegister(MCP23XXX_INTF[_regIndex] + 0x10);
		}
	}
}

/* Check if an interrupt occourred, read BOTH ports, clear interrups and give back
	what port(s) caused interrupt!
	Parameters
	none
	Returns: 16bit
	On 8 port GPIO only the first 8 bit should be used.
*/
uint16_t gpio_MCP23SXX::gpioInterruptOccourred(void)
{
	uint8_t a = 0;
	if (_ports > 8){
		uint8_t b = 0;
		if (gpioReadRegister(MCP23XXX_INTF[_regIndex])) 			b = gpioReadRegister(MCP23XXX_INTCAP[_regIndex]);
		if (_regIndex == 0){
			if (gpioReadRegister(MCP23XXX_INTF[_regIndex] + 1)) 	a = gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 1);
		} else {
			if (gpioReadRegister(MCP23XXX_INTF[_regIndex] + 0x10)) 	a = gpioReadRegister(MCP23XXX_INTCAP[_regIndex] + 0x10);
		}
		//CHECKTHIS
		#if (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
			return ((b << 8) | a);
		#else
			return ((a << 8) | b);
		#endif
	} else {
		if (gpioInterruptOccourred(0)) {
			return gpioReadRegister(MCP23XXX_INTCAP[_regIndex]);
		} else {
			return a;
		}
	}
}
#if !defined(SPI_LEGACY_METHOD) && defined(SPI_HAS_TRANSACTION)
/*
void gpio_MCP23SXX::usingInterrupt(uint8_t n)
{
	_spi.usingInterrupt(n);
}
*/

void gpio_MCP23SXX::setSpiSettings(SPISettings settings)
{
	_spi.setSpiSettings(settings);
}
#endif


void gpio_MCP23SXX::_getChipFeatures(enum gpio_MCP23SXX_chip chip,uint8_t adrs)//enum gpio_MCP23SXX_chip
{
	switch(chip){
		case MCP23S08:
		case MCP23S09:
			_chip = chip;
			_ports = 8;
			_regIndex = 1;
			if (chip == MCP23S08 && (adrs > 0x19 && adrs < 0x24)){
				_useHaen = 1;
				_adrs = adrs;
			} else {
				_useHaen = 0;
				_adrs = 0x20;
			}
		break;
		case MCP23S17:
		case MCP23S18:
			_chip = chip;
			_ports = 16;
			_regIndex = 0;
			if (chip == MCP23S17 && (adrs > 0x19 && adrs < 0x28)){
				_useHaen = 1;
				_adrs = adrs;
			} else {
				_useHaen = 0;
				_adrs = 0x20;
			}
		break;
		default:
			_useHaen = 0;
			_ports = 0;
			_adrs = 0;
			_chip = MCPNONE;
		break;
	}
}

//return correct register based on chip and bank
byte gpio_MCP23SXX::reg(gpio_MCP23SXX_reg reg)//enum gpio_MCP23SXX_reg
{
	switch(reg){
		case IODIR:
			return MCP23XXX_IODIR[_regIndex];
		case IPOL:
			return MCP23XXX_IPOL[_regIndex];
		case GPINTEN:
			return MCP23XXX_GPINTEN[_regIndex];
		case DEFVAL:
			return MCP23XXX_DEFVAL[_regIndex];
		case INTCON:
			return MCP23XXX_INTCON[_regIndex];
		case IOCON:
			if (_ports < 16){
				return MCP23XXX_IOCON[1];
			} else {
				return MCP23XXX_IOCON[0];
			}
		case GPPU:
			return MCP23XXX_GPPU[_regIndex];
		case INTF:
			return MCP23XXX_INTF[_regIndex];
		case INTCAP:
			return MCP23XXX_INTCAP[_regIndex];
		case GPIO:
			return MCP23XXX_GPIO[_regIndex];
		case OLAT:
			return MCP23XXX_OLAT[_regIndex];
		default:
			return 0xFF;//NOP?
	}
}

/* 
--------------------------------------------------------------------------
======= SPI LEGACY METHODS ===============================================
--------------------------------------------------------------------------
*/
#if defined(SPI_LEGACY_METHOD)

	void gpio_MCP23SXX::enableCS(void)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__)
			digitalWriteFast(_cs,LOW);
		#else
			digitalWrite(_cs,LOW);
		#endif
	}
	
	void gpio_MCP23SXX::disableCS(void)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__)
			digitalWriteFast(_cs,HIGH);
		#else
			digitalWrite(_cs,HIGH);
		#endif
	}

	uint8_t gpio_MCP23SXX::beginSpi(bool avoidSpiInit)
	{
		uint8_t _initError = 0xFF;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3.0 -> 3.2
		if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14) && (_miso == 255 || _miso == 12 || _miso == 8)) {
			if (!avoidSpiInit) SPI.begin();
			SPI.setMOSI(_mosi);
			if (_miso != 255) SPI.setMISO(_miso);
			SPI.setSCK(_sclk)
			if (SPI.pinIsChipSelect(_cs)){
				pinMode(_cs,OUTPUT);
				disableCS();
			} else {
				bitClear(_initError,1);
			}
		} else {
			bitClear(_initError,0);
		}
	#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)//Teensy 3.5 -> 3.6
		if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14) && (_miso == 255 || _miso == 12 || _miso == 8)) {
			_spiBus = 0;
			if (!avoidSpiInit) SPI.begin();
			SPI.setMOSI(_mosi);
			if (_miso != 255) SPI.setMISO(_miso);
			SPI.setSCK(_sclk);
			if (SPI.pinIsChipSelect(_cs)){
				pinMode(_cs,OUTPUT);
				disableCS();
			} else {
				bitClear(_initError,1);
			}
		} else if ((_mosi == 0 || _mosi == 21 || _mosi == 59 || _mosi == 61) && (_sclk == 20 || _sclk == 32 || _sclk == 60) && (_miso == 255 || _miso == 1 || _miso == 5 || _miso == 59 || _miso == 61)){
			_spiBus = 1;
			if (_mosi == _miso){
				bitClear(_initError,4);
			}
			if (!avoidSpiInit) SPI1.begin();
			SPI1.setMOSI(_mosi);
			if (_miso != 255) SPI1.setMISO(_miso);
			SPI1.setSCK(_sclk);
			if (SPI1.pinIsChipSelect(_cs)) {
				pinMode(_cs,OUTPUT);
				disableCS();
			} else {
				bitClear(_initError,1);
			}
		} else if ((_mosi == 44 || _mosi == 52) && (_sclk == 46 || _sclk == 53) && (_miso == 255 || _miso == 45 || _miso == 51)){
			_spiBus = 2;
			if (!avoidSpiInit) SPI2.begin();
			SPI2.setMOSI(_mosi);
			if (_miso != 255) SPI2.setMISO(_miso);
			SPI2.setSCK(_sclk);
			if (SPI2.pinIsChipSelect(_cs)) {
				pinMode(_cs,OUTPUT);
				disableCS();
			} else {
				bitClear(_initError,1);
			}
		} else {
			bitClear(_initError,0);
		}
	#else
		pinMode(_cs,OUTPUT);
		if (!avoidSpiInit) SPI.begin();
		disableCS();
	#endif
		return _initError;
	}


	void gpio_MCP23SXX::startTransaction(void)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(_spiSettings);
			enableCS();
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			if (_spiBus == 0){
				SPI.beginTransaction(_spiSettings);
			} else if (_spiBus == 1){
				SPI1.beginTransaction(_spiSettings);
			} else {
				SPI2.beginTransaction(_spiSettings);
			}
			enableCS();
		#else
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(_spiSettings);
			#endif
			enableCS();
		#endif
	}

	void gpio_MCP23SXX::endTransaction(void)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.endTransaction();
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			if (_spiBus == 0){
				SPI.endTransaction();
			} else if (_spiBus == 1){
				SPI1.endTransaction();
			} else {
				SPI2.endTransaction();
			}
		#else
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		#endif
	}

	void gpio_MCP23SXX::writeByte_cont(byte val)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.transfer(val);
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			if (_spiBus == 0){
				SPI.transfer(val);
			} else if (_spiBus == 1){
				SPI1.transfer(val);
			} else {
				SPI2.transfer(val);
			}
		#else
			SPI.transfer(val);
		#endif
	}

	void gpio_MCP23SXX::writeByte_last(byte val)
	{
		writeByte_cont(val);
		disableCS();
	}

	void gpio_MCP23SXX::writeWord_cont(uint16_t val)
	{
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.transfer16(val);
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			if (_spiBus == 0){
				SPI.transfer16(val);
			} else if (_spiBus == 1){
				SPI1.transfer16(val);
			} else {
				SPI2.transfer16(val);
			}
		#else
			//SPI.transfer16(val);
			SPI.transfer(val >> 8);
			SPI.transfer(val & 0xFF);
		#endif
	}

	void gpio_MCP23SXX::writeWord_last(uint16_t val)
	{
		writeWord_cont(val);
		disableCS();
	}

	uint8_t gpio_MCP23SXX::readByte_cont(void)
	{
		uint8_t result = 0;
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			result = SPI.transfer(0x00);
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			if (_spiBus == 0){
				result = SPI.transfer(0x00);
			} else if (_spiBus == 1){
				result = SPI1.transfer(0x00);
			} else {
				result = SPI2.transfer(0x00);
			}
		#else
			result = SPI.transfer(0x00);
		#endif
		return result;
	}

	uint16_t gpio_MCP23SXX::readWord_cont(void)
	{
		
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			return SPI.transfer16(0x00);
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			uint16_t result = 0;
			if (_spiBus == 0){
				result = SPI.transfer16(0x00);
			} else if (_spiBus == 1){
				result = SPI1.transfer16(0x00);
			} else {
				result = SPI2.transfer16(0x00);
			}
			return result;
		#else
			union {
				uint16_t val= 0;
				struct {
                    uint8_t lsb;
                    uint8_t msb;
				};
			} out;
			out.msb = SPI.transfer(0x00);
			out.lsb = SPI.transfer(0x00);
			return out.val;
			//result = SPI.transfer16(0x00);
		#endif
	}

	int gpio_MCP23SXX::getInterruptNumber(uint8_t pin)
	{
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		int intNum = digitalPinToInterrupt(pin);
		if (intNum != NOT_AN_INTERRUPT) {
			#if defined(__MK20DX128__) || defined(__MK20DX256__)
				SPI.usingInterrupt(intNum);
			#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
				if (_spiBus == 0){
					SPI.usingInterrupt(intNum);
				} else if (_spiBus == 1){
					SPI1.usingInterrupt(intNum);
				} else {
					SPI2.usingInterrupt(intNum);
				}
			#endif
			return intNum;
		}
		return 255;
	#elif defined(ESP8266)
		if (pin == 16 || pin == D0) return 255;//TOFIX
		return pin;
	#else
		return 255;//bogus
	#endif
	}

	uint8_t gpio_MCP23SXX::getSPIbus(void)
	{
		#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
			return _spiBus;
		#else
			return 0;
		#endif
	}

	#if defined(SPI_HAS_TRANSACTION)
	void gpio_MCP23SXX::setSpiSettings(SPISettings settings)
	{
		_spiSettings = settings;
	}
	#endif

#endif

/*
void gpio_MCP23SXX::printPacket(word data, uint8_t count)
{
	for (int i = count - 1; i >= 0; i--) {
		if (bitRead(data, i) == 1) {
			Serial.print("1");
		}
		else {
			Serial.print("0");
		}
	}
	Serial.print(" -> 0x");
	if (count == 8) {
		Serial.print((byte)data, HEX);
	}
	else {
		Serial.print(data, HEX);
	}
	Serial.print("\n");
}
*/
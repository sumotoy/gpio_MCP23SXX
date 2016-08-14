/*
Common CPU helper for sumotoy libraries.
Instead include zillion of times this one serve many libraries and needs to be included just once.
Note that the SPI speed is the top limit commonly supported by CPU/Devices, there's defines to
handle PROGMEM and so on...
*/

#ifndef __CPU_COMMONSHELPER_H
	#define __CPU_COMMONSHELPER_H
	#include <stdint.h>
	//this are for the LCD Image Converter as workaround
	#define RLE_no  (0)
	#define RLE_yes (1)
	#define RLE_proportional (0)
	#define RLE_monospaced (1)
	//end
	#if defined(ESP8266)
		#define _smCharType	uint8_t//for LCD/TFT
		#if defined(SPI_HAS_TRANSACTION)
			static const uint32_t _SPI_topLimitSpeed 	= 80000000;
		#endif
		#define ESP8266_SPIFAST 1
		#if defined(ESP8266_SPIFAST)
			#include <eagle_soc.h>
		#endif
		#define _SPI_MULTITRANSFER	1//enable burst multy byte transfer
	#elif defined(__AVR__)
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#define _FORCE_PROGMEM__	1
		#define _smCharType	unsigned char //uint8_t
		#if defined(SPI_HAS_TRANSACTION)
			static const uint32_t _SPI_topLimitSpeed 	= 8000000;
		#endif
		#define _SPI_MULTITRANSFER	//enable burst multy byte transfer
	#elif defined(__SAM3X8E__)
		#include <include/pio.h>
		#if defined(SPI_HAS_TRANSACTION)
			static const uint32_t _SPI_topLimitSpeed 	= 24000000;
		#endif
		#define _SPI_MULTITRANSFER	1//enable burst multy byte transfer
	#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		#define _smCharType	unsigned char
		#define	_TEENSYFIFO_CAPABLE 1
			static const uint32_t _SPI_topLimitSpeed 	= 30000000;
	#elif defined(__MKL26Z64__)
		#define _smCharType	unsigned char
			static const uint32_t _SPI_topLimitSpeed 	= 24000000;
	#else//all the rest
		#define _smCharType	uint8_t
		#if defined(SPI_HAS_TRANSACTION)
			static const uint32_t _SPI_topLimitSpeed 	= 8000000;
		#endif
	#endif

	#if defined(_FORCE_PROGMEM__)
		template <typename T> T PROGMEM_read (const T * sce) { static T temp; memcpy_P (&temp, sce, sizeof (T)); return temp; }
	#endif
#endif

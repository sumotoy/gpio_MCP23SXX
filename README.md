# gpio_MCP23SXX<br>
<b>Do NOT use yet! It will be ready in a couple of days...</b><br><br>
Fast, full featured library for Microchip MC23Sxx series for many CPU's, easy to use.<br>
I've started a wiki here: https://github.com/sumotoy/gpio_MCP23SXX/wiki <br>
You cannot see any code here? Do not worry, I'm usually release code only when is ready, if you see this page it means I'm working on it at the time you see this, it will be released soon.<br>

This library works with the following Microchip MCP23Sxx series:
 - <b>MCP23S08</b>: 8 Port/HAEN multi address/INT
 - <b>MCP23S09</b>: 8 Port/Fixed address/INT/Open drain out
 - <b>MCP23S17</b>: 16 Port/HAEN multi address/INT
 - <b>MCP23S18</b>: 16 Port/Fixed address/INT/Open drain out
 
 ***

 <b>Features</b>:<br>
 - All chip features fully supported.
 - Full SPI transaction compatible (where applicable).
 - Can use all CPU/related SPI bus and pins.
 - Much faster than any other library, uses custom SPI method for every MCU (see dependancies).
 - Commands are very simple, all gpio chip uses the same registers (automatically handled internally).
 - Library automatically support HAEN when applicable.
 - Possible include library in other libraries.
 - Optional SPI legacy mode (remain compatible with SPI transactions).

 ***
 
<b>Speed Improvements</b>:<br>
If the CPU supports SPI transactions there's many speed improvements, for example the limit of 10Mhz has been moved to 24Mhz without issues, Microchip is very conservative about SPI speed but I have find a way to avoid this limitation.

 ***
 
<b>Dependancies:</b>:<br>
Apart SPI, this library uses some other libraries for improve performances and save code. You need to download the dependancies you need for your CPU:<br>
  - AVR: https://github.com/sumotoy/SPI_AVR
  - ESP8266: https://github.com/sumotoy/SPI_ESP
  - Teensy 3.0->3.6: https://github.com/sumotoy/SPI_FIFO_t3
  - Teensy LC: https://github.com/sumotoy/SPI_LC
  - DUE: https://github.com/sumotoy/SPI_DUE

 ***

<b>Compatibility:</b>:<br>
I will ensure the max compatibility with most used CPU but this beta support:<br>
  - Arduino UNO and any AVR 8 bit CPU (done)
  - ESP8266 (done)
  - Teensy 3.0,  3.1 , 3.2 , 3.5 , 3.6 (done)
  - Teensy LC (working on)
  - DUE (working on)
  - Many other in preparation...

 ***

<b>MCP23S08</b><br>
- Ports: 8
- Max SPI Speed: 10Mhz (min 2v7)
- Address: 0x20...0x23
- Pin Current: 25 mA per pin
- 5V Tolerant: yes
- Supply Range: 1V8...5V5
- Max Consume: 200 mA

```
    MCP23S08 DIP Footprint
			    __ __
		SCK   [|  U  |] +++
		MOSI  [|     |] I/O 7
		MISO  [|     |] I/O 6
    adrs (A1) [|     |] I/O 5
    adrs (A0) [|     |] I/O 4
		RST   [|     |] I/O 3
		CS    [|     |] I/O 2
		INT   [|     |] I/O 1
		GND   [|_____|] I/O 0
```
<b>MCP23S09</b><br>
- Ports: 8
- Max SPI Speed: 10Mhz (min 2v7)
- Address: 0x20
- Pin Current: 25 mA per pin
- 5V Tolerant: yes
- Supply Range: 1V8...5V5
- Max Consume: 200 mA

```
    MCP23S09 DIP Footprint
			    __ __
		+++   [|  U  |] GND
		nc    [|     |] nc
		CS    [|     |] I/O 7
        SCK   [|     |] I/O 6
        MOSI  [|     |] I/O 5
		MISO  [|     |] I/O 4
		RST   [|     |] I/O 3
		INT   [|     |] I/O 2
		I/O 0 [|_____|] I/O 1
```
<b>MCP23S17</b><br>
- Ports: 16
- Max SPI Speed: 10Mhz (min 2v7)
- Address: 0x20...0x27
- Pin Current: 25 mA per pin
- 5V Tolerant: yes
- Supply Range: 1V8...5V5
- Max Consume: 400 mA

```
    MCP23S17 DIP Footprint
			    __ __
		IOB-0 [|  U  |] IOA-7
		IOB-1 [|     |] IOA-6
		IOB-2 [|     |] IOA-5
		IOB-3 [|     |] IOA-4
		IOB-4 [|     |] IOA-3
		IOB-5 [|     |] IOA-2
		IOB-6 [|     |] IOA-1
		IOB-7 [|     |] IOA-0
		++++  [|     |] INT-A
		GND   [|     |] INT-B
		CS    [|     |] RST (connect to +)
		SCK   [|     |] A2  (address 2)
		MOSI  [|     |] A1  (address 1)
		MISO  [|_____|] A0  (address 0)
```

<b>MCP23S18</b><br>
- Ports: 16
- Max SPI Speed: 10Mhz (min 2v7)
- Address: 0x20
- Pin Current: 25 mA per pin
- 5V Tolerant: yes
- Supply Range: 1V8...5V5
- Max Consume: 400 mA

```
    MCP23S18 DIP Footprint
			    __ __
		GND   [|  U  |] nc
		nc    [|     |] IOA-7
		IOB-0 [|     |] IOA-6
		IOB-1 [|     |] IOA-5
		IOB-2 [|     |] IOA-4
		IOB-3 [|     |] IOA-3
		IOB-4 [|     |] IOA-2
		IOB-5 [|     |] IOA-1
		IOB-6 [|     |] IOA-0
		IOB-7 [|     |] INT-A
		GND   [|     |] INT-B
		CS    [|     |] nc
		SCK   [|     |] RST
		MOSI  [|_____|] MISO
```
***

<b>Wiring:</b><br>
All chip can be supplied from 1V8 to 5V5 but I suggest a minimum of 3V3 volt to ensure a resonable SPI speed.<br>
If not used the RST pin should be tied to VCC!<br>
MCP23S08 and MCP23S17 use HAEN so many chip can work in the same SPI bus and sharing the same CS! To do that you should assign an hardware address using A0 & A1 pin (MCP23S08) or A0,A1,A2 pin (MCP23S17). Beware that you cannot share chip with HAEN assigned hardware (like MCP23S17) with a non haen one (like MCP23S18) even if the first one use a different address!!!<br>
I always suggest a pullUp resistor on CS line to disable chip until is inited. If using HAEN just one resistor is needed.<br>
All chip have internal pullup that can be used for detect external switches and so on but if you have long lines you may disable internal pullup and use external pullup resistors with a value between 10K to 4k7 (lower value is good in noisy environment or long lines)<br>

<b>FAQ</b><br>
- [Q] Can I have different instances with different (or same) chips?
- [R] ... Yes, but consider use HAEN and even share the same CS!
- [Q] Can I share chips with HAEN and not?
- [R] ... Yes, if you use different CS for every chip, otherwise NO.
- [Q] Can I share MCP23S08 and MCP23S17 and using HAEN?
- [R] ... Yes. Remember to assign different hardware address!
- [Q] Can I rise up the SPI freq speed to 30 or more Mhz?
- [R] ... No. Datasheet limits to 10Mhz but thanks some code tricks I can rise up to 24Mhz!

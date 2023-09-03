//
//  OSCBoards.h
//  
//
//  Created by AdrianFreed on 5/26/13.
//
//

#ifndef _OSCBoards_h
#define _OSCBoards_h



#if defined(__MK20DX128__) ||  defined(__MK20DX256__)  || defined(__MKL26Z64__) || defined(__MK66FX1M0__)
// Teensy 3.0  3.1  3.1LC 3.2 3.6
#define BOARD_HAS_CAPACITANCE_SENSING
#endif

#if defined(__AVR_ATmega32U4__) || defined(__MKL26Z64__) || defined(__MK20DX128__)||  defined(__MK20DX256__)   || defined(__MK66FX1M0__) || defined(__AVR_ATmega328_) ||   defined(__AVR_ATmega128__)

#define BOARD_HAS_DIE_TEMPERATURE_SENSOR
#endif

#if defined(__AVR_ATmega32U4__) ||  defined(__MK20DX128__) ||  defined(__MK20DX256__)  || defined(__MK66FX1M0__) || defined(__MK66FX1M0__) ||  defined(__MKL26Z64__) || defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)    || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega328_) ||   defined(__AVR_ATmega128__)


#define BOARD_HAS_DIE_POWER_SUPPLY_MEASUREMENT
#endif


#if defined(__AVR_ATmega32U4__)  || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)    || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega328_) ||   defined(__AVR_ATmega128__)
#define BOARD_HAS_ANALOG_PULLUP
#endif

// missing specs for PIC32

#if (defined(__PIC32MX__) || defined(__PIC32MZ__))
#define NUM_ANALOG_INPUTS NUM_ANALOG_PINS
#define NUM_DIGITAL_INPUTS NUM_DIGITAL_PINS
#define LED_BUILTIN PIN_LED1

#endif

#if defined(_SAMD21_)
#define BOARD_HAS_USB_SERIAL
// Required for Serial on Zero based boards
#if defined(ARDUINO_SAMD_ZERO)
// Adafruit breaks with tradition here
#define thisBoardsSerialUSB Serial
typedef decltype(Serial) actualUSBtype;

#else
#define thisBoardsSerialUSB SerialUSB
typedef decltype(SerialUSB) actualUSBtype;

#endif
#elif defined(__SAM3X8E__)

#define BOARD_HAS_USB_SERIAL
// Required for Serial on Zero based boards
#define thisBoardsSerialUSB SerialUSB
typedef decltype(SerialUSB) actualUSBtype;

// defined(__SAM3X8E__)
#elif  defined(ARDUINO_USB_CDC_ON_BOOT) || defined(CORE_TEENSY)  || defined(__AVR_ATmega32U4__) || (defined(__PIC32MX__) || defined(__PIC32MZ__))
#define BOARD_HAS_USB_SERIAL
#define thisBoardsSerialUSB Serial
typedef decltype(Serial) actualUSBtype;
#elif  defined(ARDUINO_ARCH_RP2040)
#define BOARD_HAS_USB_SERIAL
#define thisBoardsSerialUSB Serial
typedef decltype(Serial) actualUSBtype;

#endif

#ifndef analogInputToDigitalPin
int analogInputToDigitalPin(int i);
#endif

#ifdef BOARD_HAS_DIE_TEMPERATURE_SENSOR
float getTemperature();
#endif

#ifdef BOARD_HAS_DIE_POWER_SUPPLY_MEASUREMENT
float getSupplyVoltage();
#endif

#endif

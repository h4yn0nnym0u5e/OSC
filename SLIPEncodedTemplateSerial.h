/*
Extends the Serial class to encode SLIP over serial
*/

#ifndef SLIPEncodedTemplateSerial_h
#define SLIPEncodedTemplateSerial_h
#include "Arduino.h"


#include <Stream.h>


#if (defined(TEENSYDUINO) && (defined(USB_SERIAL) || defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL) || defined(USB_SERIAL_HID) || defined(USB_MIDI_SERIAL) || defined(USB_MIDI_AUDIO_DUAL_SERIAL) || defined(USB_MIDI4_SERIAL) || defined(USB_MIDI16_SERIAL) || defined(USB_MIDI_AUDIO_SERIAL) || defined(USB_MIDI16_AUDIO_SERIAL))) || (!defined(TEENSYDUINO) && defined(__AVR_ATmega32U4__)) || defined(__SAM3X8E__) || (defined(_USB) && defined(_USE_USB_FOR_SERIAL_))  || defined(_SAMD21_) || (defined(__PIC32MX__) || defined(__PIC32MZ__))


//import the serial USB object
#if defined(TEENSYDUINO) && defined (__arm__)
#if !defined(USB_HOST_TEENSY36_)
#include <usb_serial.h>
#endif
#elif defined(TEENSYDUINO) && defined (__AVR__)
#include <usb_api.h>
#elif defined(__SAM3X8E__)  || defined(_SAMD21_) 
#include <USB/USBAPI.h>
#elif (defined(__PIC32MX__) || defined(__PIC32MZ__))
#include <USB.h>
#elif defined(__AVR_ATmega32U4__)
#include "USBAPI.h"
#include <avr/wdt.h>    
#else
#error Unknown USB port
#endif


template <class serPortClass>
class SLIPEncodedUSBSerial: public Stream
{

  private:
  	enum erstate {CHAR, FIRSTEOT, SECONDEOT, SLIPESC } rstate;
  //different type for each platform
  
  	serPortClass* serial;
  	
  public:
  	SLIPEncodedUSBSerial(serPortClass&);
  	
  	int available();
  	int read();
      int readBytes( uint8_t *buffer, size_t size);
  
  	int peek();
  	void flush();
  	
  	//same as Serial.begin
  	void begin(unsigned long);
      //SLIP specific method which begins a transmitted packet
  	void beginPacket();
  	//SLIP specific method which ends a transmittedpacket
  	void endPacket();
  	// SLIP specific method which indicates that an EOT was received 
  	bool endofPacket();
  	 
  	//overrides the Stream's write function to encode SLIP
  	size_t write(uint8_t b);
      size_t write(const uint8_t *buffer, size_t size);
  	//using Print::write;	


};
#endif



#endif

// .cpp

//#include "SLIPEncodedUSBSerial.h"

/*
 CONSTRUCTOR
 */
//instantiate with the transmission layer

#if (defined(CORE_TEENSY) && (defined(USB_SERIAL) || defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL) || defined(USB_SERIAL_HID) || defined(USB_MIDI_SERIAL) || defined(USB_MIDI_AUDIO_DUAL_SERIAL) || defined(USB_MIDI4_SERIAL) || defined(USB_MIDI16_SERIAL) || defined(USB_MIDI_AUDIO_SERIAL) || defined(USB_MIDI16_AUDIO_SERIAL) )) || (!defined(CORE_TEENSY) && defined(__AVR_ATmega32U4__)) || defined(__SAM3X8E__) || (defined(_USB) && defined(_USE_USB_FOR_SERIAL_)) || defined(BOARD_maple_mini) || defined(_SAMD21_)  || defined(__ARM__) || (defined(__PIC32MX__) || defined(__PIC32MZ__))


//USB Serials
template <class serPortClass>
SLIPEncodedUSBSerial<serPortClass>::SLIPEncodedUSBSerial(serPortClass& s){
  serial = &s;
  rstate = CHAR;
}

static const uint8_t eot = 0300;
static const uint8_t slipesc = 0333;
static const uint8_t slipescend = 0334;
static const uint8_t slipescesc = 0335;
/*
 SERIAL METHODS
 */

template <class serPortClass>
bool SLIPEncodedUSBSerial<serPortClass>::endofPacket()
{
  if(rstate == SECONDEOT)
  {
    rstate = CHAR; 
    return true;
  }
  if (rstate==FIRSTEOT)
  {
        if(serial->available())
        {
            uint8_t c =serial->peek();
            if(c==eot)
            {
                serial->read(); // throw it on the floor
            }
        }
    rstate = CHAR;
    return true;
  }
  return false;
}


template <class serPortClass>
int SLIPEncodedUSBSerial<serPortClass>::available(){
back:
  int cnt = serial->available();
  
  if(cnt==0)
    return 0;
  if(rstate==CHAR)
  {
    uint8_t c =serial->peek();
    if(c==slipesc)
    {
      rstate = SLIPESC;
      serial->read(); // throw it on the floor
      goto back;
    }
    else if( c==eot)
    {
      rstate = FIRSTEOT;
      serial->read(); // throw it on the floor
      goto back;
    }
    return 1; // we may have more but this is the only sure bet
  }
  else if(rstate==SLIPESC)  
    return 1;
  else if(rstate==FIRSTEOT)
  {
    if(serial->peek()==eot)
    {
      rstate = SECONDEOT;
      serial->read(); // throw it on the floor
      return 0;
    }   
    rstate = CHAR;
  }else if (rstate==SECONDEOT) {
    rstate = CHAR;
  }
  
  return 0;
    
}

//reads a byte from the buffer
template <class serPortClass>
int SLIPEncodedUSBSerial<serPortClass>::read(){
back:
  uint8_t c = serial->read();
  if(rstate==CHAR)
  {
    if(c==slipesc)
    {
      rstate=SLIPESC;
      goto back;
    } 
    else if(c==eot){
    
      return -1; // xxx this is an error
    }

    return c;
  }
  else
  if(rstate==SLIPESC)
  {
    rstate=CHAR;
    if(c==slipescend)
      return eot;
    else if(c==slipescesc)
      return slipesc;
      else {
        // insert some error code here
        return -1;
      }

  }
  else
    return -1;
}


#ifdef FUTUREDEVELOPMENT
template <class serPortClass>
int SLIPEncodedUSBSerial<serPortClass>::readBytes( uint8_t *buffer, size_t size)
{
    int count = 0;
    while(!endofPacket() && available() && (size>0))
    {
        int c = read();
        if(c>=0)
        {
            *buffer++ = c;
            ++count;
            --size;
            
        }
        else
            break;
    }
    return count;
}
#endif


// as close as we can get to correct behavior
template <class serPortClass>
int SLIPEncodedUSBSerial<serPortClass>::peek(){
  uint8_t c = serial->peek();
  if(rstate==SLIPESC)
  {
    if(c==slipescend)
      return eot;
    else if(c==slipescesc)
      return slipesc;
  }
  return c; 
}


//encode SLIP
template <class serPortClass>
size_t SLIPEncodedUSBSerial<serPortClass>::write(uint8_t b){
  if(b == eot){ 
    serial->write(slipesc);
    return serial->write(slipescend); 
  } else if(b==slipesc) {  
    serial->write(slipesc);
    return serial->write(slipescesc); 
  } else {
    return serial->write(b);
  } 
}


template <class serPortClass>
size_t SLIPEncodedUSBSerial<serPortClass>::write(const uint8_t *buffer, size_t size)
{
    size_t result=0;
    while(size--)
        result = write(*buffer++); return result;
}


template <class serPortClass>
void SLIPEncodedUSBSerial<serPortClass>::begin(unsigned long baudrate){
  serial->begin(baudrate);
        //
        // needed on Leonardo?
        // while(!serial)
        //        ;
}


//SLIP specific method which begins a transmitted packet
template <class serPortClass>
void SLIPEncodedUSBSerial<serPortClass>::beginPacket() {  serial->write(eot); }


//signify the end of the packet with an EOT
template <class serPortClass>
void SLIPEncodedUSBSerial<serPortClass>::endPacket(){
  serial->write(eot);
#if defined(CORE_TEENSY)
    serial->send_now();
#endif
}


template <class serPortClass>
void SLIPEncodedUSBSerial<serPortClass>::flush(){
  serial->flush();
}
#endif // SLIPEncodedTemplateSerial_h

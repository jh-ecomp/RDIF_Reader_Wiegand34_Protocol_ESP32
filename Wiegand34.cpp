#include "Wiegand34.h"

#if defined(ESP8266)
    #define INTERRUPT_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
	#define INTERRUPT_ATTR IRAM_ATTR
#else
    #define INTERRUPT_ATTR
#endif


uint16_t			Wiegand34::_msgCode = 0;
volatile int 			Wiegand34::_bitCount = 0;
volatile int 			Wiegand34::_numBits = 0;
volatile byte 		Wiegand34::_card[5];
volatile unsigned long 	Wiegand34::_ticks = 0;
volatile int       Wiegand34::_pinD0 = 0;
volatile int       Wiegand34::_pinD1 = 0;

Wiegand34::Wiegand34()
{
}


uint16_t Wiegand34::getMsgCode(){
	return _msgCode;
}

bool Wiegand34::available(){
	bool ret = false;
	noInterrupts();
  if((millis() - _ticks > 25) and (_bitCount > 0)){
    _numBits = _bitCount;
    _bitCount = 0;
    _msgCode++;
    ret = true;
  }
	interrupts();
	return ret;
}

int Wiegand34::bitCount(){
	return _numBits;
}

void Wiegand34::begin(int pinD0, int pinD1){
  _pinD0 = pinD0;
  _pinD1 = pinD1;
  
	_msgCode = 0;
	_bitCount = 0;  
	pinMode(_pinD0, INPUT_PULLUP);					// Set D0 pin as input_pullup
	pinMode(_pinD1, INPUT_PULLUP);					// Set D1 pin as input_pullup
	
	attachInterrupt(digitalPinToInterrupt(_pinD0), ReadD0, FALLING);  // Hardware interrupt - high to low pulse
	attachInterrupt(digitalPinToInterrupt(_pinD1), ReadD1, FALLING);  // Hardware interrupt - high to low pulse
}

INTERRUPT_ATTR void Wiegand34::ReadD0 (){
	
	if(digitalRead(_pinD0) == LOW){
		bitWrite(_card[(33 - _bitCount) / 8], (33 - _bitCount) % 8, 0);
    _bitCount++; // Increament bit count for Interrupt connected to D0
	}
	_ticks = millis();
}

INTERRUPT_ATTR void Wiegand34::ReadD1(){
	if(digitalRead(_pinD1) == LOW){
		bitWrite(_card[(33 - _bitCount) / 8], (33 - _bitCount) % 8, 1);
    _bitCount++; // Increament bit count for Interrupt connected to D1
	}
	_ticks = millis();
}

uint32_t Wiegand34::getCardId(){

  if(_numBits != 34){
    return -1;
  }
  
	uint32_t cardId;

	for(int i = 0; i < 32; i++){
		bitWrite(cardId, i, bitRead(_card[(i + 1) / 8], (i + 1) % 8));
	}

	return cardId;								// without parity bits
}

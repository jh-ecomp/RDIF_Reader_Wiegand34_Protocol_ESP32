#ifndef _WIEGAND34_H
#define _WIEGAND34_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Wiegand34 {

public:
	Wiegand34();
	void begin(int pinD0, int pinD1);
	bool available();
	uint16_t getMsgCode();
  uint32_t getCardId();
	
private:
	static void ReadD0();
	static void ReadD1();
  static int bitCount();

	static volatile unsigned long	_ticks;
	static volatile int				    _bitCount;
	static volatile int				    _numBits;
	static uint16_t     			    _msgCode;
	static volatile byte 			    _card[5];
  static volatile int           _pinD0;
  static volatile int           _pinD1;
};

#endif

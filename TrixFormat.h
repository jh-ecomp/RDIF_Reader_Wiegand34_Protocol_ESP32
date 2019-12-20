#ifndef _TRIXFORMAT_H
#define _TRIXFORMAT_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

const uint32_t deviceId = 0xF7;

byte checkSum(uint32_t value);
String message(uint16_t msgCode, uint32_t cardId);
String trixMsg(String msg);
#endif

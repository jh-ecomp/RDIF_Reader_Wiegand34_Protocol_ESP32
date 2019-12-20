#include "TrixFormat.h"

byte checkSum(uint32_t value){
  byte checksum = 0;
  for(int i = 0; i < 32; i++){
    checksum += bitRead(value, i);
  }
  return checksum;
}

String message(uint16_t msgCode, uint32_t cardId){
  String msg = "";
  byte msgCheckSum = checkSum(deviceId) + checkSum(msgCode) + checkSum(cardId);
  
  msg += String(deviceId >> 4, 16);   // HEXA of bits 7-4
  msg += String(deviceId & 0xF, 16);  // HEXA of bits 3-0

  msg += String(msgCode >> 12, 16);         // HEXA of bits 15-12
  msg += String((msgCode >> 8) & 0xF , 16); // HEXA of bits 11-8
  msg += String((msgCode >> 4) & 0xF , 16); // HEXA of bits 7-4
  msg += String(msgCode & 0xF, 16);         // HEXA of bits 3-0

  msg += String(cardId >> 28, 16);
  msg += String((cardId >> 24) & 0xF , 16);
  msg += String((cardId >> 20) & 0xF , 16);
  msg += String((cardId >> 16) & 0xF , 16);
  msg += String((cardId >> 12) & 0xF , 16);
  msg += String((cardId >> 8) & 0xF , 16);
  msg += String((cardId >> 4) & 0xF , 16);
  msg += String(cardId & 0xF, 16);

  msg += String(msgCheckSum >> 4, 16);
  msg += String(msgCheckSum & 0xF, 16);

  return msg;
  
}

String trixMsg(String msg){
  String _trixMsg = "01";
  String subMsg = "";

  // Byte by byte is verified and changed if its special
  for(int i = 0; i < msg.length() - 1; i++){
    subMsg = msg.substring(i, i + 2);
    if(subMsg.equals("01")){
        _trixMsg += "1021";
    }else{
      if(subMsg.equals("04")){
        _trixMsg += "1024";
      }else{
        if(subMsg.equals("10")){
          _trixMsg += "1030";
        }else{
          _trixMsg += subMsg; 
         }
      }
    }
    i++;
  }
  
  _trixMsg += "04";
  return _trixMsg;
  
}

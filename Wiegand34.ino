#include "Wiegand34.h"
#include "TrixFormat.h"

#define PIN_D0 12 // Plug PIND0 into GPIO defined
#define PIN_D1 13 // Plug PIND1 into GPIO defined

Wiegand34 wg; // Our Wiegand34 object

void setup() {
  Serial.begin(115200);
  wg.begin(PIN_D0, PIN_D1); // The initialization needs PIND0 and PIND1
}

void loop() {
  if(wg.available()){
    String msg = message(wg.getMsgCode(), wg.getCardId()); // Getting message with device id + message code + card id
    Serial.print("Card code: ");
    Serial.println(wg.getCardId(), HEX);
    Serial.print("Message: ");
    Serial.println(msg);
    Serial.print("TrixMessage: ");
    Serial.println(trixMsg(msg)); // TrixFormat message
  }
  delay(2000);
}

#define PIN_D0 12
#define PIN_D1 13
#define PARITY 1
#define NON_PARITY 0

volatile uint8_t bits = 0;
volatile byte card[5];
const byte id = 127;
uint16_t cont = 0;
byte checksum = 0;
String msg = "";
String f_data = "";



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_D0, INPUT_PULLUP);
  pinMode(PIN_D1, INPUT_PULLUP);
  digitalWrite(PIN_D0, HIGH);
  digitalWrite(PIN_D1, HIGH);

  attachInterrupt(digitalPinToInterrupt(PIN_D0), pinD0Changed, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_D1), pinD1Changed, FALLING);

  

}

void loop() {

  if(bits >= 34){
    msg += String(id >> 4, 16);
    msg += String(id & 0xF, 16);
    cont++;
    msg += String(cont >> 12, 16);
    msg += String((cont >> 8) & 0xF , 16);
    msg += String((cont >> 4) & 0xF , 16);
    msg += String(cont & 0xF, 16);

    
    //Serial.print(String(card[0] >> 4, 16));
    //*
    if(NON_PARITY){
      msg += String(card[4] & 0x3, 16);
    }
    for(int i = 3; i >= 0; i--){
      msg += String(card[i] >> 4, 16);
      msg += String(card[i] & 0xF, 16);
    }
    //*/
    /*
     *  LÊ A SOMA DOS BITS DE ID
     */
    for(int i = 0; i < 8; i++){
      checksum += bitRead(id, i);
    }

    /*
     *  LÊ A SOMA DOS BITS DE CONT
     */
    for(int i = 0; i < 16; i++){
      checksum += bitRead(cont, i);
    }

    /*
     *  LÊ A SOMA DOS BITS DE CARD
     */
    for(int i = 0; i < 34; i++){
      checksum += bitRead(card[i / 8], i % 8);
    }
    msg += String(checksum, 16);
    int i;
    for(i = 0; i < msg.length(); i++){
      
      String submsg = msg.substring(i, i < msg.length() - 1 ? i + 2: i + 1);
      if(submsg.equals("01")){
        f_data += "1021";
        i++;
      }else{
        if(submsg.equals("04")){
        f_data += "1024";
        i++;
        }else{
          if(submsg.equals("10")){
            f_data += "1030";
            i++;
          }else{
            f_data += String(msg.charAt(i)); 
          }
        }
      }
    }

    
    //Serial.println(String(id));
    //Serial.println(String(cont));
    Serial.println(msg);
    Serial.println("01" + f_data + "04");
    //Serial.println(String(checksum));
    
    msg = "";
    f_data = "";
    bits = 0;
    checksum = 0;
    card[0] = 0;
    card[1] = 0;
    card[2] = 0;
    card[3] = 0;
    card[4] = 0;
  }
  delay(2000);
}


// ****************** LEITURA CONSIDERANDO QUE O PROTOCOLO ENVIA LSB -> MSB
//    SEM OS BITS DE PARIDADE
/*
void pinD0Changed(){
  if(digitalRead(PIN_D0) == LOW){
    if((bits > 0) and (bits < 33)){
      bitWrite(card[bits / 8], bits % 8, 0);
    }
    bits++;
  }
}

void pinD1Changed(){
  if(digitalRead(PIN_D1) == LOW){
    if((bits > 0) and (bits < 33)){
      bitWrite(card[(bits - 1) / 8], (bits - 1) % 8, 1);
    }
    bits++;
  }
}//*/

//    COM OS BITS DE PARIDADE
/*
void pinD0Changed(){
  if(digitalRead(PIN_D0) == LOW){
    bitWrite(card[bits / 8], bits % 8, 0);
    bits++;
  }
}

void pinD1Changed(){
  if(digitalRead(PIN_D1) == LOW){
    bitWrite(card[bits / 8], bits % 8, 1);
    bits++;
  }
}//*/



// ****************** LEITURA CONSIDERANDO QUE O PROTOCOLO ENVIA MSB -> LSB
//    SEM OS BITS DE PARIDADE
//*
void pinD0Changed(){
  if(digitalRead(PIN_D0) == LOW){
    if((bits > 0) and (bits < 33)){
      bitWrite(card[(32 - bits) / 8], (32 - bits) % 8, 0);
    }
    bits++;
  }
}

void pinD1Changed(){
  if(digitalRead(PIN_D1) == LOW){
    if((bits > 0) and (bits < 33)){
      bitWrite(card[(32 - bits) / 8], (32 - bits) % 8, 1);
    }
    bits++;
  }
}//*/

//    COM OS BITS DE PARIDADE
/*
void pinD0Changed(){
  if(digitalRead(PIN_D0) == LOW){
    bitWrite(card[(33 - bits) / 8], (33 - bits) % 8, 0);
    bits++;
  }
}

void pinD1Changed(){
  if(digitalRead(PIN_D1) == LOW){
    bitWrite(card[(33 - bits) / 8], (33 - bits) % 8, 1);
    bits++;
  }
}//*/

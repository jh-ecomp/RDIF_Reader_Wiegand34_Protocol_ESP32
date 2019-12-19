#define PIN_D0 12     // Conectar o fio verde no GPIO 12
#define PIN_D1 13     // Conectar o fio branco no GPIO 13
#define PARITY 1      // Setar PARITY para incluir os bits de paridade na mensagem
#define NON_PARITY 0  // Setar NON_PARITY para excluir os bits de paridade da mensagem

volatile uint8_t bits = 0;  // Variável que faz a contagem dos bits lidos do cartão
volatile byte card[5];      // Variável que guarda os bits lidos, máximo de 40 bits
const byte id = 127;        // ID do dispositivo, como eu não sabia de onde tirar essa informação coloquei como uma constante
uint16_t cont = 0;          // Variável que conta a quantidade de mensagens recebidas de forma sequêncial, a forma mais correta seria guardar na EEPROM para persistência
byte checksum = 0;          // Variável que soma os bits de ID +  CONT + CARD
String msg = "";            // Variável para manipulação da mensagem a ser enviada pela serial
String f_data = "";         // Variável para enviar a mensagem pela serial



void setup() {
  // put your setup code here, to run once:
  /*
   * Aqui iniciamos a serial com BAUD RATE de 115200, padrão ESP32, e colocados os pinos em INPUT_PULLUP
   * Os pinos D0 e D1 ficam com estado lógico alto na maior parte do tempo, quando um cartão é lido o pino é colocado em estado lógico baixo
   * O pino D0 é colocado em LOW para receber um bit 0, o pino D1 é colocado em LOW para receber um bit 1
   */
  Serial.begin(115200);
  pinMode(PIN_D0, INPUT_PULLUP);
  pinMode(PIN_D1, INPUT_PULLUP);
  /*
   * Atrelamos os pinos à interrupções em decida de borda, por algum motivo a interrupção não funciona por detecção de estado LOW
   */
  attachInterrupt(digitalPinToInterrupt(PIN_D0), pinD0Changed, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_D1), pinD1Changed, FALLING);
}

void loop() {
  /*
   * Toda vez que um cartão for lido, terão sido transferidos 34bits, em caso de interferências ou ruídos a váriavel é zerada
   */
  if(bits >= 34){
    /*
     * Aqui fazemos a conversão de ID e CONT para hexadeciamais, esses dados serão impressos na mensagem
     * Para cada byte é preciso fazer a separação em conjuntos de 4 bits, que representam 1 hexadecimal
     * msg tem somente 1 byte, então deslocamos 4 bits para a direta e temos o hexa dos 4 bits mais significativos
     * depois fazemos uma and com os 4 bits menos significativos e temos o hexa correspondende,
     * Vale lembrar que a operação de deslocamento não é destrutiva
     * Convertemos cada hexa em String e adicionamos à msg
     * O memo é válido para cont
     */
    msg += String(id >> 4, 16);
    msg += String(id & 0xF, 16);
    cont++;
    msg += String(cont >> 12, 16);
    msg += String((cont >> 8) & 0xF , 16);
    msg += String((cont >> 4) & 0xF , 16);
    msg += String(cont & 0xF, 16);

    /*
     * ******************************       MUITO IMPORTANTE
     * 
     * Caso queira pegar os bits de paridade mudar para PARITY
     * Adicionamos o valor lido em CARD à MSG, byte a byte, extraindo os hexadecimais assim como em ID
     */
    if(NON_PARITY){
      msg += String(card[4] & 0x3, 16);
    }
    for(int i = 3; i >= 0; i--){
      msg += String(card[i] >> 4, 16);
      msg += String(card[i] & 0xF, 16);
    }
    
    /*
     *  Lê a soma dos bits ID
     */
    for(int i = 0; i < 8; i++){
      checksum += bitRead(id, i);
    }

    /*
     *  Lê a soma dos bits CONT
     */
    for(int i = 0; i < 16; i++){
      checksum += bitRead(cont, i);
    }

    /*
     *  Lê a soma dos bits CARD
     */
    for(int i = 0; i < 34; i++){
      checksum += bitRead(card[i / 8], i % 8);
    }

    
    /*
     * Adicionamos CHECKSUM à MSG, extraindo os hexadecimais assim como em ID
     */
    msg += String(checksum >> 4, 16);
    msg += String(checksum & 0xF, 16);

    /*
     * Uma vez que temos os valores de ID, CONT, CARD e CHECKSUM em MSG, o desafio pede que caso sejam encontados bytes[01, 04, 10],
     * estes sejam substituídos por 10(20 + byte), ex: byte = 01, substituir por 1021
     * Usamos uma janela de dois caracteres e caso um dos bytes especiais apareçam fazemos a substituição, caso contrário e feita a atribuição
     * do caractere observado e pulamos para a próxima janela
     */
    int msg_char; // Índice do caractere que está sendo observado
    /*
    
    for(msg_char = 0; msg_char < msg.length(); msg_char++){
      
      String submsg = msg.substring(msg_char, msg_char < msg.length() - 1 ? msg_char + 2: msg_char + 1);  // pegamos a substring que vai ser analisada

      if(submsg.equals("01")){
        f_data += "1021";
        msg_char++; // Precisamos pular mais uma casa caso um byte especial seja encontrado
      }else{
        if(submsg.equals("04")){
        f_data += "1024";
        msg_char++;
        }else{
          if(submsg.equals("10")){
            f_data += "1030";
            msg_char++;
          }else{
            f_data += String(msg.charAt(msg_char)); 
          }
        }
      }
    }//*/

    /*
     * Manipulação alternativa dos bytes especiais
     * Usamos uma janela de dois caracteres e caso um dos bytes especiais apareçam fazemos a substituição, caso contrário e feita a atribuição 
     * e pulamos para a próxima janela
     */
     //*
     for(msg_char = 0; msg_char < msg.length() - 1; msg_char+2){
      
      String submsg = msg.substring(msg_char, msg_char + 2);  // pegamos a substring que vai ser analisada
      if(submsg.equals("01")){
        f_data += "1021";
      }else{
        if(submsg.equals("04")){
        f_data += "1024";
        }else{
          if(submsg.equals("10")){
            f_data += "1030";
          }else{
            f_data += submsg; 
          }
        }
      }
    }//*/

    Serial.println(msg);                  // Impressão de MSG no formato IDCONTCARDCHECKSUM
    Serial.println("01" + f_data + "04"); // Impressão da MSG já manipulada no formato 01IDCONTCARDCHECKSUM04
    
   /*
    * Aqui zeramos as variáveis
    */
    msg = "";
    f_data = "";
    bits = 0;
    checksum = 0;
    card[0] = 0;
    card[1] = 0;
    card[2] = 0;
    card[3] = 0;
    card[4] = 0;
  }else{
    if(bits > 0){
      bits = 0;
    }
  }
  /*
   * Delay de 2 segundos, pode ser substituido por sleep mode se a leitura do cartão for configurada para despertar a ESP32
   */
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

# Wiegand34 Protocol ESP32

Código base para leitura de Tag RFID 125kHz pelo protocolo Wiegand 34 na ESP32

O arquivo.ino implementa uma maneira simplifica de obter o valor da Tag RFID lida.
Além disso tal valor pode ser formatado nos parâmetros de TrixFormat.h


# Para implementar

Na versão atual do código não é possivel obter o dado completo da Tag RFID, 34 bits. O único retorno possível é o do valor sem os bits de paridade, 32 bits.
Também não é possivel escolher entre facility code e id code.
Não há persistência do número de mensagens lidas, o ideal seria usar a EEPROM ou um armazenameto externo.
O identificador do dispositivo tem um valor default, mas deve ser alterado caso sejam usados mais de 1 dispositivo.

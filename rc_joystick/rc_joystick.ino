#include <SPI.h>                                         
#include <nRF24L01.h>                                  
#include <RF24.h>                                       

const int Y = A0;
const int X = A1;
const int HORIZONTAL_LEFT_BUTTON = 3;
const int HORIZONTAL_RIGHT_BUTTON = 4;
const int VERTICAL_UP_BUTTON = 5;
const int VERTICAL_DOWN_BUTTON=6;

RF24 radio(9, 10);                      
int data[6];          
                         

void setup() {
    Serial.begin(9600);
    radio.begin();                                       
    radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate(RF24_1MBPS);       // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel(RF24_PA_HIGH);     // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe(0x1234567890LL);   // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
}

void loop() {                        
    data[0] = analogRead(Y);                             
    data[1] = analogRead(X);  
    data[2] = digitalRead(HORIZONTAL_LEFT_BUTTON);                         
    data[3] = digitalRead(HORIZONTAL_RIGHT_BUTTON);   
    data[4] = digitalRead(VERTICAL_UP_BUTTON);                           
    data[5] = digitalRead(VERTICAL_DOWN_BUTTON);   
    
//    Serial.print("Y = ");
//    Serial.println(analogRead(Y));
//    Serial.print("X = ");
//    Serial.println(analogRead(X));
//
//    Serial.print("HORIZONTAL_LEFT_BUTTON = ");
//    Serial.println(digitalRead(HORIZONTAL_LEFT_BUTTON));
//    Serial.print("HORIZONTAL_RIGHT_BUTTON = ");
//    Serial.println(digitalRead(HORIZONTAL_RIGHT_BUTTON));
//    Serial.print("VERTICAL_UP_BUTTON = ");
//    Serial.println(digitalRead(VERTICAL_UP_BUTTON));
//    Serial.print("VERTICAL_DOWN_BUTTON = ");
//    Serial.println(digitalRead(VERTICAL_DOWN_BUTTON));
//    delay(300);
    
    radio.write(&data, sizeof(data));                   
}

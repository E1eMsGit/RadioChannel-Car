/*********************************************************************
 * Машинка с управлением по радиоканалу
 * Платформа: Arduino UNO
 *                    
 * Подключенные модули: NRF24L01 (CE, CSN, SCK, MO, MI) - пин: 9, 10, 13, 11, 12
 *                      Сервопривод - пин: 11
 *                      Сервопривод - пин: 9                 
 *                      Драйвер моторов (IN4, IN3, ENB, IN2, IN1, ENA) - пин: 7, 8, 5, 3, 4, 6                   
 *                      
 * Управление осуществляется с помощью 3D-джойстика на передатчике.
 * 
 *********************************************************************/
 #include "Car.h"
#include <Servo.h>
#include <SPI.h>                                        
#include <nRF24L01.h>                                     
#include <RF24.h>

#define HORIZONTAL_SERVO   A1
#define VERTICAL_SERVO   A0

RF24 radio(9, 10);         // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
Car car(7, 8, 5, 3, 4, 6); // Создаём объект car для работы с библиотекой Car, указывая номера выводов драйвера моторов (IN4, IN3, ENB, IN2, IN1, ENA).
Servo horizontalServo;
Servo verticalServo; 

int rcvData[6];   
int horizontalServoValue = 70;
int verticalServoValue = 70;

int xValue;
int yValue;
bool hLeftFlag;
bool hRightFlag;
bool vUpFlag;
bool vDownFlag;
char carMoveAction;

void setup() {
    radio.begin();                                               
    radio.setChannel(5);                                         // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel(RF24_PA_HIGH);                  // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openReadingPipe(1, 0x1234567890LL); // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    radio.startListening();                                        // Включаем приемник, начинаем прослушивать открытую трубу
    
    horizontalServo.attach(HORIZONTAL_SERVO);
    verticalServo.attach(VERTICAL_SERVO);
    
    horizontalServo.write(horizontalServoValue);
    verticalServo.write(verticalServoValue);
}

void loop() {
     if (radio.available()) {                      
        radio.read(&rcvData, sizeof(rcvData));         
        xValue = rcvData[0];
        yValue = rcvData[1];
        hLeftFlag = rcvData[2];
        hRightFlag = rcvData[3];
        vUpFlag = rcvData[4];
        vDownFlag = rcvData[5];

        carMoveAction = getCarMoveAction(xValue, yValue);
     
        switch (carMoveAction) {     
            case '0': car.stand(); break;
            case '1': car.forward(); break;
            case '2': car.forwardLeft(); break;
            case '3': car.forwardRight(); break;
            case '4': car.backward(); break;
            case '5': car.backwardLeft(); break;
            case '6': car.backwardRight(); break;
            case '7': car.right(); break;
            case '8': car.left(); break;
        }

       if (hLeftFlag == 1) horizontalServoSpin(-1);
       else if (hRightFlag == 1) horizontalServoSpin(1);
       else if (vUpFlag == 1) verticalServoSpin(-1);
       else if (vDownFlag == 1) verticalServoSpin(1);
    }                                     
}

//************************ ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ ************************
// Вращает горизонтальный серво.
void horizontalServoSpin(int value) {
    horizontalServoValue += value;
    if (horizontalServoValue > 150) horizontalServoValue = 150;
    else if (horizontalServoValue < 10) horizontalServoValue = 10;
    horizontalServo.write(horizontalServoValue);
}

// Вращает вертикальный серво.
void verticalServoSpin(int value) {
    verticalServoValue += value;
    if (verticalServoValue > 90) verticalServoValue = 90;
    else if (verticalServoValue < 30) verticalServoValue = 30;   
    verticalServo.write(verticalServoValue);
}

void setCarSpeed(int value, int low_, int max_) {
    int motorSpeed;

    motorSpeed  = map(value, low_, max_, 0, 255);
    car.setMotorsSpeed(motorSpeed, motorSpeed);
}

char getCarMoveAction(int x, int y) {
    char action;

    if (y > 489 && y < 551 && x > 489 && x < 551) { //Stand.  
        action = '0';      
    } else if (y > 550 && x > 489 && x < 551) { //Forward.
        setCarSpeed(y, 551, 1023);
        action = '1'; 
    } else if (y > 550 && x < 490) { //ForwardLeft.
        setCarSpeed(y, 551, 1023);
        action = '2';      
    } else if (y > 550 && x > 550) { //ForwardRight. 
        setCarSpeed(y, 551, 1023);
        action = '3'; 
    } else if (y < 490 &&  x > 489 && x < 551) { //Backward.
        setCarSpeed(y, 489,  0);
        action = '4';   
    } else if (y < 490 && x < 490) {  //BackwardLeft.
        setCarSpeed(y, 489, 0);
        action = '5'; 
    } else if (y< 490 &&  x > 550) { //BackwardRight. 
        setCarSpeed(y, 489, 0);
        action = '6';      
    } else if (y > 489 && y < 551 && x > 550) {  //Right.
        setCarSpeed( x, 551, 1023);
        action = '7';
    } else if (y > 489 && y < 551 && x < 490) {  //Left.
        setCarSpeed(x, 489, 0);
        action = '8';
    } 
    
    return action;
}


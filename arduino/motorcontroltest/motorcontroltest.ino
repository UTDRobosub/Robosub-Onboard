
#include "robosub-serial.h"

/////////////////////////////////
//Serial setup

Serial_State* serialstate;

void sendCharFn(void* instance, char data, bool terminate){
  Serial.write(data);
}

void delayMsFn(void* instance, int ms){
  delay(ms);
}

void pollReceiveFn(void* instance){
  while(Serial.available()){
    char data = Serial.read();
    Serial_ReceiveChar(serialstate, data);
  }
}

void* serialInstance = 0;

void serial_setup(){
  Serial.begin(115200);

  serialstate = Serial_NewState(serialInstance, receiveMessageCallback, sendCharFn, delayMsFn, pollReceiveFn);

  delay(100);
}

/////////////////////////////////
//Motor setup



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//DO NOT WRITE TO PINS 5 OR 6
//EXCEPT THROUGH verticalMotorWrite()
byte verticalMotorPin1 = 5;
byte verticalMotorPin2 = 6;
byte verticalMotorSign = 0;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



#include <Servo.h>

byte servoPin11 = 11;
byte servoPin10 = 10;
byte servoPin9 = 9;
byte servoPin8 = 8;

Servo servo;
Servo servo10;
Servo servo9;
Servo servo8;

void motor_setup() {
  pinMode(verticalMotorPin1, OUTPUT);
  pinMode(verticalMotorPin2, OUTPUT);
  analogWrite(verticalMotorPin1, 0);
  analogWrite(verticalMotorPin2, 0);
  
  servo.attach(servoPin11);
  servo10.attach(servoPin10);
  servo9.attach(servoPin9);
  servo8.attach(servoPin8);

  servo.writeMicroseconds(1500); // send "stop" signal to ESC.
  servo10.writeMicroseconds(1500); // send "stop" signal to ESC.
  servo9.writeMicroseconds(1500); // send "stop" signal to ESC.
  servo8.writeMicroseconds(1500);
  delay(1000); // delay to allow the ESC to recognize the stopped signal
}

int signum(int n){
  if (n > 0) return 1;
  if (n < 0) return -1;
  return 0;
}

void verticalMotorWrite(int value){ //[-255,255]
  if( signum(value) == -verticalMotorSign ){
    analogWrite(verticalMotorPin1, 0);
    analogWrite(verticalMotorPin2, 0);
    //delay(1);
  }
  verticalMotorSign = signum(value);

  if(value > 0)
    analogWrite(verticalMotorPin1, value);
  else
    analogWrite(verticalMotorPin2, -value);
}

/////////////////////////////////
//Main program

struct MotorValues {
    short bl;
    short br;
    short ul;
    short ur;
    short v;
};

MotorValues motorvals;


void motor_update(){
  servo  .writeMicroseconds(motorvals.br); //actually br
  servo8 .writeMicroseconds(motorvals.ul); //actually fl
  servo9 .writeMicroseconds(motorvals.bl); //actually bl
  servo10.writeMicroseconds(motorvals.ur); //actually fr
  verticalMotorWrite(motorvals.v);         //vertical motors
}

void receiveMessageCallback(void* instance, char* message, int length, bool needsresponse, char** response, int* responselength){
  //your code here

  memcpy(&motorvals, message, length);
  //motor_update();
}

void setup(){
  motorvals.bl = 1500;
  motorvals.br = 1500;
  motorvals.ul = 1500;
  motorvals.ur = 1500;
  
  serial_setup();
  motor_setup();

  //motor_update();
}

void loop(){
  pollReceiveFn(0);

  motor_update();
  
  delay(1);
}


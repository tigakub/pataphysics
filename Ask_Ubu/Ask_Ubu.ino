/****************************************************

Arduino code for Ask Ubu, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

About the Pataphysical Slot Machine:
http://pataphysics.us

Last updated on September 10, 2015.

Written by Fabrice Florin, based on free libraries from Arduino and Adafruit, with guidance by Donald Day.

This free software is licensed under GPLv2.
 
It started as a test sketch for the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438

This sketch creates a fun motor party on your desk *whiirrr*
Connect a DC motor to M1
Connect a hobby servo to SERVO1
  
 ****************************************************/
 
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h> 

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a DC motor to port M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

// We'll also test out the built-in Arduino Servo library
Servo servo1;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("MMMMotor party!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Attach a servo to pin #10
  servo1.attach(10);
  servo2.attach(9);
   
  // turn on motor M1
  myMotor->setSpeed(200);
  myMotor->run(RELEASE);
  
}

int i;
void loop() {
  myMotor->run(FORWARD);
  for (i=0; i<255; i++) {
    servo1.write(map(i, 0, 255, 0, 100));
    myMotor->setSpeed(i);  
    delay(3);
 }
 
 for (i=255; i!=0; i--) {
    servo1.write(map(i, 0, 255, 0, 100));
    myMotor->setSpeed(i);  
    delay(3);
 }
 
  myMotor->run(BACKWARD);
  for (i=0; i<255; i++) {
    servo1.write(map(i, 0, 255, 0, 100));
    myMotor->setSpeed(i);  
    delay(3);
 }
 
  for (i=255; i!=0; i--) {
    servo1.write(map(i, 0, 255, 0, 100));
    myMotor->setSpeed(i);  
    delay(3);
 }
}

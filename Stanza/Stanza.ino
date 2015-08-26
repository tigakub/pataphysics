//Multiple LED flicker by gotcha (2010)
//
//
#include   <TrueRandom.h>   //Include the TrueRandom Library

int ledPin1 = 13; //Set the number of LEDs you use.
int ledPin2 = 12;
int ledPin3 = 11;
int ledPin4 = 10;
int ledPin5 = 9;
int ledPin6 = 8;
int ledPin7 = 7;
int ledPin8 = 6;
int ledPin9 = 5;
int ledPin10 = 4;



void setup()
{
pinMode(ledPin1, OUTPUT); //Sets the LED pins as output pins.
pinMode(ledPin2, OUTPUT);
pinMode(ledPin3, OUTPUT);
pinMode(ledPin4, OUTPUT);
pinMode(ledPin5, OUTPUT);
pinMode(ledPin6, OUTPUT);
pinMode(ledPin7, OUTPUT);
pinMode(ledPin8, OUTPUT);
pinMode(ledPin9, OUTPUT);
pinMode(ledPin10, OUTPUT);
}

void loop()
{ //Sets a random value of LED voltage in the range of 0V - 5V with PWM
analogWrite(ledPin1, TrueRandom.random(100,255)); //Use the same pins used above
analogWrite(ledPin2, TrueRandom.random(100,255));
analogWrite(ledPin3, TrueRandom.random(100,255));
analogWrite(ledPin4, TrueRandom.random(100,255));
analogWrite(ledPin5, TrueRandom.random(100,255));
analogWrite(ledPin6, TrueRandom.random(100,255));
analogWrite(ledPin7, TrueRandom.random(100,255));
analogWrite(ledPin8, TrueRandom.random(100,255));
analogWrite(ledPin9, TrueRandom.random(100,255));
analogWrite(ledPin10, TrueRandom.random(100,255));
delay(TrueRandom.random(0,300)); //Limits the speed.


}



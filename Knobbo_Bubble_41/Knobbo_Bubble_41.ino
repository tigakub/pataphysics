/****************************************************
Arduino code for the Ha Ha box, an interactive art "wonderbox"
for the Pataphysical Slot Machine.
This software module controls the lights and sounds for that box, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.
About the Pataphysical Slot Machine:
http://pataphysics.us
Wonderbox specifications:
http://bit.ly/wonderbox-spec
Last updated on September 26, 2015.
Written by Mark Petrakis, based on free libraries from Arduino, Adafruit and others. 
Sound playback code by Donald Day and Tim Pozar.
This free software is licensed under GPLv2.
  
 ****************************************************/
 
#include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
#include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) 

Adafruit_MCP23008 mcp;     // instantiate Adafruit_MCP23008 mcp

const int box_button = 12; // the switch for the whole box may be placed on pin 12 
                           // -- it is triggered when you open the box and sees "HIGH"
int buttonState = 0;       // variable for reading the pushbutton status
int oldbuttonState = 0;    // for button changes

int songValue = 41;  // Play Track 41 (Box #41)
int quietValue = 0;  // TRK0 means stop playing

int led1 = 0;
int led2 = 1;
int led3 = 2;
int led4 = 3;
int led5 = 4;
int led6 = 5;
int led7 = 6;

//********************  SETUP    **********************

void setup() 
{ 
  pinMode(box_button, INPUT_PULLUP);   // initialize the button's digital pin as an input.
 
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led2, OUTPUT); 
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT); 
  pinMode(led6, OUTPUT); 
  pinMode(led7, OUTPUT);
  pinMode(led7, OUTPUT);     
  
  // Wait for 10mS * box number value to try to avoid collision with other boxes doing this same thing.
  delay(10*songValue);
  // initialize the digital pin as an output. 
  
mcp.begin();      // use default address 0, based at 0x20 // This setup routine will initiate the sound playback via i2c expander

for (int i=0; i<8; i++) // commenting out the server initialization
 {
   mcp.pinMode(i, OUTPUT);  //all 8 pins output
 }       
}

//********************  MAIN LOOP    **********************

void loop() 
{ 
 
  digitalWrite(led1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led4, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led5, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led6, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led7, HIGH);   // turn the LED on (HIGH is the voltage level)
                  
  delay(2000);               // wait for a second
  digitalWrite(led1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led3, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led4, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led5, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led6, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led7, LOW);    // turn the LED off by making the voltage LOW
  delay(1500);               // wait for a second

  
  // First ask the server to play a sound if the button has been pressed.
  oldbuttonState = buttonState;
  buttonState = digitalRead(box_button);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) 
  {     
    if (buttonState == HIGH) 
    {     
      mcp.writeGPIO(songValue);
    }
    else 
    {
      mcp.writeGPIO(quietValue);
    }
  }
  oldbuttonState = buttonState;
  delay(20);
}

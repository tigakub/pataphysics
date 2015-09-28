/****************************************************

Arduino code for the Time flies box, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the lights and sounds for that box, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on September 27, 2015.

Written by Fabrice Florin, based on free libraries from Arduino, Adafruit and others. 
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

Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp

const int box_button = 2; // the switch for the whole box may be placed on pin 12 -- it is triggered when you open the box. 
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes

int songValue = 23;  // Play Track 23 (Box #23)
int quietValue = 0;  // TRK0 means stop playing

const int led_1 = 3; // LED group 1 is on pin 10
const int led_2 = 4; // LED group 2 is on pin 11
const int led_3 = 5; // LED group 2 is on pin 11


//********************  SETUP    **********************

void setup() 
{ 

  // initialize the digital pin as an output.
  pinMode(box_button, INPUT);   // initialize the button's digital pin as an input.
  pinMode(led_1, OUTPUT);     
  pinMode(led_2, OUTPUT);   
  pinMode(led_3, OUTPUT);  
   
  mcp.begin();      // use default address 0, based at 0x20
/*  
for (int i=0; i<8; i++) {
  mcp.pinMode(i, OUTPUT);  //all 8 pins output
  }
*/
delay(5000); //wait five seconds after powerup    
} 


//********************  MAIN LOOP    **********************

 
void loop() 
{ 
 
  // First ask the server to play a sound if the button has been pressed.
  
  oldbuttonState = buttonState;
  buttonState = digitalRead(box_button);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) 
  {     
    if (buttonState == HIGH) 
    {     
      
  // Play a sound, since the button has been pressed.    
      
  mcp.writeGPIO(songValue);
      
   // Then make the three groups of LEDs blink on and off -- feel free to tweak the timing as needed 
  
   }
    else 
    {
      
    mcp.writeGPIO(quietValue);

   }
  }
 
  oldbuttonState = buttonState;
  delay(20);
 
  digitalWrite(led_1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led_2, HIGH);    // turn the LED off by making the voltage LOW
  digitalWrite(led_3, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
      

}


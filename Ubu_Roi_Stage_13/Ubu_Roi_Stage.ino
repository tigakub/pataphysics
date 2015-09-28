/****************************************************
Arduino code for the Ubu Roi Stage box, an interactive art "wonderbox"
for the Pataphysical Slot Machine.
This software module controls the lights and sounds for that box, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.
About the Pataphysical Slot Machine:
http://pataphysics.us
Wonderbox specifications:
http://bit.ly/wonderbox-spec
Last updated on September 22, 2015.
Written by Fabrice Florin, Freddy Hahne Donald Day and Tim Pozar
based on free libraries from Arduino, Adafruit and others. 
Sound server playback code by Donald Day and Tim Pozar.
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

int songValue = 13;  // Play Track 13 (Box #13)
int quietValue = 0;  // TRK0 means stop playing

//********************  SETUP    **********************

void setup() 
{ 
  
  mcp.begin();  // use default address 0, based at 0x20 // This routine will initiate the sound playback with i2c expander
  delay(5000);  // adding a short delay to eliminate i2c activity on the bus
  
  /* Wait for 10mS * box number value to try to avoid collision with other boxes doing this same thing.
   delay(100*songValue); //
   initialize the digital pin as an output. 
  
for (int i=0; i<8; i++) // commenting out the server initialization
 {
   mcp.pinMode(i, OUTPUT);  //all 8 pins output
 } 
 */
 
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

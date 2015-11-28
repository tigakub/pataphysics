/*************************************************** 
Arduino code for playing sounds from a Wonderbox client, using a separate sound server with MCP23008 i2c expander and MusicMaker,
in the Wonderbox system that powers the Pataphysical Slot Machine.

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Wonderbox code repository
https://github.com/fabriceflorin/pataphysics

About the Pataphysical Slot Machine:
http://pataphysics.us

This code requires the Arduino Library for the MCP23008 (and '9) I2C I/O expander at:
https://github.com/adafruit/Adafruit-MCP23008-library
(this code needs to be placed in 'Arduino/libraries/Adafruit_MCP23008' folder on your computer)

Last updated on November 28, 2015.

Written by Donald Day and Tim Pozar, based on free libraries from Arduino and Adafruit. 

This free software is licensed under GPLv2.
  
****************************************************/
 
#include <Wire.h>
#include "Adafruit_MCP23008.h"

/* Wiring instructions:

For the Arduino or Diavolino clients in each Wonderbox:
- Connect the red wire from the pataphysical bus to the 5V pin on Arduino
- Connect the black wire from the pataphysical bus to any ground pin on Arduino
- Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
- Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #2 of the expander)

For the Arduino sound server with the MCP23008 i2c expander: (ignore these if you are only working on the clients)
- Connect pin #1 of the expander to Analog 5 (i2c clock - SCL) (green wire on the pataphysical bus)
- Connect pin #2 of the expander to Analog 4 (i2c data - SDA) (blue wire on the pataphysical bus, yellow wire on sound server)
- Connect pins #3, 4 and 5 of the expander to ground (address selection)
- Connect pin #6 and 18 of the expander to 5V (power and reset disable)
- Connect pin #9 of the expander to ground (common ground)
- Output #0 is on pin 10 so connect an LED or whatever from that to ground

The code below is a basic toggle test for i/o expansion. It flips pin #0 of a MCP23008 i2c pin expander up and down. 
*/

// writeGPIO(data);


Adafruit_MCP23008 mcp;

const int buttonPin = 12;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
int songValue = 27;  // Play Track 27 (Box #27)
int quietValue = 0;  // TRK0 means stop playing


//LIGHTS
//Cycle through red, green, blue, yellow, purple, by varying
//intensity of red, blue, and green. For two LEDs, blinking through colors faster and faster then slower and slower

/*
Adapted from Adafruit Arduino - Lesson 3. RGB LED
Diffuse RGB LED wired with common negative pin connected
to ground, other 3 pins connected via 270 Ohm resistors to pins
11, 10, 9 -- for two LEDS in synch, r and r2 are pin3 and pin6;
g and g2 are pin4 and pin9, b and b2 are pin5 and pin10 */

int redPin = 3;
int greenPin = 4;
int bluePin = 5;
int red2Pin = 6;
int green2Pin = 9;
int blue2Pin = 10;
int delayPeriodSlow = 250 ; // just example values lower value is faster
// original value set to 250
int delayPeriodFast = 25;
int delayPeriodIncrement = 10;

void setup()
{

pinMode(buttonPin, INPUT);

pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT); 
pinMode(red2Pin, OUTPUT);
pinMode(green2Pin, OUTPUT);
pinMode(blue2Pin, OUTPUT); 

delay(5000); //wait five seconds before calling the sound server
mcp.begin();      // use default address 0, based at 0x20

}

void loop()
{
  
  
 oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
//    mcp.begin();      // use default address 0, based at 0x20
      mcp.writeGPIO(songValue);
    }
    else {
      mcp.writeGPIO(quietValue);
    }
  }
  oldbuttonState = buttonState;
  delay(20);

  
int i;
for(i = delayPeriodSlow; i >= delayPeriodFast; i = i - delayPeriodIncrement) //speed up 
{
flashColors(i);
}
for(i = delayPeriodFast; i <= delayPeriodSlow; i = i + delayPeriodIncrement) //slow down
{
flashColors(i);
}
} 
void flashColors(int delayPeriod)

{
setColor(255, 0, 0); // red
delay(delayPeriod);
setColor(0, 255, 0); // green
delay(delayPeriod);
setColor(0, 0, 255); // blue
delay(delayPeriod);
setColor(255, 255, 0); // yellow
delay(delayPeriod); 
setColor(80, 0, 80); // purple
delay(delayPeriod);
}

void setColor(int red, int green, int blue)
{
analogWrite(redPin, red);
analogWrite(greenPin, green);
analogWrite(bluePin, blue); 
analogWrite(red2Pin, red);
analogWrite(green2Pin, green);
analogWrite(blue2Pin, blue); 
}

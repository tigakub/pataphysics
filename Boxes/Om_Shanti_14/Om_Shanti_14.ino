/*

 Arduino code for the Om Shanti Wonderbox, an interactive art scene.
 
 In this scene, three 'eye flowers' sing 'Om Shanti'. 
 We originally played sounds using the Adafruit Waveshield, 
 but we now play them from a separate sound server over our i2c bus, 
 using an MCP23008 i2c expander and MusicMaker.
 
 Three white LEDs in the ceiling light up the scene (5v).
 Each eye flower has an  RGB LED that automatically cycles through
 the colors of the rainbow, using their own embedded circuit (3.3V).
 
 The circuit:
 - Arduino processor
 - 3 white LEDs (using 5V, pins 6 to 8) 
 - 3 RGB LEDs (now using 3.3V pin) 
 - WaveShield sound processor 

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Photos and info about Pataphysical Studios:
http://pataphysics.us

 Last updated on November 29, 2015.
 Written by Fabrice Florin, with guidance from Donald Day and Anselm Hook, 
 based on free libraries from Arduino, Adafruit and others. 
 Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
 
 */

////////////////////////////////////
// SETTINGS
////////////////////////////////////


// BUTTON SETTINGS

const int buttonPin = 9;     // the number of the button pin
int buttonState = 0;         // variable for reading the button status
int oldbuttonState = 0;      // for button changes


// SOUND SERVER SETTINGS

#include <Wire.h>
#include "Adafruit_MCP23008.h"

/* Wiring instructions:
- Connect the red wire from the pataphysical bus to the 5V pin on Arduino
- Connect the black wire from the pataphysical bus to any ground pin on Arduino
- Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
- Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #2 of the expander)
The code below is a basic toggle test for i/o expansion. It flips pin #0 of a MCP23008 i2c pin expander up and down. 
*/

Adafruit_MCP23008 mcp;

int songValue = 14;  // Play Track 14 (Box #14)
int quietValue = 0;  // TRK0 means stop playing




////////////////////////////////////////////
// SETUP
////////////////////////////////////////////
 
  
  void setup() {
  // Set up serial port
  Serial.begin(9600);
 Serial.println("Set up button, lights, sounds.");      // 
 
 // Setup Button
  pinMode(buttonPin, INPUT);
  
 // Setup LEDs
   // initialize each LED pin as an output, turn on:
  for (int thisPin = 6; thisPin < 9; thisPin++)  {
   pinMode(thisPin, OUTPUT);      // sets the pin as output
   digitalWrite(thisPin, LOW);    // sets the LED off 
  }

 // Setup Sound Server
   delay(200*songValue); // wait a few seconds to call the sound server
  mcp.begin();      // use default address 0, based at 0x20
}



////////////////////////////////////////////
// LOOP
////////////////////////////////////////////


void loop() {
  oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // check if the button is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
      
      
    // play sound     
     mcp.writeGPIO(songValue);
      delay(200);            // delay 200 milliseconds
      Serial.println ("Playing song with Sound Server.");
      
      // turn LEDs on
        for (int thisPin = 6; thisPin < 9; thisPin++)  {
       digitalWrite(thisPin, HIGH);    // turn the LEDs on 
      }
    }
    else {
         
    // stop sound
     mcp.writeGPIO(quietValue);
      delay(200);            // delay 200 milliseconds
      Serial.println ("Stopping sound from Sound Server.");
      
      // turn LEDs off
        for (int thisPin = 6; thisPin < 9; thisPin++)  {
        digitalWrite(thisPin, LOW);    // turn the LEDs off 
        }
     }
   }
  oldbuttonState = buttonState;
  delay(20);
}



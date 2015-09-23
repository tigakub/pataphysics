/****************************************************

Arduino code for X-Ray Skull, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the skull's sound and motion, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.

A separate software module controls the skull's eye lights, 
using an Arduino Uno.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on September 21, 2015.

Written by Jean Bolte, with help from Donald Day and Fabrice Florin, 
based on free libraries from Arduino, Adafruit and Barraganstudio.com. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
  
 ****************************************************/
 
#include <Servo.h> // for controlling servo motor
#include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
#include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) 

Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp

const int box_button = 12; // the switch for the whole box may be placed on pin 12 -- it is triggered when you open the box. 

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 


int SoundPlaybackState = 0; // set to "0" by default, so the sound will play the first time.
int songValue = 28;  // Play Track 28 (Box #28), the X-Ray Skull
int quietValue = 0;  // TRK0 means stop playing


//********************  SETUP    **********************

void setup() 
{ 

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 

  
Serial.begin(9600); // enables serial connection
    
    
mcp.begin();      // use default address 0, based at 0x20 // This setup routine will initiate the sound playback via i2c expander

for (int i=0; i<8; i++) 
 {
   mcp.pinMode(i, OUTPUT);  //all 8 pins output
 }   
    
} 


//********************  MAIN LOOP    **********************

 
void loop() 
{ 
 
    
 if (SoundPlaybackState == 0) // (we have not yet played the sound)
    {  

    playSound() ; // This will play the sound, when the code is ready.

    }
  
  for(pos = 0; pos < 60; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(50);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 60; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(50);                       // waits 15ms for the servo to reach the position 
  } 
}



//********************  PLAY SOUND    **********************

void playSound() // PLAY SOUND: This will play a sound, if all goes well, using the i2c expander, for sound playback

  {
  
mcp.writeGPIO(songValue);  

  Serial.print("We are now playing the sound.");
  Serial.println(" ");


   SoundPlaybackState = 1; // We've played the sound. Don't play it again.


  } // end playSound



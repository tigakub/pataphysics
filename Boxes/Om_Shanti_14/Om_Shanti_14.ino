/*

 Arduino code for the Om Shanti Wonderbox, an interactive art scene.
 
 In this scene, three 'eye flowers' sing 'Om Shanti'. 
 We first implemented sound playback using the Adafruit Waveshield.
 
 We also plan to play sounds from a separate sound server over our i2c bus, 
 using an MCP23008 i2c expander and MusicMaker (that code is now commented out).
 
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

 Last updated on November 28, 2015.
 Written by Fabrice Florin <fabriceflorin@gmail.com>, with guidance from Donald Day and Anselm Hook. 
 Sound code based on the Digital Audio Player sketch (dapHC.pde) using the Wave HC library published by Adafruit:
 http://www.ladyada.net/make/waveshield/libraryhc.html 

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

/* Commenting out idc server code for now, until it's more stable
// #include <Wire.h>
// #include "Adafruit_MCP23008.h"

Wiring instructions:
- Connect the red wire from the pataphysical bus to the 5V pin on Arduino
- Connect the black wire from the pataphysical bus to any ground pin on Arduino
- Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
- Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #2 of the expander)
The code below is a basic toggle test for i/o expansion. It flips pin #0 of a MCP23008 i2c pin expander up and down. 

Adafruit_MCP23008 mcp;

int songValue = 14;  // Play Track 14 (Box #14)
int quietValue = 0;  // TRK0 means stop playing
*/

// WAVESHIELD SETTINGS


#include "WaveUtil.h"
#include "WaveHC.h"

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're playing

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

#define DEBOUNCE 100  // button debouncer

int dirLevel = 0; // declare dirLevel to avoid error message in setup


// FREE RAM FUNCTION
// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}



////////////////////////////////////////////
// SETUP
////////////////////////////////////////////
 
  
  void setup() {
  // Set up serial port
  Serial.begin(9600);
  putstring_nl("WaveHC with 6 buttons");
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
 
 // Setup Button
  pinMode(buttonPin, INPUT);
  
 // Setup LEDs
   // initialize each LED pin as an output, turn off for now:
  for (int thisPin = 6; thisPin < 9; thisPin++)  {
   pinMode(thisPin, OUTPUT);      // sets the pin as output
   digitalWrite(thisPin, LOW);    // sets the LED off 
  }

/* Commenting out idc server code for now, until it's more stable
 // Setup Sound Server
  delay(5000); //wait five seconds after powerup
  mcp.begin();      // use default address 0, based at 0x20
*/
   
  // Setup Waveshield
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
 
// Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Ready!");
  dirLevel = 0;
}



////////////////////////////////////////////
// LOOP
////////////////////////////////////////////
 
 //SOUND SERVER LOOP
 

void loop() {
  oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // check if the button is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
      /* Commenting out idc server code for now, until it's more stable
      mcp.writeGPIO(songValue);
      delay(200);            // delay 200 milliseconds
      Serial.println ("Playing song with Sound Server.");
      */
      Serial.println ("Playing song with Waveshield.");
      delay(200);            // delay 200 milliseconds
      Serial.println("");      // print carriage return
      playcomplete("OMSHAN~1.WAV");
          }
    else {
     /* Commenting out idc server code for now, until it's more stable
      mcp.writeGPIO(quietValue);
      delay(200);            // delay 200 milliseconds
      Serial.println ("Stopping sound from Sound Server.");
      */
    
      buttonState = digitalRead(buttonPin);
      // check if the button is pressed.
       if (buttonState != oldbuttonState) {     
          if (buttonState == LOW) {     
             Serial.println ("Button has been pressed.");
                if (wave.isplaying) {// if Waveshield is already playing something, so stop it!
                wave.stop(); // stop the Waveshield playback
                }
           delay(200);            // delay 200 milliseconds
           Serial.println ("Stopping sound from Waveshield.");
           }
        }
    }
  }
  oldbuttonState = buttonState;
  delay(20);
}


/////////////////////////////////////////////////////
// UTILITIES
/////////////////////////////////////////////////////


// Plays a full file from beginning to end with no pause.
void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  //do nothing
   }
}

void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
}

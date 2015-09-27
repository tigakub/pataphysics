/*

 Arduino code for the Eye Flower Wonderbox, an interactive art scene.
 
 In this scene, three 'eye flowers' are featured in turn, using multiple LEDs 
 to light up each eye flower -- and sing to each other in sequence,
 to simulate a musical conversation synchronized with the lights.
 
 The circuit:
 - Arduino processor
 - 3 white LEDs (pins 7 to 9) 
 - 3 RGB LEDs (pins 10 to 12) 
 - WaveShield sound processoer 

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Photos and info about Pataphysical Studios:
http://pataphysics.us


 Last updated on April 13, 2014.
 Written by Fabrice Florin <fabriceflorin@gmail.com>, with guidance from Donald Day and Anselm Hook. 
 LED code based on an LED sketch created 2006 by David A. Mellis, modified 30 Aug 2011 by Tom Igoe. 
 Sound code based on the Digital Audio Player sketch (dapHC.pde) using the Wave HC library published by Adafruit:
http://www.ladyada.net/make/waveshield/libraryhc.html 

 This code is licensed freely under CC-BY-SA.
 
 */

////////////////////////////////////
// INCLUDES
////////////////////////////////////

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

////////////////////////////////////
//SETTINGS
////////////////////////////////////

// LED SETTINGS

int timer = 2000;           // The higher the number, the slower the timing.

// pins 7 to 9 are RGB LEDs with pulsing circuit (3.3V, 20 mA) 
// pin 6 is for 3 white LEDs (5V, 20 mA)

// SOUND SETTINGS

char* thisFlowerSoundName = "";

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're playing

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time


#define DEBOUNCE 100  // button debouncer

uint8_t dirLevel; // indent level for file/dir names    (for prettyprinting)
dir_t dirBuf;     // buffer for directory reads


/////////////////////////////////////////////////////
// UTILITIES
/////////////////////////////////////////////////////

/*
 * print dir_t name field. The output is 8.3 format, so like SOUND.WAV or FILENAME.DAT
 */
void printName(dir_t &dir)
{
  for (uint8_t i = 0; i < 11; i++) {     // 8.3 format has 8+3 = 11 letters in it
    if (dir.name[i] == ' ')
        continue;         // dont print any spaces in the name
    if (i == 8) 
        Serial.print('.');           // after the 8th letter, place a dot
    char printme = dir.name[i];
    Serial.print(printme);      // print the n'th digit
  }
  if (DIR_IS_SUBDIR(dir)) 
    Serial.print('/');       // directories get a / at the end
}

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

/*
 * list recursively - possible stack overflow if subdirectories too nested
 */
void lsR(FatReader &d)
{
  int8_t r;                     // indicates the level of recursion
  
  while ((r = d.readDir(dirBuf)) > 0) {     // read the next file in the directory 
    // skip subdirs . and ..
    if (dirBuf.name[0] == '.') 
      continue;
    
    for (uint8_t i = 0; i < dirLevel; i++) 
      Serial.print(' ');        // this is for prettyprinting, put spaces in front
    printName(dirBuf);          // print the name of the file we just found
    Serial.println();           // and a new line
    
    if (DIR_IS_SUBDIR(dirBuf)) {   // we will recurse on any direcory
      FatReader s;                 // make a new directory object to hold information
      dirLevel += 2;               // indent 2 spaces for future prints
      if (s.open(vol, dirBuf)) 
        lsR(s);                    // list all the files in this directory now!
      dirLevel -=2;                // remove the extra indentation
    }
  }
  sdErrorCheck();                  // are we doign OK?
}



byte check_switches()
{
  static byte previous[6];
  static long time[6];
  byte reading;
  byte pressed;
  byte index;
  pressed = 0;

  for (byte index = 0; index < 6; ++index) {
    reading = digitalRead(14 + index);
    if (reading == LOW && previous[index] == HIGH && millis() - time[index] > DEBOUNCE)
    {
      // switch pressed
      time[index] = millis();
      pressed = index + 1;
      break;
    }
    previous[index] = reading;
  }
  // return switch number (1 - 6)
  return (pressed);
}

void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file ");
    Serial.println(name);
    return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("\nNot a valid WAV");
    return;
  }
  
  // ok time to play! start playback
  wave.play();
}

// Plays a full file from beginning to end with no pause.
void playcomplete(char* name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

////////////////////////////////////////////
// SETUP
////////////////////////////////////////////


void setup() {        
 
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging
  
   putstring_nl("WaveHC with 6 buttons");
  
   putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
  
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  
  // initialize each LED pin as an output:
  for (int thisPin = 6; thisPin < 10; thisPin++)  {
   pinMode(thisPin, OUTPUT);      // sets the pin as output
   digitalWrite(thisPin, LOW);    // sets the LED on 
  }
  
   
  // pin13 LED
  pinMode(13, OUTPUT);
 
  // enable pull-up resistors on switch pins (analog inputs)
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
 
  
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
  putstring_nl("Files found:");
  dirLevel = 0;
  // Print out all of the files in all the directories.
  lsR(root);
}

#define FLOWERCOUNT 3

void loop() {

  // Here are the flower files that we wish to play
  char* flowerNames[FLOWERCOUNT] = { "OMSHAN~1.WAV", "OMSHAN~2.WAV", "OMSHAN~3.WAV" };  

  for (int thisFlower = 0; thisFlower < FLOWERCOUNT; thisFlower++) { 
  
    int thisFlowerLEDColor = thisFlower + 1 + 6;  // find this flower's RGB LED pin
    thisFlowerSoundName = flowerNames[thisFlower];
 
    pinMode(thisFlowerLEDColor, OUTPUT); // turn on the RGB LED pin
    playcomplete(thisFlowerSoundName); 
    delay(timer);        
    
    // turn everything off, reset variables
    pinMode(thisFlowerLEDColor, INPUT);   // turn the RGB LED pin off   
       
               
    thisFlowerSoundName = ""; // empty the sound file name
      
   // Debugging code to check activity in the Serial Monitor:  
    Serial.print(" Flower: "); // send this flower RGB pin numer 
    Serial.print(thisFlower); // send this flower RGB pin numer 
    Serial.print(" Color LED: "); // send this flower RGB pin numer 
    Serial.print(thisFlowerLEDColor); // send this flower RGB pin numer 
     Serial.print(" Sound file: "); // send this flower Sound file name
     Serial.println (thisFlowerSoundName);
        delay(200);            // delay 200 milliseconds
    Serial.println("");      // print carriage return

     delay(500);   

///PHOTO TRANSISTOR CODE

// lets peek at the analog photo resistor status

     pinMode(6,INPUT);
     int photoResistorValue = analogRead(6);
     Serial.print("analog photo resistor read status ");
     Serial.print(photoResistorValue);
     Serial.println("");
     if(photoResistorValue > 200 ) {
        Serial.println("Photo Resistor Value is big!");
     }     

  }
}



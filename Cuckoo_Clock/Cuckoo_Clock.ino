/****************************************************

Arduino code for Bali Cuckoo Clock, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

About Bali Cuckoo Clock:
http://bit.ly/bali-cuckoo-clock

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

About the Pataphysical Slot Machine:
http://pataphysics.us

Last updated on July 28, 2015.

Written by Fabrice Florin, based on free libraries from Arduino and Adafruit. Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
  
 ****************************************************/
 
#include <Servo.h> // for controlling servo motors
// #include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
// #include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library

// Connect the red wire from the pataphysical bus to the VIN pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #1 of the expander)

const int box_button = 2; // the switch for the whole box is on pin 2 -- it is triggered when you open the box 
const int temple_outside_light_left = 3; // temple left light LED is on pin 3
const int temple_outside_light_right = 4; // temple right light LED is on pin 4
const int temple_inside_light = 5; // temple inside light LEDs is on pin 5 
const int garuda_front_light = 6; // garuda front light LED is on pin 6 
const int garuda_eyes = 7; // garuda_eyes LEDs is on pin 7 
// const int temple_back_light = n/a; // temple back light LED (rainbow) is on the 3.3V pin on Arduino

Servo servo1;  // create 1st servo object (Garuda moves back and forth - HiTech HS 785HB winch servo - pin 8)  
Servo servo2;  // create 2nd servo object (Garuda left wing - pin 9)  
Servo servo3;  // create 3rd servo object (Garuda right wing - pin 10)
Servo servo4;  // create 4th servo object (Garuda head - pin 11)
Servo servo5;  // create 5th servo object (left door - pin 12)
Servo servo6;  // create 6th servo object (right door - pin 13)

int doors = 0;    // generic variable to store door positions for moving them: closed (0) to open (180) 
int doorstatus = 0;    // generic variable to store door status for knowing when to move: closed (0) or open (1) 
int loc = 0;    // generic variable to store Garuda's location: angle between 93 and 112 
int dest = 0;    // generic variable to store Garuda's destination angle: front (e.g.: 93) or back (e.g.: 112)
int dest_front = 93; // Garuda's front destination angle
int dest_back = 112; // Garuda's back destination angle 
int pos = 0;    // generic variable to store Garuda's head positions: left (0) to right (45) 
int head_pos_left = 0;    // Garuda's left head position 
int head_pos_right = 45;    // Garuda's right head position 
int wing_pos = 0;    // generic variable to store Garuda's wing positions: back (0) to front (60) 
int wing_pos_back = 0;    // Garuda's back wing position 
int wing_pos_front = 75;    // Garuda's front wing position 
int garuda_wait = 0; // set to "1" if garuda needs to wait until someone opens the box again (which presses the button)

unsigned long garuda_start;
unsigned long garuda_time;
unsigned long garuda_act_total_duration;

int buttonState = 0;         // variable for reading the button status
int oldbuttonState = 0;      // for tracking button changes
int songValue = 0;           // for sound playback

//********************  SET UP    **********************

void setup() 
{ 

Serial.begin(9600); // enables serial connection

pinMode(box_button, INPUT);   // initialize the button's digital pin as an input. 
pinMode(temple_outside_light_left, OUTPUT);   // initialize the left temple light's digital pin as an output. 
pinMode(temple_outside_light_right, OUTPUT);   // initialize the right temple light's digital pin as an output. 
pinMode(temple_inside_light, OUTPUT);   // initialize the temple inside light's digital pin as an output. 
pinMode(garuda_front_light, OUTPUT);   // initialize Garuda front light digital pin as an output. 
pinMode(garuda_eyes, OUTPUT);   // initialize Garuda eye lights digital pin as an output. 

servo1.attach(8);  // attaches the servo on pin 8 to the servo object
servo2.attach(9);  // attaches the servo on pin 9 to the servo object
servo3.attach(10);  // attaches the servo on pin 10 to the servo object 
servo4.attach(11);  // attaches the servo on pin 11 to the servo object 
servo5.attach(12);  // attaches the servo on pin 12 to the servo object 
servo6.attach(13);  // attaches the servo on pin 13 to the servo object 

loc = servo1.read();

    Serial.println("=== Setup"); // to check we're doing the right thing in setup
    Serial.print("Setup Garuda location: ");
    Serial.println(loc);
    Serial.print("Setup door status: ");
    Serial.println(doorstatus);
    Serial.print("Setup Garuda wait status: ");
    Serial.println(garuda_wait);
    Serial.println(" ");
    
} 


// What we want
// 1. Open box (button release)
// 2. Play sound / turn lights on
// 3. Open doors
// 4. Garuda moves forward
// 5. Garuda performs (turn head, blink eyes, wings forward/back)
// 6. Garuda moves backwards
// 7. Close doors
// 8. Wait for button (auick press starts step 2 again)
// 9. Close box (long button press stops lights and music)

//********************  MAIN LOOP    **********************

void loop() 
{ 
  
   Serial.println("=== Start loop");  // to check we're doing the right thing in setup
   loc = servo1.read();    
   Serial.print("Current location: ");
   Serial.println(loc);
   Serial.print("Garuda wait status: ");
   Serial.println(garuda_wait);
   Serial.println(" ");
   
   // CHECK BUTTON: Check if the button has been pressed, and act accordingly.

   buttonState = digitalRead(box_button);

   Serial.print("Button State:");  // print the button state
   Serial.println(buttonState);  // print the button state

  if (garuda_wait == 1) // garuda now needs to wait until someone opens the box again (which presses the button)
  {  
    
    Serial.println("Stop everything because Garuda is done.");  // explain that we're stopping because Garuda is done.

  return;
  }


  if (buttonState == 1) // STOP BUTTON LOOP: if button is pressed, stop everything, send Garuda to back and close doors.
  {  
          
  Serial.println("Stop everything because button is now pressed.");  // explain that we're stopping.
 
     dest = dest_back;

     Serial.println("Send Garuda to the back, close the doors and stop.");
     Serial.print("Going to: ");
     Serial.println(dest);  // tell us which direction Garuda is now going  
  
     servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)

     delay(2000);

     if ( loc == dest_back )     // GARUDA IS IN THE BACK, TELL HIM TO CLOSE DOORS.
     {
   
     Serial.println("Garuda is now in the back, close doors");
   
     closeDoors();  
 
     Serial.println("Doors are now closed, stop everything.");
     }
   }
  
   if (buttonState == 0) // if button is not pressed, make Garuda perform.  // STANDARD LOOP
   { 
    
   Serial.println("Garuda will now perform for you");

     if ( loc == dest_back )   // STANDARD LOOP 1: GARUDA IS IN THE BACK, TELL HIM TO OPEN DOORS, THEN MOVE TO THE FRONT (OR CLOSE DOORS IF HE'S DONE) 
     {

     Serial.println("Garuda is in the back");

      if (doorstatus == 0) // Doors are closed, open them and have Garuda move to front
      {
 
      openDoors();
     
      delay(2000); 
   
      dest = dest_front;
   
      Serial.print("Going to: ");
      Serial.println(dest);  // tell us which direction Garuda is expected to be going  
  
      servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)

      Serial.println("Wait 2 seconds");

      delay(2000); 
 
      garuda(); // START GARUDA ACT LOOP -- Garuda does his act (flaps its wings, shakes its head and sings advice) 

      } 
     }
    
    
     if ( loc == dest_front )     // STANDARD LOOP 2: GARUDA IS IN THE FRONT, TELL HIM TO MOVE TO THE BACK, THEN CLOSE DOORS
     {

     Serial.println("Garuda is in the front");
   
     dest = dest_back;
   
     Serial.print("Going to: ");
     Serial.println(dest);  // tell us which direction Garuda is expected to be going  
  
     servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)

     Serial.println("Wait 2 seconds");

     delay(2000); 

     closeDoors();
     
     }

     if ( loc == dest_back )
     {
       if (doorstatus == 0)
       {
       garuda_wait = 1; // set to "1" because garuda now needs to wait until someone opens the box again (which presses the button)
       }
     }
   }
   
  Serial.println("=== End loop");
  Serial.println(" "); // add a blank line when a new loop ends
  
} // END LOOP 
 
 

//********************  GARUDA    **********************


void garuda() 

{ 
  
Serial.println("start garuda"); // tell us when Garuda loop is invoked
    
if (dest == dest_front) // If Garuda is going to the front
{

 
garuda_start = millis(); // Garuda start time
garuda_time = 0; //  Garuda's time spent since start
garuda_act_total_duration = 5000; // total time of the Garuda act 
 
 
  blinkTempleLightsOn();
  blinkInsideLightsOn();
  blinkFrontLightOn();

 
while (garuda_time <= garuda_act_total_duration)

{
    
  for(wing_pos = wing_pos_front; wing_pos >= wing_pos_back; wing_pos -=2)     // GARUDA FLAPS HIS WINGS BACKWARD - servos go from 75 degrees to 0 degrees in steps of 2 degrees 
  {                                  // except servo 3 that goes from 0 degrees to 65 degrees 
    servo2.write(wing_pos);              // tell servo 2 (Garuda left wing) to go to position in variable 'wing_pos'
    servo3.write(-(wing_pos - wing_pos_front));        // tell servo 3 (Garuda right wing) to go to position in variable 'wing_pos'
    
     blinkEyesOn();
     delay (50);
     blinkEyesOff();   
   }
 
 for(wing_pos = wing_pos_back; wing_pos < wing_pos_front; wing_pos += 2)  // GARUDA FLAPS HIS WINGS FORWARD - servos go from 0 degrees to 75 degrees in steps of 2 degrees 
  {                                  // except servo 3 that goes from 65 degrees to 0 degrees 
    servo2.write(wing_pos);              // tell servo 2 (Garuda left wing) to go to position in variable 'wing_pos' 
    servo3.write((wing_pos_front - wing_pos));         // tell servo 3 (Garuda right wing) to go to position "75" minus variable 'wing_pos' 
    
     blinkEyesOn();
     delay (50);
     blinkEyesOff();   
   } 

  Serial.println("Garuda shakes his head"); // tell us when Garuda head shake loop is invoked

   for(pos = 0; pos < 60; pos += 1)  // GARUDA SHAKES HIS HEAD LEFT - servo 4 goes from 0 degrees to 45 degrees in steps of 1 degrees 
   {                                  // 
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 
    
     blinkEyesOn();
     delay (30);
     blinkEyesOff();   
   } 
  
  for(pos = 60; pos >=1; pos -=1)     // GARUDA SHAKES HIS HEAD RIGHT - servo 4 goes from 45 degrees to 0 degrees in steps of 1 degrees 
  {                                  //  
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 
    
     blinkEyesOn();
     delay (30);
     blinkEyesOff();   
   }

 
garuda_time = millis() - garuda_start; //  Garuda's time spent since start

//Serial.println(garuda_time); // tell how much time Garuda has used so far

} //end while


  blinkTempleLightsOff();
  blinkInsideLightsOff();
  blinkFrontLightOff();


Serial.println("end garuda"); // tell us when Garuda loop ends

Serial.println(garuda_time); // tell how much time Garuda has used so far

//garuda_time = 0; // reset timer to 0

//Serial.println("Garuda stopped"); // tell us Garuda has stopped
   
}

// } else
// {
//   Serial.println("Don't move Garuda"); 
// } 
     
//return;

} // end garuda()


//********************  DOORS    **********************


void openDoors()

{
    
if (doorstatus == 0)        // OPEN DOOR LOOP
{

  for(doors = 180; doors >=0; doors -=2)     // servos go from 180 degrees to 0 degrees in steps of 1 degrees 
{                                  // except servo 6 that goes from 0 degrees to 180 degrees 
servo5.write(doors);              // tell servo 5 (left door) to go to position in variable 'doors'
servo6.write(-(doors-180));        // tell servo 6 (right door) to go to position in variable 'doors'

delay(50);                       // waits 50ms for the servo to reach the position 
}

//Serial.println("= = = = = = ="); // add a blank line
//Serial.println("New sequence"); // add a blank line
//Serial.println(" "); // add a blank line
  
Serial.println("The door is now closed: 0"); // tell us if the door was "opened" ("1") or "closed" ("0")
Serial.println("Opening the door"); // "opening door"
Serial.println(" "); // add a blank line

doorstatus = 1;

garuda_wait = 0;

Serial.print("New Garuda is done waiting, ready to go out: ");
Serial.println(garuda_wait);

// Serial.println("Status:");
// Serial.println(doorstatus);

}

} // end openDoors()



void closeDoors()
  
{
  
if (doorstatus == 1)        // CLOSE DOOR LOOP

{

  for(doors = 0; doors <=180; doors +=2)     // servos go from 0 degree to 180 degrees in steps of 1 degrees 
{                                  // except servo 6 that goes from 0 degrees to 180 degrees 
servo5.write(doors);              // tell servo 5 (left door) to go to position in variable 'doors'
servo6.write(-(doors-180));        // tell servo 6 (right door) to go to position in variable 'doors'

delay(50);                       // waits 50ms for the servo to reach the position 
}
  
Serial.println("The door is now open: 1"); // tell us if the door was "opened" ("1") or "closed" ("0")
Serial.println("Closing the door"); // "closing door"
Serial.println(" "); // add a blank line

doorstatus = 0;

garuda_wait = 1;

Serial.print("Garudo is behind the doors, needs to wait: ");
Serial.println(garuda_wait);

}
  
} // endcloseDoors()


//********************  LIGHTS    **********************


void blinkEyesOn()
{
  digitalWrite(garuda_eyes, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end blinkEyesOn


void blinkEyesOff()
{
 digitalWrite(garuda_eyes, LOW);    // turn the LED off by making the voltage LOW
} // end blinkEyesOff


void blinkFrontLightOn()
{
  digitalWrite(garuda_front_light, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Garudo front light is on");
  
} // end blinkEyesOn


void blinkFrontLightOff()
{
 digitalWrite(garuda_front_light, LOW);    // turn the LED off by making the voltage LOW
   Serial.println("Garudo front light is off");
} // end blinkEyesOff

void blinkInsideLightsOn()
{
  digitalWrite(temple_inside_light, HIGH);   // turn the LED on (HIGH is the voltage level)
} // blinkInsideLightsOn


void blinkInsideLightsOff()
{
 digitalWrite(temple_inside_light, LOW);    // turn the LED off by making the voltage LOW
} // end blinkInsideLightsOff


void blinkTempleLightsOn()
{
  digitalWrite(temple_outside_light_left, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(temple_outside_light_right, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end blinkTempleLightsOn


void blinkTempleLightsOff()
{
 digitalWrite(temple_outside_light_left, LOW);    // turn the LED off by making the voltage LOW
 digitalWrite(temple_outside_light_right, LOW);    // turn the LED off by making the voltage LOW
} // end blinkTempleLightsOff


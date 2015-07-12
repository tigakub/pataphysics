/*

Arduino code for Bali Cuckoo Clock, an interactive art box
for the Pataphysical Slot Machine.

About Bali Cuckoo Clock:
http://bit.ly/bali-cuckoo-clock

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

About Pataphysical Studios:
http://pataphysics.us


Last updated on July 12, 2015.
Written by Fabrice Florin, based on free libraries from Arduino and Adafruit Industries. 

This free software is licensed under GPLv2.
  
 ****************************************************/
 
 #include <Servo.h> 

Servo servo1;  // create 1st servo object (Garuda moves back and forth - HiTech HS 785HB multi-turn winch servo)  
Servo servo2;  // create 2nd servo object (Garuda left wing)  
Servo servo3;  // create 3rd servo object (Garuda right wing)   
Servo servo4;  // create 4th servo object (Garuda head)  
Servo servo5;  // create 5th servo object (left door)  
Servo servo6;  // create 6th servo object (right door)   

int loc = 0;    // generic variable to store Garuda's location: angle between 93 and 112 
int dest = 0;    // generic variable to store Garuda's destination angle: front (e.g.: 93) or back (e.g.: 112)
int dest_front = 93; // Garuda's front destination angle
int dest_back = 112; // Garuda's back destination angle 
int head_pos = 0;    // generic variable to store Garuda's head positions: left (0) to right (45) 
int head_pos_left = 0;    // Garuda's left head position 
int head_pos_right = 45;    // Garuda's right head position 
int wing_pos = 0;    // generic variable to store Garuda's wing positions: back (0) to front (60) 
int wing_pos_back = 0;    // Garuda's back wing position 
int wing_pos_front = 75;    // Garuda's front wing position 
int doors = 0;    // generic variable to store door positions for moving them: closed (0) to open (180) 
int doorstatus = 0;    // generic variable to store door status for knowing when to move: closed (0) or open (1) 
int garuda_wait = 0; // variable to indicate if garuda needs to wait so we can close, then open the doors
int garuda_eyes = 7; // garuda_eyes LEDs is on pin 7 
int garuda_front_light = 6; // garuda front light LED is on pin 6 
int temple_back_light = 5; // temple back light LED is on pin 5
int temple_inside_lights = 4; // temple inside lights LEDs is on pin 4 
int temple_outside_light_1 = 3; // temple light 1 LED is on pin 3
int temple_outside_light_2 = 2; // temple light 2 LED is on pin 2


unsigned long garuda_start;
unsigned long garuda_time;
unsigned long garuda_act_total_duration;


void setup() 
{ 

Serial.begin(9600); // enables serial connection

  servo1.attach(8);  // attaches the servo on pin 8 to the servo object
  servo2.attach(9);  // attaches the servo on pin 9 to the servo object
  servo3.attach(10);  // attaches the servo on pin 10 to the servo object 
  servo4.attach(11);  // attaches the servo on pin 11 to the servo object 
  servo5.attach(12);  // attaches the servo on pin 12 to the servo object 
  servo6.attach(13);  // attaches the servo on pin 13 to the servo object 

pinMode(garuda_eyes, OUTPUT);   // initialize Garuda eyes digital pin as an output. 
pinMode(garuda_front_light, OUTPUT);   // initialize Garuda front light's digital pin as an output. 
pinMode(temple_back_light, OUTPUT);   // initialize the back temple light's  digital pin as an output. 
pinMode(temple_inside_lights, OUTPUT);   // initialize the temple inside lights' digital pin as an output. 
pinMode(temple_outside_light_1, OUTPUT);   // initialize the right temple light's  digital pin as an output. 
pinMode(temple_outside_light_2, OUTPUT);   // initialize the left temple light's  digital pin as an output. 

  Serial.println("=== Setup");

 loc = servo1.read();
    
    Serial.print("Setup Garuda location: ");
    Serial.println(loc);
    Serial.print("Setup door status: ");
    Serial.println(doorstatus);
    Serial.print("Setup Garuda wait status: ");
    Serial.println(garuda_wait);
    Serial.println(" ");


} 


// What we want
// 1. Open doors
// 2. Move forward
// 3. Stop at front
// 4. Garuda performs
// 5. Move backwards
// 6. Stop at back
// 7. Close doors


void loop() 
{ 
  
   Serial.println("=== Start loop");
   loc = servo1.read();    
   Serial.print("Current location: ");
   Serial.println(loc);
  
if ( loc == dest_front )     // GARUDA IS IN THE FRONT, TELL HIM TO MOVE TO THE BACK
 {
  Serial.println("Garuda is in the front");
   
   dest = dest_back;
    
 }
 
   else  if ( loc == dest_back ) // GARUDA IS IN THE BACK, TELL HIM TO CLOSE & OPEN DOORS, THEN MOVE TO THE FRONT 
 {

   Serial.println("Garuda is in the back");
   
  closeDoors();  

  delay(3000); 

  openDoors();

  dest = dest_front;
 }



  Serial.print("Going to: ");
  Serial.println(dest);  // tell us which direction Garuda is expected to be going  

  
  servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)


  Serial.println("Wait 2 seconds");

  delay(2000); 
  
if (dest == dest_front) // If Garuda is going to the front
{

  garuda(); // START GARUDA ACT LOOP -- Garuda does his act (flaps its wings, shakes its head and sings advice) 
  
}
  

  Serial.println("=== End loop");
 
  Serial.println(" "); // add a blank line when a new loop ends


  } // end loop
 


void garuda() 

{ 
  
Serial.println("start garuda"); // tell us when Garuda loop is invoked
    
if (dest == dest_front) // If Garuda is going to the front
{

 
garuda_start = millis(); // Garuda start time
garuda_time = 0; //  Garuda's time spent since start
garuda_act_total_duration = 8000; // total time of the Garuda act 
 
 
  blinkTempleLightsOn();
  blinkInsideLightsOn();
  blinkFrontLightOn();

 
while (garuda_time <= garuda_act_total_duration)

{
  
   for(head_pos = head_pos_right; head_pos < head_pos_left; head_pos += 1)  // GARUDA SHAKES HIS HEAD LEFT - servo 4 goes from 0 degrees to 45 degrees in steps of 1 degrees 
  {                                  // 
     servo4.write(head_pos);              // tell servo 4 (Garuda head) to go to position in variable 'head_pos' 
    
     blinkEyesOn();
     delay (30);
     blinkEyesOff();   
  } 
  for(head_pos = head_pos_left; head_pos >=head_pos_right; head_pos -=1)     // GARUDA SHAKES HIS HEAD RIGHT - servo 4 goes from 45 degrees to 0 degrees in steps of 1 degrees 
  {                                  //  
     servo4.write(head_pos);              // tell servo 4 (Garuda head) to go to position in variable 'head_pos' 
    
     blinkEyesOn();
     delay (30);
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
  
  for(wing_pos = wing_pos_front; wing_pos >= wing_pos_back; wing_pos -=2)     // GARUDA FLAPS HIS WINGS BACKWARD - servos go from 75 degrees to 0 degrees in steps of 2 degrees 
  {                                  // except servo 3 that goes from 0 degrees to 65 degrees 
    servo2.write(wing_pos);              // tell servo 2 (Garuda left wing) to go to position in variable 'wing_pos'
    servo3.write(-(wing_pos - wing_pos_front));        // tell servo 3 (Garuda right wing) to go to position in variable 'wing_pos'
    
     blinkEyesOn();
     delay (50);
     blinkEyesOff();   
   }
 
  Serial.println("Garuda shakes his head"); // tell us when Garuda head shake loop is invoked

   for(head_pos = head_pos_right; head_pos < head_pos_left; head_pos += 1)  // GARUDA SHAKES HIS HEAD LEFT - servo 4 goes from 0 degrees to 45 degrees in steps of 1 degrees 
  {                                  // 
     servo4.write(head_pos);              // tell servo 4 (Garuda head) to go to position in variable 'head_pos' 
    
     blinkEyesOn();
     delay (30);
     blinkEyesOff();   
  } 
  for(head_pos = head_pos_left; head_pos >=head_pos_right; head_pos -=1)     // GARUDA SHAKES HIS HEAD RIGHT - servo 4 goes from 45 degrees to 0 degrees in steps of 1 degrees 
  {                                  //  
     servo4.write(head_pos);              // tell servo 4 (Garuda head) to go to position in variable 'head_pos' 
    
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

Serial.print("New Garuda is done waiting, ready to go out.");


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


// Serial.println("Status:");
// Serial.println(doorstatus);

}
  
} // endcloseDoors()



void blinkEyesOn()
{
  digitalWrite(garuda_eyes, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end blinkEyesOn()


void blinkEyesOff()
{
 digitalWrite(garuda_eyes, LOW);    // turn the LED off by making the voltage LOW
} // end blinkEyesOff()


void blinkFrontLightOn()
{
  digitalWrite(garuda_front_light, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Garudo front light is on");
  
} // end blinkEyesOn()


void blinkFrontLightOff()
{
 digitalWrite(garuda_front_light, LOW);    // turn the LED off by making the voltage LOW
   Serial.println("Garudo front light is off");
} // end blinkEyesOff()

void blinkInsideLightsOn()
{
  digitalWrite(temple_inside_lights, HIGH);   // turn the LED on (HIGH is the voltage level)
} // blinkInsideLightsOn()


void blinkInsideLightsOff()
{
 digitalWrite(temple_inside_lights, LOW);    // turn the LED off by making the voltage LOW
} // end blinkInsideLightsOff()


void blinkTempleLightsOn()
{
  digitalWrite(temple_outside_light_1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(temple_outside_light_2, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end blinkTempleLightsOn()


void blinkTempleLightsOff()
{
 digitalWrite(temple_outside_light_1, LOW);    // turn the LED off by making the voltage LOW
 digitalWrite(temple_outside_light_2, LOW);    // turn the LED off by making the voltage LOW
} // end blinkTempleLightsOff()

/*

Arduino code for Bali Cuckoo Clock, an interactive art box
for the Pataphysical Slot Machine.

About Bali Cuckoo Clock:
http://bit.ly/bali-cuckoo-clock

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Learn more about Pataphysical Studios:
http://pataphysics.us


Last updated on November 15, 2014.
Written by Fabrice Florin, based on Arduino Servo libraries.

This code is licensed freely under CC-BY-SA-3.0.
 
 */
 
 #include <Servo.h> 

Servo servo1;  // create 1st servo object (Garuda moves back and forth)  
Servo servo2;  // create 2nd servo object (Garuda left wing)  
Servo servo3;  // create 3rd servo object (Garuda right wing)   
Servo servo4;  // create 4th servo object (Garuda head)  
Servo servo5;  // create 5th servo object (left door)  
Servo servo6;  // create 6th servo object (right door)   

// a maximum of eight servo objects can be created 


int dir = 0;    // generic variable to store Garuda's direction: forward, backward or stop 
int pos = 0;    // generic variable to store Garuda's wing positions: closed (0) to open (60) 
int needToStopGaruda = 0; // variable to indicate if garuda needs to stop moving back and forth
//int doorstatus = 0;    // generic variable to store door status: closed (0) or open (1) 
//int doors = 0;    // generic variable to store door positions: closed (0) to open (90) 

unsigned long garuda_start;
unsigned long garuda_time;
unsigned long garuda_act_total_duration;

int sensorPinBack = 0; //analog pin 0 for sensing BACK position --reflection on right side of tape (green cable)
int sensorPinFront = 1; //analog pin 1 for sensing FRONT position -- reflection on left side of tape (blue cable)


void setup() 
{ 

  Serial.begin(9600); // enables serial connection for sensors

  servo1.attach(8);  // attaches the servo on pin 8 to the servo object 
  servo2.attach(9);  // attaches the servo on pin 9 to the servo object
  servo3.attach(10);  // attaches the servo on pin 10 to the servo object 
  servo4.attach(11);  // attaches the servo on pin 11 to the servo object 
  servo5.attach(12);  // attaches the servo on pin 12 to the servo object 
  servo6.attach(13);  // attaches the servo on pin 13 to the servo object 

} 

void loop() 
{ 
  
  Serial.println("=== Start loop");

// What we want
// 1. Open doors
// 2. Move forward
// 3. Stop at front
// 4. Garuda performs its act
// 5. Move backwards
// 6. Stop at back
// 7. Close doors

//openDoors();   
 

// Find direction of movement using sensors
findDirectionOfMovement();
    
 // GARUDA MOVES WHERE HE'S TOLD
 // Serial.println("tell us which direction Garuda is expected to be going");
  Serial.println(dir);  // tell us which direction Garuda is expected to be going  
  
  servo1.write(dir);              // tell servo1 to go in the direction it’s been told (forward or backward)
  
  
 //////////// YOU CAN REMOVE THIS PART - START
 // Trying to make it do different things depending on positions
 //  if (needToStopGaruda == 1 && dir == -90)  
 // if the garuda has arrived at the front, stop for garuda act, perform garuda act, then carry on with loop
 //  {  
 //    Serial.println("Garuda has arrived at the front, stop for garuda act, perform garuda act");
 //    servo1.write(0);
 //    needToStopGaruda = 0;
 //    garuda();
 //    servo1.write(dir); // move back
 //    
 //  } else if (needToStopGaruda == 1 && dir == 90)  // if it has arrived at the back, stop and close doors
 //  {  
 //     Serial.println("Garuda  has arrived at the back, stop and close doors");
 //   servo1.write(0);
 //    needToStopGaruda = 0;
 //    closeDoors();
 //   
 //  } else { 
 //     Serial.println("Garuda needs to keep moving");
 //      servo1.write(dir);              // tell servo1 to go in the direction it’s been told (forward or backward)
 //  }
 ////////// YOU CAN REMOVE THIS PART - END
 
 
 
// START GARUDA ACT LOOP -- Garuda does his act (flaps its wings, shakes its head -- and someday, sings advice) 

garuda(); 

// END GARUDA ACT LOOP
    
    
 // moveDoors();
 
  Serial.println("=== End loop");
 
  Serial.println(" "); // add a blank line when a new loop ends

  // return;

  } // end loop
 
       


void findDirectionOfMovement() {    // Find direction of movement using sensors

  
// sensor on left is calle pinBack, sensor on right is called pinFront
// sensor values: < 150 means reflective tape, > 150 means black tape

  int valRight = analogRead(sensorPinBack); // Back = sensor on right tape
  int valLeft = analogRead(sensorPinFront); // Front = sensor on left tape
  
// Serial.println("Find direction of movement");
// Serial.println("Right tape ");
// Serial.println(valRight); // tell us which value the sensor is seeing on the right tape
// Serial.println("Left tape ");
// Serial.println( valLeft); // tell us which value the sensor is seeing on the left tape
 
// Back position: 
  if (( valLeft > 150 ) && (valRight < 150))     // TELL GARUDA TO MOVE FORWARD 
  { 
    // needToStopGaruda = 1; //garuda is at back
     dir = 90;    // change the direction to move FORWARD  
     Serial.println("Garuda is at Back, left tape black > 150, right tape reflective < 150");  // tell us if Garuda has reached the back (the sensor detects the tape's reflective spot on the right)
  //   Serial.println(dir);  // tell us which direction Garuda is expected to be going   
     Serial.println("Move forward");
  }
  // middle position:
  else if ((valLeft > 150) && (valRight > 150) )   // Keep going, whichever direction Garuda is moving
  {
      // needToStopGaruda = 0; //garuda is in middle
  Serial.println("Keep moving: left and right tape both black"); 
     
  }
  // Front position: left tape reflective, right tape black
 else if ((valLeft < 150) && (valRight > 150) )      // TELL GARUDA TO MOVE BACKWARD 
  {
      // needToStopGaruda = 1; //garuda is at front: stop for time of garuda act

    Serial.println("Reached Front: left tape reflective < 150, right tape black > 150");  // tell us if Garuda has reached the front (the sensor detects the tape's reflective spot on the left
    // Serial.println(dir);  // tell us which direction Garuda is expected to be going    
     dir = -90;    // change the direction to move BACKWARD 
     Serial.println("Move Backward");
   
  } else
 {
   Serial.println("Unexpected condition! left and right tape both reflective < 150"); 
 } 
 
 } // end find direction of movement
 


// GARUDA method
// Garuda flaps left wing, flap right wing, move head

void garuda() 

{ 
Serial.println("start garuda"); // tell us when Garuda loop is invoked
    
if (dir = -90) 
{

garuda_start = millis(); // Garuda start time
garuda_time = 0; //  Garuda's time spent since start
garuda_act_total_duration = 500; // total time of the Garuda act // was 5000
 
while (garuda_time <= garuda_act_total_duration)

{
  
  for(pos = 0; pos < 60; pos += 1)  // servos go from 0 degrees to 60 degrees in steps of 2 degrees 
  {                                  // except servo 3 that goes from 60 degrees to 0 degrees 
    servo2.write(pos);              // tell servo 2 (Garuda left wing) to go to position in variable 'pos' 
    servo3.write((60-pos));         // tell servo 3 (Garuda right wing) to go to position "60" minus variable 'pos' 
   delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  
  for(pos = 60; pos >=1; pos -=1)     // servos go from 60 degrees to 0 degrees in steps of 2 degrees 
  {                                  // except servo 3 that goes from 0 degrees to 60 degrees 
    servo2.write(pos);              // tell servo 2 (Garuda left wing) to go to position in variable 'pos'
    servo3.write(-(pos-60));        // tell servo 3 (Garuda right wing) to go to position in variable 'pos'
   delay(15);                       // waits 15ms for the servo to reach the position 
  }
 
  Serial.println("Garuda shakes his head"); // tell us when Garuda head shake loop is invoked

   for(pos = 0; pos < 45; pos += 1)  // servo 3 goes from 0 degrees to 45 degrees in steps of 1 degrees 
  {                                  // 
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 
   delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 45; pos >=1; pos -=1)     // servo 4 goes from 45 degrees to 0 degrees in steps of 1 degrees 
  {                                  //  
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }

  garuda_time = millis() - garuda_start; //  Garuda's time spent since start
      
Serial.println("end garuda"); // tell us when Garuda loop ends
Serial.println(garuda_time); // tell how much time Garuda has used so far

} //end while

  //Serial.println(garuda_time); // tell how much time Garuda has used so far


garuda_time = 0; // reset timer to 0

Serial.println("Garuda stopped"); // tell us Garuda has stopped
     
 } else
 {
   Serial.println("Don't move Garuda"); 
 } 
     
return;


} // end garuda()


// void openDoors() {
  
  
// } // end openDoors()



// void closeDoors() {
  
// } // endcloseDoors()



// moveDoors method 
// Claude and Benedicte suggest two separate methods: openDoors and closeDoors
// void moveDoors() {

// if (doorstatus == 1)        // CLOSE DOOR LOOP
//  {
//  for(doors = 90; doors >=1; doors -=2)     // servos go from 90 degrees to 0 degrees in steps of 2 degrees 
//  {                                  // except servo 6 that goes from 0 degrees to 90 degrees 
//    servo5.write(doors);              // tell servo 5 (left door) to go to position in variable 'doors'
//    servo6.write(-(doors-90));        // tell servo 6 (right door) to go to position in variable 'doors'
//   delay(15);                       // waits 15ms for the servo to reach the position 
//  }
  
//  Serial.println("close door"); // tell us what time it is
//  Serial.println("The time is: " + millis()); // tell us what time it is
//  Serial.println(" "); // add a blank line
//  delay(1000); // wait a second, before doing the next thing
  
//   doorstatus = 0;

//}

#include <Arduino.h>

#include <HCSR04.h> // I am using the library at https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib

//Right Engine Pins
const int rightEnable = 10;
const int rightA = 9;
const int rightB = 8;

int distanceRight = 0;

//Left Engine Pins
const int leftEnable = 11;
const int leftA = 12;
const int leftB = 13;

int distanceLeft = 0;

//Right Sensor Pins //IMPORTANT NOTE: I messed up with left-right while connecting the HCSR04's so these should be inverted
const int rightTrigger = 3;
const int rightEcho = 4;
HCSR04 hcRight(rightTrigger, rightEcho); //Initialise the sensor as an object (with the help of the library)

//Left Sensor Pins
const int leftTrigger = 6;
const int leftEcho = 7;
HCSR04 hcLeft(leftTrigger, leftEcho);

// These are values to be tweaked to optimize cruise
const int defaultSpeed = 900;   // Default speed of the vehicle, could be changed while working
const int crashDistance = 16;   // After this distance, the ship will back off
const int turnDistance = 50;    //  After this distance, the ship will start to slow down related engines
const int deAccMultiplier = 80; // The value to be used for calculations while slowing down

void Onwards(int speed)
{                             // Sets up for forwards motion with given speed
  digitalWrite(rightA, HIGH); // a high A value could mean the motors are set to go forwards
  digitalWrite(rightB, LOW);

  digitalWrite(leftA, HIGH);
  digitalWrite(leftB, LOW);

  //analogWrite(leftEnable, speed);
  //analogWrite(rightEnable, speed);
  digitalWrite(leftEnable, HIGH);
  digitalWrite(rightEnable, HIGH);
}

void haltTheEngines()
{ // A method to stop the engines if needs be
  digitalWrite(leftEnable, LOW);
  digitalWrite(rightEnable, LOW);
  Serial.println("We are stopping");
}

void backOff()
{
  //Backs off
  Serial.println("CAPTAAAAIN");
  while (hcLeft.dist() < 30)
  { //Left motor stops, right motor backwards
    digitalWrite(leftEnable, LOW);

    digitalWrite(rightA, LOW);
    digitalWrite(rightB, HIGH);
  }
}

void slowLeft()
{
  //Slows down the left motor if there is an obstacle on the right
  long leftEngineValue = distanceLeft * defaultSpeed / deAccMultiplier;
  analogWrite(leftEnable, leftEngineValue);
  Serial.print("Left engine is now at ");
  Serial.println(leftEngineValue);
}

void slowRight()
{
  //Slows down the right motor if there is an obstacle on the left
  long rightEngineValue = defaultSpeed * distanceLeft / deAccMultiplier;
  analogWrite(rightEnable, rightEngineValue);
  Serial.print("Right engine is now at ");
  Serial.println(rightEngineValue);
}

void setup()
{

  Serial.begin(9600);

  //Set the trigger and echo pins of the ultrasonic sensors
  pinMode(rightTrigger, OUTPUT);
  pinMode(leftTrigger, OUTPUT);

  pinMode(rightEcho, INPUT);
  pinMode(leftEcho, INPUT);

  for (int i = 8; i < 14; i++) // Set the Engine pins as output
  {
    pinMode(i, OUTPUT);
  }

  //Activate this function to be able to input default speed value from serial
  /*  
  while (Serial.available() == 0)
  {
    // Do nothing till input
  }

  defaultSpeed = Serial.parseInt();
  Serial.println(defaultSpeed);
 */

  Onwards(defaultSpeed); // Starts the motion forwards with default speed
}

void loop()
{
  distanceRight = hcRight.dist();
  Serial.print("The right sensor reads ");
  Serial.println(distanceRight);

  distanceLeft = hcLeft.dist();
  Serial.print("The left sensor reads ");
  Serial.println(distanceLeft);

  if (distanceLeft > 1 && distanceRight > 1)
  { // Sometimes the sensors read 0 by fault, which can cause some problems with the engines

    if (distanceLeft < crashDistance && distanceRight < crashDistance)
    { //Back of if we are about to crash
      backOff();
    }
    else if (distanceLeft < turnDistance) // These got inverted because I confused right and left with the Ultrasonic sensors
    {
      //slow left engine
      Serial.println("Obstacle on the right");
      Serial.println("Slowing down the left engine");
      slowLeft();
    }
    else if (distanceRight < turnDistance)
    {
      //slow right engine
      Serial.println("Obstacle on the left");
      Serial.println("Slowing down the right engine");
      slowRight();
    }
    else
    {
      Onwards(defaultSpeed);
    }
  }

  delay(1000); //Rest a bit
}
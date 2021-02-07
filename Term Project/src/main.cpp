#include <Arduino.h>

#include <HCSR04.h> // I am using the library at https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib

//Right Engine Pins
const int rightEnable = 10;
const int rightA = 9;
const int rightB = 8;

int speedValueRight = 0;

//Left Engine Pins
const int leftEnable = 11;
const int leftA = 12;
const int leftB = 13;

int speedValueLeft = 0;

//Right Sensor Pins
const int rightTrigger = 3;
const int rightEcho = 4;
HCSR04 hcRight(rightTrigger, rightEcho); //Initialise the sensor as an object (with the help of the library)

//Left Sensor Pins
const int leftTrigger = 6;
const int leftEcho = 7;
HCSR04 hcLeft(leftTrigger, leftEcho);

int speedValue = 300; // Default speed of the vehicle

const int crashDistance = 20; // These are values to be tweaked to optimize drive
const int turnDistance = 30;

void Onwards()
{ // Sets up for forwards motion
  digitalWrite(rightA, HIGH);
  digitalWrite(rightB, LOW);

  digitalWrite(leftA, HIGH);
  digitalWrite(leftB, LOW);
}

void backOff()
{
  //Backs off
  while (hcLeft.dist() < 20)
  { //Left motor stops, right motor backwards
  }
}

void slowLeft()
{
  //Slows down the left motor
}

void slowRight()
{
  //Slows down the right motor
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

  /*   while (Serial.available() == 0)
  {
    // Do nothing till input
  }

  speedValue = Serial.parseInt();
 */
  analogWrite(leftEnable, speedValue);
  analogWrite(rightEnable, speedValue);

  // Serial.println(speedValue);

  Onwards();
}

void loop()
{
  speedValueRight = hcRight.dist();
  Serial.print("The right sensor reads ");
  Serial.println(speedValueRight);

  speedValueLeft = hcLeft.dist();
  Serial.print("The left sensor reads ");
  Serial.println(speedValueLeft);

  if (speedValueLeft < 10 && speedValueRight < 10)
  { //Back of if we are about to crash
    //backOff();
    digitalWrite(leftEnable, LOW);
    digitalWrite(rightEnable, LOW);
    Serial.println("We are stopping");
  }
  else if (speedValueLeft < 30)
  {
    //slow right engine
    //slowRight();
    long rightEngineValue = speedValue * speedValueLeft / 100;
    analogWrite(rightEnable, rightEngineValue);
    Serial.print("Right engine is now at ");
    Serial.println(rightEngineValue);
  }
  else if (speedValueRight < 30)
  {
    //slow left engine
    //slowLeft();
    long leftEngineValue = speedValueLeft * speedValue / 100;
    analogWrite(leftEnable, speedValue * speedValueRight / 100);
    Serial.print("Left engine is now at ");
    Serial.println(leftEngineValue);
  }
  else
  {
    analogWrite(leftEnable, speedValue);
    analogWrite(rightEnable, speedValue);
  }

  delay(500);
}
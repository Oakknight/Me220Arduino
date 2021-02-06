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

int speedValue = 0;

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

  digitalWrite(rightA, HIGH);
  digitalWrite(rightB, LOW);

  digitalWrite(leftA, HIGH);
  digitalWrite(leftB, LOW);

  while (Serial.available() == 0)
  {
    // Do nothing till input
  }

  speedValue = Serial.parseInt();

  analogWrite(10, speedValue);
  analogWrite(11, speedValue);

  Serial.println(speedValue);
}

void loop()
{
  Serial.print("The right sensor reads ");
  Serial.println(hcRight.dist());

  Serial.print("The left sensor reads ");
  Serial.println(hcLeft.dist());

  delay(500);
}
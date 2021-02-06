#include <Arduino.h>

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

//Left Sensor Pins
const int leftTrigger = 6;
const int leftEcho = 7;

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

  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);

  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
}

void loop()
{

  while (Serial.available() == 0)
  {
  }

  speedValue = Serial.parseInt();

  analogWrite(10, speedValue);
  analogWrite(11, speedValue);

  Serial.println(speedValue);
}
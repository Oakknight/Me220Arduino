#include <Arduino.h>

#define whiteBtn 2
#define blueBtn 3
#define analogPin A5

//Right Engine Pins
bool digital8 = false;
bool digital9 = false;
bool digital10 = false;
const int rightEnable = 11;

//Left Engine Pins
bool digital2 = false;
bool digital3 = true;
const int leftEnable = 11;

//Right Sensor Pins
const int rightTrigger = 3;
const int rightEcho = 4;

//Left Sensor Pins
const int leftTrigger = 6;
const int leftEcho = 7;

int speedValue = 0;

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  // attachInterrupt(digitalPinToInterrupt(2), NotifyButtonWhite, RISING);
  // attachInterrupt(digitalPinToInterrupt(3), NotifyButtonBlue, RISING);

  pinMode(whiteBtn, INPUT);
  pinMode(blueBtn, INPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

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

  speedValue = Serial.parseInt(); //va

  analogWrite(10, speedValue);
  analogWrite(11, speedValue);

  Serial.println(speedValue);

  /*/
  speedValue = analogRead(analogPin);
  Serial.println(speedValue);

  if (digitalRead(whiteBtn))
  {
    analogWrite(10, speedValue/4);
  }
  else
  {
    digitalWrite(10, LOW);
  }

  // speedValueue of the digital 10 pin is always same with digital 2
  if (!digitalRead(blueBtn))
    digitalWrite(8, HIGH);
  else
    digitalWrite(8, LOW);
  if (digitalRead(blueBtn) && digitalRead(whiteBtn))
    digitalWrite(9, HIGH);
  else
    digitalWrite(9, LOW);
    //*/
}
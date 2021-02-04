#include <Arduino.h>

#define qrdPin A0

void setup()
{
  Serial.begin(9600);
  Serial.println("READY");
  pinMode(qrdPin, INPUT);
}

void loop()
{
  Serial.println(analogRead(qrdPin));
  delay(20);
}
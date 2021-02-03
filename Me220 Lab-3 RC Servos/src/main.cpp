#include <Arduino.h>

#define rcServo 9

long waitVar = 1500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready");
}

void loop() {
  // put your main code here, to run repeatedly:

 // waitVar = Serial.parseInt();

  digitalWrite(rcServo,HIGH);

  delayMicroseconds(1000);

  digitalWrite(rcServo, LOW);


}
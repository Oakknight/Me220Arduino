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

/*
 Using a normal piece of paper, the value in the serial monitor decreases as we bring the paper closer to the sensor

 Using the paper painted with the transition pattern, we observe that the values are much larger when around the black parts
 Since we kow that the sensor works by measuring how much of the infrared is reflected, we can deduce that darker colors will give
 us a larger value, due to the fact that they absorb more light. When we used a more reflective or white object, we observed that the
 value is much smaller. This is because more light is reflected from the object to the phototransistor. 

 When we change the value of the POT, we adjust the sensitivity of the sensor in some sense.


/*/
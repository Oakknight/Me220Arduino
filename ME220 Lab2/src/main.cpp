#include <Arduino.h>

//pins for the players' buttons
#define playerOne 3 //blue button
#define playerTwo 2 //red button

#define runDelay 400     //delay in ms before light moves to the next point, Game speed property
#define numPins 8        //number of LEDS
#define debounceDelay 20 // will be used for debouncing

const int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11};  //Ledpins array

void FirstGameLoop()
{
  while (true)
  {

    
  }
}




void Diagnostics()
{ // A function to see if all leds have been connected correctly
  Serial.println("Running Diagnostics");
  Serial.println("Checking Led Pins");
  for (int i = 0; i < numPins; i++)
  {
    digitalWrite(ledPins[i], HIGH);
    Serial.println("Led ");
    Serial.print(i);
    Serial.print(" should now be HIGH at pin ");
    Serial.print(ledPins[i]);
    delay(1000);
  }
  for (int i = 7; i > -1; i--)
  {
    digitalWrite(ledPins[i], LOW);
    Serial.println("Led ");
    Serial.print(i);
    Serial.print(" should now be LOW at pin ");
    Serial.print(ledPins[i]);
    delay(1000);
  }
  Serial.println(" Diagnostics complete ");
}

void setup()
{

  Serial.begin(9600);

  for (int i = 0; i < numPins; i++) //Setting up the pins for the LEDS
    pinMode(ledPins[i], OUTPUT);

  pinMode(playerOne, INPUT); //Setting up input pin for P1
  pinMode(playerOne, INPUT); //Setting up input pin for P2

  //Diagnostics();// Run this function to diagnose your circuit

  
}

void loop()
{
  // put your main code here, to run repeatedly:
  FirstGameLoop();
}

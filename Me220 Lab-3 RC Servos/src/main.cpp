
/*
// I wrote this program for lab on top of the sweep example 
//While doing the first part (2.a) at first it worked, then my rc servo stopped working and I don't know why. but I wanted to do the other parts before I break all of my servos.

Here is the code I used for the first part

void serialEvent() //I used serialEvent to update when a new value is entered. And it actually worked for some time.
{
  while (Serial.available())
  {
    // get the new integer:
    int inWait = Serial.parseInt(); //This holds the new value we enter
    // set it to our wait time:
    highTime = inWait;
  }
} 

void setup()
{
  Serial.begin(9600);
  Serial.println("Ready");
}

void loop()
{

  serialEvent();

  digitalWrite(rcServo, HIGH);

  delayMicroseconds(highTime);

  digitalWrite(rcServo, LOW);

  //delayMicroseconds(lowTime - HighTime);
}
*/

//Now onwards to the other parts of this lab
#include <Arduino.h>
#include <Servo.h>

#define servoPin 9
#define potPin A5

Servo myservo; // create servo object to control the servo

unsigned long pos = 150;  // variable to store the servo position
unsigned long newPos = 0; //This holds the new value we enter
unsigned long potPos = 0;

String commProtocol = "";
bool isValid = false;

void setup()
{
  Serial.begin(9600);
  myservo.attach(servoPin); // attaches the servo on pin 9 to the servo object
}

void serialEvent()
{
  newPos = Serial.parseInt(); //Store the input

  if (newPos > 0) // If it is bigger than 0 and less than 180, we set it as the new position.
  {
    Serial.println(newPos);
    if (newPos <= 180)
    {
      //set pos to new value if it is valid
      pos = newPos;
      Serial.println(pos);
    }
    else if (newPos > 180)
    {
      pos = 0;
      Serial.println("wrong input"); // Inputs bigger than 180 are accepted, but not processed.
    }
  }
  myservo.write(pos);
}

void ConvertPotToServo(int potValue)
{ // This function will be used to convert the value we get from the POT to a degree between 0-180

  Serial.println(potValue);
  potPos = potValue * 10; //When I multiplied it with 100 instead of 10, very weird stuff happened. It might have been overflown, but I couldn't grasp it
  potPos = potPos * 10;
  pos = potPos / 568;
  Serial.println(pos);
  myservo.write(pos);
}

void SetAndWaitServo(int position, int waitTime) // First argument is position, second is time
{                                                //This function sets the servo to the position we want for the time we want.
  for (size_t i = 0; i < waitTime; i++)
  {
    myservo.write(position);
    delay(1);
  }
}

void GetCommProtocol()
{
  Serial.println("Please enter input for the movement pattern");
  while (Serial.available() == 0)
  {
    //Wait till we enter an input
  }
  commProtocol = Serial.readString();
  Serial.println("You have entered: " + commProtocol);
  if (commProtocol.charAt(0) == 's' && commProtocol.charAt(commProtocol.length() - 1 == 'e'))
  { // Check if the string starts with s and ends with an e, to be accepted by our protocol
    Serial.println("Processing");
    isValid = true;
  }
  else
  {
    Serial.println("Your input is invalid");
    isValid = false;
  }

  if (isValid)
  { //only process the string if it is in valid format
    String waitFor = "";
    String waitAt = "";
    for (int i = 1; i < (int)commProtocol.length() - 1; i++)
    {
      while (i < commProtocol.indexOf(':')) //this is the value for the first position
      {
        waitAt += commProtocol.charAt(i); // Adds every charachter to our string till we see the ':'
        i++;
      }
      i++;

      while (i < commProtocol.indexOf(','))
      {
        waitFor += commProtocol.charAt(i);
        i++;
      }
    }
    Serial.println("We will wait at: " + waitAt + " for " + waitFor);
    //Serial.println(waitAt);
    SetAndWaitServo(waitAt.toInt(), waitFor.toInt());
  }

  //Now that we got our string, we need to process it
}

// I will be using values [0,180] instead of [-90,90], since it is easier to work with all positive integers

void loop()
{

  //ConvertPotToServo(analogRead(potPin)); //Used to move the servo using the POT

  GetCommProtocol();

  //serialEvent(); //This is used to move the servo using position inputs between 0-180
}

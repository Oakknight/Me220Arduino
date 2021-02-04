
/*
// I wrote this program for lab on top of the sweep example 
//While doing the first part (2.a) at first it worked, then my rc servo stopped working and I don't know why. but I wanted to do the other parts before I break all of my servos.

Here is the code I used for the first part

void SerialEvent() //I used serialEvent to update when a new value is entered. And it actually worked for some time.
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

String commProtocol = ""; // Our input for complex motion patters goes here
bool isValid = false;

int modeChosen = 0;

// I will be using values [0,180] instead of [-90,90], since it is easier to work with all positive integers

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

// We will need a function to count the position-wait pairs in our string
// We will do this by counting the number of ',' charachters and adding one
int HowManyPairs(String inputString)
{
  int count = 1;
  for (size_t i = 0; i < inputString.length(); i++)
  {
    if (inputString.charAt(i) == ',')
      count++;
  }
  return count;
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
    int pairAmount = HowManyPairs(commProtocol);
    String waitFor = "";
    String waitAt = "";
    for (int i = 1; i < (int)commProtocol.length() - 1; i++)
    { // This for loop will go back to the begginning only when it is finished a pair since we also increment the "i" inside the while loops

      while (i < commProtocol.indexOf(':')) //this is the value for the first position
      {
        waitAt += commProtocol.charAt(i); // Adds every character after the between the 's' and ':' to the waitAt String
        i++;
      }

      i++; // We increment i one more time to account for ':'

      for (size_t j = 0; j < i; j++) //Cleaning up the string
      {
        commProtocol.setCharAt(j, 'a');
      }

      while (i < commProtocol.indexOf('e'))
      {
        if (commProtocol.charAt(i) == ',')
          break;
        waitFor += commProtocol.charAt(i);
        i++;
      }

      // After we have processed a portion of the string, we want to remove that portion
      // So that we can move on to the next pair
      for (size_t j = 0; j < i; j++) //Clean up the string everytime we process a portion
      {
        commProtocol.setCharAt(j, 'a');
      }
      Serial.println("We will wait at: " + waitAt + " for " + waitFor);
      SetAndWaitServo(waitAt.toInt(), waitFor.toInt());
      // After we use the waitAt and waitFor values, we want to reset them in order to prepare for the next pair
      waitAt = "";
      waitFor = "";
    }

    Serial.println(pairAmount);
  }

  //Now that we got our string, we need to process it
}
// Some pattern strings to test for
// s100:1500,60:3000e
// s100:1500,60:3000,150:2000,15:600e
// s100:1500,60:3000,150:2000,15:600,40:1200,90:1500e
// s100:1500,60:3000,150:2000,15:600,40:1200,90:1500,100:1500,60:3000,150:2000,15:600,26:3400,16:400,90:500,170:500,000,150:2000,15:600,26:3400,16:400,90:500,170:500e

void loop()
{
  if (modeChosen == 0)
  {
    Serial.println(" What would you like to do?");
    Serial.println("Enter 1, to control the servo using POT");
    Serial.println("Enter 2, to control the servo using the communication protocol");
    Serial.println("Enter 3, to control the servo using raw position inputs");

    while (Serial.available() == 0)
    {
      // Wait till input
    }
    modeChosen = Serial.parseInt();
  }

  switch (modeChosen)
  {
  case 1:
    ConvertPotToServo(analogRead(potPin));
    break;
  case 2:
    GetCommProtocol();
    break;
  case 3:
    serialEvent();
    break;

  default:
    Serial.println("wrong input, please reset");
    break;
  }

  //ConvertPotToServo(analogRead(potPin)); //Used to move the servo using the POT

  //GetCommProtocol(); // Used to give complex motion patterns to the servo using a protocol

  //serialEvent(); //This is used to move the servo using position inputs between 0-180
}

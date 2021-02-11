#include <Arduino.h>

/*/WARNING
I have salvaged some of the code and electronics for this lab from my project

The Functions we need to provide:
- Apply a certain DC value to a specified motor
- Apply different DC values to each motor
- Stop an individual motor
- Stop all motors
- Change direction of a motor
- Change direction of both motors

So basically, we have three main actions: Stop, Change direction, and apply DC value.

Also, we have three identifiers for each action: Led Motor, DC Motor or All Motors.And an extra duty cycle value if we want
to change it. 
/
The first char will specify our action and will be stored in the actionChar variable:
'A': To apply a certain DC Value,
'C': To change the direction,
'S': To stop the motor which will be specified in the next char
The second charachter will specify our target:
'A': For all motors,
'L': For the LED Motor,
'D': For the DC Motor.

And if the user provides an integer value at the end, that will be our dc value.

Therefore, we need three functions for the actions, and three variables to process the input

Example Inputs:
AA500, will apply a pwm value of 500 to all motors
AD700, will apply a pwm value of 700 to the DC mOtor

SL, will stop the LED Motor
SA, will stop all motors
SD, will stop the dc motor

CD, will change the direction of the DC Motor

and so on...


Side Note: I believe I might have somehow burned a motor or my driver during the demonstration, but the code is working.


//*/

//DC Motor Pins
const int dcEnable = 10; // How fast will our motor turn? Not to be mistaken with duty cycle
const int dcA = 9;
const int dcB = 8;
byte dcDirection = 0;

//Led Motor Pins
const int ledEnable = 11; //How bright will our LED be
const int ledA = 12;
const int ledB = 13;
byte ledDirection = 0; // This variable will keep the direction of turning, 1 for when A is high and 2 for when B is HIGH

//The variables we will use to process the input
String protocolInput = ""; //This will store the input from the user
//char actionChar;           // This will store our first charachter.
char targetChar;     // This will store the target of our actions
String dcValue = ""; // This will store our dc value if we have one

void changeDirection()
{ //This function will change the direction of our motors
  Serial.println("We are changing direction now");

  if (targetChar == 'L')
  {
    //For the LED Motor
    if (ledDirection == 1)
    {
      digitalWrite(ledA, LOW);
      digitalWrite(ledB, HIGH);
      ledDirection = 2;
    }
    else if (ledDirection == 2)
    {
      digitalWrite(ledA, HIGH);
      digitalWrite(ledB, LOW);
      ledDirection = 1;
    }
  }
  else if (targetChar == 'D')
  {
    //For DC Motor
    if (dcDirection == 1)
    {
      digitalWrite(dcA, LOW);
      digitalWrite(dcB, HIGH);
      dcDirection = 2;
    }
    else if (dcDirection == 2)
    {
      digitalWrite(dcA, HIGH);
      digitalWrite(dcB, LOW);
      dcDirection = 1;
    }
  }
  else if (targetChar == 'A')
  { //For all motors
    if (dcDirection == 1)
    {
      digitalWrite(dcA, LOW);
      digitalWrite(dcB, HIGH);
      dcDirection = 2;
    }
    else if (dcDirection == 2)
    {
      digitalWrite(dcA, HIGH);
      digitalWrite(dcB, LOW);
      dcDirection = 1;
    }
    if (ledDirection == 1)
    {
      digitalWrite(ledA, LOW);
      digitalWrite(ledB, HIGH);
      ledDirection = 2;
    }
    else if (ledDirection == 2)
    {
      digitalWrite(ledA, HIGH);
      digitalWrite(ledB, LOW);
      ledDirection = 1;
    }
  }
}

void stopMotor()
{
  //This will stop the specified motor(s)

  if (targetChar == 'A')
  {
    Serial.println("Stopping all motors");
    digitalWrite(ledEnable, LOW);
    digitalWrite(dcEnable, LOW);
  }
  else if (targetChar == 'L')
  {
    //Stop Led Motor
    digitalWrite(ledEnable, LOW);
    Serial.println("Stopping LED motor");
  }
  else if (targetChar == 'D')
  {
    //Stop DC Motor
    digitalWrite(dcEnable, LOW);
    Serial.println("Stopping dc motor");
  }
}

void applydc()
{
  Serial.println("Applying duty cycle value");
  if (targetChar == 'A')
  {
    analogWrite(ledEnable, dcValue.toInt());
    analogWrite(dcEnable, dcValue.toInt());
  }
  else if (targetChar == 'L')
  {
    analogWrite(ledEnable, dcValue.toInt());
  }
  else if (targetChar == 'D')
  {
    analogWrite(dcEnable, dcValue.toInt());
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 8; i < 14; i++) // Set the pins with a loop
  {
    pinMode(i, OUTPUT);
  }
  dcDirection = 1;
  ledDirection = 1;
  digitalWrite(ledA, HIGH);
  digitalWrite(ledB, LOW);

  digitalWrite(dcA, HIGH);
  digitalWrite(dcB, LOW);

  //digitalWrite(ledEnable, HIGH);
  //digitalWrite(dcEnable, HIGH);
}

void loop()
{
  Serial.println("Please enter data");
  while (Serial.available() == 0)
  {
    // Do nothing till input
  }

  protocolInput = Serial.readString();  // Read Input
  targetChar = protocolInput.charAt(1); // Store our target
  Serial.print("Our target is: ");
  Serial.println(targetChar);
  Serial.print("The operation is: ");
  Serial.println(protocolInput.charAt(0));

  if (protocolInput.charAt(0) == 'C')
  {
    changeDirection();
  }
  if (protocolInput.charAt(0) == 'S')
  {
    //Stop the motor specified
    stopMotor();
  }
  if (protocolInput.charAt(0) == 'A')
  {
    //Parse the integer value for dc
    dcValue = "";
    for (size_t i = 2; i < protocolInput.length(); i++)
    { // This for loop will get us the integer value at the end
      dcValue += protocolInput.charAt(i);
      Serial.println(dcValue);
    }
    applydc();
  }
}

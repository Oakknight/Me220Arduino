
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

  // highTime = Serial.parseInt(); //This approach didn't work, therefore I switched to the serialEvent function

  digitalWrite(rcServo, HIGH);

  delayMicroseconds(highTime);

  digitalWrite(rcServo, LOW);

  //delayMicroseconds(lowTime - HighTime);
}
*/

//Now onwards to the other parts of this lab

#include <Servo.h>

#define servoPin 9
#define potPin A5

Servo myservo;  // create servo object to control the servo

unsigned long pos = 150;    // variable to store the servo position
unsigned long newPos = 0; //This holds the new value we enter
unsigned long potPos = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  
}

void serialEvent()
{ 
  newPos = Serial.parseInt(); //Store the input

  if (newPos > 0) // If it is bigger than 0 and less than 180, we set it as the new position.
  {
    Serial.println(newPos);
    if(newPos <= 180){
       //set pos to new value if it is valid
       pos = newPos;
       Serial.println(pos);
    }else if(newPos > 180 ){
      pos = 0;
      Serial.println("wrong input"); // Inputs bigger than 180 are accepted, but not processed.
    }    
    
  }
}

void convertPotToServo(int potValue){ // This function will be used to convert the value we get from the POT to a degree between 0-180

  Serial.println(potValue);
  potPos = potValue * 10; //When I multiplied it with 100 instead of 10, very weird stuff happened. It might have been overflown, but I couldn't grasp it
  potPos = potPos * 10;
  pos = potPos / 568;
  Serial.println(pos);
  
  
}

// I will be using values [0,180] instead of [-90,90], since it is easier to work with all positive integers

void loop() {

// Using this line in combination with the serialEvent function enables us to move the servo by entering values between 0-180
//Or we can use it with the Potentiometer.

convertPotToServo(analogRead(potPin));
myservo.write(pos);






  /*
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  /*/
  //serialEvent(); //This is used to move the servo using position inputs between 0-180
}

#include <Arduino.h>

#define whiteBtn 2
#define blueBtn 3

bool digital8 = false;
bool digital9 = false;
bool digital10 = false;

bool digital2 = false;
bool digital3 = true;

void NotifyButtonWhite()
{
  Serial.println("You have pressed the white button (pin 2) ");
  //digitalWrite()
}

void NotifyButtonBlue()
{
  Serial.println("You have pressed the blue button (pin 3) ");
  //digital
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), NotifyButtonWhite, RISING);
  attachInterrupt(digitalPinToInterrupt(3), NotifyButtonBlue, RISING);

  pinMode(whiteBtn, INPUT);
  pinMode(blueBtn, INPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop()
{
  digitalWrite(10, digitalRead(whiteBtn));
  // Value of the digital 10 pin is always same with digital 2
  if (!digitalRead(blueBtn))
    digitalWrite(8, HIGH);
  else
    digitalWrite(8, LOW);
  if(digitalRead(blueBtn) && digitalRead(whiteBtn))
    digitalWrite(9,HIGH);
  else
    digitalWrite(9,LOW);
}

//This is the code for part 1 of the lab, Used for the LED Motor
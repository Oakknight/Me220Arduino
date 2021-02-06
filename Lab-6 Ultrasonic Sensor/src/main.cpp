#include <Arduino.h>
#include <Servo.h>

const int pingPin = 12; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 11; // Echo Pin of Ultrasonic Sensor
const int servoPin = 3; // Servo Pin
long duration;          // duration of signal

unsigned int max = 2500;
unsigned int min = 500;

Servo myservo;

void moveServo(long distance)
{                          /*
  Y: This function will take the value coming from the ultrasonic sensor and convert it to a position output for the servo
  It does this by using a normal linear equation formula AX + B = Y
  when the formula is simplified, it comes down to (180*X - 180*min)/difference = y
  The y value is the angle we want 
  Also, the ideal min and max values of the ultrasonic sensor is 20 and 4000 mm respectively, but we do not always get those
  values. If we use them by default, we might limit the operation range of our servo.
  Therefore, I decided to allow it to "Self-calibrate", by adjusting its max-min values, if needs be.
  /*/
  unsigned int difference; // Will be used for linear mapping of values

  unsigned int servoPosition = 0; // We will set the servo to this position

  if (distance > max)
  {
    max = distance;
  }
  if (distance < min)
  {
    min = distance;
  }

  difference = max - min;
  servoPosition = ((distance - min) * 180) / difference;
  Serial.print("Servo pos is: ");
  Serial.println(servoPosition);
  myservo.write(servoPosition);
}

long readUltraSonic()
{
  /*
Reads the ultrasonic sensor
This is a standart function where
a ping signal is sent through trigger pin
and an echo is expected in return
*/
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  // listen for echo and return it, and use a time out since
  // sensor has a range of 4 meters, why is there a 25000
  // pleare read the help for pulseIn
  return pulseIn(echoPin, HIGH, 25000); //Y:25000 is the value of timeout,the function returns 0 if no pulse received within the timeout
}
long microseconds2Millimeters(long microseconds)
{
  /*
convert microsecond to centimeters:
speed of sound is 343 m / s * 1000 mm / 1 m
speed of sound is 343 * 1000 mm / s * 1 s / 1000000 us
speed of sound is 0.343 mm / us
however, use of floating point is not a good idea here
best long integer approximation for us to mm is:
note that sound travels back and forth
hence distance to the obstacle shoould be half the distance
that the sound wave traveled... note this fact.
to get a proper result update the following return expression
so that returned value is the best integer approxiamtion of
the sensed distance
NOTE that you have to fix this so that the mm values returned
matches the cm values displayed when you click on the sensor
and move the target circle around
*/

  //Y: Here is the microsecond to mm conversion
  long dist = microseconds * 343;
  dist = dist / 2000;

  return dist;
}

void setup()
{
  // setup pin directions
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600); // Starting Serial Terminal
  myservo.attach(servoPin);
}

void loop()
{
  duration = readUltraSonic();
  Serial.print("Total flight time: ");
  Serial.print(duration);
  Serial.print("us, total distance: ");
  Serial.print(microseconds2Millimeters(duration));
  Serial.print("mm");
  Serial.println();
  moveServo(microseconds2Millimeters(duration));
  delay(250); // rest for a while
}
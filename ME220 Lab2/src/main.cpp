#include <Arduino.h>

//pins for the players' buttons
#define playerOne 3 //blue button
#define playerTwo 2 //red button

#define runDelay 400     //delay in ms before light moves to the next point, Game speed property
#define numPins 8        //number of LEDS
#define debounceDelay 20 // will be used for debouncing

const int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11}; //Ledpins array
volatile int activeShot = 0;
// This variable will hold record for the active bullets. It will always be between [0-3].
// If player one has a bullet it will be 1, if player two has a bullet it will be 2. If both players have bullets, it will be three.
//This way, we aim to identify double-shooting, shooting while there is still an active bullet.
//Each player will have their own shooting and double-shooting functions,
//and they will be active with respect to the value of the activeShot variable

int stunTimeOne = 0; //These variables will hold the remaining stun time in frames if a player tries to shoot again and gets punished.
int stunTimeTwo = 0; //They will be set to 5 at the punish function, then will be decreased by one each frame, if they are bigger than zero

void FirstGameLoop() // This will be the loop where our first game will run
{
  while (true)
  {
    //The game will consist of "frames" which have a period of one LED movement between them
    //  runDelay will be this time and it will specify how fast the game will run

    {                      // See if the player can shoot
      if (activeShot == 0) // If there are no bullets, we will attach two interrupts that allows both players to shoot
      {
        attachInterrupt(digitalPinToInterrupt(playerOne), PlayerOneShoot, RISING);
        attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
      }
      if (activeShot == 1)
      {
        attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
      }
      if (activeShot == 2)
      {
        attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerOneShoot, RISING);
      }
      //This section could possibly be simplified to use less lines of code, but I am focusing on the function for now
    }

    // If a player is stunned, their stun timer decreases by one in each frame
    { //Check Stun
      if (stunTimeTwo > 0)
        stunTimeTwo--;
      if (stunTimeOne > 0)
        stunTimeOne--;
    }

    delay(runDelay); //The period of a frame
  }
}

void PlayerOneShoot()
{                       // When the first player shoots, we record the bullet and then detach their interrupt to shoot
  if (stunTimeOne == 0) // We don't want the player to shoot if they are already stunned
  {
    detachInterrupt(digitalPinToInterrupt(playerOne));
    activeShot += 1;
    //We then attach the new interrupt to punish the player if they try to shoot again
    attachInterrupt(digitalPinToInterrupt(playerOne), PunishPlayerOne, RISING);
  }
}

void PlayerTwoShoot()
{ // When the second player shoots, we record the bullet and then detach their interrupt to shoot
  if (stunTimeTwo == 0)
  {
    detachInterrupt(digitalPinToInterrupt(playerTwo));
    activeShot += 2;
    //We then attach the new interrupt to punish the player if they try to shoot again
    attachInterrupt(digitalPinToInterrupt(playerTwo), PunishPlayerTwo, RISING);
  }
  //Nothing happens if the player is stunned
}

void PunishPlayerOne()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.
  detachInterrupt(digitalPinToInterrupt(playerOne));
  stunTimeOne = 5;
}

void PunishPlayerTwo()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.
  detachInterrupt(digitalPinToInterrupt(playerTwo));
  stunTimeTwo = 5;
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

  //We will ask for input regarding the game mode, then execute that mode on the loop

  //TODO: Add selection
}

void loop()
{
  // First or second game will be run according to the information coming from the setup function
  //TODO: Add if for the game select
  FirstGameLoop();
}

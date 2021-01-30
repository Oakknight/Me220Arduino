#include <Arduino.h>

//pins for the players' buttons
#define playerOne 3 //blue button, left, starting from pin 11, index 7
#define playerTwo 2 //red button, right, starting from pin 4, index 0
// For reference, we will take blue button on the left and red button on the right

#define runDelay 1000    //delay in ms before light moves to the next point, Game speed property
#define numPins 8        //number of LEDS
#define debounceDelay 20 // will be used for debouncing

const int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11}; //Ledpins array

//At first, I tried to use an array that was to keep track of the bullets in the grid. Specifically, which LED lights up because of whose bullet
//But that didn't seem to work fine and was quite buggy.
//This time, I will try creating seperate variables which will hold the positions of the bullets, if they are present.
int posBulletOne = -1; //We are initialising them from -1 since, 0 will actually be a position we will use
int posBulletTwo = -1; //When a player shoots, we will set these variables to their designated bullet start positions
//With each frame, bullets will be moved with respect to these two values

volatile int activeShot = 0;
// This variable will hold record for the active bullets. It will always be between [0-3].
// If player one has a bullet it will be 1, if player two has a bullet it will be 2. If both players have bullets, it will be three.
//This way, we aim to identify double-shooting, shooting while there is still an active bullet.
//Each player will have their own shooting and double-shooting functions,
//and they will be active with respect to the value of the activeShot variable

int stunTimeOne = 0; //These variables will hold the remaining stun time in frames if a player tries to shoot again and gets punished.
int stunTimeTwo = 0; //They will be set to 5 at the punish function, then will be decreased by one each frame, if they are bigger than zero

void PunishPlayerOne()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.
  Serial.println("p1p");
  posBulletOne = -1; //Reset the bullet position and remove bullet from the activeShot record
  if (activeShot == 1)
    activeShot = 0;
  if (activeShot == 3)
    activeShot -= 1;
  detachInterrupt(digitalPinToInterrupt(playerOne));
  stunTimeOne = 5;
}

void PunishPlayerTwo()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.
  Serial.println("p2p");
  posBulletTwo = -1; //Reset bullet pos and remove bullet from activeShot record
  if (activeShot == 2)
    activeShot = 0; //I could instead just lower it by 2 but I believe this way would be more foolproof
  if (activeShot == 3)
    activeShot -= 2;
  detachInterrupt(digitalPinToInterrupt(playerTwo));
  stunTimeTwo = 5;
}

void PlayerOneShoot()
{                       // When the first player shoots, we record the bullet and then detach their interrupt to shoot
  if (stunTimeOne == 0) // We don't want the player to shoot if they are already stunned
  {
    Serial.println("p1s");
    detachInterrupt(digitalPinToInterrupt(playerOne));
    posBulletOne = 7; //The bullet is LED 8
    activeShot += 1;
    //We then attach the new interrupt to punish the player if they try to shoot again
    attachInterrupt(digitalPinToInterrupt(playerOne), PunishPlayerOne, RISING);
  }
}

void PlayerTwoShoot()
{ // When the second player shoots, we record the bullet and then detach their interrupt to shoot
  if (stunTimeTwo == 0)
  {
    Serial.println("p2s");
    detachInterrupt(digitalPinToInterrupt(playerTwo));
    posBulletTwo = 0; //The bullet is at LED 1
    activeShot += 2;
    //We then attach the new interrupt to punish the player if they try to shoot again
    attachInterrupt(digitalPinToInterrupt(playerTwo), PunishPlayerTwo, RISING);
  }
  //Nothing happens if the player is stunned
}

void FirstGameLoop() // This will be the loop where our first game will run
{

  //The game will consist of "frames" which have a period of one LED movement between them
  //  runDelay will be this time and it will specify how fast the game will run
  delay(runDelay);

  {                      // See if the player can shoot
    if (activeShot == 0) // If there are no bullets, we will attach two interrupts that allows both players to shoot
    {
      attachInterrupt(digitalPinToInterrupt(playerOne), PlayerOneShoot, RISING);
      attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
    }
    if (activeShot == 1 && stunTimeTwo < 1)
    {
      attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
    }
    if (activeShot == 2 && stunTimeOne < 1)
    {
      attachInterrupt(digitalPinToInterrupt(playerOne), PlayerOneShoot, RISING);
    }
    //This section could possibly be simplified to use less lines of code, but I am focusing on the function for now
  }

  //Light up the LEDs using the posBullet variables
  if (posBulletOne > -1)
  {
    digitalWrite(ledPins[posBulletOne], HIGH);
    //Serial.print("LED set to HIGH");
    //Serial.println(posBulletOne);
  }

  if (posBulletTwo > -1)
  {
    digitalWrite(ledPins[posBulletTwo], HIGH);
    //Serial.print("LED set to HIGH");
    //Serial.println(posBulletTwo);
  }

  delay(runDelay); //The period of a frame

  //Turn off the LED's after the delay
  digitalWrite(ledPins[posBulletOne], LOW);
  digitalWrite(ledPins[posBulletTwo], LOW);

  //Then move the bullets forward by adjusting their posBullet var values ONLY IF THEY WERE ALREADY NOT -1

  if (posBulletOne > -1)
  {
    posBulletOne--;
    if (posBulletOne == -1)
    {
      activeShot -= 1; //Remove the bullet record
    }
  }

  if (posBulletTwo > -1)
  {
    posBulletTwo++;
  }

  //Reset the second bullet if it goes off-grid
  if (posBulletTwo > 7)
  {
    posBulletTwo = -1; //Reset Position
    activeShot -= 2;   //Remove the bullet record
  }

  //One of them moves towards right while the other moves to the left
  //Later we will add the code to check for collisions

  // If a player is stunned, their stun timer decreases by one in each frame
  //Check Stun
  if (stunTimeTwo > 0)
  {
    Serial.print("P2 is stunned: ");
    Serial.println(stunTimeTwo);
    stunTimeTwo--;
  }

  if (stunTimeOne > 0)
  {

    Serial.print("P1 is stunned: ");
    Serial.println(stunTimeOne);
    stunTimeOne--;
  }
  // Decreasing the stun after the delay in order to prevent a possible bug where the player can shoot when they still have 1 more frame to wait

  /*
  Serial.print("Bullet one: ");
  Serial.println(posBulletOne);

  Serial.print("Bullet two: ");
  Serial.println(posBulletTwo);
  */
 
}

void Diagnostics()
{ // A function to see if all leds have been connected correctly
  Serial.println("Running Diagnostics");
  Serial.println("Checking Led Pins");
  for (int i = 0; i < numPins; i++)
  {
    digitalWrite(ledPins[i], HIGH);
    Serial.print("Led ");
    Serial.print(i);
    Serial.print(" should now be HIGH at pin ");
    Serial.println(ledPins[i]);
    delay(1000);
  }
  for (int i = 7; i > -1; i--)
  {
    digitalWrite(ledPins[i], LOW);
    Serial.print("Led ");
    Serial.print(i);
    Serial.print(" should now be LOW at pin ");
    Serial.println(ledPins[i]);
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
  Serial.println("Ready");
}

void loop()
{
  // First or second game will be run according to the information coming from the setup function
  //TODO: Add if for the game select
  FirstGameLoop();
}

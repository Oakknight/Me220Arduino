#include <Arduino.h>

//pins for the players' buttons
#define playerOne 3 //blue button, left, starting from pin 11, index 7
#define playerTwo 2 //red button, right, starting from pin 4, index 0
// For reference, we will take blue button on the left and red button on the right

#define runDelay 500      //delay in ms before light moves to the next point, Game speed property
#define numPins 8         //number of LEDS
#define debounceDelay 100 // will be used for debouncing

const int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11}; //Ledpins array

volatile long lastPressTimeA = 0; //These will be used for debouncing
volatile long lastPressTimeB = 0; //There are two because we don't want to debounce another players button push

int scoreA = 0; // These will be used to keep track of the scores of th players
int scoreB = 0;

int gameMode = 0;
bool gameChosen = false;

int winner = 0;

//General variables for the first game

//At first, I tried to use an array that was to keep track of the bullets in the grid. Specifically, which LED lights up because of whose bullet
//But that didn't seem to work fine and was quite buggy.
//This time, I will try creating seperate variables which will hold the positions of the bullets, if they are present.
volatile int posBulletOne = -1; //We are initialising them from -1 since, 0 will actually be a position we will use
volatile int posBulletTwo = -1; //When a player shoots, we will set these variables to their designated bullet start positions
//With each frame, bullets will be moved with respect to these two values

volatile int activeShot = 0;
// This variable will hold record for the active bullets. It will always be between [0-3].
// If player one has a bullet it will be 1, if player two has a bullet it will be 2. If both players have bullets, it will be three.
//This way, we aim to identify double-shooting, shooting while there is still an active bullet.
//Each player will have their own shooting and double-shooting functions,
//and they will be active with respect to the value of the activeShot variable

int stunTimeOne = 0; //These variables will hold the remaining stun time in frames if a player tries to shoot again and gets punished.
int stunTimeTwo = 0; //They will be set to 5 at the punish function, then will be decreased by one each frame, if they are bigger than zero

//General variables for the second game

long time_target = 0; //These are of type long since time in ms could be a pretty big number
long time_guess = 0;
long target_start = 0;
long guess_start = 0;

volatile int nextPlayer = 3; // We will change this value to change the guessing and target setting player
bool targetSet = false;
bool guessSet = false;
bool phaseSet = false;
bool phaseGuess = false;
bool lights = false;

void SwitchPlayer() // Simple function to switch players
{
  switch (nextPlayer)
  {
  case 2:
    nextPlayer = 3;
    break;
  case 3:
    nextPlayer = 2;
    break;

  default:
    break;
  }
}

void SetTargetTime()
{
  if (micros() / 1000 - target_start > debounceDelay)
  { //only register button after debouncing
    // Serial.println("rb");
    detachInterrupt(digitalPinToInterrupt(nextPlayer));
    noInterrupts();
    time_target = micros() / 1000 - target_start;
    targetSet = true;
    //After recording the time button was pressed down, we change the nextPlayer value
    SwitchPlayer();
    lights = false;
  }
}
void StartTargetTime() //We record the time when the button is pressed down,
{                      // remove the interrupt, then attach another to record when the button is released

  detachInterrupt(digitalPinToInterrupt(nextPlayer));
  target_start = micros() / 1000;
  lights = true;
  attachInterrupt(digitalPinToInterrupt(nextPlayer), SetTargetTime, FALLING);
}

void SetGuessTime()
{

  if (micros() / 1000 - guess_start > debounceDelay)
  {
    detachInterrupt(digitalPinToInterrupt(nextPlayer));
    time_guess = micros() / 1000 - guess_start;
    guessSet = true;
    lights = false;
  }
}

void StartGuessTime() //Same process we used to record the target time
{
  detachInterrupt(digitalPinToInterrupt(nextPlayer));
  guess_start = micros() / 1000;
  lights = true;
  attachInterrupt(digitalPinToInterrupt(nextPlayer), SetGuessTime, FALLING);
}

void ResetSet()
{
  guessSet = false;
  targetSet = false;
  phaseSet = false;
  phaseGuess = false;
  lastPressTimeA = 0;
  lastPressTimeB = 0;
  time_guess = 0;
  time_target = 0;
  lights = false;
  target_start = 0;
  guess_start = 0;
}

void ResetGame()
{
  ResetSet();
  scoreA = 0;
  scoreB = 0;
  gameMode = 0;
  Serial.println("Returning to the menu");
}

void PunishPlayerOne()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.

  if (micros() / 1000 - lastPressTimeA > debounceDelay) //Only execute if enough time to debounce has passed
  {
    //Serial.println("p1p");
    digitalWrite(ledPins[posBulletOne], LOW);
    posBulletOne = -1; //Reset the bullet position and remove bullet from the activeShot record
    if (activeShot == 1)
      activeShot = 0;
    if (activeShot == 3)
      activeShot -= 1;
    detachInterrupt(digitalPinToInterrupt(playerOne));
    stunTimeOne = 5;
  }
}

void PunishPlayerTwo()
{ //If the player tries to shoot while there is still a bullet, they will be stunned.
  //We detach the punishing interrupt and add a stun counter.

  if (micros() / 1000 - lastPressTimeB > debounceDelay) //Only execute if enough time to debounce has passed
  {
    //Serial.println("p2p");
    digitalWrite(ledPins[posBulletTwo], LOW);
    posBulletTwo = -1; //Reset bullet pos and remove bullet from activeShot record
    if (activeShot == 2)
      activeShot = 0; //I could instead just lower it by 2 but I believe this way would be more foolproof
    if (activeShot == 3)
      activeShot -= 2;
    detachInterrupt(digitalPinToInterrupt(playerTwo));
    stunTimeTwo = 5;
  }
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
    lastPressTimeA = micros() / 1000; //Save the button press time of Player One
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
    lastPressTimeB = micros() / 1000; //Save the button pres time of Player Two
  }
  //Nothing happens if the player is stunned
}

void FirstGameLoop() // This will be the loop where our first game will run
{

  //The game will consist of "frames" which have a period of one LED movement between them
  //  runDelay will be this time and it will specify how fast the game will run

  {                      // See if the player can shoot
    if (activeShot == 0) // If there are no bullets, we will attach two interrupts that allows both players to shoot
    {
      if (stunTimeTwo < 1)
      {
        attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
      }
      if (stunTimeOne < 1)
      {
        attachInterrupt(digitalPinToInterrupt(playerOne), PlayerOneShoot, RISING);
      }
    }
    if (activeShot == 1 && stunTimeTwo < 1)
    { //There is a bullet which belongs to player 1
      attachInterrupt(digitalPinToInterrupt(playerTwo), PlayerTwoShoot, RISING);
    }
    if (activeShot == 2 && stunTimeOne < 1)
    { // There is a bullet which belongs to player 2
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

  //Check for collision before moving the bullets

  if (posBulletTwo != -1)
  {
    if (posBulletOne == posBulletTwo || posBulletOne - 1 == posBulletTwo || posBulletTwo + 1 == posBulletOne)
    {
      posBulletTwo = -1;
      posBulletOne = -1;
      activeShot = 0;
      Serial.println("Bullets have clashed and cancelled each other!");
    }
  }

  //Then move the bullets forward by adjusting their posBullet var values ONLY IF THEY WERE ALREADY NOT -1
  if (posBulletOne > -1)
  {
    posBulletOne--;
    if (posBulletOne == -1)
    {                  // This means the bullet hit the other player
      activeShot -= 1; //Remove the bullet record
      scoreA += 1;     //Player One gains a point
      Serial.println("Player ONE has been HIT!");
      Serial.print("New score is: ");
      Serial.print(scoreA);
      Serial.print(" - ");
      Serial.println(scoreB);
    }
  }

  if (posBulletTwo > -1)
  {
    posBulletTwo++;
  }

  //Reset the second bullet if it goes off-grid
  if (posBulletTwo > 7)
  {                    // Hit means the bullet hit PLayer one,
    posBulletTwo = -1; //Reset Position
    activeShot -= 2;   //Remove the bullet record
    scoreB += 1;
    Serial.println("Player ONE has been HIT!");
    Serial.print("New score is: ");
    Serial.print(scoreA);
    Serial.print(" - ");
    Serial.println(scoreB);
  }
  //One of them moves towards right while the other moves to the left

  // If a player is stunned, their stun timer decreases by one in each frame
  //Check Stun
  if (stunTimeTwo > 0)
  {
    Serial.print("P2 is stunned: ");
    Serial.println(stunTimeTwo);
    stunTimeTwo--;
    posBulletTwo = -1;
  }

  if (stunTimeOne > 0)
  {

    Serial.print("P1 is stunned: ");
    Serial.println(stunTimeOne);
    stunTimeOne--;
    posBulletOne = -1;
  }
  // Decreasing the stun after the delay in order to prevent a possible bug where the player can shoot when they still have 1 more frame to wait

  /*
  Serial.print("Bullet one: ");
  Serial.println(posBulletOne);

  Serial.print("Bullet two: ");
  Serial.println(posBulletTwo);
  */

  delay(runDelay);
}

void SecondGameLoop() // This will be the loop where the second game will run
{

  //We attach the interrupt to the player who will be guessing
  if (!targetSet) // The following code will be executed once for each set
  {
    if (!phaseSet)
    {
      Serial.print("Player ");
      Serial.print(nextPlayer);
      Serial.println(" will be setting target");
      phaseSet = true;
      delay(runDelay);
      attachInterrupt(digitalPinToInterrupt(nextPlayer), StartTargetTime, RISING);
    }
  }
  //We will change the "nextPlayer" to keep track of which player will go

  //After attaching the interrupt we will wait for the button press and recordings

  if (targetSet) //After we set the target, we will prompt the guessing player
  {
    if (!phaseGuess)
    {
      Serial.println("Target time has been set ");
      Serial.print("Player ");
      Serial.print(nextPlayer);
      Serial.println(" will now try to guess the time.");

      delay(runDelay);
      phaseGuess = true;
      interrupts();
      attachInterrupt(digitalPinToInterrupt(nextPlayer), StartGuessTime, RISING);
    }
  }

  //Whenever there is a process of keeping time, we set the lights var to true;
  // We want the LEDs to light up whenever a button is being pressed

  //Set all LEDs to their required state
  noInterrupts();
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(ledPins[i], lights);
  }
  interrupts();

  if (guessSet) // If a guess has been made, we will compare the target and guess values to find a winner for the set.
  {
    int error = abs(time_target - time_guess);
    if (error < time_target / 10) // If the time difference is not in our accepted range, target setting player wins.
    {
      // SwitchPlayer();
      winner = nextPlayer;
      //SwitchPlayer();
    }
    else
    {
      SwitchPlayer();
      winner = nextPlayer;
      SwitchPlayer();
    }
    Serial.print("Player ");
    Serial.print(winner);
    Serial.println(" has won this set!");
    Serial.print("Target time: ");
    Serial.println(time_target);
    Serial.print("Guess time: ");
    Serial.println(time_guess);

    //Assign a point to the winning player
    if (nextPlayer == 2)
    {
      scoreB++;
    }
    else if (nextPlayer == 3)
    {
      scoreA++;
    }
    ResetSet();
  }
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
  /*
  Serial.println("Switch func");
  Serial.println(nextPlayer);
  SwitchPlayer();
  Serial.println("Should change now");
  Serial.println(nextPlayer);
  SwitchPlayer();
  Serial.println("Should change now");
  Serial.println(nextPlayer);
  SwitchPlayer();
  Serial.println("Should change now");
  Serial.println(nextPlayer);
  SwitchPlayer();
  Serial.println("Should change now");
  Serial.println(nextPlayer);
  // */
}

void loop()
{
  // First or second game will be run according to the information coming from the setup function

  //*
  switch (gameMode)
  {
  case 0:
    Serial.println("Please choose a gamemode: 1 or 2");
    delay(2000);
    if (Serial.available() > 0)
    {
      gameMode = Serial.parseInt();
    }

    break;
  case 1:
    if (!gameChosen)
    {
      Serial.println("Game mode 1 chosen, LIGHT BATTLE!");
      gameChosen = true;
    }

    FirstGameLoop();
    break;

  case 2:
    if (!gameChosen)
    {
      Serial.println("Game mode 2 chosen, GUESSING GAME!");
      gameChosen = true;
    }
    SecondGameLoop();
    break;

  default:
    Serial.println("Weird input");
    break;
  }

  //*/

  //FirstGameLoop();

  //SecondGameLoop();

  //GameOver Check for both games
  //TODO: Add return to menu
  if (scoreA == 5) //Game over, return to menu
  {
    Serial.print("GAME OVER, PLAYER ONE HAS WON!");
    ResetGame();
  }
  if (scoreB == 5) //Game over, return to menu
  {
    Serial.print("GAME OVER, PLAYER TWO HAS WON!");
    ResetGame();
  }
}

#include <Arduino.h>

#define interruptPin 3 // For Button Input
#define runDelay 50    // delay before light moves

int incomingMode = 0;                       // for Mode input by da user
int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11}; //An array for all your favourite led pins!
int pressCounter = 0;                       // To count button presses
int debouncer = 100;                        // To wait in milliseconds
int pushDown = 0;                           // for the button problem
int state;                                  //keeps the state of a ledpin for display
int timeCounter = 0;                        //To count time at the third mode
int userInput;                              // To hold the number sent by the user
int modeChosen;                             //To store the chosen gamemode
unsigned long lastCount = 0;                //For debouncing

const byte numPins = 8; // num of leds

void RunningLights() //Vanilla, Mode 1
{

    for (int i = 0; i < numPins; i++)
    {
        digitalWrite(ledPins[i], HIGH); //take the pin and light it up
        delay(runDelay);                //Wait For It..
        digitalWrite(ledPins[i], LOW);  // lights out!
    }                                   //First loop to go right to left

    for (int i = numPins; i > -1; i--)
    {
        digitalWrite(ledPins[i], HIGH);
        delay(runDelay); //Wait For It..
        digitalWrite(ledPins[i], LOW);
    } // Second loop to go left to right
}

void StopTheLights() //For Mode 2, TODO:Fix this
{                    //TODO:Add debouncing
    pushDown = digitalRead(interruptPin);

    while (pushDown == 1)
    {
        Serial.println("OMG STOP TOUCHING ME!");
        pushDown = digitalRead(interruptPin);
    }
}

void TimerCountUp() //Mode 3 Count in Binary with timer
{                   //TODO: Go over this function just in case

    while (true)
    {

        timeCounter++;               //Increase 1 second each cycle
        Serial.println(timeCounter); //Show the current second we are in

        //I took the following binary convertion code from the internet
        /* convert presses to binary and store it as a string */
        String binNumber = String(timeCounter, BIN);
        /* get the length of the string */
        int binLength = binNumber.length();
        if (timeCounter <= 255)
        { // if we have less or equal to 255 presses
            // here is the scary code
            for (int i = 0, x = 1; i < binLength; i++, x += 2)
            {
                if (binNumber[i] == '0')
                    state = LOW;
                if (binNumber[i] == '1')
                    state = HIGH;
                digitalWrite(ledPins[i] + binLength - x, state);
            }
        }
        else
        {
            Serial.println("Congratulations, you pressed the button too much!");
        }
        delay(995); //Not 1000 ms to be more accurate
    }
}

void BinaryDisplay() //Mode 5, Display a number entered by the user
{
    userInput = 0;
    //int oldUserInput;

    if (Serial.available() > 0)
    {
        userInput = Serial.parseInt();

        Serial.print("User input is ");
        Serial.println(userInput);

        // convert user input to binary and store it as a string
        String binNumber = String(userInput, BIN);
        /* get the length of the string */
        int binLength = binNumber.length();
        if (userInput <= 255)
        { // if we have less or equal to 255 presses
            // here is the scary code
            for (int i = 0, x = 1; i < binLength; i++, x += 2)
            {
                if (binNumber[i] == '0')
                    state = LOW;
                if (binNumber[i] == '1')
                    state = HIGH;
                digitalWrite(ledPins[i] + binLength - x, state);
            }
        }
        else
        {
            Serial.println("We can not display that number");
        }
    }
}

void AddCount() // Mode 4, Function to increase count by one and update the lights
{
    if (millis() - lastCount > debouncer)
    { //Only execute the function if enough time has passed from the last count

        pressCounter++;
        Serial.println(pressCounter);

        /* convert presses to binary and store it as a string */
        String binNumber = String(pressCounter, BIN); //this is super convenient

        Serial.print("The binary for the count is ");
        Serial.println(binNumber);
        lastCount = millis(); //This records the last time of counting

        /* get the length of the string */
        int binLength = binNumber.length();
        if (pressCounter <= 255)
        { // if we have less or equal to 255 presses
            // here is the scary code
            for (int i = 0, x = 1; i < binLength; i++, x += 2)
            {
                if (binNumber[i] == '0')
                    state = LOW;
                if (binNumber[i] == '1')
                    state = HIGH;
                digitalWrite(ledPins[i] + binLength - x, state);
            }
        }
        else
        {
            Serial.println("Congratz you pressed the button too much!");
        }
    }
}

// There were some problems with the scope so I put the loop and setup functions here

void setup()
{

    Serial.begin(9600);

    //Led pins set to be output
    for (int i = 0; i < numPins; i++)
    {
        pinMode(ledPins[i], OUTPUT);
    }
    pinMode(interruptPin, INPUT);

    Serial.println("Enter:");
    Serial.println("1 for running lights");
    Serial.println("2 for running lights with pause and resume");
    Serial.println("3 for binary count up - on timer");
    Serial.println("4 for binary count up - on button");
    Serial.println("5 for binary display of numbers [1-255]");

    while (modeChosen == 0) //This while loop waits for user input to proceed
    {
        if (Serial.available() > 0)
            modeChosen = Serial.parseInt(); //Read input and save it to a variable
    }

    Serial.println("You chose wisely!");
    Serial.print("Now running gamemode ");
    Serial.println(modeChosen);

    if (modeChosen == 4)
        attachInterrupt(digitalPinToInterrupt(interruptPin), AddCount, RISING); //Use the button to call the increment method
    if (modeChosen == 2)
    {
        attachInterrupt(digitalPinToInterrupt(interruptPin), StopTheLights, CHANGE);
    }
}

void loop()
{

    switch (modeChosen)
    {
    case 1:
        /* code */
        RunningLights();
        break;
    case 2:
        //Vanilla Code will be run by attaching an interrupt first

        //TODO: Make method stop till further change
        //TODO: Use *CHANGE* with a loop and debouncing
        RunningLights();
        break;
    case 3:
        TimerCountUp();
        break;
    case 4:
        //Interrupt attached in setup

        break;
    case 5:
        BinaryDisplay();
        break;

    default:
        Serial.println("Weird Mod Chosen, please reset machine");
        delay(1000);
        break;
    }
}
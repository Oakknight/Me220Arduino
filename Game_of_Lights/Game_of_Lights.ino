
#define playerOne 3
#define playerTwo 2
#define runDelay 400
#define numPins 8 
#define debounceDelay 20


const int ledPins[] = {4, 5, 6, 7, 8, 9, 10, 11};
volatile int bullets[] = {0, 0, 0, 0, 0, 0, 0, 0};

bool oneHit = false;
bool twoHit = false;

volatile bool shootOne = false;
volatile bool shootTwo = false;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  for(int i = 0; i < numPins; i++)
    pinMode(ledPins[i], OUTPUT);

  pinMode(playerOne, INPUT);
  pinMode(playerOne, INPUT);

  //Diagnostics();// Run this function to diagnose your circuit

  attachInterrupt(digitalPinToInterrupt(playerOne), ShootOne , FALLING);
  attachInterrupt(digitalPinToInterrupt(playerTwo), ShootTwo , FALLING);
   
}

void loop() {
  // put your main code here, to run repeatedly:
  FirstGameLoop();

}


void FirstGameLoop(){
  while(true){

    for(int i = 0; i < numPins; i++) { // check all pins
      if(bullets[i] == 1){ //Checks if there is a bullet from p1, then move it forwards
        if(i == numPins-1){
          shootOne = false;
          twoHit = true;
          Serial.println("Player Two has been Hit!");
        }
        bullets[i+1] = 1; // moves the bullet left
        bullets[i] = 0;  // clear the prev bullet
      }
      
      if(bullets[7-i] == 2){ //Checks if there is a bullet from p2, then move it forwards
        if(i == numPins-1){
          shootTwo = false;
          oneHit = true;
          Serial.println("Player One has been Hit!");
        }
        bullets[6-i] = 2; //moves the bullet right
        bullets[7-i] = 0; // clear the prev bullet
      }
      for(int j = 0; j<numPins; j++){
        digitalWrite(ledPins[j], bullets[j]);
      }
      delay(runDelay);
    } 
  }
}



void ShootOne(){
  if(!shootOne){
    shootOne = true;
  Serial.println(" Player One Shoots ");
  bullets[0] = 1; //Creates a bullet at starting location
  digitalWrite(ledPins[0], HIGH);
  }  
}

void ShootTwo(){
  if(!shootTwo){
   shootTwo = true;
   Serial.println(" Player Two Shoots ");
   bullets[7] = 2; //Creates a bullet at starting location
   digitalWrite(ledPins[7], HIGH);
  }
}

void Diagnostics(){ // A function to see if all leds have been connected correctly
  Serial.println("Running Diagnostics");
  Serial.println("Checking Led Pins");
  for(int i = 0; i<numPins; i++){
    digitalWrite(ledPins[i], HIGH);
    Serial.println("Led ");
    Serial.print(i);
    Serial.print(" should now be HIGH at pin ");
    Serial.print(ledPins[i]);
    delay(1000);   
  }
  for(int i = 7; i > -1; i--){
    digitalWrite(ledPins[i], LOW);
    Serial.println("Led ");
    Serial.print(i);
    Serial.print(" should now be LOW at pin ");
    Serial.print(ledPins[i]);
    delay(1000); 
  }
  Serial.println(" Diagnostics complete ");
}

/*
  LEDMatrix

  TODO Beschreibung ???
  
  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

//Matrix Dimensions

#define PIN      6
#define N_LEDS 448
#define NEO_MATRIX_HIGHT 16
#define NEO_MATRIX_WIDTH 28


//Debug 
bool debugMode = false;
const byte ledPin = 13;

// Interrupts
const byte interruptExecutePin = 2;
volatile boolean shouldRun = false;
const byte interruptResetPin = 1;
volatile boolean isReset = false;


void setup() {
  //Interrupts
  
  //TODO Check Debug State
  //For Debug Purpose Only
  pinMode(ledPin, OUTPUT);
  
  //Interrupts
  pinMode(interruptExecutePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptExecutePin), startAction, RISING);
  //TODO Reset Button
  
  //TODO Init Matrix
}

void loop() {
  //TODO reset when needed
  if(shouldRun == false) return;
  
  //TODO Check rotary switch
  
  //TODO Check execution count
  //TODO reset when needed
  
  //TODO read SDCard file A<rotarySwitch>_<executionCount>.txt
  
  //TODO reset when needed
  //TODO blink action if needed
  
  //TODO reset if needed
  //TODO write Pattern
   
  showDebugPinAction();
  shouldRun = false;
  
}

// Interrupts
void startAction() {
  shouldRun = true;
}

//TODO Reset Button


void showDebugPinAction(){
  digitalWrite(ledPin, HIGH);
  delay(500);                  // waits for a second
  digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
  delay(500);                  // waits for a second
  digitalWrite(ledPin, HIGH);
  delay(500);                  // waits for a second
  digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
  delay(500);
}

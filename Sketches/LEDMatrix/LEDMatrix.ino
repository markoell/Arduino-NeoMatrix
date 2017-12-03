/*
  LEDMatrix

  TODO Beschreibung ???
  
  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <gamma.h>
#include <Adafruit_NeoMatrix.h>
#include <SPI.h>
#include <SD.h>

//Pin Configuration
#define SDCARD_CS_PIN 10
#define BUTTON_EXEC_PIN 2
#define BUTTON_RESET_PIN 3
#define EVENT_TRIGGER_ACTION RISING

//Debug
#define DEBUG_TRIGGER_PIN 7
#define DEBUG_READ_PIN 8


//Matrix Dimensions
#define DATA_PIN	6
#define N_LEDS 448
#define NEO_MATRIX_HIGHT 16
#define NEO_MATRIX_WIDTH 28

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, DATA_PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

//LED Colors                  black                 red                       blue                      gelb
const uint16_t colors[4] = { matrix.Color(0, 0, 0), matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

//Debug 
bool debugMode = false;  //TODO disable Debug when Jumper can read
const byte outputPin = DATA_PIN;

// Interrupts
const byte interruptNextPin = BUTTON_EXEC_PIN;
volatile boolean nextAction = false;
const byte interruptReversePin = BUTTON_RESET_PIN;
volatile boolean lastAction = false;

//Global
volatile boolean sdCardFailed;

void setup() {
  
  if(debugMode){
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("DebugMode Set");
  }
  
  //Interrupts
  printDebugMessages("Init Interrupt Pins");
  pinMode(interruptNextPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptNextPin), executeNextAction, EVENT_TRIGGER_ACTION);
  pinMode(interruptReversePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptReversePin), executeLastAction, EVENT_TRIGGER_ACTION);

  //Init SD card reader
  printDebugMessages("Initialize SD Card");
  if (!SD.begin(SDCARD_CS_PIN)) {
    printDebugMessages("Initialization failed!");
    errorPin(outputPin);
  }
  printDebugMessages("initialization done.");
  
  //Check Debug State
  pinMode(DEBUG_TRIGGER_PIN, OUTPUT);
  pinMode(DEBUG_READ_PIN, INPUT);
  digitalWrite(DEBUG_TRIGGER_PIN, HIGH);
  delayMicroseconds(500);
  int val = digitalRead(DEBUG_READ_PIN);
  digitalWrite(DEBUG_TRIGGER_PIN, LOW);
  if (val == HIGH) {
    debugMode = true;
  }

  printDebugMessages(String("DebugMode: ") + String(debugMode));
  //For Debug Purpose Only
  pinMode(outputPin, OUTPUT);

  //TODO Init Matrix
}

void loop() {
  static byte executionCount;
  byte executionSwitch = 1; //TODO provide value from rotary switch
  
  //TODO reset when needed
  //TODO don't return here
  if (nextAction == false) return;
  executionCount++;

  if(lastAction){
    executionCount--;
    lastAction = false;
    return;
  }
  
  
  String executionCountAsString = String(executionCount);
  
  //TODO Check rotary switch

  //TODO Check execution count
  printDebugMessages(executionCountAsString);

  //TODO reset when needed
  
  //TODO read SDCard file A<rotarySwitch>_<executionCount>.txt
  String fileName = String("A" + String(executionSwitch) + "_" + executionCountAsString);
  printDebugMessages("Open File " + fileName);

  File myFile;
  myFile = SD.open(fileName);

  while (myFile.available()) {
    //Serial.print(String("+" + myFile.read()));
    HandleSDCardInput(myFile.read());
  }
  
  //TODO reset when needed
  //TODO blink action if needed
  
  //TODO reset if needed
  //TODO write Pattern
  if(debugMode){
    showDebugPinAction(outputPin);  
  }
  myFile.close();
  nextAction = false;
  
}

// Interrupts
void executeNextAction() {
  nextAction = true;
}

void executeLastAction(){
  lastAction = true;
}

#pragma region SDCard

boolean HandleSDCardInput(int input) {

  Serial.print(input);
  Serial.write(input);
  static char buffer[4];
  static byte position;
  static boolean isCmd = true;
  static boolean isComment = false;

  char t = (char)input;
  switch (t) {
    case ':':
      isCmd = false;
      position = 0;
      break;
    case '#':
      isComment = true;
      break;
    case '|': break;
    case '\r': 
    case '\n': 
      isCmd = true;
      isComment = false;
      break;
    case ' ': break;
    default: 
      if (!isComment) {
        buffer[position] = t;
        position++;
      }
      return false;
  }

  /*if (position >= 3) {
    position = 0;
  }*/
  
  return true;
}

#pragma endregion


#pragma region DebugMethods

void printDebugMessages(String msg) {
  if (debugMode) {
    Serial.println(msg);
  }
}

void showDebugPinAction(int ledPin) {
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, HIGH);
  delay(500);                  // waits for a second
  digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
  delay(500);                  // waits for a second
  digitalWrite(ledPin, HIGH);
  delay(500);                  // waits for a second
  digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
  delay(500);
}

void errorPin(int ledPin) {
  pinMode(ledPin, OUTPUT);
  while (true) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
}

#pragma endregion


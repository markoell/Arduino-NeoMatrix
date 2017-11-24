/*
  LEDMatrix

  TODO Beschreibung ???
  
  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

#include <SPI.h>
#include <SD.h>

//Pin Configuration
#define SDCARD_CS_PIN 10
#define BUTTON_EXEC_PIN 2
#define BUTTON_RESET_PIN 3


//Matrix Dimensions

#define MATRIX_DATA_PIN	6
#define N_LEDS 448
#define NEO_MATRIX_HIGHT 16
#define NEO_MATRIX_WIDTH 28

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

//LED Colors
const uint16_t colors[16];

//Debug 
bool debugMode = true;  //TODO disable Debug when Jumper can read
const byte outputPin = MATRIX_DATA_PIN;

// Interrupts
const byte interruptExecutePin = BUTTON_EXEC_PIN;
volatile boolean shouldRun = false;
const byte interruptResetPin = BUTTON_RESET_PIN;
volatile boolean isResetSet = false;

//Global
volatile boolean sdCardFailed;

//SD Module
const int chipSelect = SDCARD_CS_PIN;

void setup() {
  
  if(debugMode){
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("DebugMode Set");
  }
  else{
	for (byte i=0; i<3; i++){
		for(byte j=0; j<5;j++){
			colors[(i*5+j)] = GetColor(i, j);		
		}
	}
  }
  
  //Interrupts
  printDebugMessages("Init Interrupt Pins");
  pinMode(interruptExecutePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptExecutePin), startAction, RISING);
  //TODO Reset Button
  pinMode(interruptResetPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptResetPin), resetAction, RISING);

  //Init SD card reader
  if (!SD.begin(chipSelect)) {
    printDebugMessages("initialization failed!");
    sdCardFailed = true;
    return;
  }
  printDebugMessages("initialization done.");
  
  //TODO Check Debug State
  //For Debug Purpose Only
  pinMode(outputPin, OUTPUT);
  
  
  
  //TODO Init Matrix
}

void loop() {

  if (sdCardFailed) {
    errorPin(outputPin);
  }

  static byte executionCount;
  byte executionSwitch = 1; //TODO provide value from rotary switch
  
  //TODO reset when needed
  if(isResetSet){
    printDebugMessages("Reset is Pressed");
    executionCount = 0;
    isResetSet = false;
    return;
  }
  if(shouldRun == false) return;
  executionCount++;
  
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
  shouldRun = false;
  
}


uint16_t GetColor(byte i, byte y){
	switch (i){
	case 0: 
		return matrix.Color(y * 50, 0, 0);
		case 1: 
		return matrix.Color(0, y * 50, 0);
		case 2: 
		return matrix.Color(0, 0 , y * 50);
		default:
	}
}


// Interrupts
void startAction() {
  shouldRun = true;
}

void resetAction(){
  isResetSet = true;
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


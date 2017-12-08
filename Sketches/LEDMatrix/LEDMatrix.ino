/*
  LEDMatrix

  TODO Beschreibung ???

  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

//#include <SPI.h>
//#include <SD.h>   //Problem Memory (uses 512 Byte on runtime) 

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>

//#include <gamma.h>

//#include <avr/pgmspace.h>

//Pin Configuration
const uint8_t SDCARD_CS_PIN = 10;
const uint8_t BUTTON_EXEC_PIN = 2;
const uint8_t BUTTON_RESET_PIN = 3;
const uint8_t EVENT_TRIGGER_ACTION = RISING;

//Debug
const uint8_t DEBUG_TRIGGER_PIN = 7;
const uint8_t DEBUG_READ_PIN = 8;


//Matrix Dimensions
const uint8_t DATA_PIN = 6;
const uint16_t N_LEDS = 448;  //To much pins, will collide with SD Card
const int NEO_MATRIX_HIGHT = 16;
const int NEO_MATRIX_WIDTH = 28;

//  !!Don't touch this unless you know what you do!!
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, DATA_PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

//other options
const uint8_t MAX_COLOR_VAL = 255;

#pragma region Color

//LED Colors                  black                 red                       blue                      gelb
const uint16_t colorsbw[] = { 0, matrix.Color(MAX_COLOR_VAL, MAX_COLOR_VAL, MAX_COLOR_VAL) };
const uint16_t colorsRed[15] =
{
  matrix.Color(0x10, 0, 0),
  matrix.Color(0x20, 0, 0),
  matrix.Color(0x30, 0, 0),
  matrix.Color(0x40, 0, 0),
  matrix.Color(0x50, 0, 0),
  matrix.Color(0x60, 0, 0),
  matrix.Color(0x70, 0, 0),
  matrix.Color(0x80, 0, 0),
  matrix.Color(0x90, 0, 0),
  matrix.Color(0xA0, 0, 0),
  matrix.Color(0xB0, 0, 0),
  matrix.Color(0xC0, 0, 0),
  matrix.Color(0xD0, 0, 0),
  matrix.Color(0xE0, 0, 0),
  matrix.Color(0xF0, 0, 0)
};

const uint16_t colorsGreen[15] =
{
  matrix.Color(0, 0x10, 0),
  matrix.Color(0, 0x20, 0),
  matrix.Color(0, 0x30, 0),
  matrix.Color(0, 0x40, 0),
  matrix.Color(0, 0x50, 0),
  matrix.Color(0, 0x60, 0),
  matrix.Color(0, 0x70, 0),
  matrix.Color(0, 0x80, 0),
  matrix.Color(0, 0x90, 0),
  matrix.Color(0, 0xA0, 0),
  matrix.Color(0, 0xB0, 0),
  matrix.Color(0, 0xC0, 0),
  matrix.Color(0, 0xD0, 0),
  matrix.Color(0, 0xE0, 0),
  matrix.Color(0, 0xF0, 0)
};

const uint16_t colorsBlue[15] =
{
  matrix.Color(0, 0, 0x10),
  matrix.Color(0, 0, 0x20),
  matrix.Color(0, 0, 0x30),
  matrix.Color(0, 0, 0x40),
  matrix.Color(0, 0, 0x50),
  matrix.Color(0, 0, 0x60),
  matrix.Color(0, 0, 0x70),
  matrix.Color(0, 0, 0x80),
  matrix.Color(0, 0, 0x90),
  matrix.Color(0, 0, 0xA0),
  matrix.Color(0, 0, 0xB0),
  matrix.Color(0, 0, 0xC0),
  matrix.Color(0, 0, 0xD0),
  matrix.Color(0, 0, 0xE0),
  matrix.Color(0, 0, 0xF0)
};

#pragma endregion

//Output
static boolean isDebugMode;
const uint8_t outputPin = DATA_PIN;

#pragma region DebugOutputMessages

const char debugMsg_DebugModeSet[] PROGMEM = "DebugMode Set";
const char debugMsg_InitInterruptPin[] PROGMEM = "Initialize Interrupt Pin";
const char debugMsg_InitSDCardReader[] PROGMEM = "Initialize SD Card Reader";
const char debugMsg_InitFailed[] PROGMEM = "Initialization failed!";
const char debugMsg_InitDone[] PROGMEM = "Initialization done.";

#pragma endregion

// Interrupts
const uint8_t interruptNextPin = BUTTON_EXEC_PIN;
volatile boolean nextAction = false;
const byte interruptReversePin = BUTTON_RESET_PIN;
volatile boolean lastAction = false;

//Global


void setup() {

  isDebugMode = CheckDebugState(DEBUG_TRIGGER_PIN, DEBUG_READ_PIN);

  //InitializeSerialPortInDebugMode(isDebugMode);
  InitializeSerialPortInDebugMode(true);

  //Interrupts
  RegisterInterrupt(interruptNextPin, executeNextAction, EVENT_TRIGGER_ACTION);
  RegisterInterrupt(interruptReversePin, executeLastAction, EVENT_TRIGGER_ACTION);

  //Initialize output to Matrix
  InitializeDataPort(isDebugMode, DATA_PIN);
//Serial.println("test");
while(1){
      ;
    }
  //Init SD card reader
//  InitializeSDCardReader(SDCARD_CS_PIN);
}

void loop() {
  return;
  /*
  static byte executionCount;
  byte executionSwitch = 1; //TODO provide value from rotary switch

  //TODO reset when needed
  //TODO don't return here
  if (nextAction == false) return;
  executionCount++;

  if (lastAction) {
    executionCount--;
    lastAction = false;
    return;
  }
  String executionCountAsString = String(executionCount);

  //TODO Check rotary switch

  //TODO Check execution count
  //printDebugMessages(executionCountAsString);

  //TODO reset when needed

  //TODO read SDCard file A<rotarySwitch>_<executionCount>.txt
  String fileName = String("A" + String(executionSwitch) + "_" + executionCountAsString);
  //printDebugMessages("Open File " + fileName);

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
  if (isDebugMode) {
    showDebugPinAction(outputPin);
 // }
  myFile.close();
  nextAction = false;
  */
}


boolean CheckDebugState(const uint8_t triggerPin, const uint8_t inputPin) {
  //Check Debug State
  pinMode(triggerPin, OUTPUT);
  pinMode(inputPin, INPUT);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(500);
  int val = digitalRead(inputPin);
  digitalWrite(triggerPin, LOW);
  if (val == HIGH) {
    return false;
  }
  return true;
}

void InitializeSerialPortInDebugMode(const boolean debugMode) {
  if (debugMode) {
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    printDebugMessages(debugMode, debugMsg_DebugModeSet);
  }
}

#pragma region Interrupts
// Interrupts

void RegisterInterrupt(const uint8_t interruptPin, void (*executeAction)() , const uint8_t trigger) {
  printDebugMessages(debugMsg_InitInterruptPin, String(interruptPin).c_str());
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), executeAction, trigger);
}
void executeNextAction() {
  nextAction = true;
}

void executeLastAction() {
  lastAction = true;
}

#pragma endregion

#pragma region SDCard
/*
void InitializeSDCardReader(uint8_t csPin) {
  printDebugMessages(debugMsg_InitSDCardReader);
  if (!SD.begin(csPin)) {
    printDebugMessages(debugMsg_InitFailed);
    errorPin(outputPin);
  }
  printDebugMessages(debugMsg_InitDone);
}
*/
boolean HandleSDCardInput(int input) {
return false;
}

#pragma endregion


#pragma region Output

void InitializeDataPort(const boolean isDebugMode, const uint8_t outputPin) {
  //For Debug Purpose Only
  Serial.println("Hello");
  if (isDebugMode) {
    pinMode(outputPin, OUTPUT);
  }
  else {
    matrix.begin();
    //matrix.setBrightness(0);
    matrix.fillScreen(matrix.Color(128,0,0));
    matrix.show();
    Serial.println("Show");
  }

  //TODO Init Matrix
}

#pragma endregion

#pragma region DebugMethods

void printDebugMessages(const char* msg) {
  printDebugMessages(isDebugMode, msg, NULL);
}

void printDebugMessages(const boolean debugMode, const char* msg) {
  printDebugMessages(debugMode, msg, NULL);
}

void printDebugMessages(const char* msg, const char* appendix) {
  printDebugMessages(isDebugMode, msg, appendix);
}

void printDebugMessages(const boolean debugMode, const char* msg, const char* appendix) {
  return;
  if (!debugMode) { return; }
  char buffer[30];
  strcpy_P(buffer, (char*)pgm_read_word(msg));
  if (appendix != NULL) {
    strcat(buffer, " ");
    strcat(buffer, appendix);
  }
  Serial.println(buffer);
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
  if (isDebugMode) {
    pinMode(ledPin, OUTPUT);
    while (true) {
      digitalWrite(ledPin, HIGH);
      delay(250);
      digitalWrite(ledPin, LOW);
      delay(250);
    }
  }
  else {
    //TODO Write Matrix Status LED [First in Line]
  }
}

#pragma endregion


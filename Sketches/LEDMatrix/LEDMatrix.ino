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

#include <stdlib.h>     /* srand, rand */

#include "LEDSigns.h"

//Pin Configuration
const uint8_t BUTTON_EXEC_PIN = 2;
const uint8_t BUTTON_RESET_PIN = 3;
const uint8_t EVENT_TRIGGER_ACTION = RISING;

//Debug
const uint8_t DEBUG_TRIGGER_PIN = 7;
const uint8_t DEBUG_READ_PIN = 8;

//Matrix Dimensions
const uint8_t DATA_PIN = 6;

const int NEO_MATRIX_HIGHT = 16;
const int NEO_MATRIX_WIDTH = 28;
const uint16_t TOTAL_NUMBER_LEDS = NEO_MATRIX_HIGHT * NEO_MATRIX_WIDTH;

//  !!Don't touch this unless you know what you do!!
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, DATA_PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

#pragma region Color

const uint8_t MID_COLOR_VAL = 0x7F;
const uint8_t MAX_COLOR_VAL = 0xFF;

const uint16_t Colors[] = { 0, matrix.Color(MID_COLOR_VAL, 0, 0), matrix.Color(MAX_COLOR_VAL, 0, 0) };

enum Color : byte { r = 0, g, b };
enum Luminance : byte { off = 0, mid, max };

const Luminance DEFAUL_LUMINANCE = Luminance::max;
/*
const uint16_t colorsbw[] = { 0, matrix.Color(MAX_COLOR_VAL, MAX_COLOR_VAL, MAX_COLOR_VAL) };
const uint16_t colorsRed[2] =
{
  matrix.Color(MID_COLOR_VAL, 0, 0),
  matrix.Color(MAX_COLOR_VAL, 0, 0)
};

const uint16_t colorsGreen[2] =
{
  matrix.Color(0, MID_COLOR_VAL, 0),
  matrix.Color(0, MAX_COLOR_VAL, 0)
};

const uint16_t colorsBlue[2] =
{
  matrix.Color(0, 0, MID_COLOR_VAL),
  matrix.Color(0, 0, MAX_COLOR_VAL)
};

const uint16_t* const matrixColors[][2] = { colorsRed, colorsGreen, colorsBlue };
*/
#pragma endregion

//Output
static boolean isDebugMode;
const uint8_t outputPin = DATA_PIN;

#pragma region DebugOutputMessages
/*
const char debugMsg_DebugModeSet[] PROGMEM = "DebugMode Set";
const char debugMsg_InitInterruptPin[] PROGMEM = "Initialize Interrupt Pin";
const char debugMsg_InitSDCardReader[] PROGMEM = "Initialize SD Card Reader";
const char debugMsg_InitFailed[] PROGMEM = "Initialization failed!";
const char debugMsg_InitDone[] PROGMEM = "Initialization done.";
*/
#pragma endregion

// Interrupts
const uint8_t interruptNextPin = BUTTON_EXEC_PIN;
volatile boolean nextAction = false;
const byte interruptReversePin = BUTTON_RESET_PIN;
volatile boolean lastAction = false;

//Global
const uint32_t DEFAULT_DELAY_IN_MS = 7500;
const uint32_t DEFAULT_ENTERDELAY_IN_MS = 250;

void RegisterInterrupt(const uint8_t, void(*)(), const uint8_t);

void setup() {

  isDebugMode = ScanDebugState(DEBUG_TRIGGER_PIN, DEBUG_READ_PIN);
  InitializeSerialPortInDebugMode(isDebugMode);
  //InitializeSerialPortInDebugMode(true);

  //Interrupts
  RegisterInterrupt(interruptNextPin, executeNextAction, EVENT_TRIGGER_ACTION);
  RegisterInterrupt(interruptReversePin, executeLastAction, EVENT_TRIGGER_ACTION);

  //Initialize output to Matrix
  InitializeDataPort(isDebugMode, DATA_PIN);
}

void loop() {
  delay(250);
  static uint8_t cycleCount;

  /*Serial.print("Cycle: ");
  Serial.println(cycleCount);*/

  if (nextAction == false && lastAction == false) { 
    if (cycleCount == 0)
    {
      ShowStatusOk();
    }
    return;
  }

  if (nextAction == true) {
    cycleCount++;
  }
  else if (lastAction && cycleCount > 0) {
    cycleCount--;
  }

  if (cycleCount == 1) {
  //  ShowEnter1884();
 //   cycleCount++;
    return;
  }
  else if (cycleCount == 2) {
    Flicker(DEFAULT_DELAY_IN_MS, cycleCount);
    ShowYear(arr1884);
    delay(2000);
    if (isDebugMode) {
      okPin(DATA_PIN);
    }
    ShowYear(arr1984);
  }
  else if (cycleCount == 3) {
//    ShowEnter1884();
//    cycleCount++;
    return;
  }
  else if (cycleCount == 4) {
    Flicker(DEFAULT_DELAY_IN_MS, cycleCount);
    ShowYear(arr1884);
  }
  else if (cycleCount == 5) {
    //ShowEnter2118();
//    cycleCount++;
    return;
  }
  else if (cycleCount == 6) {
    Flicker(DEFAULT_DELAY_IN_MS, cycleCount);
    ShowYear(arr2118);
  }
  else if (cycleCount >= 7) {
    Clear();
    cycleCount = 0;
    delay(500);
  }
  ResetActionTrigger();
  return;
}

#pragma region Init

boolean ScanDebugState(const uint8_t triggerPin, const uint8_t inputPin) {
  //Check Debug State
  pinMode(triggerPin, OUTPUT);
  pinMode(inputPin, INPUT);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);
  int val = digitalRead(inputPin);
  delayMicroseconds(20);
  digitalWrite(triggerPin, LOW);
  if (val == HIGH) {
    return true;
  }
  return false;
}

void InitializeSerialPortInDebugMode(const boolean debugMode) {
  if (debugMode) {
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
//    printDebugMessages(debugMode, debugMsg_DebugModeSet);
  }
}

#pragma region Interrupts
// Interrupts

void RegisterInterrupt(const uint8_t interruptPin, void(*executeAction)(), const uint8_t trigger) {
  //printDebugMessages(debugMsg_InitInterruptPin, String(interruptPin).c_str());
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), executeAction, trigger);
}
void executeNextAction() {
  if (isDebugMode) {
    Serial.print("Trigger Next");
  }
  nextAction = true;
}

void executeLastAction() {
  if (isDebugMode) {
    Serial.print("Trigger Last");
  }
  lastAction = true;
}

#pragma endregion

#pragma region Output

void InitializeDataPort(const boolean isDebugMode, const uint8_t outputPin) {
  //For Debug Purpose Only
  if (isDebugMode) {
    pinMode(outputPin, OUTPUT);
    Serial.print(" Init Debug");
  }
  else {
    matrix.begin();
    matrix.fillScreen(Colors[0]);
    matrix.show();
  }

}

void Clear() {
  matrix.fillScreen(Colors[Luminance::off]);
  matrix.show();
}

#pragma endregion

#pragma endregion

#pragma region Execution

void ShowStatusOk() {
  okPin(DATA_PIN);
  for (int i = 0; i < 20; i++) {
    delay(250);
    if (nextAction == true || lastAction == true) { return; }
  }
}

void ResetActionTrigger() {
  if (isDebugMode) {
    Serial.println("Reset");
  }
  nextAction = lastAction = false;
}

void Flicker(uint32_t ms, uint8_t count) {

  uint32_t start = millis();
  uint32_t end = start + ms;
  if (isDebugMode) {
    Serial.print(" Show Debug");
    showDebugPinAction(DATA_PIN, count);
  }
  else {
    do {
      FillMatrixRandom(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT);
      matrix.show();
    } while (millis() < end);
  }
}

void FillMatrixRandom(int sizeX, int sizeY) {
  for (int y = 0; y < sizeY; y++) {
    for (int x = 0; x < sizeX; x++) {
      matrix.drawPixel(x, y, Colors[rand() % (sizeof(Colors) / sizeof(uint16_t))]);
    }
  }
}
/*
void ShowEnter1884() {
  ShowYear(arr0001);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr0018);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr0188);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr1884);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  if (isDebugMode) {
    okPin(DATA_PIN);
  }
}

void ShowEnter2118() {
  ShowYear(arr0002);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr0021);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr0211);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  ShowYear(arr2118);
  delay(DEFAULT_ENTERDELAY_IN_MS);
  if (isDebugMode) {
    okPin(DATA_PIN);
  }
}
*/
void ShowYear(const uint8_t arr[16][4]) {
  matrix.fillScreen(Colors[Luminance::off]);


  for (int y = 0; y < NEO_MATRIX_HIGHT; y++) {
    uint8_t pos = NEO_MATRIX_WIDTH - 1;
    for (int x = 3; x >= 0; x--)
    {
      for (int i = 0; i < 8; i++) {
        boolean res = arr[y][x] & (0x01 << i);
        if (isDebugMode) {
          Serial.print(pos);
          Serial.print(':');
          Serial.print(res);
          Serial.print(',');
        }
        else {
          matrix.drawPixel(pos, y, (res > 0) ? Colors[DEFAUL_LUMINANCE] : Colors[Luminance::off]);
        }
        if (pos == 0) { break; }
        pos--;
      }
      if (pos == 0) { break; }
    }
    if (isDebugMode) {
      Serial.println();
    }
    matrix.show();
  }
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

void Pause() {
  while (1) {}
}

void showDebugPinAction(int ledPin, uint8_t count) {
  pinMode(ledPin, OUTPUT);

  for (uint8_t i = 0; i < count; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(500);                  // waits for a second
    digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
    delay(500);                  // waits for a second
  }
}

void okPin(int ledPin) {
  static uint32_t statuslightOnTimeInMs = 100;
  if (isDebugMode) {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    delay(statuslightOnTimeInMs);
    digitalWrite(ledPin, LOW);
  }
  else {
    matrix.drawPixel(NEO_MATRIX_WIDTH - 1, NEO_MATRIX_HIGHT - 1, Colors[DEFAUL_LUMINANCE]);
    matrix.show();
    delay(statuslightOnTimeInMs);
    matrix.drawPixel(NEO_MATRIX_WIDTH - 1, NEO_MATRIX_HIGHT - 1, Colors[Luminance::off]);
    matrix.show();
  }
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


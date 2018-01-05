/*
  LEDMatrix

  TODO Beschreibung ???

  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

//#include <SPI.h>
//#include <SD.h>   //Problem Memory (uses 512 Byte on runtime) 

#include "FastLED.h"
//#include <avr/pgmspace.h>

#include <stdlib.h>     /* srand, rand */

#include "LEDSigns.h"

//Pin Configuration
const uint8_t BUTTON_EXEC_PIN = 2;
const uint8_t BUTTON_RESET_PIN = 3;
const uint8_t EVENT_TRIGGER_ACTION = RISING;

const uint8_t FUNCTION_SWITCH_PIN = 4; //TODO whitch pin for Switch

//Debug
const uint8_t DEBUG_READ_PIN = 7;

//Matrix Dimensions
const uint8_t DATA_PIN = 6;
const uint8_t LED_PIN = 13;

const uint8_t NEO_MATRIX_HIGHT = 16;
const uint8_t NEO_MATRIX_WIDTH = 28;
const uint16_t TOTAL_NUMBER_LEDS = NEO_MATRIX_HIGHT * NEO_MATRIX_WIDTH;

CRGB leds[TOTAL_NUMBER_LEDS];

#pragma region Color

const uint8_t MID_COLOR_VAL = 0x7F;
const uint8_t MAX_COLOR_VAL = 0xFF;

const CRGB Colors[] = { CRGB::Black, CRGB(MID_COLOR_VAL, 0, 0), CRGB(MAX_COLOR_VAL, 0, 0) };

enum Luminance : byte { off = 0, mid, max };

const Luminance DEFAUL_LUMINANCE = Luminance::max;

#pragma endregion

//Output
static boolean isDebugMode;

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
const uint32_t DEFAULT_DELAY_IN_MS = 5500;
const uint32_t DEFAULT_ENTERDELAY_IN_MS = 250;

void RegisterInterrupt(const uint8_t, void(*)(), const uint8_t);

void setup() {
  isDebugMode = ScanDebugState(DEBUG_READ_PIN);
  InitializeSerialPortInDebugMode(isDebugMode);
  //InitializeSerialPortInDebugMode(true);

  //Interrupts
  RegisterInterrupt(interruptNextPin, executeNextAction, EVENT_TRIGGER_ACTION);
  RegisterInterrupt(interruptReversePin, executeLastAction, EVENT_TRIGGER_ACTION);

  //Initialize output to Matrix
  InitializeDataPort(isDebugMode, DATA_PIN, TOTAL_NUMBER_LEDS);
}

void loop() {

  uint8_t val = ReadSwitchValue(FUNCTION_SWITCH_PIN);
  bool resetCycleCounter = false;
  static uint8_t cycleCount;

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
  
  switch (val) {
  case 1:
    resetCycleCounter = RunFirst(cycleCount);
    break;
  case 2:
    resetCycleCounter = RunSecond(cycleCount);
    break;
  case 3:
    resetCycleCounter = RunThird(cycleCount);
    break;
  }
  if (resetCycleCounter) {
    cycleCount = 0;
  }
  ResetActionTrigger();
  return;
}

bool RunFirst(const uint8_t cycle) {
  if (cycle == 1) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    ShowYear(arr1884);
    delay(2000);
    if (isDebugMode) {
      okPin(DATA_PIN);
    }
    ShowYear(arr1984);
  }
  else if (cycle == 2) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    ShowYear(arr1884);
  }
  else if (cycle == 3) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    ShowYear(arr2118);
  }
  else if (cycle == 4) {
    ShowYear(arr2118); //TODO set Array 2018
  }
  else if (cycle > 4) {
    Clear();
    delay(500);
    return true;
  }
  return false;
}

bool RunSecond(const uint8_t cycle) {
  return true;
}

bool RunThird(const uint8_t cycle) {
  return true;
}

#pragma region Init

boolean ScanDebugState(const uint8_t inputPin) {
  //Check Debug State
  pinMode(inputPin, INPUT_PULLUP);
  delayMicroseconds(20);
  int val = digitalRead(inputPin);
  delayMicroseconds(20);
  pinMode(inputPin, INPUT);
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
    Serial.println(F("Serial Print on"));
  }
}

#pragma region Interrupts

void RegisterInterrupt(const uint8_t interruptPin, void(*executeAction)(), const uint8_t trigger) {
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

void InitializeDataPort(const boolean isDebugMode, const uint8_t outputPin, const uint16_t numberOfLeds) {
  //For Debug Purpose Only
  if (isDebugMode) {
    pinMode(outputPin, OUTPUT);
    Serial.print(" Init Debug");
  }
  else {
    //it is not possible to use parameter as inputPin
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, numberOfLeds);
    Clear();
  }
}

void Clear() {
  FastLED.clear();
  FastLED.show();
}

#pragma endregion

#pragma region Execution

const uint8_t ReadSwitchValue(const uint8_t pin) {
  return 1; //TODO read switch value
}

void ShowStatusOk() {
  okPin(DATA_PIN);
  for (int i = 0; i < 20; i++) {
    delay(DEFAULT_ENTERDELAY_IN_MS);
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
  //TODO add interrupt
  uint32_t start = millis();
  uint32_t end = start + ms;
  if (isDebugMode) {
    Serial.print(" Show Debug");
    showDebugPinAction(DATA_PIN, count);
  }
  else {
    do {
      FillMatrixRandom(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT);
      FastLED.show();
    } while (millis() < end);
  }
}

void FillMatrixRandom(const uint8_t sizeX, const uint8_t sizeY) {
  uint16_t numberLeds = sizeX * sizeY;
  FillMatrixRandom(numberLeds);
}

void FillMatrixRandom(const uint16_t numberLeds) {
  for (int i = 0; i < numberLeds; i++) {
    leds[i] = Colors[rand() % (sizeof(Colors) / sizeof(CRGB))];
  }
}

void ShowYear(const uint8_t arr[16][4]) {
  Clear();
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
          leds[XY(pos, y, true)] = res > 0 ? Colors[DEFAUL_LUMINANCE] : Colors[Luminance::off];
        }
        if (pos == 0) { break; }
        pos--;
      }
      if (pos == 0) { break; }
    }
    if (isDebugMode) {
      Serial.println();
    }
  }
  FastLED.show();
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
    leds[0] = Colors[Luminance::mid];
    FastLED.show();
    delay(statuslightOnTimeInMs);
    leds[0] = Colors[Luminance::off];
    FastLED.show();
  }
}

void errorPin(int ledPin) {

  long delayInMs = 250;
  if (isDebugMode) {
    pinMode(ledPin, OUTPUT);
    while (true) {
      digitalWrite(ledPin, HIGH);
      delay(delayInMs);
      digitalWrite(ledPin, LOW);
      delay(delayInMs);
    }
  }
  else {
    leds[0] = Colors[Luminance::mid];
    FastLED.show();
    delay(delayInMs);
    leds[0] = Colors[Luminance::off];
    FastLED.show();
    delay(delayInMs);
  }
}

uint16_t XY(uint8_t x, uint8_t y, bool kMatrixSerpentineLayout)
{
  uint16_t i;

  if (kMatrixSerpentineLayout == false) {
    i = (y * NEO_MATRIX_WIDTH) + x;
  }
  else
  {
    uint8_t stepsX;
    if (y & 0x01) {
      // Even rows run backwards
      stepsX = x;
    }
    else {
      // Odd rows run forwards
      stepsX = (NEO_MATRIX_WIDTH - 1) - x;
    }
    i = (y * NEO_MATRIX_WIDTH) + stepsX;
  }

  return TOTAL_NUMBER_LEDS - 1 - i; //starting point is TOP LEFT instead BOTTOM RIGHT
}

#pragma endregion

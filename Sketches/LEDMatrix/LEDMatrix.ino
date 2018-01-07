/*
  LEDMatrix

  TODO Beschreibung ???

  Online on Bitbucket https://bitbucket.org/markoell/arduino-neomatrix
  Get latest source code: 'git clone https://markoell@bitbucket.org/markoell/arduino-neomatrix.git'
*/

//#include <SPI.h>
//#include <SD.h>   //Problem Memory (uses 512 Byte on runtime) 

#define ARDUINO_SAMD_ZERO
#include "FastLED.h"
//#include <avr/pgmspace.h>

#include <stdlib.h>     /* srand, rand */

#include "LEDSigns.h"

//Pin Configuration
const uint8_t BUTTON_EXEC_PIN = 3;
const uint8_t EVENT_TRIGGER_ACTION = CHANGE;
const long EXPECTABLE_PUSH_BUFFER_IN_MS = 130;
const long EXTRA_TIME_BUFFER = 100;

const uint8_t FUNCTION_SWITCH_DATA_PIN = 9;
const uint8_t FUNCTION_SWITCH_LATCH_PIN = 8;
const uint8_t FUNCTION_SWITCH_CLOCK_PIN = 7;

//Debug
const uint8_t DEBUG_READ_PIN = 5;

//Matrix Dimensions
const uint8_t DATA_PIN = 6;
const uint8_t LED_PIN = LED_BUILTIN;

const uint8_t NEO_MATRIX_HIGHT = 16;
const uint8_t NEO_MATRIX_WIDTH = 28;
const uint16_t TOTAL_NUMBER_LEDS = NEO_MATRIX_HIGHT * NEO_MATRIX_WIDTH;

CRGB leds[TOTAL_NUMBER_LEDS];

#pragma region Color

const uint8_t MID_COLOR_VAL = 0x7F;
const uint8_t MAX_COLOR_VAL = 0xFF;

const CRGB Colors[] = { CRGB::Black, CRGB(MID_COLOR_VAL, 0, 0), CRGB(MAX_COLOR_VAL, 0, 0), (0, MID_COLOR_VAL, 0), (0, MAX_COLOR_VAL, 0), (0, 0, MID_COLOR_VAL), (0, 0, MAX_COLOR_VAL), (MAX_COLOR_VAL, MAX_COLOR_VAL, MAX_COLOR_VAL) };

enum Luminance : byte { off = 0, rmid, rmax, gmid, gmax, bmid, bmax, white };

const Luminance DEFAULT_LUMINANCE = Luminance::rmax;

#pragma endregion

//Output
static boolean isDebugMode;

// Interrupts
const uint8_t interruptNextPin = BUTTON_EXEC_PIN;
volatile boolean nextAction = false;
volatile long timeDiff = 0;

//Global
const uint32_t DEFAULT_DELAY_IN_MS = 1200;
const uint32_t DEFAULT_ENTERDELAY_IN_MS = 250;

void RegisterInterrupt(const uint8_t, void(*)(), const uint8_t);

void setup() {
  isDebugMode = ScanDebugState(DEBUG_READ_PIN);
  InitializeSerialPortInDebugMode(isDebugMode);
  //InitializeSerialPortInDebugMode(true);

  //Interrupts
  RegisterInterrupt(interruptNextPin, executeNextAction, EVENT_TRIGGER_ACTION);

  //Initialize Switch
  InitializeSwitch();
  
  //Initialize output to Matrix
  InitializeDataPort(isDebugMode, DATA_PIN, TOTAL_NUMBER_LEDS);
}

void loop() {

  uint8_t val = ReadSwitchValue(FUNCTION_SWITCH_DATA_PIN);
  bool resetCycleCounter = false;
  static uint8_t cycleCount;

  if(millis() >= timeDiff + EXTRA_TIME_BUFFER){
    ResetSwitchTimeBuffer();
  }
  
  if (nextAction == false) { 
    if (cycleCount == 0)
    {
      BlinkStatusLedOk();
    }
    return;
  }

  if (nextAction == true) {
    cycleCount++;
  }
  
  switch (val) {
  case 1:
    resetCycleCounter = Run1(cycleCount);
    break;
  case 2:
    resetCycleCounter = Run2(cycleCount);
    break;
  case 3:
    resetCycleCounter = Run3(cycleCount);
    break;
  }
  if (resetCycleCounter) {
    cycleCount = 0;
  }
  ResetActionTrigger();
  return;
}

bool Run1(const uint8_t cycle) {
  if (cycle == 1) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(arr1884);
    delay(2000);
    DisplayValue(arr1984);
  }
  else if (cycle == 2) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(arr1884);
  }
  else if (cycle == 3) {
    Flicker(DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(arr2118);
  }
  else {
    Clear();
    delay(500);
    return true;
  }
  return false;
}

void ResetSwitchTimeBuffer(){
  timeDiff = 0;
}

bool Run2(const uint8_t cycle) {
  if (cycle == 1) {
    Flicker(Colors[Luminance::bmax], DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(Colors[Luminance::bmax], arr2018);
  }
  else if (cycle == 2) {
    Flicker(Colors[Luminance::bmax], DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(Colors[Luminance::bmax], arr1965);
  }
  else {
    Clear();
    delay(500);
    return true;
  }
  return false;
}

bool Run3(const uint8_t cycle) {
  if (cycle == 1) {
    //DisplayValue(Colors[Luminance::bmax], arr60);
  }
  else if (cycle == 2) {
    //DisplayValue(Colors[Luminance::bmax], arr70);
  }
  else if (cycle == 3) {
    //DisplayValue(Colors[Luminance::bmax], arr80);
  }
  else if (cycle == 3) {
    DisplayDisco();
  }
  else {
    Clear();
    delay(500);
    return true;
  }
  return false;
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

void InitializeSwitch(){
  // TODO Init
}

#pragma region Interrupts

void RegisterInterrupt(const uint8_t interruptPin, void(*executeAction)(), const uint8_t trigger) {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), executeAction, trigger);
}
void executeNextAction() {
  if (isDebugMode && Serial) {
    Serial.print("Trigger Next");
  }
  if(0 == timeDiff){
    timeDiff = millis() + EXPECTABLE_PUSH_BUFFER_IN_MS;
    return;
  }
  else if(timeDiff >= millis()){
    return;
  }
  else{
    ResetSwitchTimeBuffer();
  }
  nextAction = true;
}

#pragma endregion

#pragma region Output

void InitializeDataPort(const boolean isDebugMode, const uint8_t outputPin, const uint16_t numberOfLeds) {
  //For Debug Purpose Only
  if (isDebugMode) {
    pinMode(outputPin, OUTPUT);
    if(Serial) {
      Serial.print(" Init Debug");
    }
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

void BlinkStatusLedOk() {
  if(isDebugMode){
    BlinkStatusLedOk(LED_PIN);
  }
  else{
    BlinkStatusLedOk(DATA_PIN);
  }
  for (int i = 0; i < 20; i++) {
    delay(DEFAULT_ENTERDELAY_IN_MS);
    if (nextAction == true) { return; }
  }
}

void ResetActionTrigger() {
  if (isDebugMode && Serial) {
    Serial.println("Reset");
  }
  nextAction = false;
}

void Flicker(long ms, uint8_t count) {
  Flicker(Colors[DEFAULT_LUMINANCE], ms, count);
}

void Flicker(const CRGB color, long ms, uint8_t count) {
  //TODO add interrupt
  long start = millis();
  long end = start + ms;
  if (isDebugMode) {
    if(Serial){
      Serial.print(F("Show Debug"));
    }
    showDebugPinAction(LED_PIN, count);
  }
  else {
    nextAction = false;
    do {
      FillMatrixRandom(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT);
      FastLED.show();
    } while (millis() < end && nextAction == false);
    nextAction = true;
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

void DisplayValue(const uint8_t arr[16][4]) {
  DisplayValue(Colors[DEFAULT_LUMINANCE], arr);
}

void DisplayValue(const CRGB color, const uint8_t arr[16][4]) {
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
          leds[XY(pos, y, true)] = res > 0 ? color : Colors[Luminance::off];
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

void DisplayDisco() {
  nextAction = false;
  while (nextAction == false) {

    //DisplayValue(Colors[Luminance::bmax], Disco); //TODO
  }
}

#pragma endregion

#pragma region DebugMethods

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

void BlinkStatusLedOk(uint32_t ledPin) {
  static uint32_t statuslightOnTimeInMs = 100;
  if (isDebugMode) {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    delay(statuslightOnTimeInMs);
    digitalWrite(ledPin, LOW);
  }
  else {
    leds[0] = Colors[Luminance::rmid];
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
    leds[0] = Colors[Luminance::rmid];
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

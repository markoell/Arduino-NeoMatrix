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
const unsigned long EXPECTABLE_PUSH_BUFFER_IN_MS = 200;
const unsigned long EXTRA_TIME_BUFFER = 300;

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

const CRGB ColorsRed[] = { CRGB::Black, CRGB(MID_COLOR_VAL, 0, 0), CRGB::Red };
const CRGB ColorsGreen[] = { CRGB::Black, (0, MID_COLOR_VAL, 0), CRGB::Green };
const CRGB ColorsBlue[] = { CRGB::Black, (0, 0, MID_COLOR_VAL), CRGB::Blue };
const CRGB ColorsWhite[] = { CRGB::Black, (MID_COLOR_VAL, MID_COLOR_VAL, MID_COLOR_VAL), CRGB::White };

enum Luminance : byte { off = 0, mid, max };

const Luminance DEFAULT_LUMINANCE = Luminance::max;

#pragma endregion

//Output
static boolean isDebugMode;

// Interrupts
const uint8_t interruptNextPin = BUTTON_EXEC_PIN;
volatile boolean nextAction = false;
volatile unsigned long timeDiff = 0;

//Global
const unsigned long DEFAULT_DELAY_IN_MS = 5500;
const unsigned long DEFAULT_ENTERDELAY_IN_MS = 250;

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
  bool resetCycleCounter = false;
  static uint8_t cycleCount;
  static uint16_t oldSwitchvalue = 1; 

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

  uint8_t switchValue = ReadSwitchValue();

  if (oldSwitchvalue != switchValue) {
    cycleCount = 0;
    oldSwitchvalue = switchValue;
  }
  
  switch (switchValue) {
  case 1:
    resetCycleCounter = Run1(cycleCount);
    break;
  case 2:
    resetCycleCounter = Run2(cycleCount);
    break;
  case 3:
    resetCycleCounter = Run3(cycleCount);
    break;
  default:
    resetCycleCounter = true;
    break;
  }
  if (resetCycleCounter) {
    cycleCount = 0;
  }
  ResetActionTrigger();
  return;
}

bool Run1(const uint8_t cycle) {
  const CRGB *usedColors = ColorsRed;
  const CRGB displayColor = usedColors[DEFAULT_LUMINANCE];
  if (cycle == 1) {
    Flicker(usedColors, DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(displayColor, arr1884);
    delay(2000);
    DisplayValue(displayColor, arr1984);
  }
  else if (cycle == 2) {
    Flicker(usedColors, DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(displayColor, arr1884);
  }
  else if (cycle == 3) {
    Flicker(usedColors, DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(displayColor, arr2118);
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
  const CRGB *usedColors = ColorsBlue;
  if (cycle == 1) {
    Flicker(usedColors, DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(usedColors[Luminance::max], arr2018);
  }
  else if (cycle == 2) {
    Flicker(usedColors, DEFAULT_DELAY_IN_MS, cycle);
    DisplayValue(usedColors[Luminance::max], arr1965);
  }
  else {
    Clear();
    delay(500);
    return true;
  }
  return false;
}

bool Run3(const uint8_t cycle) {
  CRGB color = ColorsWhite[DEFAULT_LUMINANCE];
  if (cycle == 1) {
    DisplayValue(color, arr60er);
  }
  else if (cycle == 2) {
    DisplayValue(color, arr70er);
  }
  else if (cycle == 3) {
    DisplayValue(color, arr80er);
  }
  else if (cycle == 4) {
    DisplayDisco(color);
    Clear();
    return true;
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
    SerialUSB.begin(9600); // opens serial port, sets data rate to 9600 bps
    while (!SerialUSB) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    SerialUSB.println(F("Serial Print on"));
  }
}

void InitializeSwitch(){

  pinMode(FUNCTION_SWITCH_LATCH_PIN, OUTPUT);
  pinMode(FUNCTION_SWITCH_CLOCK_PIN, OUTPUT);
  pinMode(FUNCTION_SWITCH_DATA_PIN, INPUT);

  }

#pragma region Interrupts

void RegisterInterrupt(const uint8_t interruptPin, void(*executeAction)(), const uint8_t trigger) {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), executeAction, trigger);
}
void executeNextAction() {
  if (isDebugMode && SerialUSB) {
    SerialUSB.print("Trigger Next");
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
    if(SerialUSB) {
      SerialUSB.print(" Init Debug");
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

const uint8_t ReadSwitchValue() {

  uint16_t switchVar1 = 0;
  ExecuteParallelRead(FUNCTION_SWITCH_LATCH_PIN);
  switchVar1 = shiftIn(FUNCTION_SWITCH_DATA_PIN, FUNCTION_SWITCH_CLOCK_PIN);


  if (switchVar1 == 0) {
    return 1;
  }

  uint8_t position = 0;
  while (position < 13) {
    if (switchVar1 == 1 << position++) {
      return position;
    }
  }
  return 1;
}

uint16_t shiftIn(int myDataPin, int myClockPin) {
  int i;
  int temp = 0;
  int pinState;
  uint16_t myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
  
  for (i = 15; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    
    digitalWrite(myClockPin, 1);
  }
  return myDataIn & 0x0FFF;
}

void ExecuteParallelRead(uint8_t latchPin) {
  digitalWrite(latchPin, HIGH);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchPin, LOW);
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
  if (isDebugMode && SerialUSB) {
    SerialUSB.println("Reset");
  }
  nextAction = false;
}

void Flicker(const CRGB usedColors[], long ms, uint8_t count) {
  unsigned long start = millis();
  unsigned long end = start + ms;
  if (isDebugMode) {
    showDebugPinAction(LED_PIN, count);
  }
  else {
    nextAction = false;
    unsigned long tmpTime = 0;
    do {
      FillMatrixRandom(NEO_MATRIX_WIDTH, NEO_MATRIX_HIGHT, usedColors);
      FastLED.show();
      delay(50);

      tmpTime = millis();
    } while (tmpTime < end && nextAction == false);
    nextAction = true;
  }
}

void FillMatrixRandom(const uint8_t sizeX, const uint8_t sizeY, const CRGB usedColors[]) {
  uint16_t numberLeds = sizeX * sizeY;
  FillMatrixRandom(numberLeds, usedColors);
}

void FillMatrixRandom(const uint16_t numberLeds, const CRGB usedColors[]) {
  for (int i = 0; i < numberLeds; i++) {
    leds[i] = usedColors[rand() % (int) DEFAULT_LUMINANCE];
  }
}

void DisplayValue(const CRGB color, const uint8_t arr[16][4]) {
  Clear();
  for (int y = 0; y < NEO_MATRIX_HIGHT; y++) {
    uint8_t pos = NEO_MATRIX_WIDTH - 1;
    for (int x = 3; x >= 0; x--)
    {
      for (int i = 0; i < 8; i++) {
        boolean res = arr[y][x] & (0x01 << i);
        if (isDebugMode && SerialUSB) {
          SerialUSB.print(pos);
          SerialUSB.print(':');
          SerialUSB.print(res);
          SerialUSB.print(',');
        }
        else {
          leds[XY(pos, y, true)] = res > 0 ? color : ColorsWhite[Luminance::off];
        }
        if (pos == 0) { break; }
        pos--;
      }
      if (pos == 0) { break; }
    }
    if (isDebugMode && SerialUSB) {
      SerialUSB.println();
    }
  }
  FastLED.show();
}

void DisplayDisco(const CRGB color) {
  const int stepSize = 2;

  Clear();

  nextAction = false;
  static int counter = 0;
  /*
  boolean disco[36][NEO_MATRIX_HIGHT];
  SerialUSB.println("Disko1");
  for (int y = 0; y < NEO_MATRIX_HIGHT; y++) {
    uint8_t pos = 35;
    for (int x = 4; x >= 0; x--) {
      for (int i = 0; i < 8; i++) {
        disco[pos][y] = arrDisco[x][y] & (0x01 << i);
        if (pos == 0) { break; }
        pos--;
      }
      if (pos == 0) { break; }
    }
  }
  if(SerialUSB) SerialUSB.println("Disko2");

  uint8_t count = 1;
  */
  while (nextAction == false) {
    delay(DEFAULT_ENTERDELAY_IN_MS);
    //SerialUSB.println("Disko3");

    //int discoX = 0;
    //if (count % stepSize == 0) {
    //  SerialUSB.println("Disko4");

    //  int tmpX = discoX;
    //  for (int x = 0; x < NEO_MATRIX_WIDTH; x++) {
    //    for (int y = 0; y < NEO_MATRIX_HIGHT; y++) {
    //      SerialUSB.print("Disko5: ");
    //      SerialUSB.print(x);
    //      SerialUSB.print("; ");
    //      SerialUSB.print(y);
    //      SerialUSB.print("; ");
    //      SerialUSB.print(tmpX);
    //      SerialUSB.print("; ");

    //      if (x < NEO_MATRIX_WIDTH - stepSize) {
    //        SerialUSB.print("Disko6: ");
    //        leds[XY(x, y, true)] = leds[XY(x + stepSize, y, true)];
    //      }
    //      else {
    //        SerialUSB.print("Disko7: ");
    //        if (tmpX < 36) {
    //          SerialUSB.print("Disko8: ");
    //          leds[XY(x, y, true)] = disco[tmpX][y] ? color : ColorsWhite[Luminance::off];
    //        }
    //        else if (tmpX < 40) {
    //          SerialUSB.print("Disko9: ");
    //          leds[XY(x, y, true)] = ColorsWhite[Luminance::off];
    //        }
    //        else {
    //          SerialUSB.print("Disko10: ");
    //          discoX = 0;
    //        }
    //        tmpX++;
    //      }
    //      SerialUSB.print("Disko11: ");
    //      SerialUSB.println(leds[XY(x, y, true)]);
    //    }
    //  }
    //  FastLED.show();
    //}
    //
    //
    //count++;
    
    switch (counter)
    {
    case 1:
      DisplayValue(color, arrDisco1); //TODO
      break;
    case 2:
      DisplayValue(color, arrDisco2); //TODO
      break;
    case 3:
      DisplayValue(color, arrDisco3); //TODO
      break;
    case 4:
      DisplayValue(color, arrDisco4); //TODO
      break;
    case 5:
      DisplayValue(color, arrDisco5); //TODO
      break;
    case 6:
      DisplayValue(color, arrDisco6); //TODO
      break;
    case 7:
      DisplayValue(color, arrDisco7); //TODO
      break;
    case 8:
      DisplayValue(color, arrDisco8); //TODO
      break;
    case 9:
      DisplayValue(color, arrDisco9); //TODO
      break;
    case 10:
      DisplayValue(color, arrDisco10); //TODO
      break;
    case 11:
      DisplayValue(color, arrDisco11); //TODO
      break;
    case 12:
      DisplayValue(color, arrDisco12); //TODO
      break;
    case 13:
      DisplayValue(color, arrDisco13); //TODO
      break;
    case 14:
      DisplayValue(color, arrDisco14); //TODO
      break;
    case 15:
      DisplayValue(color, arrDisco15); //TODO
      counter = 6;
      continue;
    default:
      break;
    }
    counter++;
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
    leds[0] = ColorsRed[Luminance::mid];
    FastLED.show();
    delay(statuslightOnTimeInMs);
    leds[0] = ColorsRed[Luminance::off];
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
    leds[0] = ColorsRed[Luminance::mid];
    FastLED.show();
    delay(delayInMs);
    leds[0] = ColorsRed[Luminance::off];
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

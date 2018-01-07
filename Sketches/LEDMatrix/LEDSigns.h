#ifndef LEDSigns_h
#define LEDSigns_h

#include "Arduino.h"

const uint8_t arr1884[][4] = {
  { 0x1, 0x1F, 0x1F, 0x36 },
  { 0x3, 0x3F, 0xBF, 0xB6 },
  { 0x7, 0x31, 0xB1, 0xB6 },
  { 0xF, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x3F, 0xBF, 0xBF },
  { 0x3, 0x1F, 0x1F, 0x3F },
  { 0x3, 0x3F, 0xBF, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x7, 0xBF, 0xBF, 0x86 },
  { 0x7, 0x9F, 0x1F, 0x6 }
};

const uint8_t arr1965[][4] = {
  { 0x1, 0x1F, 0x1F, 0x3F },
  { 0x3, 0x3F, 0xBF, 0xBF },
  { 0x7, 0x31, 0xB1, 0xB0 },
  { 0xF, 0x31, 0xB0, 0x30 },
  { 0x3, 0x31, 0xB0, 0x30 },
  { 0x3, 0x31, 0xB0, 0x30 },
  { 0x3, 0x3F, 0xBF, 0x3E },
  { 0x3, 0x3F, 0xBF, 0x9F },
  { 0x3, 0x1F, 0xBF, 0x87 },
  { 0x3, 0x01, 0xB1, 0x83 },
  { 0x3, 0x01, 0xB1, 0x83 },
  { 0x3, 0x01, 0xB1, 0x83 },
  { 0x3, 0x01, 0xB1, 0x83 },
  { 0x3, 0x31, 0xB1, 0xB3 },
  { 0x7, 0xBF, 0xBF, 0xBF },
  { 0x7, 0x9F, 0x1F, 0x1E }
};

const uint8_t arr1984[][4] = {
  { 0x1, 0x1F, 0x1F, 0x36 },
  { 0x3, 0x3F, 0xBF, 0xB6 },
  { 0x7, 0x31, 0xB1, 0xB6 },
  { 0xF, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x31, 0xB1, 0xB6 },
  { 0x3, 0x3F, 0xBF, 0xBF },
  { 0x3, 0x3F, 0x9F, 0x3F },
  { 0x3, 0x1F, 0xBF, 0x86 },
  { 0x3, 0x01, 0xB1, 0x86 },
  { 0x3, 0x01, 0xB1, 0x86 },
  { 0x3, 0x01, 0xB1, 0x86 },
  { 0x3, 0x01, 0xB1, 0x86 },
  { 0x3, 0x31, 0xB1, 0x86 },
  { 0x7, 0xBF, 0xBF, 0x86 },
  { 0x7, 0x9F, 0x1F, 0x06 }
};

const uint8_t arr2018[][4] = {
  { 0x7, 0xC7, 0x82, 0x3E },
  { 0xF, 0xEF, 0xC6, 0x7F },
  { 0xC, 0x6C, 0xCE, 0x63 },
  { 0x0, 0x6C, 0xDE, 0x63 },
  { 0x0, 0x6C, 0xC6, 0x63 },
  { 0x0, 0xCC, 0xC6, 0x63 },
  { 0x0, 0xCC, 0xC6, 0x7F },
  { 0x1, 0x8C, 0xC6, 0x3E },
  { 0x1, 0x8C, 0xC6, 0x7F },
  { 0x3, 0x0C, 0xC6, 0x63 },
  { 0x3, 0x0C, 0xC6, 0x63 },
  { 0x6, 0x0C, 0xC6, 0x63 },
  { 0x6, 0x0C, 0xC6, 0x63 },
  { 0xC, 0x0C, 0xC6, 0x63 },
  { 0xF, 0xEF, 0xCF, 0x7F },
  { 0xF, 0xE7, 0x8F, 0x3E }
};

const uint8_t arr2118[][4] = {
  { 0x7, 0xE0, 0x82, 0x3E },
  { 0xF, 0xF1, 0x86, 0x7F },
  { 0xC, 0x33, 0x8E, 0x63 },
  { 0x0, 0x37, 0x9E, 0x63 },
  { 0x0, 0x31, 0x86, 0x63 },
  { 0x0, 0x31, 0x86, 0x63 },
  { 0x0, 0x71, 0x86, 0x7F },
  { 0x0, 0xE1, 0x86, 0x3E },
  { 0x3, 0x81, 0x86, 0x7F },
  { 0x7, 0x01, 0x86, 0x63 },
  { 0xE, 0x01, 0x86, 0x63 },
  { 0xC, 0x01, 0x86, 0x63 },
  { 0xC, 0x01, 0x86, 0x63 },
  { 0xC, 0x01, 0x86, 0x63 },
  { 0xF, 0xF3, 0xCF, 0x7F },
  { 0xF, 0xF3, 0xCF, 0x3E }
};

#endif

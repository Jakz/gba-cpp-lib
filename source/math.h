#pragma once

#include <cstdlib>
#include <type_traits>

#include "common.h"
#include "fixed_point.h"

class math
{
private:
  static constexpr size_t SIN_TABLE_SIZE = 512;
  static fpp sinLookupTable[SIN_TABLE_SIZE];
  
public:
  static void init();
  
  /* returns sin/cos function in 4.12 fixed point format, input range 0x0000 - 0x01FF (9 bits, 0-511) */
  static fpp sin(u16 a);
  static fpp cos(u16 a);
  
  static constexpr u16 PI = 256;
};
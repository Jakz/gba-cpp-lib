#include "math.h"

#include <cmath>

static constexpr float FPI = 3.14159265358979323846f;

fpp math::sinLookupTable[SIN_TABLE_SIZE];

void math::init()
{
  float incr = (2 * FPI) / SIN_TABLE_SIZE;
  
  for (size_t i = 0; i < SIN_TABLE_SIZE; ++i)
    sinLookupTable[i] = fpp(sinf(incr*i));
}

fpp math::sin(u16 a)
{
  return sinLookupTable[a & 0x01FF];
}

fpp math::cos(u16 a)
{
  return sinLookupTable[(a + math::PI/2) & 0x01FF];
}

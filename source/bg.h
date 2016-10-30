#pragma once

#include "common.h"
#include "fixed_point.h"
#include "math.h"

enum class bg_priority
{
  LOWER = 3,
  LOW = 2,
  HIGH = 1,
  HIGHER = 0
};

struct bg_cnt
{
  enum
  {
    BG_PRIORITY_MASK = 0x3
  };

  u16 value;
  
  void setPriority(bg_priority priority) { value = (value & ~BG_PRIORITY_MASK) | static_cast<u32>(priority); }
  bg_priority priority() { return static_cast<bg_priority>(value & BG_PRIORITY_MASK); }
};
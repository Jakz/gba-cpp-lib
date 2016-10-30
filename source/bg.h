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

enum class bg_size
{
  _256x256 = 0,
  _512x256 = 1,
  _256x512 = 2,
  _512x512 = 3
};

struct bg_cnt
{
  enum
  {
    BG_PRIORITY_MASK          = 0x0003,
    BG_CHAR_BASE_BLOCK_MASK   = 0x000C,
    BG_MOSAIC_FLAG            = 0x0040,
    BG_COLOR_MODE             = 0x0080,
    BG_SCREEN_BASE_BLOCK_MASK = 0x1F00,
    BG_DISPLAY_OVERFLOW       = 0x2000,
    BG_SCREEN_SIZE_MASK       = 0xC000,

    BG_CHAR_BASE_BLOCK_SHIFT = 2,
    BG_SCREEN_BASE_BLOCK_SHIFT = 8,
    BG_SCREEN_SIZE_SHIFT = 14
  };

  u16 value;
  
  void setPriority(bg_priority priority) { value = (value & ~BG_PRIORITY_MASK) | static_cast<u32>(priority); }
  bg_priority priority() { return static_cast<bg_priority>(value & BG_PRIORITY_MASK); }
  
  void setCharBaseBlock(u32 index) { value = (value & ~BG_CHAR_BASE_BLOCK_MASK) | (index << BG_CHAR_BASE_BLOCK_SHIFT);  }

  void setScreenBaseBlock(u32 index) { value = (value & ~BG_SCREEN_BASE_BLOCK_MASK) | (index << BG_SCREEN_BASE_BLOCK_SHIFT);  }

  void setSize(bg_size size) { value = (value & ~BG_SCREEN_SIZE_MASK) | (static_cast<u32>(size) << BG_SCREEN_SIZE_SHIFT); }
};
#pragma once

#include "common.h"

enum class timer_freq
{
  MHZ16 = 0,
  KHZ262k,
  KHZ65k,
  KHZ16k
};

template<addr_t BASE>
struct timer
{
  u16 pvalue;
  u16 pcnt;
  
  u16 value() { return pvalue; }
  void setReload(fu16 v) { pvalue = v; }
  
  void setFrequency(timer_freq f) { pcnt = (pcnt & ~0x0003) | static_cast<u16>(f); }
};

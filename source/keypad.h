#pragma once

#include "common.h"

enum Key : u16
{
  KEY_NONE = 0x0000,
  KEY_A = 0x0001,
  KEY_B = 0x0002,
  KEY_SELECT = 0x0004,
  KEY_START = 0x0008,
  
  KEY_RIGHT = 0x0010,
  KEY_LEFT = 0x0020,
  KEY_UP = 0x0040,
  KEY_DOWN = 0x0080,
  
  KEY_R = 0x0100,
  KEY_L = 0x0200
};

struct KeyStatus
{
  const u16 status;
  
  KeyStatus(u16 status) : status(status) { }
  
  bool isPressed(Key key) const { return !(status & key); }
};


class Keypad
{
public:
  const KeyStatus* poll() const { return reinterpret_cast<const KeyStatus*>(as<u16>(PORT_KEYINPUT)); }
};
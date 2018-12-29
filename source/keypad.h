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
  u16 status;
  
  KeyStatus(u16 status) : status(status) { }
  bool isPressed(Key key) const { return !(status & (u16)key); }
};


class Keypad
{
public:
  static const KeyStatus* poll() { return reinterpret_cast<const KeyStatus*>(as<u16>(PORT_KEYINPUT)); }
};

class KeypadManager
{
  KeyStatus current;
  KeyStatus previous;
  
public:
  KeypadManager() : current(0), previous(0) { }
  
  void update()
  {
    previous = current;
    current = *Keypad::poll();
  }
  
  bool isReleased(Key key) { return !current.isPressed(key) && previous.isPressed(key); }
  bool isPressed(Key key) { return current.isPressed(key) && !previous.isPressed(key); }
};

class KeypadDelayer
{
private:
  Key _key;
  u16 mod;
  u16 counter;
  
public:
  KeypadDelayer(u16 mod) : _key(Key::KEY_NONE), counter(0), mod(mod) { }
  
  inline Key key() const { return _key; }
  inline void setKey(Key key) { _key = key; }
  inline void reset() { counter = 0; }
  
  bool incrementAndCheck(Key key)
  {
    if (key != _key)
    {
      setKey(key);
      reset();
    }
    
    ++counter;
    if (counter == mod)
    {
      counter = 0;
      return true;
    }
    
    return false;
  }
};

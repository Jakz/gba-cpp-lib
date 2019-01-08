#pragma once

#include "common.h"
#include "fixed_point.h"
#include "math.h"

enum class priority
{
  HIGHEST = 0,
  HIGH = 1,
  LOW = 2,
  LOWER = 3
};

enum class bg_screen_size
{
  SIZE32x32 = 0,
  SIZE64x32 = 1,
  SIZE32x64 = 2,
  SIZE64x64 = 3
};

struct port_bg_cnt
{
  enum
  {
    PRIORITY_MASK         = 0x0003,
    CHAR_BASE_BLOCK_MASK  = 0x000C,
    CHAR_BASE_BLOCK_SHIFT = 3,
    MOSAIC_FLAG           = 0x0040,
    COLOR_MODE_FLAG       = 0x0080,
    
    SCREEN_BLOCK_MASK     = 0x1F00,
    SCREEN_BLOCK_SHIFT    = 8,
    
    OVERFLOW_MODE_FLAG    = 0x2000,
    
    SCREEN_SIZE_MASK      = 0xC000,
    SCREEN_SIZE_SHIFT     = 13
  };
  
  u16 value;
  
  inline void set(u16 v) { value = v; }
  
  inline void setPriority(fu16 v) { value = (value & ~PRIORITY_MASK) | v; }
  inline void setTileDataBlock(fu16 v) { value = (value & ~CHAR_BASE_BLOCK_MASK) | (v << CHAR_BASE_BLOCK_SHIFT); }
  inline void setTileMapBlock(fu16 v) { value = (value & ~SCREEN_BLOCK_MASK) | (v << SCREEN_BLOCK_SHIFT); }
  inline void setScreenSize(bg_screen_size v) { value = (value & ~SCREEN_SIZE_MASK) | (static_cast<u16>(v) << SCREEN_SIZE_SHIFT); }
  
  inline void enableMosaic() { value |= MOSAIC_FLAG; }
  inline void disableMosaic() { value &= ~MOSAIC_FLAG; }
  
  inline void setBlocks(fu16 data, fu16 map) { value = (value & ~(CHAR_BASE_BLOCK_MASK|CHAR_BASE_BLOCK_SHIFT)) | (data << CHAR_BASE_BLOCK_SHIFT) | (map << SCREEN_BLOCK_SHIFT); }
  
  static inline addr_t tileDataAtIndex(fu16 i) { return VRAM_BASE + TILE_DATA_BASE_MULTIPLIER*i; }
  static inline addr_t tileMapAtIndex(fu16 i) { return VRAM_BASE + TILE_MAP_BASE_MULTIPLIER*i; }
};

using port_bg_hofs = u16;
using port_bg_vofs = u16;

template<addr_t BASE, addr_t POSX, addr_t POSY>
struct bg_entry
{
  inline void setOffset(fu16 x, fu16 y) { setX(x); setY(y); }
  inline void setBlocks(fu16 data, fu16 map) { setTileDataBlock(data); setTileMapBlock(map); }
  
  inline void setX(fu16 x) { *as<port_bg_hofs>(POSX) = x; }
  inline void setY(fu16 y) { *as<port_bg_vofs>(POSY) = y; }
  inline void setPriority(fu16 v) { as<port_bg_cnt>(BASE)->setPriority(v); }
  inline void setTileDataBlock(fu16 v) { as<port_bg_cnt>(BASE)->setTileDataBlock(v); }
  inline void setTileMapBlock(fu16 v) { as<port_bg_cnt>(BASE)->setTileMapBlock(v); }
  inline void setScreenSize(bg_screen_size v) { as<port_bg_cnt>(BASE)->setScreenSize(v); }
};

using bg0_entry = bg_entry<PORT_BG0CNT, PORT_BG0HOFS, PORT_BG0VOFS>;
using bg1_entry = bg_entry<PORT_BG1CNT, PORT_BG1HOFS, PORT_BG1VOFS>;
using bg2_entry = bg_entry<PORT_BG2CNT, PORT_BG2HOFS, PORT_BG2VOFS>;
using bg3_entry = bg_entry<PORT_BG3CNT, PORT_BG3HOFS, PORT_BG3VOFS>;

struct tile_entry
{
  enum
  {
    INDEX_MASK =    0x03FF,
    HOR_FLIP_FLAG = 0x0400,
    VER_FLIP_FLAG = 0x0800,
    PALETTE_MASK  = 0xF000,
    
    HOR_FLIP_SHIFT = 10,
    VER_FLIP_SHIT  = 11,
    PALETTE_SHIFT  = 12
  };
  
  u16 value;
  
  inline void set(u16 value) { this->value = value; }
  
  inline void setIndex(fu16 v) { value = (value & ~INDEX_MASK) | v; }
  inline void setPalette(fu16 i) { value = (value & ~PALETTE_MASK) | (i << PALETTE_SHIFT); }
  
  inline void flipHorizontal() { value |= HOR_FLIP_FLAG; }
  inline void flipVertical() { value |= VER_FLIP_FLAG; }
  
  inline void set(fu16 index, fu16 palette, bool flipH, bool flipV)
  {
    value = index | (palette << PALETTE_SHIFT) | (flipH << HOR_FLIP_SHIFT) | (flipV << VER_FLIP_SHIT);
  }
};

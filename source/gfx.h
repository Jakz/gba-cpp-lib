#pragma once

#include "common.h"
#include "oam.h"

struct color_t
{
  u16 data;
  
  constexpr color_t(u32 red, u32 green, u32 blue) : data(red | (green<<5) | (blue<<10)) { }
  constexpr color_t(u32 data) : data(data) { }
  constexpr color_t(u16 data) : data(data) { }
  constexpr operator u16() const { return data; }
  
  constexpr u32 red() const { return data & 0x1F; }
  constexpr u32 green() const { return (data >> 5) & 0x1F; }
  constexpr u32 blue() const { return (data >> 10) & 0x1F; }
};

struct palette_t
{
  color_t colors[16];
  
  color_t& operator[](size_t i) { return colors[i]; }
  const color_t& operator[](size_t i) const { return colors[i]; }
};

template<size_t W, size_t H, typename T> struct canvas
{
  static constexpr size_t MODE3_BPP = 2;
  
  T data[W*H];
  
  T get(u32 x, u32 y) const { return data[y*W + x]; }
  void set (u32 x, u32 y, u32 color) { data[y*W +x] = color; }
  
  void clear()
  {
    for (u32 i = 0; i < W*H*sizeof(T)/sizeof(u32); ++i)
      as<u32>(data)[i] = 0;
  }
  
  void clear(u32 color)
  {
    u32 fill = 0;
    for (size_t i = 0; i < sizeof(u32)/sizeof(T); ++i)
      fill |= color << (i*sizeof(T)*8);
    for (u32 i = 0; i < W*H*sizeof(T)/sizeof(u32); ++i)
      as<u32>(data)[i] = fill;
  }
};

using mode3_canvas = canvas<240,160,color_t>;
using mode5_canvas = canvas<160,128,color_t>;

enum class video_mode
{
  MODE0 = 0x0,
  MODE1 = 0x1,
  MODE2 = 0x2,
  MODE3 = 0x3,
  MODE4 = 0x4,
  MODE5 = 0x5,
};

struct port_disp_cnt
{
  u16 value;
  
  enum
  {
    VIDEO_MODE3 = static_cast<u32>(video_mode::MODE3),
    
    CGB_MODE =  0x0008,
    FRAME_SELECT = 0x0010,
    HBLANK_FREE = 0x0020,
    OBJ_MAPPING_TYPE = 0x0040,
    FORCED_BLANK = 0x0080,
    ENABLE_BG0 = 0x0100,
    ENABLE_BG1 = 0x0200,
    ENABLE_BG2 = 0x0400,
    ENABLE_BG3 = 0x0800,
    ENABLE_OBJ = 0x1000,
    ENABLE_WINDOW0 = 0x2000,
    ENABLE_WINDOW1 = 0x4000,
    ENABLE_OBJ_WINDOW = 0x8000
  };
  
  void set(u16 value) { this->value = value; }
  u16 get() const { return value; }
  
  static constexpr u16 VIDEO_MODE_MASK = 0x0007;
  
  void setMode(video_mode mode) { value = (value & ~VIDEO_MODE_MASK) | static_cast<u32>(mode); }
  video_mode mode() const { return static_cast<video_mode>(value & VIDEO_MODE_MASK); }
  
  void enableBG2() { value = (value & ~ENABLE_BG2) | ENABLE_BG2; }
  bool isBG2Enabled() { return (value & ENABLE_BG2) != 0; }
  
  void enableOBJ() { value = (value & ~ENABLE_OBJ) | ENABLE_OBJ; }
  bool isOBJEnabled() { return (value & ENABLE_OBJ) != 0; }
  
  void setObjMapping1D() { value |= OBJ_MAPPING_TYPE; }
  void setObjMapping2D() { value &= ~OBJ_MAPPING_TYPE; }
};

struct port_disp_stat
{
  enum
  {
    VBLANK_FLAG = 0x0001,
    HBLANK_FLAG = 0x0002,
    VCOUNTER_FLAG = 0x0004,
    VBLANK_IRQ_ENABLE = 0x0008,
    HBLANK_IRQ_ENABLE = 0x0010,
    VCOUNTER_IRQ_ENABLE = 0x0020,
  };

  u16 value;
  
  bool isVBlank() const { return (value & VBLANK_FLAG) != 0; }
  bool isHBlank() const { return (value & HBLANK_FLAG) != 0; }
  bool isVCounter() const { return (value & VCOUNTER_FLAG) != 0; }
  
  void enableVBlankIRQ() { value |= VBLANK_IRQ_ENABLE; }
  void disableVBlankIRQ() { value &= ~VBLANK_IRQ_ENABLE; }
  
  void enableHBlankIRQ() { value |= HBLANK_IRQ_ENABLE; }
  void disableHBlankIRQ() { value &= ~HBLANK_IRQ_ENABLE; }
  
  void enableVCounterIRQ() { value |= VCOUNTER_IRQ_ENABLE; }
  void disableVCounterIRQ() { value &= ~VCOUNTER_IRQ_ENABLE; }
  
  void setVCount(u32 value) { value = (value & 0xFF00) | (value & 0xFF); }
  u32 vcount() const { return value & 0xFF; }
};


class Gfx
{
public:
  static constexpr u32 WIDTH = 240;
  static constexpr u32 HEIGHT = 160;
      
  void waitVsync()
  {
    u16 scanLine;
    while ((scanLine = *as<vu16>(PORT_VCOUNT)) < 160);
  }
  
  oam_entry* getOAM(u32 index) { return reinterpret_cast<oam_entry*>(as<u16>(OAM_BASE) + 4 * index); }
  oam_affine* getAffineOAM(u32 index) { return reinterpret_cast<oam_affine*>(as<u16>(OAM_BASE) + sizeof(oam_affine)*index); }
  
  inline palette_t& getObjPalette(u32 paletteIndex) { return *reinterpret_cast<palette_t*>(as<u16>(VRAM_PALETTE_OBJ) + paletteIndex*sizeof(palette_t)); }
  
  inline void setObjPalette(u32 paletteIndex, u32 colorIndex, u32 color)
  {
    as<u16>(VRAM_PALETTE_OBJ)[paletteIndex*PALETTE_SMALL_SIZE + colorIndex] = color;
  }
  
  inline mode3_canvas* getMode3Canvas() { return as<mode3_canvas>(VRAM_BASE); }

  inline u32* getObjTileData(u32 index) { return as<u32>(VRAM_OBJ_TILES) + index*OBJ_TILE_SIZE_4BPP/sizeof(u32); }
  
  inline port_disp_cnt* dispCnt() { return as<port_disp_cnt>(PORT_BASE); }
};

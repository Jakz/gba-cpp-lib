#pragma once

#include "common.h"
#include "oam.h"
#include "bg.h"

#include <array>

struct sprite_info_t
{
  const void* data;
  u16 width;
  u16 height;
  
  inline u32 sizeAs4BPP() const { return sizeInTiles()*OBJ_TILE_SIZE_4BPP; }
  inline u32 sizeInTiles() const { return width*height; }
};

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

using palette_t = std::array<u16, 16>;

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
    ENABLE_OBJ_WINDOW = 0x8000,
    
    ENABLE_BG_MASK = ENABLE_BG0 | ENABLE_BG1 | ENABLE_BG2 | ENABLE_BG3
  };
  
  void set(u16 value) { this->value = value; }
  u16 get() const { return value; }
  
  static constexpr u16 VIDEO_MODE_MASK = 0x0007;
  
  void setMode(video_mode mode) { value = (value & ~VIDEO_MODE_MASK) | static_cast<u32>(mode); }
  video_mode mode() const { return static_cast<video_mode>(value & VIDEO_MODE_MASK); }
  
  /* background related */
  inline void enableBG0() { value = (value & ~ENABLE_BG0) | ENABLE_BG0; }
  inline void enableBG1() { value = (value & ~ENABLE_BG1) | ENABLE_BG1; }
  inline void enableBG2() { value = (value & ~ENABLE_BG2) | ENABLE_BG2; }
  inline void enableBG3() { value = (value & ~ENABLE_BG3) | ENABLE_BG3; }
  inline void enableBGs() { value = value | ENABLE_BG_MASK; }

  inline void enableBG(bool bg0, bool bg1, bool bg2, bool bg3)
  {
    value = (value & ~ENABLE_BG_MASK);
    value = value | (bg0 ? ENABLE_BG0 : 0) | (bg0 ? ENABLE_BG1 : 0) | (bg0 ? ENABLE_BG2 : 0) | (bg0 ? ENABLE_BG3 : 0);
  }
  
  /* obj related */
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

/* special effects */
struct port_mosaic_size
{
  enum
  {
    MOSAIC_MASK = 0xF,
    MOSAIC_BG_H_SHIFT = 0,
    MOSAIC_BG_V_SHIFT = 4,
    MOSAIC_OBJ_H_SHIFT = 8,
    MOSAIC_OBJ_V_SHIFT = 12,
  };
  
  u16 value;
  
  inline void setBG(fu16 h, fu16 v) { value = (value & 0xFF00) | (h << MOSAIC_BG_H_SHIFT) | (v << MOSAIC_BG_V_SHIFT); }
  inline void setOBJ(fu16 h, fu16 v) { value = (value & 0x00FF) | (h << MOSAIC_OBJ_H_SHIFT) | (v << MOSAIC_OBJ_V_SHIFT); }
};

enum class fx_type
{
  NONE = 0,
  ALPHA_BLEND = 1,
  BRIGHTNESS_INCREASE = 2,
  BRIGHTNESS_DECREASE = 3
};

enum class fx_target
{
  BG0 = 0b000001,
  BG1 = 0b000010,
  BG2 = 0b000100,
  BG3 = 0b001000,
  OBJ = 0b010000,
  BD  = 0b100000,
};

struct port_blend_cnt
{
  enum
  {
    BLEND_BG0 = 0b000001,
    BLEND_BG1 = 0b000010,
    BLEND_BG2 = 0b000100,
    BLEND_BG3 = 0b001000,
    BLEND_OBJ = 0b010000,
    BLEND_BD  = 0b100000,
    
    FX_MASK  = 0x3,
    FX_SHIFT = 6,
    
    TARGET_2ND_SHIFT = 8,
  };
  
  u16 value;
  
  inline void setFX(fx_type type) { value = (value & ~(FX_MASK << FX_SHIFT)) | (static_cast<u16>(type) << FX_SHIFT); }
  inline void enable1stTarget(fx_target target) { value |= static_cast<u16>(target); }
  inline void enable2stTarget(fx_target target) { value |= static_cast<u16>(target) << TARGET_2ND_SHIFT; }
  
  inline void disable1stTarget() { value &= 0xFF00 | (FX_MASK << FX_SHIFT); }
  inline void disable2ndTarget() { value &= 0x00FF; }
};

struct port_blend_alpha
{
  sfp evb;
  sfp eva;

  void set1st(float v) { eva = v; }
  void set2nd(float v) { evb = v; }
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
  
  oam_entry* getOAM(u32 index) { return reinterpret_cast<oam_entry*>(as<oam_entry>(OAM_BASE) + index); }
  oam_affine* getAffineOAM(u32 index) { return reinterpret_cast<oam_affine*>(as<oam_affine>(OAM_BASE) + index); }
  
  inline palette_t& getObjPalette(u32 paletteIndex) { return *reinterpret_cast<palette_t*>(as<u16>(VRAM_PALETTE_OBJ) + paletteIndex*sizeof(palette_t)); }
  inline palette_t& getBgPalette(fu16 paletteIndex) { return *reinterpret_cast<palette_t*>(as<u16>(VRAM_PALETTE_BG) + paletteIndex*sizeof(palette_t)); }
  
  inline void setObjPalette(u32 paletteIndex, u32 colorIndex, u32 color)
  {
    as<u16>(VRAM_PALETTE_OBJ)[paletteIndex*PALETTE_SMALL_SIZE + colorIndex] = color;
  }
  
  inline mode3_canvas* getMode3Canvas() { return as<mode3_canvas>(VRAM_BASE); }

  inline u32* getObjTileData(u32 index) { return as<u32>(VRAM_OBJ_TILES) + index*OBJ_TILE_SIZE_4BPP/sizeof(u32); }
  
  inline port_disp_cnt* dispCnt() { return as<port_disp_cnt>(PORT_BASE); }
  inline port_mosaic_size* mosaicSize() { return as<port_mosaic_size>(PORT_MOSAIC); }
  inline port_blend_cnt* blendCnt() { return as<port_blend_cnt>(PORT_BLDCNT); }
  inline port_blend_alpha* blendAlpha() { return as<port_blend_alpha>(PORT_BLDALPHA); }

  /* background related */
  inline port_bg_cnt* bg0Cnt() { return as<port_bg_cnt>(PORT_BG0CNT); }
  inline port_bg_hofs* bg0Hofs() { return as<port_bg_hofs>(PORT_BG0HOFS); }
  inline port_bg_vofs* bg0Vofs() { return as<port_bg_hofs>(PORT_BG0VOFS); }
  
  inline port_bg_cnt* bg1Cnt() { return as<port_bg_cnt>(PORT_BG1CNT); }
  inline port_bg_hofs* bg1Hofs() { return as<port_bg_hofs>(PORT_BG1HOFS); }
  inline port_bg_vofs* bg1Vofs() { return as<port_bg_hofs>(PORT_BG1VOFS); }
  
  inline port_bg_cnt* bg2Cnt() { return as<port_bg_cnt>(PORT_BG2CNT); }
  inline port_bg_hofs* bg2Hofs() { return as<port_bg_hofs>(PORT_BG2HOFS); }
  inline port_bg_vofs* bg2Vofs() { return as<port_bg_hofs>(PORT_BG2VOFS); }
  
  inline port_bg_cnt* bg3Cnt() { return as<port_bg_cnt>(PORT_BG3CNT); }
  inline port_bg_hofs* bg3Hofs() { return as<port_bg_hofs>(PORT_BG3HOFS); }
  inline port_bg_vofs* bg3Vofs() { return as<port_bg_hofs>(PORT_BG3VOFS); }
  
  inline bg0_entry bg0() { return bg0_entry(); }
  inline bg1_entry bg1() { return bg1_entry(); }
  inline bg2_entry bg2() { return bg2_entry(); }
  inline bg3_entry bg3() { return bg3_entry(); }
  
  inline u32* getBgTileData(fu16 block, fu16 index) { return as<u32>(port_bg_cnt::tileDataAtIndex(block)) + index*BG_TILE_SIZE_4BPP/sizeof(u32); }
  inline tile_entry* getBgTileMap(fu16 block) { return as<tile_entry>(port_bg_cnt::tileMapAtIndex(block)); }
};

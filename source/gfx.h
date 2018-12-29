#pragma once

#include "common.h"
#include "oam.h"

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
    ENABLE_OBJ_WINDOW = 0x8000
  };
  
  void set(u16 value) { this->value = value; }
  u16 get() const { return value; }
  
  static constexpr u16 VIDEO_MODE_MASK = 0x0007;
  
  void setMode(video_mode mode) { value = (value & ~VIDEO_MODE_MASK) | static_cast<u32>(mode); }
  video_mode mode() const { return static_cast<video_mode>(value & VIDEO_MODE_MASK); }
  
  inline void enableBG0() { value = (value & ~ENABLE_BG0) | ENABLE_BG0; }
  inline void enableBG1() { value = (value & ~ENABLE_BG1) | ENABLE_BG1; }
  inline void enableBG2() { value = (value & ~ENABLE_BG2) | ENABLE_BG2; }
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
  
  /* background related */
  inline port_bg_cnt* bg0Cnt() { return as<port_bg_cnt>(PORT_BG0CNT); }
  inline port_bg_hofs* bg0Hofs() { return as<port_bg_hofs>(PORT_BG0HOFS); }
  inline port_bg_vofs* bg0Vofs() { return as<port_bg_hofs>(PORT_BG0VOFS); }
  
  inline port_bg_cnt* bg1Cnt() { return as<port_bg_cnt>(PORT_BG1CNT); }
  inline port_bg_hofs* bg1Hofs() { return as<port_bg_hofs>(PORT_BG1HOFS); }
  inline port_bg_vofs* bg1Vofs() { return as<port_bg_hofs>(PORT_BG1VOFS); }
  
  inline bg0_entry bg0() { return bg0_entry(); }
  inline bg1_entry bg1() { return bg1_entry(); }
  inline bg2_entry bg2() { return bg2_entry(); }
  
  inline u32* getBgTileData(fu16 block, fu16 index) { return as<u32>(port_bg_cnt::tileDataAtIndex(block)) + index*BG_TILE_SIZE_4BPP/sizeof(u32); }
  inline tile_entry* getBgTileMap(fu16 block) { return as<tile_entry>(port_bg_cnt::tileMapAtIndex(block)); }
};

#pragma once

#include "common.h"
#include "fixed_point.h"
#include "math.h"

enum class oam_mode
{
  NORMAL = 0,
  SEMI_TRANSPARENT = 1,
  OBJ_WINDOW = 2,
};

enum class oam_shape
{
  SQUARE = 0,
  WIDE = 1,
  TALL = 2,
};

enum class oam_size
{
  SIZE8x8 = 0,
  SIZE16x8 = 0,
  SIZE8x16 = 0,
  
  SIZE16x16 = 1,
  SIZE32x8 = 1,
  SIZE8x32 = 1,
  
  SIZE32x32 = 2,
  SIZE32x16 = 2,
  SIZE16x32 = 2,
  
  SIZE64x64 = 3,
  SIZE64x32 = 3,
  SIZE32x64 = 3
};

struct oam_entry
{
  u16 attr0;
  u16 attr1;
  u16 attr2;
  
  static constexpr u16 MASK_Y = 0x00FF;
  static constexpr u16 MASK_X = 0x01FF;
  static constexpr u16 MASK_AFFINE_INDEX = 0x3E00;
  static constexpr u16 SHIFT_AFFINE_INDEX = 9;
  
  /* attr0 */
  inline u32 y() const { return attr0 & MASK_Y; }
  inline void setY(u32 value) { attr0 = (attr0 & ~MASK_Y) | (value & MASK_Y); }
  
  inline bool isAffineEnabled() const { return attr0 & 0x0100; }
  inline void setAffineEnabled(bool v) { attr0 = (attr0 & ~0x0100) | (v << 8); }
  
  inline bool isDoubleSize() const { return attr0 & 0x0200; }
  inline void setDoubleSize(bool v) { attr0 = (attr0 & ~0x0200) | (v << 9); }
  
  inline bool isHidden() const { return attr0 & 0x0200; }
  inline oam_mode mode() const { return static_cast<oam_mode>((attr0 & 0x0C00) >> 10); }
  inline bool isMosaic() const { return attr0 & 0x1000; }
  inline bool hasLargePalette() const { return attr0 & 0x2000; }
  
  inline oam_shape shape() const { return static_cast<oam_shape>((attr0 & 0xC000) >> 14); }
  inline void setShape(oam_shape shape) { attr0 = (attr0 & 0x3FFF) | (static_cast<u16>(shape) << 14); }
   
  /* attr1 */
  inline u32 x() const { return attr1 & MASK_X; }
  inline void setX(u32 value) { attr1 = (attr1 & ~MASK_X) | (value & MASK_X); }

  inline u32 affineIndex() const { return (attr1 & MASK_AFFINE_INDEX) >> SHIFT_AFFINE_INDEX; }
  inline void setAffineIndex(u32 index) { attr1 = (attr1 & ~MASK_AFFINE_INDEX) | (index << SHIFT_AFFINE_INDEX); }
  
  inline bool isFlipX() const { return attr1 & 0x1000; }
  inline void setFlipX(bool v) { attr1 = (attr1 & ~0x1000) | (v << 12); }
  
  inline bool isFlipY() const { return attr1 & 0x2000; }
  inline void setFlipY(bool v) { attr1 = (attr1 & ~0x2000) | (v << 13); }
  
  inline oam_size size() const { return static_cast<oam_size>((attr1 & 0xC000) >> 14);}
  inline void setSize(oam_size size) { attr1 = (attr1 & ~0xC000) | (static_cast<u16>(size) << 14); }

  /* attr2 */
  inline u32 tileIndex() const { return (attr2 & 0x03FF); }
  inline void setTileIndex(u32 index) { attr2 = (attr2 & ~0x03FF) | (index & 0x03FF); }
  
  inline u32 priority() const { return (attr2 & 0x0C00) >> 10; }
  
  inline u32 palette() const { return (attr2 & 0xF000) >> 12; }
  inline void setPalette(u32 index) { attr2 = (attr2 & ~0xF000) | ((index & 0xF) << 12); }
  
  void reset() { attr0 = attr1 = attr2 = 0; }
};

struct oam_affine
{
  u16 ___fill0[3];
  fp pa;
  u16 ___fill1[3];
  fp pb;
  u16 ___fill2[3];
  fp pc;
  u16 ___fill3[3];
  fp pd;
  
  template<typename T> void set(fp sx, fp sy, u16 a)
  {
    T c = math::cos(a), s = math::sin(a);
    pa = c * sx;
    pb = -s * sx;
    pc = s * sy;
    pd = c * sy;
  }
  
  static void setAffine(oam_affine* affine, fp sx, fp sy, fp a)
  {
    /*affine->pa = c * sx;
    affine->pb = -s * sx;
    affine->pc = s * sy;
    affine->pd = c * sy;*/
    affine->pa = sx;
    affine->pb = fp(0);
    affine->pc = fp(0);
    affine->pd = sy;
  }
};
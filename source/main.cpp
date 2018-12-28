#include <array>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "math.h"
#include "gfx.h"
#include "bg.h"
#include "keypad.h"
#include "timer.h"
#include "bios.h"

struct Ship
{
  u16 x;
  u16 y;
};

alignas(4) color_t shipPalette[] = { color_t(7, 9, 10), color_t(10, 12, 13), color_t(12, 13, 14), color_t(13, 14, 14), color_t(13, 14, 15), color_t(19, 19, 20), color_t(20, 20, 21), color_t(20, 21, 21), color_t(21, 22, 23), color_t(22, 23, 23), color_t(22, 23, 23), color_t(23, 24, 24), color_t(23, 24, 24), color_t(24, 25, 25), color_t(24, 25, 25), color_t(0, 0, 0) };

alignas(4) uint8_t shipGfx[] = {
  0x10,0x42,0x44,0x44,0x41,0xA7,0xEE,0xEE,0x52,0xA9,0xAA,0xAA,0x64,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,
  0x44,0x44,0x24,0x01,0xEE,0xEE,0x7A,0x14,0xAA,0xAA,0xAA,0x25,0xBB,0xBB,0xBB,0x46,0xBB,0xBB,0xBB,0x47,0xBB,0xBB,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,
  0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xBB,0x64,0xBB,0xBB,0xBB,0x52,0xAA,0xAA,0xAA,0x41,0xA7,0xEE,0xEE,0x10,0x42,0x44,0x44,
  0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xBB,0xBB,0xBB,0x47,0xBB,0xBB,0xBB,0x46,0xAA,0x88,0xA8,0x25,0xDE,0xDD,0x6A,0x13,0x44,0x44,0x24,0x01,
};

alignas(4) uint32_t mirror[] = {
  0x00000000,0x00000000,0x00010000,0x00121000,0x11143000,0x66665300,0xBBBA9830,0xFFFED300,
  0x00000000,0x00000000,0x00001000,0x00012100,0x00034111,0x00376666,0x0389ACBB,0x003DEFFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
};

alignas(4) color_t mirrorPalette[] = { color_t(0, 0, 0), color_t(8, 8, 8), color_t(22, 23, 24), color_t(5, 6, 6), color_t(13, 14, 15), color_t(17, 18, 19), color_t(20, 21, 22), color_t(16, 18, 19), color_t(20, 22, 23), color_t(25, 28, 29), color_t(28, 30, 31), color_t(30, 31, 31), color_t(30, 31, 31), color_t(10, 10, 11), color_t(13, 14, 14), color_t(15, 15, 15) };

alignas(4) u32 highlight[] = {
  0x11111110,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,
  0x01111111,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,
  0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x11111110,
  0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x01111111,
};

color_t highlightColor = color_t(31, 5, 5);

int main(void) {
  static_assert(sizeof(port_disp_cnt) == sizeof(u16), "Must be 2 bytes");
  static_assert(sizeof(oam_entry) == sizeof(u16)*3, "Must be 6 bytes");
  static_assert(sizeof(oam_affine) == sizeof(oam_entry)*4 + sizeof(s16)*4, "Must be correct size");
  static_assert(sizeof(palette_t) == sizeof(color_t)*16, "Must be 32 bytes");
  
  Gfx gfx;
  KeypadManager keypad;
  
  math::init();
  
  port_disp_cnt* dispCnt = gfx.dispCnt();
  dispCnt->set(0);
  dispCnt->setMode(video_mode::MODE0);
  dispCnt->enableBG0();
  dispCnt->enableBG1();
  dispCnt->enableOBJ();
  dispCnt->setObjMapping1D();
  //dispCnt->set(port_disp_cnt::VIDEO_MODE3 | port_disp_cnt::ENABLE_BG2 | port_disp_cnt::ENABLE_OBJ | port_disp_cnt::OBJ_MAPPING_TYPE);
  
  port_bg_cnt* bg0cnt =  gfx.bg0Cnt();
  bg0cnt->set(0);
  bg0cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg0cnt->setTileDataBlock(0);
  bg0cnt->setTileMapBlock(16);
  bg0cnt->setPriority(1);
  
  port_bg_cnt* bg1cnt = gfx.bg1Cnt();
  bg1cnt->set(0);
  bg1cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg1cnt->setTileDataBlock(0);
  bg1cnt->setTileMapBlock(17);
  bg1cnt->setPriority(0);

  
  *gfx.bg0Hofs() = 0;
  *gfx.bg0Vofs() = 0;
  
  *gfx.bg1Hofs() = 0;
  *gfx.bg1Vofs() = 0;
  
  memcpy(&gfx.getBgPalette(0), shipPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  memcpy(&gfx.getBgPalette(1), mirrorPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  
  memset(&gfx.getObjPalette(0), 0, PALETTE_SMALL_SIZE * sizeof(color_t));
  gfx.getObjPalette(0)[1] = color_t(31, 5, 5);
  
  auto* shipTiles = gfx.getObjTileData(512);
  
  auto* bgTile = gfx.getBgTileData(0, 0);
  
  memcpy(shipTiles, highlight, OBJ_TILE_SIZE_4BPP * 2 * 2);
  memcpy(bgTile, shipGfx, BG_TILE_SIZE_4BPP * 2 * 2);
  
  memcpy(gfx.getBgTileData(0, 4), mirror, BG_TILE_SIZE_4BPP * 2 * 2);
  
  auto* tileMap = gfx.getBgTileMap(16);
  
  for (int x = 0; x < 32; ++x)
    for (int y = 0; y < 32; ++y)
    {
      tileMap[y*32 + x].set((x & 0x01) | ((y & 0x01) << 1));
      
      gfx.getBgTileMap(17)[y*32 + x].set(((x & 0x01) | ((y & 0x01) << 1)) + 4);
      gfx.getBgTileMap(17)[y*32 + x].setPalette(2);
    }
  
  oam_entry* object = gfx.getOAM(0);
  
  memset(object, 0, sizeof(oam_entry));
  
  object->setPalette(0);
  object->setTileIndex(512);
  object->setShape(oam_shape::SQUARE);
  object->setSize(oam_size::SIZE16x16);
  
  object->setAffineMode(oam_affine_mode::DISABLED);
  object->setAffineIndex(0);

  Ship ship;
  ship.x = 0;
  ship.y = 0;

  while (1)
  {        
    keypad.update();
    
    if (keypad.isPressed(KEY_RIGHT))
    {
      if (ship.x < 14)
        ++ship.x;
    }
    else if (keypad.isPressed(KEY_LEFT))
    {
      if (ship.x > 0)
        --ship.x;
    }
    
    if (keypad.isPressed(KEY_UP))
    {
      if (ship.y > 0)
        --ship.y;
    }
    else if (keypad.isPressed(KEY_DOWN))
    {
      if (ship.y < 9)
        ++ship.y;
    }
    
    object->setX(ship.x * 16);
    object->setY(ship.y * 16);

    gfx.waitVsync();
  }
  
  return 0;
}



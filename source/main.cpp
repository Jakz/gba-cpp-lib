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

u32 shipGfx[] = {
  0x44444210,0xEEEEA741,0xAAAAA952,0xBBBBBB64,0xBBBBBB74,0xBBBBBB74,0xCCBBBB74,0xCCBBBB74,
  0x01244444,0x147AEEEE,0x25AAAAAA,0x46BBBBBB,0x47BBBBBB,0x47BBBBBB,0x47BBBCCC,0x47BBBCCC,
  0xCCBBBB74,0xCCBBBB74,0xCCBBBB74,0xBBBBBB74,0xBBBBBB64,0xAAAAAA52,0xEEEEA741,0x44444210,
  0x47BBBCCC,0x47BBBCCC,0x47BBBCCC,0x47BBBBBB,0x46BBBBBB,0x25A888AA,0x136ADDDE,0x01244444,
 };

u32 highlight[] = {
  0x11111110,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,
  0x01111111,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,
  0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x11111110,
  0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x01111111,
};

color_t highlightColor = color_t(32, 5, 5);

int main(void) {
  static_assert(sizeof(port_disp_cnt) == sizeof(u16), "Must be 2 bytes");
  static_assert(sizeof(oam_entry) == sizeof(u16)*3, "Must be 6 bytes");
  static_assert(sizeof(oam_affine) == sizeof(oam_entry)*4 + sizeof(s16)*4, "Must be correct size");
  
  
  Gfx gfx;
  KeypadManager keypad;
  
  math::init();
  
  port_disp_cnt* dispCnt = gfx.dispCnt();
  dispCnt->set(0);
  dispCnt->setMode(video_mode::MODE0);
  dispCnt->enableBG0();
  dispCnt->enableOBJ();
  dispCnt->setObjMapping1D();
  //dispCnt->set(port_disp_cnt::VIDEO_MODE3 | port_disp_cnt::ENABLE_BG2 | port_disp_cnt::ENABLE_OBJ | port_disp_cnt::OBJ_MAPPING_TYPE);
  
  port_bg_cnt* bg0cnt =  gfx.bg0Cnt();
  bg0cnt->set(0);
  bg0cnt->setScreenSize(bg_screen_size::SIZE32x32);
  bg0cnt->setTileDataBlock(0);
  bg0cnt->setTileMapBlock(16);
  
  *gfx.bg0Hofs() = 0;
  *gfx.bg0Vofs() = 0;
  
  memcpy(&gfx.getBgPalette(0), shipPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  
  memset(&gfx.getObjPalette(0), 0, PALETTE_SMALL_SIZE * sizeof(color_t));
  gfx.getObjPalette(0)[1] = color_t(31, 5, 5);
  
  auto* shipTiles = gfx.getObjTileData(512);
  
  auto* bgTile = gfx.getBgTileData(0, 0);
  
  for (int i = 0; i < 8 * 4; ++i)
  {
    *shipTiles = highlight[i];
    ++shipTiles;
    
    *bgTile = shipGfx[i];
    ++bgTile;
  }
  
  auto* tileMap = gfx.getBgTileMap(16);
  
  for (int x = 0; x < 32; ++x)
    for (int y = 0; y < 32; ++y)
    {
      tileMap[y*32 + x].set((x & 0x01) | ((y & 0x01) << 1));
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



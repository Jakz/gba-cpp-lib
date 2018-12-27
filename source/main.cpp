#include <array>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "math.h"
#include "gfx.h"
#include "bg.h"
#include "keypad.h"
#include "bios.h"

struct Ship
{
  u32 x;
  u32 y;
  u32 accx;
  u32 accy;
  Key lastDirx;
  Key lastDiry;
};

alignas(4) color_t shipPalette[] = { color_t(7, 9, 10), color_t(10, 12, 13), color_t(12, 13, 14), color_t(13, 14, 14), color_t(13, 14, 15), color_t(19, 19, 20), color_t(20, 20, 21), color_t(20, 21, 21), color_t(21, 22, 23), color_t(22, 23, 23), color_t(22, 23, 23), color_t(23, 24, 24), color_t(23, 24, 24), color_t(24, 25, 25), color_t(24, 25, 25), color_t(0, 0, 0) };

u32 shipGfx[] = {
  0x44444210,0xEEEEA741,0xAAAAA952,0xBBBBBB64,0xBBBBBB74,0xBBBBBB74,0xCCBBBB74,0xCCBBBB74,
  0x01244444,0x147AEEEE,0x25AAAAAA,0x46BBBBBB,0x47BBBBBB,0x47BBBBBB,0x47BBBCCC,0x47BBBCCC,
  0xCCBBBB74,0xCCBBBB74,0xCCBBBB74,0xBBBBBB74,0xBBBBBB64,0xAAAAAA52,0xEEEEA741,0x44444210,
  0x47BBBCCC,0x47BBBCCC,0x47BBBCCC,0x47BBBBBB,0x46BBBBBB,0x25A888AA,0x136ADDDE,0x01244444,
 };

int main(void) {
  static_assert(sizeof(port_disp_cnt) == sizeof(u16), "Must be 2 bytes");
  static_assert(sizeof(oam_entry) == sizeof(u16)*3, "Must be 6 bytes");
  static_assert(sizeof(oam_affine) == sizeof(oam_entry)*4 + sizeof(s16)*4, "Must be correct size");
  
  
  Gfx gfx;
  Keypad keypad;
  
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
    
  auto& palette = gfx.getObjPalette(0);
  
  memcpy(&palette, shipPalette, PALETTE_SMALL_SIZE * sizeof(color_t));
  memcpy(&gfx.getBgPalette(0), shipPalette, PALETTE_SMALL_SIZE * sizeof(color_t));

  
  auto* shipTiles = gfx.getObjTileData(512);
  
  auto* bgTile = gfx.getBgTileData(0, 0);
  
  for (int i = 0; i < 8 * 4; ++i)
  {
    *shipTiles = shipGfx[i];
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
  object->hide();

  Ship ship;
  ship.x = 50;
  ship.y = Gfx::HEIGHT/2 - 8;
  ship.accx = ship.accy = 0;
  ship.lastDirx = ship.lastDiry = KEY_NONE;
  
    
  while (1)
  {        
    const KeyStatus* keys = keypad.poll();
    
    if (keys->isPressed(KEY_RIGHT))
    {
      ship.accx = ship.lastDirx == KEY_RIGHT ? ship.accx+1 : 0;
      if (ship.accx > 1)
      {
        if (ship.x < 228)
          ++ship.x;
        ship.accx = 0;
      }
      ship.lastDirx = KEY_RIGHT;
    }
    else if (keys->isPressed(KEY_LEFT))
    {
      ship.accx = ship.lastDirx == KEY_LEFT ? ship.accx+1 : 0;
      if (ship.accx > 2)
      {
        if (ship.x > 20)
          --ship.x;
        ship.accx = 0;
      }
      ship.lastDirx = KEY_LEFT;
    }
    else
    {
      ship.lastDirx = KEY_NONE;
      ship.accx = 0;
    }
    
    if (keys->isPressed(KEY_UP))
    {
      ship.accy = ship.lastDiry == KEY_UP ? ship.accy+1 : 0;
      if (ship.accy > 2)
      {
        if (ship.y > 10)
          --ship.y;
        ship.accy = 0;
      }
      ship.lastDiry = KEY_UP;
    }
    else if (keys->isPressed(KEY_DOWN))
    {
      ship.accy = ship.lastDiry == KEY_DOWN ? ship.accy+1 : 0;
      if (ship.accy > 2)
      {
        if (ship.y < Gfx::HEIGHT - 10)
          ++ship.y;
        ship.accy = 0;
      }
      ship.lastDiry = KEY_DOWN;
    }
    else
    {
      ship.lastDiry = KEY_NONE;
      ship.accy = 0;
    }
    
    object->setX(ship.x - 20);
    object->setY(ship.y - 8);
    
    static bool growing = true;
    static const fpp incr = fpp(0.005f);
    
    static fpp scale = fpp(1.0f);
    static u16 angle = 0;

    /*auto* affine = gfx.getAffineOAM(0);
    
    affine->set<fpp>(scale, scale, angle>>1);

    ++angle;
    
    if (growing)
    {
      scale += incr;
      
      if (scale > 2.0f)
      {
        scale = 2.0f;
        growing = false;
      }
    }
    else
    {
      scale -= incr;
      
      if (scale < 0.5f)
      {
        scale = 0.5f;
        growing = true;
      }
    }*/
    
    
    /*if (growing)
    {
      --s;
      if (s < 1<<7)
      {
        s = 1 <<7;
        growing = false;
      }
    }
    else
    {
      ++s;
      if (s > 1<<9)
      {
        s = 1 << 9;
        growing = true;
      }
    }*/


    gfx.waitVsync();
  }
  
  return 0;
}



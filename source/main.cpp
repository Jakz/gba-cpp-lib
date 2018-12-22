#include <array>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "math.h"
#include "gfx.h"
#include "bg.h"
#include "keypad.h"
#include "bios.h"


struct Star
{
  u16 x;
  u16 y;
  u16 accumulator;
  u16 speed;
};

struct Ship
{
  u32 x;
  u32 y;
  u32 accx;
  u32 accy;
  Key lastDirx;
  Key lastDiry;
};

alignas(4) color_t shipPalette[] = { 
  color_t(0, 0, 0), color_t(2, 11, 14), color_t(17, 29, 27), color_t(12, 12, 14), color_t(26, 26, 28), color_t(8, 9, 11), color_t(14, 16, 18), color_t(13, 23, 23), color_t(10, 11, 13), color_t(21, 21, 23), color_t(13, 13, 15), color_t(18, 18, 20), color_t(6, 6, 8), color_t(14, 5, 20), color_t(22, 12, 28), color_t(27, 20, 31) };

u32 shipGfx[] = {
  0x00000000,0x00000000,0x30000000,0x00000000,0x00000000,0x00000000,0x11111110,0x22222221,
  0x00000000,0x00033333,0x00344444,0x05466665,0x59588810,0xA55A6175,0xCC831211,0x9B312722,
  0x00000000,0x00000000,0x00000000,0xDDD00000,0xFFED5555,0xEED33338,0xD5ACCCCC,0x665B9999,
  0x00000000,0x00000000,0x00000000,0x00000DDD,0x0000DEFF,0x000DEDEE,0x00DEDEDD,0x0DEEED36,
  0x11111110,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xAAB51111,0x55550000,0xBBB50000,0xA6BB5000,0x99999500,0x55555000,0x00000000,0x00000000,
  0x556AAAAA,0x668B6A55,0xC111193B,0x17227193,0x01111059,0x00000005,0x00000000,0x00000000,
  0xCADDD555,0x0C6BBB66,0x00CCCCC1,0x00000012,0x00000001,0x00000000,0x00000000,0x00000000
 };

std::array<Star,128> stars;

int main(void) {
  static_assert(sizeof(port_disp_cnt) == sizeof(u16), "Must be 2 bytes");
  static_assert(sizeof(oam_entry) == sizeof(u16)*3, "Must be 6 bytes");
  static_assert(sizeof(oam_affine) == sizeof(oam_entry)*4 + sizeof(s16)*4, "Must be correct size");
  
  
  Gfx gfx;
  Keypad keypad;
  
  math::init();
  
  port_disp_cnt* dispCnt = gfx.dispCnt();
  dispCnt->set(0);
  dispCnt->setMode(video_mode::MODE3);
  dispCnt->enableBG2();
  dispCnt->enableOBJ();
  dispCnt->setObjMapping1D();
  //dispCnt->set(port_disp_cnt::VIDEO_MODE3 | port_disp_cnt::ENABLE_BG2 | port_disp_cnt::ENABLE_OBJ | port_disp_cnt::OBJ_MAPPING_TYPE);
  
  for (auto& star : stars)
  {
    star.x = rand()%Gfx::WIDTH;
    star.y = rand()%Gfx::HEIGHT;
    star.speed = rand()%16 + 1;
    star.accumulator = 0;
  }
  
  static const color_t colors[] =
  {
    color_t(6, 6, 31),
    color_t(15, 15, 31),
    color_t(22, 22, 31), 
    color_t(31,31,31),
  };
    
  auto& palette = gfx.getObjPalette(0);
    
  for (u32 i = 0; i < 16; ++i)
    palette[i] = shipPalette[i];
  
  auto* shipTiles = gfx.getObjTileData(512);
  
  for (int i = 0; i < 8 * 8; ++i)
  {
    *shipTiles = shipGfx[i];
    ++shipTiles;
  }
  
  oam_entry* object = gfx.getOAM(0);
  
  memset(object, 0, sizeof(oam_entry));
  
  object->setPalette(0);
  object->setTileIndex(512);
  object->setShape(oam_shape::WIDE);
  object->setSize(oam_size::SIZE32x16);
  
  object->setAffineEnabled(true);
  object->setAffineIndex(0);
  object->setDoubleSize(true);

  Ship ship;
  ship.x = 50;
  ship.y = Gfx::HEIGHT/2 - 8;
  ship.accx = ship.accy = 0;
  ship.lastDirx = ship.lastDiry = KEY_NONE;
  
  mode3_canvas* canvas = gfx.getMode3Canvas();
  
  canvas->clear(color_t(0,0,0));
  
  while (1)
  {    
    for (auto& star : stars)
    {
      canvas->set(star.x, star.y, 0);
            
      if (ship.lastDirx == KEY_RIGHT)
        star.accumulator += star.speed << 1;
      else if (ship.lastDirx == KEY_LEFT)
        star.accumulator += star.speed >> 1;
      else
        star.accumulator += star.speed;
      
      if (star.accumulator > 100)
      {
        star.accumulator %= 100;
        
        if (star.x == 0)
        {
          star.x = Gfx::WIDTH - 1;
          star.y = rand() % Gfx::HEIGHT;          
        }
        else
          --star.x;
        
      }
    
      u32 speedRange = (star.speed - 1)/ 4;
      u32 color = colors[speedRange];
      
      canvas->set(star.x, star.y, color);  
    }
    
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
    
    affine->set<fpp>(scale, scale, angle>>1);*/

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
    }
    
    
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



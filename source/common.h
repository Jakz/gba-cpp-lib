#pragma once

#include <cstdint>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef u32 addr_t;

using s32 = int32_t;
using s16 = int16_t;

using fu16 = int_fast16_t;

using vu16 = volatile u16;

static constexpr u32 KB16 = 16384;
static constexpr u32 KB2 = 2048;

static constexpr addr_t PORT_BASE = 0x04000000;

static constexpr addr_t PORT_BG0CNT = PORT_BASE + 0x08;
static constexpr addr_t PORT_BG1CNT = PORT_BASE + 0x0A;
static constexpr addr_t PORT_BG2CNT = PORT_BASE + 0x0C;
static constexpr addr_t PORT_BG3CNT = PORT_BASE + 0x0E;

static constexpr addr_t PORT_BG0HOFS = PORT_BASE + 0x10;
static constexpr addr_t PORT_BG0VOFS = PORT_BASE + 0x12;
static constexpr addr_t PORT_BG1HOFS = PORT_BASE + 0x14;
static constexpr addr_t PORT_BG1VOFS = PORT_BASE + 0x16;
static constexpr addr_t PORT_BG2HOFS = PORT_BASE + 0x18;
static constexpr addr_t PORT_BG2VOFS = PORT_BASE + 0x1A;
static constexpr addr_t PORT_BG3HOFS = PORT_BASE + 0x1C;
static constexpr addr_t PORT_BG3VOFS = PORT_BASE + 0x1E;

static constexpr addr_t VRAM_BASE = 0x06000000;
static constexpr addr_t VRAM_OBJ_TILES = 0x06010000;
static constexpr u32 OBJ_TILE_SIZE_4BPP = 0x20;
static constexpr u32 BG_TILE_SIZE_4BPP = 0x20;

static constexpr u32 TILE_DATA_BASE_MULTIPLIER = KB16;
static constexpr u32 TILE_MAP_BASE_MULTIPLIER = KB2;

static constexpr addr_t VRAM_PALETTE_BG = 0x05000000;
static constexpr addr_t VRAM_PALETTE_OBJ = 0x05000200;
static constexpr u32 PALETTE_SMALL_SIZE = 16;
static constexpr u32 PALETTE_LARGE_SIZE = 256;

static constexpr addr_t OAM_BASE = 0x07000000;

static constexpr addr_t PORT_VCOUNT = PORT_BASE + 0x6;
static constexpr addr_t PORT_DISPCNT = PORT_BASE;
static constexpr addr_t PORT_DISPSTAT = PORT_BASE + 0x4;


static constexpr addr_t PORT_KEYINPUT = PORT_BASE + 0x130;

template <typename T> T* as(addr_t address) { return static_cast<T*>((void*)address); }
template <typename T, typename U> T* as(U* address) { return static_cast<T*>((void*)address); }
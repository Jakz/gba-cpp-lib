#pragma once

#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef u32 addr_t;

using s32 = int32_t;
using s16 = int16_t;

using vu16 = volatile u16;

static constexpr addr_t PORT_BASE = 0x04000000;

static constexpr addr_t VRAM_BASE = 0x06000000;
static constexpr addr_t VRAM_OBJ_TILES = 0x06010000;
static constexpr u32 OBJ_TILE_SIZE_4BPP = 0x20;

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
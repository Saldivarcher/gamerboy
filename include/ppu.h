#pragma once

#include "memory.h"
#include "registers.h"

#include <cstdint>

namespace gb {

enum class VideoMode {
  // Horizontal Blank
  // Mode 0
  HBLANK,

  // Vertical Blank
  // Mode 1
  VBLANK,

  // Sprite Attribute Table
  // Mode 2
  ACCESS_OAM,

  // Video Memory
  // Mode 3
  ACCESS_VRAM,
};

// https://mgba-emu.github.io/gbdoc/#draw-modes
constexpr uint16_t HBLANK_CYCLES = 204;
constexpr uint16_t VBlANK_CYCLES = 4560;
constexpr uint16_t ACCESS_OAM_CYCLES = 80;
constexpr uint16_t ACCESS_VRAM_CYCLES = 172;

constexpr uint16_t LINE_CYCLES = 456;
constexpr uint32_t FRAME_CYCLES = 70224;

class Gameboy;

class PPU {
public:
  PPU(Gameboy &gb);

  void cycle(uint64_t cycles);

private:
  uint64_t m_current_cycle = 0;
  VideoMode m_current_video_mode = VideoMode::ACCESS_OAM;

  Gameboy &m_gb;
  Memory &m_mem;

  Register m_lcd_control;
  Register m_lcd_status;
  Register m_scroll_y;
  Register m_scroll_x;
  Register m_line_y;
  Register m_line_y_compare;
  Register m_direct_mem_access;
  Register m_backgroud_palette;
  Register m_object_palette_0;
  Register m_object_palette_1;
  Register m_window_y;
  Register m_window_x;
};

} // namespace gb

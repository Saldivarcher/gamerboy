#include "ppu.h"

#include "gameboy.h"

namespace gb {

PPU::PPU(Gameboy &gb) : m_gb(gb), m_mem(m_gb.get_memory()) {}

void PPU::cycle(uint64_t cycle) {
  switch (m_current_video_mode) {

  case VideoMode::ACCESS_OAM:
    break;
  case VideoMode::ACCESS_VRAM:
    break;
  case VideoMode::HBLANK:
    break;
  case VideoMode::VBLANK:
    break;

  default:
    utility::error("Should not be here!", 3);
  }
}

} // namespace gb

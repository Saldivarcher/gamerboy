#pragma once

#include <cstdint>

enum class VideoMode {
  ACCESS_OAM,
  ACCESS_VRAM,
  HBLANK,
  VBLANK,
};

class PPU {
public:
  PPU();

  void cycle(uint64_t cycles);

private:
  uint64_t m_current_cycle;

  VideoMode m_current_video_mode = VideoMode::ACCESS_OAM;
};

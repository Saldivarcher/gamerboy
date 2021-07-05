#pragma once
#include "cartridge.h"
#include "cpu.h"
#include "memory.h"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace gb {
class Gameboy {
public:
  Gameboy(const char *path);
  ~Gameboy();

  void on();

private:
  inline bool did_quit() { return m_did_close; }

  void run();
  void process();

  CPU m_cpu;
  Memory m_mem;
  NoMbc m_cartridge;

  bool m_did_close;
  std::filesystem::path m_rom_path;
  std::vector<std::byte> m_rom_data;

  utility::sdl_window_ptr m_window = {nullptr, SDL_DestroyWindow};
  utility::sdl_renderer_ptr m_renderer = {nullptr, SDL_DestroyRenderer};
  utility::sdl_texture_ptr m_texture = {nullptr, SDL_DestroyTexture};
};
} // namespace gb

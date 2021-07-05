#pragma once

#include "gameboy.h"
#include "utility.h"

#include <SDL2/SDL.h>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

namespace gb {

class Driver {
public:
  Driver(const char *path);

  ~Driver() {
    SDL_Quit();
  }

  void drive();
  void process();

  inline bool did_quit() { return m_did_close; }

private:
  bool m_did_close;
  fs::path m_rom_path;

  Gameboy m_gameboy;

  std::vector<std::byte> m_rom_data;

  utility::sdl_window_ptr m_window = {nullptr, SDL_DestroyWindow};
  utility::sdl_renderer_ptr m_renderer = {nullptr, SDL_DestroyRenderer};
  utility::sdl_texture_ptr m_texture = {nullptr, SDL_DestroyTexture};
};

} // namespace gb

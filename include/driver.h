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

  inline bool did_quit() { return this->did_close; }

private:
  bool did_close;
  fs::path rom_path;

  Gameboy gameboy;

  std::vector<std::byte> rom_data;

  utility::sdl_window_ptr window = {nullptr, SDL_DestroyWindow};
  utility::sdl_renderer_ptr renderer = {nullptr, SDL_DestroyRenderer};
  utility::sdl_texture_ptr texture = {nullptr, SDL_DestroyTexture};
};

} // namespace gb

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
  Driver(const char *path) : rom_path(path), gameboy(rom_data) {
    if (!fs::exists(rom_path))
      utility::error("Does this path to rom really exist?", 1);

    rom_data = utility::get_rom_data(this->rom_path);

    SDL_Init(SDL_INIT_VIDEO);

    window.reset(SDL_CreateWindow("gamerboy", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, 320, 288,
                                  SDL_WINDOW_OPENGL));

    if (window == nullptr)
      utility::error("Unable to create window", 1);

    renderer.reset(SDL_CreateRenderer(window.get(), -1,
                                      SDL_RENDERER_ACCELERATED |
                                          SDL_RENDERER_PRESENTVSYNC));

    texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, 320, 288));
  }

  ~Driver() {
    SDL_Quit();
  }

  void drive();
  void process();

  bool did_quit() { return this->did_close; }

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

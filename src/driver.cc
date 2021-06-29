#include "driver.h"

namespace gb {

Driver::Driver(const char *path) : rom_path(path), gameboy(rom_data) {
  if (!fs::exists(this->rom_path))
    utility::error("Does this path to rom really exist?", 1);

  this->rom_data = utility::get_rom_data(this->rom_path);

  SDL_Init(SDL_INIT_VIDEO);

  this->window.reset(SDL_CreateWindow("gamerboy", SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED, 320, 288,
                                      SDL_WINDOW_OPENGL));

  if (this->window == nullptr)
    utility::error("Unable to create window", 1);

  this->renderer.reset(SDL_CreateRenderer(
      window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

  this->texture.reset(SDL_CreateTexture(renderer.get(),
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING, 320, 288));
}

void Driver::drive() {
  while (!did_quit()) {
    process();
    this->gameboy.run();
  }
}

void Driver::process() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      this->did_close = true;
      break;
    }
  }
}

} // namespace gb

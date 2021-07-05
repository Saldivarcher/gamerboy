#include "gameboy.h"

namespace gb {

Gameboy::Gameboy(const char *path)
    : m_rom_path(path), m_rom_data(utility::get_rom_data(m_rom_path)),
      m_cartridge(m_rom_data), m_cpu(m_cartridge, m_mem), m_mem(m_cartridge) {

  SDL_Init(SDL_INIT_VIDEO);

  m_window.reset(SDL_CreateWindow("gamerboy", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, 320, 288,
                                  SDL_WINDOW_OPENGL));

  if (m_window == nullptr)
    utility::error("Unable to create window", 1);

  m_renderer.reset(
      SDL_CreateRenderer(m_window.get(), -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

  m_texture.reset(SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, 320, 288));
}

Gameboy::~Gameboy() { SDL_Quit(); }

void Gameboy::on() {
  while (!did_quit()) {
    process();
    run();
  }
}

void Gameboy::process() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      m_did_close = true;
      break;
    }
  }
}

void Gameboy::run() { m_cpu.cycle(); }

} // namespace gb

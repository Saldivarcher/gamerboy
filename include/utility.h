#pragma once

#include <SDL2/SDL.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

namespace gb {
namespace utility {

struct Mask {
  std::uint16_t value;
  std::uint16_t mask;

  friend bool operator<(Mask const &a, Mask const &b) {
    return a.value < (a.mask & b.value);
  }

  friend bool operator<(Mask const &a, std::uint16_t b) {
    return a.value < (a.mask & b);
  }

  friend bool operator<(std::uint16_t a, Mask const &b) {
    return b.value < (b.mask & a);
  }
};

using sdl_texture_ptr =
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using sdl_renderer_ptr =
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using sdl_window_ptr =
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;

static void error(const char *msg, uint8_t status) {
  std::cerr << msg << std::endl;
  std::exit(status);
}

static std::vector<std::byte> get_data(const fs::path &&p) {

  if (!fs::exists(p))
    utility::error("Does this path to rom really exist?", 1);

  std::ifstream ifs(p, std::ios::binary | std::ios::ate);
  auto end = ifs.tellg();

  ifs.seekg(0, std::ios::beg);

  auto size = std::size_t(end - ifs.tellg());

  if (!size)
    return {};

  std::vector<std::byte> buffer(size);

  if (!ifs.read((char *)buffer.data(), buffer.size()))
    error("Cannot insert ROM data!", 1);

  return buffer;
}

static std::vector<std::byte> get_boot_rom_data() {
  return get_data(fs::path("../boot_rom/dmg_boot.bin"));
}

static std::vector<std::byte> get_rom_data(const fs::path &p) {
  return get_data(std::move(p));
}

} // namespace utility
} // namespace gb

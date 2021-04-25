#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <utility>

#include "cartridge.h"
#include "utility.h"

namespace gb {

class Memory {
public:
  Memory(Cartridge &c);

  uint8_t read_memory(uint8_t addr);

private:
  Cartridge &cartridge;

  // Gameboys have a 8192 bytes of ram.
  std::array<uint8_t, 0x2000> ram;

  // The boot rom only has 255 bytes.
  std::array<uint8_t, 0xFF> boot_rom;

  // using memory_method_t = std::function<uint8_t(Memory &, uint8_t)>;
  typedef uint8_t (Memory::*memory_method_t)(uint16_t);

  uint8_t read_rom(uint16_t);
  uint8_t read_mbc_rom(uint16_t);
  uint8_t read_vram(uint16_t);
  uint8_t read_mbc_ram(uint16_t);
  uint8_t read_ram(uint16_t);
  uint8_t read_banked_ram(uint16_t);
  uint8_t read_high_memory(uint16_t);

  std::map<utility::Mask, memory_method_t, std::less<>> read_map = {
      {utility::Mask{0x0000, 0xFFFF}, &Memory::read_rom},
      {utility::Mask{0x1000, 0x0FFF}, &Memory::read_rom},
      {utility::Mask{0x2000, 0x0FFF}, &Memory::read_rom},
      {utility::Mask{0x3000, 0x0FFF}, &Memory::read_rom},
      {utility::Mask{0x4000, 0x0FFF}, &Memory::read_mbc_rom},
      {utility::Mask{0x5000, 0x0FFF}, &Memory::read_mbc_rom},
      {utility::Mask{0x6000, 0x0FFF}, &Memory::read_mbc_rom},
      {utility::Mask{0x7000, 0x0FFF}, &Memory::read_mbc_rom},
      {utility::Mask{0x8000, 0x0FFF}, &Memory::read_vram},
      {utility::Mask{0x9000, 0x0FFF}, &Memory::read_vram},
      {utility::Mask{0xA000, 0x0FFF}, &Memory::read_mbc_ram},
      {utility::Mask{0xB000, 0x0FFF}, &Memory::read_mbc_ram},
      {utility::Mask{0xC000, 0x0FFF}, &Memory::read_ram},
      {utility::Mask{0xD000, 0x0FFF}, &Memory::read_banked_ram},
      {utility::Mask{0xE000, 0x0FFF}, &Memory::read_ram},
      {utility::Mask{0xF000, 0x0FFF}, &Memory::read_high_memory},
  };
};

} // namespace gb

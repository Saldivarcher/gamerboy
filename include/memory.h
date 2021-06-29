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
  Memory(NoMbc &c);

  uint8_t read_memory(uint16_t addr);
  void write_memory(uint16_t addr, uint8_t value);

private:
  NoMbc &cartridge;

  // Gameboys have a 8192 bytes of ram.
  std::array<uint8_t, 0x2000> ram;

  // The boot rom only has 255 bytes.
  std::vector<std::byte> boot_rom;

  bool boot_rom_disabled = false;
  inline bool is_boot_rom_disabled() { return this->boot_rom_disabled; };

  // using memory_method_t = std::function<uint8_t(Memory &, uint8_t)>;
  typedef uint8_t (Memory::*memory_read_method_t)(uint16_t);
  typedef void (Memory::*memory_write_method_t)(uint16_t, uint8_t);

  uint8_t read_rom(uint16_t addr);
  uint8_t read_mbc_rom(uint16_t addr);
  uint8_t read_vram(uint16_t addr);
  uint8_t read_mbc_ram(uint16_t addr);
  uint8_t read_ram(uint16_t addr);
  uint8_t read_banked_ram(uint16_t addr);
  uint8_t read_high_memory(uint16_t addr);

  std::map<utility::Mask, memory_read_method_t, std::less<>> read_map = {
      {utility::Mask{0x0000, 0xF000}, &Memory::read_rom},
      {utility::Mask{0x1000, 0xF000}, &Memory::read_rom},
      {utility::Mask{0x2000, 0xF000}, &Memory::read_rom},
      {utility::Mask{0x3000, 0xF000}, &Memory::read_rom},
      {utility::Mask{0x4000, 0xF000}, &Memory::read_mbc_rom},
      {utility::Mask{0x5000, 0xF000}, &Memory::read_mbc_rom},
      {utility::Mask{0x6000, 0xF000}, &Memory::read_mbc_rom},
      {utility::Mask{0x7000, 0xF000}, &Memory::read_mbc_rom},
      {utility::Mask{0x8000, 0xF000}, &Memory::read_vram},
      {utility::Mask{0x9000, 0xF000}, &Memory::read_vram},
      {utility::Mask{0xA000, 0xF000}, &Memory::read_mbc_ram},
      {utility::Mask{0xB000, 0xF000}, &Memory::read_mbc_ram},
      {utility::Mask{0xC000, 0xF000}, &Memory::read_ram},
      {utility::Mask{0xD000, 0xF000}, &Memory::read_banked_ram},
      {utility::Mask{0xE000, 0xF000}, &Memory::read_ram},
      {utility::Mask{0xF000, 0xF000}, &Memory::read_high_memory},
  };

  void write_mbc(uint16_t addr, uint8_t value);
  void write_ram(uint16_t addr, uint8_t value);
  void write_mbc_ram(uint16_t addr, uint8_t value);
  void write_vram(uint16_t addr, uint8_t value);
  void write_banked_ram(uint16_t addr, uint8_t value);
  void write_high_memory(uint16_t addr, uint8_t value);

  std::map<utility::Mask, memory_write_method_t, std::less<>> write_map = {
      {utility::Mask{0x0000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x1000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x2000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x3000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x4000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x5000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x6000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x7000, 0xF000}, &Memory::write_mbc},
      {utility::Mask{0x8000, 0xF000}, &Memory::write_vram},
      {utility::Mask{0x9000, 0xF000}, &Memory::write_vram},
      {utility::Mask{0xA000, 0xF000}, &Memory::write_mbc_ram},
      {utility::Mask{0xB000, 0xF000}, &Memory::write_mbc_ram},
      {utility::Mask{0xC000, 0xF000}, &Memory::write_ram},
      {utility::Mask{0xD000, 0xF000}, &Memory::write_banked_ram},
      {utility::Mask{0xE000, 0xF000}, &Memory::write_ram},
      {utility::Mask{0xF000, 0xF000}, &Memory::write_high_memory},
  };
};

} // namespace gb

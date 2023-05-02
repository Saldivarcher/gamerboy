#pragma once

#include "cartridge.h"
#include "utility.h"

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <utility>

namespace gb {

class Gameboy;

class Memory {
public:
  Memory(Gameboy &gb);

  uint8_t read_memory(uint16_t addr);
  void write_memory(uint16_t addr, uint8_t value);

private:
  Gameboy &m_gb;
  NoMbc &m_cartridge;

  // Gameboys have a 8192 bytes of ram.
  std::array<uint8_t, 0x2000> m_ram;

  // The boot rom only has 255 bytes.
  std::vector<std::byte> m_boot_rom;

  bool m_boot_rom_disabled = false;
  inline bool is_boot_rom_disabled() { return m_boot_rom_disabled; };

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

  // Memory Map
  // clang-format off
  // 0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
  // 4000	7FFF	16 KiB ROM Bank 01~NN	From cartridge, switchable bank via mapper (if any)
  // 8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
  // A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
  // C000	CFFF	4 KiB Work RAM (WRAM)
  // D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1~7
  // E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	Nintendo says use of this area is prohibited.
  // FE00	FE9F	Sprite attribute table (OAM)
  // FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited
  // FF00	FF7F	I/O Registers
  // FF80	FFFE	High RAM (HRAM)
  // clang-format on

  std::map<utility::Mask, memory_read_method_t, std::less<>> m_read_map = {
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

  std::map<utility::Mask, memory_write_method_t, std::less<>> m_write_map = {
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

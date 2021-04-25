#include "memory.h"

namespace gb {
Memory::Memory(Cartridge &c) : cartridge(c) {
  this->boot_rom = utility::get_boot_rom_data();
}

uint8_t Memory::read_memory(uint8_t addr) {
  if (this->read_map.count(addr) > 0) {
    auto c = this->read_map.find(addr);
    auto fp = c->second;
    return (this->*fp)(addr);
  }

  return 0;
}

uint8_t Memory::read_rom(uint16_t addr) {
  if (addr < 0x100)
    return this->boot_rom[addr];
  return this->cartridge.read(addr);
}

uint8_t Memory::read_ram(uint16_t addr) { return addr; }
uint8_t Memory::read_vram(uint16_t addr) { return addr; }
uint8_t Memory::read_mbc_rom(uint16_t addr) { return addr; }
uint8_t Memory::read_mbc_ram(uint16_t addr) { return addr; }
uint8_t Memory::read_high_memory(uint16_t addr) { return addr; }
uint8_t Memory::read_banked_ram(uint16_t addr) { return addr; }

} // namespace gb

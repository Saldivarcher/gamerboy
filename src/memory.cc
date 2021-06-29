#include "memory.h"

namespace gb {
Memory::Memory(NoMbc &c) : cartridge(c) {
  this->boot_rom = utility::get_boot_rom_data();
}

uint8_t Memory::read_memory(uint16_t addr) {
  if (this->read_map.count(addr)) {
    auto read_method = this->read_map.find(addr)->second;
    return (this->*read_method)(addr);
  }

  return 0;
}

uint8_t Memory::read_rom(uint16_t addr) {
  if (addr < 0x100 && !this->is_boot_rom_disabled())
    return static_cast<uint8_t>(this->boot_rom[addr]);

  return this->cartridge.read(addr);
}

uint8_t Memory::read_ram(uint16_t addr) { return addr; }
uint8_t Memory::read_vram(uint16_t addr) { return addr; }
uint8_t Memory::read_mbc_rom(uint16_t addr) { return addr; }
uint8_t Memory::read_mbc_ram(uint16_t addr) { return addr; }
uint8_t Memory::read_high_memory(uint16_t addr) { return addr; }
uint8_t Memory::read_banked_ram(uint16_t addr) { return addr; }

void Memory::write_memory(uint16_t addr, uint8_t value) {
  if (this->write_map.count(addr)) {
    auto write_method = this->write_map.find(addr)->second;
    return (this->*write_method)(addr, value);
  }
}

void Memory::write_mbc(uint16_t addr, uint8_t value) { return; }
void Memory::write_ram(uint16_t addr, uint8_t value) { return; }
void Memory::write_mbc_ram(uint16_t addr, uint8_t value) { return; }
void Memory::write_vram(uint16_t addr, uint8_t value) { return; }
void Memory::write_banked_ram(uint16_t addr, uint8_t value) { return; }

void Memory::write_high_memory(uint16_t addr, uint8_t value) {
  if (addr < 0xFF80) {
    switch (addr & 0xFF) {
    case 0x50:
      this->boot_rom_disabled = true;
    }
  }
}

} // namespace gb

#include "cartridge.h"

namespace gb {

uint8_t NoMbc::read(uint16_t addr) {
  return static_cast<uint8_t>(m_rom_data.at(addr));
}

void NoMbc::write(uint16_t addr, uint8_t value) { return; }

} // namespace gb

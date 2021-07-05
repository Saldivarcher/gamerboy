#include "cartridge.h"

namespace gb {

u_int8_t NoMbc::read(u_int16_t addr) {
  return static_cast<u_int8_t>(m_rom_data.at(addr));
}

void NoMbc::write(u_int16_t addr, u_int8_t value) { return; }

} // namespace gb

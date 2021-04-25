#include "cartridge.h"

namespace gb {

u_int8_t Cartridge::read(u_int16_t addr) {
  return static_cast<int>(this->rom_data.at(addr));
}

} // namespace gb

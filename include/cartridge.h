#pragma once

#include <cstddef>
#include <vector>

namespace gb {
class Cartridge {
public:
  Cartridge(const std::vector<std::byte> &data) : rom_data(data){};

  u_int8_t read(u_int16_t addr);

private:
  const std::vector<std::byte> &rom_data;
};
} // namespace gb

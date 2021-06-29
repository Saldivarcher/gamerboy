#pragma once
#include "cartridge.h"
#include "cpu.h"
#include "memory.h"

#include <cstddef>
#include <vector>

namespace gb {
class Gameboy {
public:
  Gameboy(const std::vector<std::byte> &v);

  void run();

private:
  CPU cpu;
  Memory mem;
  NoMbc cartridge;
};
} // namespace gb

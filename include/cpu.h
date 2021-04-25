#pragma once
#include "cartridge.h"
#include "memory.h"

#include <array>
#include <cstdint>
#include <map>

namespace gb {

constexpr uint8_t WORD_REGISTER_LENGTH = 5;
enum Registers {
  AF,
  BC,
  DE,
  HL,
  SP,
};

class CPU {
public:
  CPU(Cartridge &, Memory &);

  void cycle();

private:
  uint8_t cycle_read();

  uint16_t pc;
  uint8_t interrupt_enable;

  std::array<uint16_t, WORD_REGISTER_LENGTH> registers;

  Cartridge &cartridge;
  Memory &mem;
};
} // namespace gb

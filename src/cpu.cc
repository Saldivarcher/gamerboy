#include "cpu.h"

namespace gb {

CPU::CPU(Cartridge &c, Memory &m) : cartridge(c), mem(m) {
  this->pc = 0x0;
}

void CPU::cycle() {
  uint8_t opcode = cycle_read();
  //
}

uint8_t CPU::cycle_read() {
  uint8_t ret = mem.read_memory(this->pc++);
  return ret;
}

} // namespace gb

#include "cpu.h"

namespace gb {

CPU::CPU(NoMbc &c, Memory &m) : cartridge(c), memory(m) { this->pc = 0x0; }

void CPU::cycle() {
  uint8_t opcode = cycle_read();
  if (this->opcode_map.count(opcode)) {
    auto method_ptr = this->opcode_map.find(opcode)->second;
    (this->*method_ptr)(opcode);
  }
}

uint8_t CPU::cycle_read() {
  uint8_t ret = this->memory.read_memory(this->pc++);
  return ret;
}

void CPU::op_nop(uint8_t opcode) {}

void CPU::op_ld_rr_d16(uint8_t opcode) {
  auto compose_word = [&, this]() -> uint16_t {
    uint16_t value = this->memory.read_memory(this->pc++);
    value |= this->memory.read_memory(this->pc++) << 8;
    return value;
  };

  uint8_t register_id = (opcode >> 4) + 1;
  uint16_t value = compose_word();
  this->registers[register_id] = value;
}

void CPU::op_ld_drr_a(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint16_t reg = this->registers[register_id];
  this->memory.write_memory(reg, opcode);
}

void CPU::op_inc_rr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  this->registers[register_id]++;
}

void CPU::op_dec_rr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  this->registers[register_id]--;
}

void CPU::op_inc_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  this->registers[register_id] += 0x100;
  this->registers[Registers::AF] &=
      ~(Flags::SUBTRACT_FLAG | Flags::HALF_CARRY_FLAG | Flags::ZERO_FLAG);
}

void CPU::op_dec_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  this->registers[register_id] -= 0x100;

  this->registers[Registers::AF] &=
      ~(Flags::HALF_CARRY_FLAG | Flags::ZERO_FLAG);

  this->registers[Registers::AF] |= Flags::SUBTRACT_FLAG;
}

void CPU::op_ld_da8_a(uint8_t opcode) {
  uint16_t address = 0xFF00 + this->memory.read_memory(this->pc++);
  uint8_t reg = this->registers[Registers::AF] >> 8;
  this->memory.write_memory(address, reg);
}

void CPU::op_ld_a_b(uint8_t opcode) {
  uint16_t a = registers[Registers::AF];
  uint16_t b = registers[Registers::BC];

  // Clear upper bits
  a &= 0xFF00;
  // Now set upper bits
  a |= (b & 0xFF00);

  registers[Registers::AF] = a;
}

} // namespace gb

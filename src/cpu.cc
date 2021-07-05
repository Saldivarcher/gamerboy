#include "cpu.h"

namespace gb {

CPU::CPU(NoMbc &c, Memory &m) : m_cartridge(c), m_memory(m) { m_pc = 0x0; }

void CPU::cycle() {
  uint8_t opcode = cycle_read(m_pc++);
  if (opcode_map.count(opcode)) {
    auto method_ptr = opcode_map.find(opcode)->second;
    (this->*method_ptr)(opcode);
  }
}

uint8_t CPU::cycle_read(uint16_t address) {
  uint8_t ret = m_memory.read_memory(address);
  return ret;
}

// Opcode: 0x00
// No operation
void CPU::op_nop(uint8_t opcode) {}

// Opcode: x1
// rr=nn (rr may be BC,DE,HL or SP)
void CPU::op_ld_rr_d16(uint8_t opcode) {
  auto compose_word = [&, this]() -> uint16_t {
    uint16_t value = cycle_read(m_pc++);
    value |= cycle_read(m_pc++) << 8;
    return value;
  };

  uint8_t register_id = (opcode >> 4) + 1;
  uint16_t value = compose_word();
  m_registers[register_id] = value;
}

// Opcode: x02, 0x12
// (BC)=A && (DE)=A
void CPU::op_ld_drr_a(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint16_t reg = m_registers[register_id];
  m_memory.write_memory(reg, opcode);
}

// Opcode: 0x0A, 0x1A
// A=(BC), A=(DE)
void CPU::op_ld_a_drr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[Registers::AF] &= 0xFF;
  m_registers[Registers::AF] |= cycle_read(m_registers[register_id]) << 8;
}

// Opcode: 0x08
// (nn)=SP
void CPU::op_ld_da16_sp(uint8_t opcode) {
  uint16_t address = cycle_read(m_pc++);
  address |= cycle_read(m_pc++) << 8;
  m_memory.write_memory(address, m_registers[Registers::SP] & 0xFF);
  m_memory.write_memory(address, m_registers[Registers::SP] >> 8);
}

// Opcode: x6
void CPU::op_ld_hr_d8(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] &= 0xFF;
  m_registers[register_id] |= cycle_read(m_pc++) << 8;
}

// Opocde: xE
void CPU::op_ld_lr_d8(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] &= 0xFF;
  m_registers[register_id] |= cycle_read(m_pc++);
}

// Opcode: x3
// rr = rr+1 ; rr may be BC,DE,HL,SP
void CPU::op_inc_rr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id]++;
}

// Opcode: xB
// rr = rr-1 ; rr may be BC,DE,HL,SP
void CPU::op_dec_rr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id]--;
}

// Opcode: x4
// Flags: z0h-
// r=r+1 && (HL)=(HL)+1
void CPU::op_inc_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] += 0x100;

  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
}

// Opcode: x5
// Flags: z1h-
// r=r-1
void CPU::op_dec_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] -= 0x100;

  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, true);
}

void CPU::op_inc_lr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint8_t value = m_registers[register_id].get_lower_value() + 1;
  m_registers[register_id].set_low_value(value);

  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);

  if ((m_registers[register_id] & 0x0F) == 0) {
    m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, true);
  }

  if ((m_registers[register_id] & 0xFF) == 0) {
    m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, true);
  }
}

void CPU::op_dec_lr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint8_t value = m_registers[register_id].get_lower_value() - 1;
  m_registers[register_id].set_low_value(value);

  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, true);

  if ((m_registers[register_id] & 0x0F) == 0) {
    m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, true);
  }

  if ((m_registers[register_id] & 0xFF) == 0) {
    m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, true);
  }
}

// Opcode: 0xE0
// write to io-port n (memory FF00+n)
void CPU::op_ld_da8_a(uint8_t opcode) {
  uint16_t address = 0xFF00 + m_memory.read_memory(m_pc++);
  uint8_t reg = m_registers[Registers::AF] >> 8;
  m_memory.write_memory(address, reg);
}

// Opcode: 7x
// (HL) = r
void CPU::op_ld_a_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_value();
  m_registers[Registers::AF].set_upper_value(b);
}

// Opcode: 0x07
// Flags: 000c
// Rotate A left
void CPU::op_rlca(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_value();
  uint8_t f = m_registers[Registers::AF].get_lower_value();

  // Check highest bit of `a`
  bool carry = ((a & 0x80) != 0);

  // Shift a over by 1 bit
  a <<= 1;

  // Clear out flags
  f &= 0x00;

  if (carry) {
    a |= 0x01;
    f |= Flags::CARRY_FLAG;
  }

  m_registers[Registers::AF].set_word(a, f);
}

// Opcode: 0x17
// Flags: 000c
// Rotate A left through carry
void CPU::op_rla(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_value();
  uint8_t f = m_registers[Registers::AF].get_lower_value();

  // Check highest bit of `a`
  bool most_sig_bit = (a & 0x80) != 0;
  bool carry_bit = (f & Flags::CARRY_FLAG) != 0;

  // Shift a over by 1 bit
  a <<= 1;
  // Clear out flags
  f &= 0x00;

  if (carry_bit) {
    a |= 0x01;
  }
  if (most_sig_bit) {
    f |= Flags::CARRY_FLAG;
  }

  m_registers[Registers::AF].set_word(a, f);
}

// Opcode: 0x0F
// Flags: 000c
// Rotate A right
void CPU::op_rrca(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_value();
  uint8_t f = m_registers[Registers::AF].get_lower_value();

  // Check lowest bit of `a`
  bool carry = ((a & 0x01) != 0);

  // Shift a over by 1 bit
  a >>= 1;

  // Clear out flags
  f &= 0x00;

  if (carry) {
    a |= 0x80;
    f |= Flags::CARRY_FLAG;
  }

  m_registers[Registers::AF].set_word(a, f);
}

// Opcode: 0x1F
// Flags: 000c
// Rotate A right through carry
void CPU::op_rra(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_value();
  uint8_t f = m_registers[Registers::AF].get_lower_value();

  // Check lowest bit of `a`
  bool lowest_sig_bit = ((a & 0x01) != 0);
  bool carry_bit = (f & Flags::CARRY_FLAG) != 0;

  // Shift a over by 1 bit
  a >>= 1;

  // Clear out flags
  f &= 0x00;

  if (carry_bit) {
    a |= 0x80;
  }

  if (lowest_sig_bit) {
    f |= Flags::CARRY_FLAG;
  }

  m_registers[Registers::AF].set_word(a, f);
}

} // namespace gb

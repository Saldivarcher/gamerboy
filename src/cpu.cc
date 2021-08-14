#include "cpu.h"

namespace gb {

CPU::CPU(NoMbc &c, Memory &m) : m_cartridge(c), m_memory(m) { m_pc = 0x0; }

void CPU::cycle() {
  uint8_t opcode = read_memory(m_pc++);
  if (opcode_map.count(opcode)) {
    auto method_ptr = opcode_map.find(opcode)->second;
    (this->*method_ptr)(opcode);
  }
}

uint8_t CPU::read_memory(uint16_t address) {
  uint8_t ret = m_memory.read_memory(address);
  return ret;
}

void CPU::write_memory(uint16_t address, uint8_t value) {
  m_memory.write_memory(address, value);
}

bool CPU::condition_code(uint8_t opcode) {
  uint8_t f = m_registers[Registers::AF].get_lower_register();
  switch (get_conditional_code(opcode)) {
  case 0:
    return !(f & Flags::ZERO_FLAG);
  case 1:
    return (f & Flags::ZERO_FLAG);
  case 2:
    return !(f & Flags::CARRY_FLAG);
  case 3:
    return (f & Flags::CARRY_FLAG);
  }

  return false;
}

// Opcode: 0x00
// Flags: ----
// No operation
void CPU::op_nop(uint8_t opcode) {}

// Opcode: x1
// rr=nn (rr may be BC,DE,HL or SP)
void CPU::op_ld_rr_d16(uint8_t opcode) {
  auto compose_word = [&, this]() -> uint16_t {
    uint16_t value = read_memory(m_pc++);
    value |= read_memory(m_pc++) << 8;
    return value;
  };

  uint8_t register_id = get_register(opcode);
  uint16_t value = compose_word();
  m_registers[register_id] = value;
}

// Opcode: x02, 0x12
// Flags: ----
// (BC)=A && (DE)=A
void CPU::op_ld_drr_a(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint16_t reg = m_registers[register_id];
  write_memory(reg, opcode);
}

// Opcode: 0x0A, 0x1A
// Flags: ----
// A=(BC), A=(DE)
void CPU::op_ld_a_drr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[Registers::AF] &= 0xFF;
  m_registers[Registers::AF] |= read_memory(m_registers[register_id]) << 8;
}

// Opcode: 0x22
// Flags: ----
// (HL)=A, HL=HL+1
void CPU::op_ld_dhli_a(uint8_t opcode) {
  write_memory(m_registers[Registers::HL]++,
               m_registers[Registers::AF].get_upper_register());
}

// Opcode: 0x2A
// Flags: ----
// A=[HL], HL=HL+1
void CPU::op_ld_a_dhli(uint8_t opcode) {
  m_registers[Registers::AF].set_upper_register(
      read_memory(m_registers[Registers::HL]++));
}

// Opcode: 0x32
// Flags: ----
// (HL)=A, HL=HL-1
void CPU::op_ld_dhld_a(uint8_t opcode) {
  write_memory(m_registers[Registers::HL]--,
               m_registers[Registers::AF].get_upper_register());
}

// Opcode: 0x3A
// Flags: ----
// A=[HL], HL=HL-1
void CPU::op_ld_a_dhld(uint8_t opcode) {
  m_registers[Registers::AF].set_upper_register(
      read_memory(m_registers[Registers::HL]--));
}

// Opcode: 0x36
// Flags: ----
// (HL)=n
void CPU::op_ld_dhl_d8(uint8_t opcode) {
  uint8_t address = read_memory(m_pc++);
  write_memory(m_registers[Registers::HL], address);
}

// Opcode: 0x08
// Flags: ----
// (nn)=SP
void CPU::op_ld_da16_sp(uint8_t opcode) {
  uint16_t address = read_memory(m_pc++);
  address |= read_memory(m_pc++) << 8;
  write_memory(address, m_registers[Registers::SP] & 0xFF);
  write_memory(address, m_registers[Registers::SP] >> 8);
}

// Opcode: x6
// r=n
void CPU::op_ld_hr_d8(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] &= 0xFF;
  m_registers[register_id] |= read_memory(m_pc++) << 8;
}

// Opcode: xE
// r=n
void CPU::op_ld_lr_d8(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] &= 0xFF;
  m_registers[register_id] |= read_memory(m_pc++);
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

// Opcode: 0x34
// Flags: z0h-
// (HL)=(HL)+1
void CPU::op_inc_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]) + 1;
  write_memory(m_registers[Registers::HL], value);

  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG,
                                      (value & 0x0F) == 0);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, (value & 0xFF) == 0);
}

// Opcode: 0x35
// Flags: z1h-
// (HL)=(HL)-1
void CPU::op_dec_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]) - 1;
  write_memory(m_registers[Registers::HL], value);

  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, true);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG,
                                      (value & 0x0F) == 0);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, (value & 0xFF) == 0);
}

// Opcode: x4
// Flags: z0h-
// r=r+1
void CPU::op_inc_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] += 0x100;

  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);

  bool has_half_carry = ((m_registers[register_id] & 0x0F00) == 0);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, has_half_carry);

  bool has_zero = ((m_registers[register_id] & 0xFF00) == 0);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, has_zero);
}

// Opcode: x5
// Flags: z1h-
// r=r-1
void CPU::op_dec_hr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  m_registers[register_id] -= 0x100;

  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, true);

  bool has_half_carry = ((m_registers[register_id] & 0x0F00) == 0);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, has_half_carry);

  bool has_zero = ((m_registers[register_id] & 0xFF00) == 0);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, has_zero);
}

void CPU::op_inc_lr(uint8_t opcode) {
  uint8_t register_id = get_register(opcode);
  uint8_t value = m_registers[register_id].get_lower_register() + 1;
  m_registers[register_id].set_lower_register(value);

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
  uint8_t value = m_registers[register_id].get_lower_register() - 1;
  m_registers[register_id].set_lower_register(value);

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
  write_memory(address, reg);
}

// Opcode: 0x41
void CPU::op_ld_b_c(uint8_t opcode) {
  uint8_t c = m_registers[Registers::BC].get_lower_register();
  m_registers[Registers::BC].set_upper_register(c);
}

// Opcode: 0x42
void CPU::op_ld_b_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::BC].set_upper_register(d);
}

// Opcode: 0x43
void CPU::op_ld_b_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::BC].set_upper_register(e);
}

// Opcode: 0x44
void CPU::op_ld_b_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();

  m_registers[Registers::BC].set_upper_register(h);
}

// Opcode: 0x45
void CPU::op_ld_b_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::BC].set_upper_register(l);
}

// Opcode: 0x46
void CPU::op_ld_b_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::BC].set_upper_register(value);
}

// Opcode: 0x47
void CPU::op_ld_b_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::BC].set_upper_register(a);
}

// Opcode: 0x48
void CPU::op_ld_c_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::BC].set_lower_register(b);
}

// Opcode: 0x4A
void CPU::op_ld_c_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::BC].set_lower_register(d);
}

// Opcode: 0x4B
void CPU::op_ld_c_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::BC].set_lower_register(e);
}

// Opcode: 0x4C
void CPU::op_ld_c_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  m_registers[Registers::BC].set_lower_register(h);
}

// Opcode: 0x4D
void CPU::op_ld_c_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::BC].set_lower_register(l);
}

// Opcode: 0x4E
void CPU::op_ld_c_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::BC].set_lower_register(value);
}

// Opcode: 0x4F
void CPU::op_ld_c_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::BC].set_lower_register(a);
}

// Opcode: 0x50
void CPU::op_ld_d_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::BC].set_lower_register(b);
}

// Opcode: 0x51
void CPU::op_ld_d_c(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::DE].set_upper_register(d);
}

// Opcode: 0x53
void CPU::op_ld_d_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::DE].set_upper_register(e);
}

// Opcode: 0x54
void CPU::op_ld_d_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  m_registers[Registers::DE].set_upper_register(h);
}

// Opcode: 0x55
void CPU::op_ld_d_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::DE].set_upper_register(l);
}

// Opcode: 0x56
void CPU::op_ld_d_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::DE].set_upper_register(value);
}

// Opcode: 0x57
void CPU::op_ld_d_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::DE].set_upper_register(a);
}

// Opcode: 0x58
void CPU::op_ld_e_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::DE].set_lower_register(b);
}

// Opcode: 0x59
void CPU::op_ld_e_c(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::DE].set_lower_register(d);
}

// Opcode: 0x5A
void CPU::op_ld_e_d(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::DE].set_lower_register(e);
}

// Opcode: 0x5C
void CPU::op_ld_e_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  m_registers[Registers::DE].set_lower_register(h);
}

// Opcode: 0x5D
void CPU::op_ld_e_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::DE].set_lower_register(l);
}

// Opcode: 0x5E
void CPU::op_ld_e_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::DE].set_lower_register(value);
}

// Opcode: 0x5F
void CPU::op_ld_e_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::DE].set_lower_register(a);
}

// Opcode: 0x60
void CPU::op_ld_h_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::HL].set_lower_register(b);
}

// Opcode: 0x61
void CPU::op_ld_h_c(uint8_t opcode) {
  uint8_t c = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::HL].set_upper_register(c);
}

// Opcode: 0x62
void CPU::op_ld_h_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::HL].set_upper_register(d);
}

// Opcode: 0x63
void CPU::op_ld_h_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::HL].set_upper_register(e);
}

// Opcode: 0x65
void CPU::op_ld_h_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::HL].set_upper_register(l);
}

// Opcode: 0x66
void CPU::op_ld_h_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::HL].set_upper_register(value);
}

// Opcode: 0x67
void CPU::op_ld_h_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::HL].set_upper_register(a);
}
// Opcode: 0x68
void CPU::op_ld_l_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::HL].set_lower_register(b);
}

// Opcode: 0x69
void CPU::op_ld_l_c(uint8_t opcode) {
  uint8_t c = m_registers[Registers::BC].get_lower_register();
  m_registers[Registers::HL].set_lower_register(c);
}

// Opcode: 0x6A
void CPU::op_ld_l_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::HL].set_lower_register(d);
}

// Opcode: 0x6B
void CPU::op_ld_l_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::HL].set_lower_register(e);
}

// Opcode: 0x6C
void CPU::op_ld_l_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  m_registers[Registers::HL].set_lower_register(h);
}

// Opcode: 0x6E
void CPU::op_ld_l_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::HL].set_lower_register(value);
}

// Opcode: 0x6F
void CPU::op_ld_l_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  m_registers[Registers::HL].set_lower_register(a);
}

// Opcode: 0x70
void CPU::op_ld_dhl_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  write_memory(m_registers[Registers::HL], b);
}

// Opcode: 0x71
void CPU::op_ld_dhl_c(uint8_t opcode) {
  uint8_t c = m_registers[Registers::DE].get_upper_register();
  write_memory(m_registers[Registers::HL], c);
}

// Opcode: 0x72
void CPU::op_ld_dhl_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  write_memory(m_registers[Registers::HL], d);
}

// Opcode: 0x73
void CPU::op_ld_dhl_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  write_memory(m_registers[Registers::HL], e);
}

// Opcode: 0x74
void CPU::op_ld_dhl_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  write_memory(m_registers[Registers::HL], h);
}

// Opcode: 0x75
void CPU::op_ld_dhl_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  write_memory(m_registers[Registers::HL], l);
}

// Opcode: 0x77
void CPU::op_ld_dhl_a(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  write_memory(m_registers[Registers::HL], a);
}

// Opcode: 0x78
void CPU::op_ld_a_b(uint8_t opcode) {
  uint8_t b = m_registers[Registers::BC].get_upper_register();
  m_registers[Registers::AF].set_upper_register(b);
}

// Opcode: 0x79
void CPU::op_ld_a_c(uint8_t opcode) {
  uint8_t c = m_registers[Registers::BC].get_lower_register();
  m_registers[Registers::AF].set_upper_register(c);
}

// Opcode: 0x7A
void CPU::op_ld_a_d(uint8_t opcode) {
  uint8_t d = m_registers[Registers::DE].get_upper_register();
  m_registers[Registers::AF].set_upper_register(d);
}

// Opcode: 0x7B
void CPU::op_ld_a_e(uint8_t opcode) {
  uint8_t e = m_registers[Registers::DE].get_lower_register();
  m_registers[Registers::AF].set_upper_register(e);
}

// Opcode: 0x7C
void CPU::op_ld_a_h(uint8_t opcode) {
  uint8_t h = m_registers[Registers::HL].get_upper_register();
  m_registers[Registers::AF].set_upper_register(h);
}

// Opcode: 0x7D
void CPU::op_ld_a_l(uint8_t opcode) {
  uint8_t l = m_registers[Registers::HL].get_lower_register();
  m_registers[Registers::AF].set_upper_register(l);
}

// Opcode: 0x7E
void CPU::op_ld_a_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  m_registers[Registers::AF].set_upper_register(value);
}

// Opcode: 0x07
// Flags: 000c
// Rotate A left
void CPU::op_rlca(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();

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
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();

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
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();

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
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();

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

// Opcode: x9
// Flags: -0hc
// HL = HL+rr ; rr may be BC,DE,HL,SP
void CPU::op_add_hl_rr(uint8_t opcode) {
  int8_t register_id = get_register(opcode);
  uint16_t rr = m_registers[register_id];
  uint16_t hl = m_registers[Registers::HL];

  uint16_t result = hl + rr;

  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG,
                                      (result & 0x10000) != 0);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG,
                                      (rr & 0xFFF) + (hl + 0xFFF) > 0xFFF);
  m_registers[Registers::HL].set_word(result);
}

void CPU::op_add_a_b(uint8_t opcode) {
  int8_t b = m_registers[Registers::BC].get_upper_register();
  add_a_r(b);
}

void CPU::op_add_a_c(uint8_t opcode) {
  int8_t c = m_registers[Registers::BC].get_lower_register();
  add_a_r(c);
}

void CPU::op_add_a_d(uint8_t opcode) {
  int8_t d = m_registers[Registers::DE].get_upper_register();
  add_a_r(d);
}

void CPU::op_add_a_e(uint8_t opcode) {
  int8_t e = m_registers[Registers::DE].get_lower_register();
  add_a_r(e);
}

void CPU::op_add_a_h(uint8_t opcode) {
  int8_t h = m_registers[Registers::HL].get_upper_register();
  add_a_r(h);
}

void CPU::op_add_a_l(uint8_t opcode) {
  int8_t l = m_registers[Registers::HL].get_lower_register();
  add_a_r(l);
}

void CPU::op_add_a_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  add_a_r(value);
}

void CPU::op_add_a_a(uint8_t opcode) {
  int8_t a = m_registers[Registers::AF].get_upper_register();
  add_a_r(a);
}

void CPU::op_adc_a_b(uint8_t opcode) {
  int8_t b = m_registers[Registers::BC].get_upper_register();
  adc_a_r(b);
}

void CPU::op_adc_a_c(uint8_t opcode) {
  int8_t c = m_registers[Registers::BC].get_lower_register();
  adc_a_r(c);
}

void CPU::op_adc_a_d(uint8_t opcode) {
  int8_t d = m_registers[Registers::DE].get_upper_register();
  adc_a_r(d);
}

void CPU::op_adc_a_e(uint8_t opcode) {
  int8_t e = m_registers[Registers::DE].get_lower_register();
  adc_a_r(e);
}

void CPU::op_adc_a_h(uint8_t opcode) {
  int8_t h = m_registers[Registers::HL].get_upper_register();
  adc_a_r(h);
}

void CPU::op_adc_a_l(uint8_t opcode) {
  int8_t l = m_registers[Registers::HL].get_lower_register();
  adc_a_r(l);
}

void CPU::op_adc_a_dhl(uint8_t opcode) {
  uint8_t value = read_memory(m_registers[Registers::HL]);
  adc_a_r(value);
}

void CPU::op_adc_a_a(uint8_t opcode) {
  int8_t a = m_registers[Registers::AF].get_upper_register();
  add_a_r(a);
}

void CPU::add_a_r(uint8_t value) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t result = a + value;

  m_registers[Registers::AF].set_upper_register(result);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, result == 0);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG,
                                      (a & 0xF) + (value & 0xF) > 0xF);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, (result & 0x100) != 0);
}

void CPU::adc_a_r(uint8_t value) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();
  uint8_t carry = (f & Flags::CARRY_FLAG) ? 0x60 : 0x00;

  uint16_t word_result = a + value + carry;
  uint8_t result = a + value + carry;

  m_registers[Registers::AF].set_upper_register(result);

  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, result == 0);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG,
                                      (a & 0xF) + (value & 0xF) + carry > 0xF);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, word_result > 0xFF);
}

// Opcode: 0x18
// Flags: ----
// relative jump to nn (PC=PC+8-bit signed)
// jr PC+dd
void CPU::op_jr_r8(uint8_t opcode) {
  m_pc = static_cast<int8_t>(read_memory(m_pc) + 1);
}

// Opcode: 0x20, 0x28, 0x30, 0x38
// Flags: ----
// conditional jump if nz,z,nc,c
void CPU::op_jr_cc_r8(uint8_t opcode) {
  int8_t offset = static_cast<int8_t>(read_memory(m_pc++));
  if (condition_code(opcode)) {
    m_pc += offset;
  }
}

// Opcode: 0x27
// Flags: z-0c
// decimal adjust A
void CPU::op_daa(uint8_t opcode) {
  uint8_t a = m_registers[Registers::AF].get_upper_register();
  uint8_t f = m_registers[Registers::AF].get_lower_register();

  uint8_t carry = (f & Flags::CARRY_FLAG) ? 0x60 : 0x00;
  bool half_carry = (f & Flags::HALF_CARRY_FLAG);
  bool subtract = (f & Flags::SUBTRACT_FLAG);

  if (!subtract) {
    if ((a & 0x0F) > 0x09)
      carry |= 0x06;
    if (a > 0x99)
      carry |= 0x60;
    a += carry;
  } else {
    a -= carry;
  }

  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, carry >= 0x60);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::ZERO_FLAG, a == 0x00);

  m_registers[Registers::AF].set_upper_register(a);
}

// Opcode: 0x2F
// Flags: -11-
// A = A xor FF
void CPU::op_cpl(uint8_t opcode) {
  uint8_t a = ~(m_registers[Registers::AF].get_upper_register());
  m_registers[Registers::AF].set_upper_register(a);

  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, true);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, true);
}

// Opcode: 0x37
// Flags: -001
void CPU::op_scf(uint8_t opcode) {
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, true);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
}

// Opcode: 0x3F
// Flags: -00c
// cy=cy xor 1
void CPU::op_ccf(uint8_t opcode) {
  uint8_t f = m_registers[Registers::AF].get_lower_register();
  bool compliment = !(f & Flags::CARRY_FLAG);
  m_registers[Registers::AF].set_flag(Flags::CARRY_FLAG, compliment);
  m_registers[Registers::AF].set_flag(Flags::HALF_CARRY_FLAG, false);
  m_registers[Registers::AF].set_flag(Flags::SUBTRACT_FLAG, false);
}

void CPU::op_stop(uint8_t opcode) {}

} // namespace gb

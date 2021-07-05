#pragma once

#include "cartridge.h"
#include "memory.h"
#include "registers.h"

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

enum Flags {
  CARRY_FLAG = 0x10,
  HALF_CARRY_FLAG = 0x20,
  SUBTRACT_FLAG = 0x40,
  ZERO_FLAG = 0x80,
};

class CPU {
public:
  CPU(NoMbc &, Memory &);

  void cycle();

private:
  uint8_t cycle_read(uint16_t address);
  inline uint8_t get_register(uint8_t opcode) { return (opcode >> 4) + 1; }

  uint16_t m_pc;
  uint8_t m_interrupt_enable;

  std::array<Register, WORD_REGISTER_LENGTH> m_registers;

  NoMbc &m_cartridge;
  Memory &m_memory;

  typedef void (CPU::*opcode_method_t)(uint8_t);

  // Opcode names:
  // r = 8-bit register
  // lr = low 8-bit register
  // hr = high 8-bit register
  // rr = 16-bit register
  // d8 = 8-bit immediate
  // d16 = 16-bit immediate
  // d.. = [..]
  // cc = condition code (z, nz, c, nc)

  void op_nop(uint8_t opcode);

  // Load
  void op_ld_da8_a(uint8_t opcode);
  void op_ld_da16_sp(uint8_t opcode);
  void op_ld_a_b(uint8_t opcode);
  void op_ld_hr_d8(uint8_t opcode);
  void op_ld_lr_d8(uint8_t opcode);
  void op_ld_rr_d16(uint8_t opcode);
  void op_ld_drr_a(uint8_t opcode);
  void op_ld_a_drr(uint8_t opcode);

  // Inc and Dec 16 bit
  void op_inc_rr(uint8_t opcode);
  void op_dec_rr(uint8_t opcode);

  // Inc and Dec 8 bit high reg
  void op_inc_hr(uint8_t opcode);
  void op_dec_hr(uint8_t opcode);

  // Inc and Dec 8 bit low reg
  void op_inc_lr(uint8_t opcode);
  void op_dec_lr(uint8_t opcode);

  // Rotate
  void op_rlca(uint8_t opcode);
  void op_rla(uint8_t opcode);
  void op_rrca(uint8_t opcode);
  void op_rra(uint8_t opcode);

  // Add
  void op_add_hl_rr(uint8_t opcode);

  // clang-format off
  std::map<uint8_t, opcode_method_t> opcode_map{
      {0x00, &CPU::op_nop},
      {0x01, &CPU::op_ld_rr_d16},
      {0x02, &CPU::op_ld_drr_a},
      {0x03, &CPU::op_inc_rr},
      {0x04, &CPU::op_inc_hr},
      {0x05, &CPU::op_dec_hr},
      {0x06, &CPU::op_ld_hr_d8},
      {0x07, &CPU::op_rlca},
      {0x08, &CPU::op_ld_da16_sp},
      // {0x09, &CPU::op_add_hl_rr},
      {0x0A, &CPU::op_ld_a_drr},
      {0x0B, &CPU::op_dec_rr},
      {0x0C, &CPU::op_inc_lr},
      {0x0D, &CPU::op_dec_lr},
      {0x0E, &CPU::op_ld_lr_d8},
      {0x0F, &CPU::op_rrca},
      {0x11, &CPU::op_ld_rr_d16},
      {0x12, &CPU::op_ld_drr_a},
      {0x13, &CPU::op_inc_rr},
      {0x14, &CPU::op_inc_hr},
      {0x16, &CPU::op_ld_hr_d8},
      {0x17, &CPU::op_rla},
      {0x1A, &CPU::op_ld_a_drr},
      {0x1B, &CPU::op_dec_rr},
      {0x1C, &CPU::op_inc_lr},
      {0x1D, &CPU::op_dec_lr},
      {0x1E, &CPU::op_ld_lr_d8},
      {0x1F, &CPU::op_rra},
      {0x21, &CPU::op_ld_rr_d16},
      {0x23, &CPU::op_inc_rr},
      {0x24, &CPU::op_inc_hr},
      {0x26, &CPU::op_ld_hr_d8},
      {0x2B, &CPU::op_dec_rr},
      {0x2C, &CPU::op_inc_lr},
      {0x2D, &CPU::op_dec_lr},
      {0x2E, &CPU::op_ld_lr_d8},
      {0x31, &CPU::op_ld_rr_d16},
      {0x33, &CPU::op_inc_rr},
      {0x34, &CPU::op_inc_hr},
      {0x3B, &CPU::op_dec_rr},
      {0x78, &CPU::op_ld_a_b},
      {0xE0, &CPU::op_ld_da8_a},
  };
  // clang-format on
};
} // namespace gb

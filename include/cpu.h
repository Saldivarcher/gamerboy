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
  uint8_t cycle_read();
  inline uint8_t get_register(uint8_t opcode) { return (opcode >> 4) + 1; }

  uint16_t pc;
  uint8_t interrupt_enable;

  std::array<uint16_t, WORD_REGISTER_LENGTH> registers;

  NoMbc &cartridge;
  Memory &memory;

  typedef void (CPU::*opcode_method_t)(uint8_t);

  // Opcodes
  void op_nop(uint8_t opcode);
  void op_ld_rr_d16(uint8_t opcode);
  void op_ld_drr_a(uint8_t opcode);
  void op_inc_rr(uint8_t opcode);
  void op_dec_rr(uint8_t opcode);
  void op_inc_hr(uint8_t opcode);
  void op_dec_hr(uint8_t opcode);
  void op_ld_da8_a(uint8_t opcode);
  void op_ld_a_b(uint8_t opcode);

  // clang-format off
  std::map<uint8_t, opcode_method_t> opcode_map{
      {0x00, &CPU::op_nop},
      {0x01, &CPU::op_ld_rr_d16},
      {0x02, &CPU::op_ld_drr_a},
      {0x03, &CPU::op_inc_rr},
      {0x04, &CPU::op_inc_hr},
      {0x05, &CPU::op_dec_hr},
      {0x0B, &CPU::op_dec_rr},
      {0xE0, &CPU::op_ld_da8_a},
      {0x11, &CPU::op_ld_rr_d16},
      {0x12, &CPU::op_ld_drr_a},
      {0x13, &CPU::op_inc_rr},
      {0x14, &CPU::op_inc_hr},
      {0x1B, &CPU::op_dec_rr},
      {0x21, &CPU::op_ld_rr_d16},
      {0x23, &CPU::op_inc_rr},
      {0x24, &CPU::op_inc_hr},
      {0x2B, &CPU::op_dec_rr},
      {0x31, &CPU::op_ld_rr_d16},
      {0x33, &CPU::op_inc_rr},
      {0x34, &CPU::op_inc_hr},
      {0x3B, &CPU::op_dec_rr},
      {0x78, &CPU::op_ld_a_b},
  };
  // clang-format on
};
} // namespace gb

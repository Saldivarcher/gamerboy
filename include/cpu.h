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
  uint8_t read_memory(uint16_t address);
  void write_memory(uint16_t address, uint8_t value);

  bool condition_code(uint8_t opcode);
  inline uint8_t get_register(uint8_t opcode) { return (opcode >> 4) + 1; }
  inline uint8_t get_conditional_code(uint8_t opcode) { return (opcode >> 3); }

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
  void op_ld_hr_d8(uint8_t opcode);
  void op_ld_lr_d8(uint8_t opcode);
  void op_ld_rr_d16(uint8_t opcode);
  void op_ld_drr_a(uint8_t opcode);
  void op_ld_a_drr(uint8_t opcode);
  void op_ld_dhli_a(uint8_t opcode);
  void op_ld_a_dhli(uint8_t opcode);
  void op_ld_dhld_a(uint8_t opcode);
  void op_ld_a_dhld(uint8_t opcode);
  void op_ld_dhl_d8(uint8_t opcode);

  void op_ld_b_c(uint8_t opcode);
  void op_ld_b_d(uint8_t opcode);
  void op_ld_b_e(uint8_t opcode);
  void op_ld_b_h(uint8_t opcode);
  void op_ld_b_l(uint8_t opcode);
  void op_ld_b_dhl(uint8_t opcode);
  void op_ld_b_a(uint8_t opcode);

  void op_ld_c_b(uint8_t opcode);
  void op_ld_c_d(uint8_t opcode);
  void op_ld_c_e(uint8_t opcode);
  void op_ld_c_h(uint8_t opcode);
  void op_ld_c_l(uint8_t opcode);
  void op_ld_c_dhl(uint8_t opcode);
  void op_ld_c_a(uint8_t opcode);

  void op_ld_d_b(uint8_t opcode);
  void op_ld_d_c(uint8_t opcode);
  void op_ld_d_e(uint8_t opcode);
  void op_ld_d_h(uint8_t opcode);
  void op_ld_d_l(uint8_t opcode);
  void op_ld_d_dhl(uint8_t opcode);
  void op_ld_d_a(uint8_t opcode);

  void op_ld_e_b(uint8_t opcode);
  void op_ld_e_c(uint8_t opcode);
  void op_ld_e_d(uint8_t opcode);
  void op_ld_e_h(uint8_t opcode);
  void op_ld_e_l(uint8_t opcode);
  void op_ld_e_dhl(uint8_t opcode);
  void op_ld_e_a(uint8_t opcode);

  void op_ld_h_b(uint8_t opcode);
  void op_ld_h_c(uint8_t opcode);
  void op_ld_h_d(uint8_t opcode);
  void op_ld_h_e(uint8_t opcode);
  void op_ld_h_l(uint8_t opcode);
  void op_ld_h_dhl(uint8_t opcode);
  void op_ld_h_a(uint8_t opcode);

  void op_ld_l_b(uint8_t opcode);
  void op_ld_l_c(uint8_t opcode);
  void op_ld_l_d(uint8_t opcode);
  void op_ld_l_e(uint8_t opcode);
  void op_ld_l_h(uint8_t opcode);
  void op_ld_l_dhl(uint8_t opcode);
  void op_ld_l_a(uint8_t opcode);

  void op_ld_dhl_b(uint8_t opcode);
  void op_ld_dhl_c(uint8_t opcode);
  void op_ld_dhl_d(uint8_t opcode);
  void op_ld_dhl_e(uint8_t opcode);
  void op_ld_dhl_h(uint8_t opcode);
  void op_ld_dhl_l(uint8_t opcode);
  void op_ld_dhl_a(uint8_t opcode);

  void op_ld_a_b(uint8_t opcode);
  void op_ld_a_c(uint8_t opcode);
  void op_ld_a_d(uint8_t opcode);
  void op_ld_a_e(uint8_t opcode);
  void op_ld_a_h(uint8_t opcode);
  void op_ld_a_l(uint8_t opcode);
  void op_ld_a_dhl(uint8_t opcode);

  // Inc and Dec 16 bit
  void op_inc_rr(uint8_t opcode);
  void op_dec_rr(uint8_t opcode);
  void op_inc_dhl(uint8_t opcode);
  void op_dec_dhl(uint8_t opcode);

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

  void op_add_a_b(uint8_t opcode);
  void op_add_a_c(uint8_t opcode);
  void op_add_a_d(uint8_t opcode);
  void op_add_a_e(uint8_t opcode);
  void op_add_a_h(uint8_t opcode);
  void op_add_a_l(uint8_t opcode);
  void op_add_a_dhl(uint8_t opcode);
  void op_add_a_a(uint8_t opcode);

  void op_adc_a_b(uint8_t opcode);
  void op_adc_a_c(uint8_t opcode);
  void op_adc_a_d(uint8_t opcode);
  void op_adc_a_e(uint8_t opcode);
  void op_adc_a_h(uint8_t opcode);
  void op_adc_a_l(uint8_t opcode);
  void op_adc_a_dhl(uint8_t opcode);
  void op_adc_a_a(uint8_t opcode);

  void add_a_r(uint8_t value);
  void adc_a_r(uint8_t value);

  // Jumps
  void op_jr_r8(uint8_t opcode);
  void op_jr_cc_r8(uint8_t opcode);

  void op_daa(uint8_t opcode);
  void op_cpl(uint8_t opcode);
  void op_scf(uint8_t opcode);
  void op_ccf(uint8_t opcode);
  void op_stop(uint8_t opcode);

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
      {0x09, &CPU::op_add_hl_rr},
      {0x0A, &CPU::op_ld_a_drr},
      {0x0B, &CPU::op_dec_rr},
      {0x0C, &CPU::op_inc_lr},
      {0x0D, &CPU::op_dec_lr},
      {0x0E, &CPU::op_ld_lr_d8},
      {0x0F, &CPU::op_rrca},

      {0x10, &CPU::op_stop},
      {0x11, &CPU::op_ld_rr_d16},
      {0x12, &CPU::op_ld_drr_a},
      {0x13, &CPU::op_inc_rr},
      {0x14, &CPU::op_inc_hr},
      {0x15, &CPU::op_dec_hr},
      {0x16, &CPU::op_ld_hr_d8},
      {0x17, &CPU::op_rla},
      {0x18, &CPU::op_jr_r8},
      {0x19, &CPU::op_add_hl_rr},
      {0x1A, &CPU::op_ld_a_drr},
      {0x1B, &CPU::op_dec_rr},
      {0x1C, &CPU::op_inc_lr},
      {0x1D, &CPU::op_dec_lr},
      {0x1E, &CPU::op_ld_lr_d8},
      {0x1F, &CPU::op_rra},

      {0x20, &CPU::op_jr_cc_r8},
      {0x21, &CPU::op_ld_rr_d16},
      {0x22, &CPU::op_ld_dhli_a},
      {0x23, &CPU::op_inc_rr},
      {0x24, &CPU::op_inc_hr},
      {0x25, &CPU::op_dec_hr},
      {0x26, &CPU::op_ld_hr_d8},
      {0x27, &CPU::op_daa},
      {0x28, &CPU::op_jr_cc_r8},
      {0x29, &CPU::op_add_hl_rr},
      {0x2A, &CPU::op_ld_a_dhli},
      {0x2B, &CPU::op_dec_rr},
      {0x2C, &CPU::op_inc_lr},
      {0x2D, &CPU::op_dec_lr},
      {0x2E, &CPU::op_ld_lr_d8},
      {0x2F, &CPU::op_cpl},

      {0x30, &CPU::op_jr_cc_r8},
      {0x31, &CPU::op_ld_rr_d16},
      {0x32, &CPU::op_ld_dhld_a},
      {0x33, &CPU::op_inc_rr},
      {0x34, &CPU::op_inc_dhl},
      {0x35, &CPU::op_dec_dhl},
      {0x36, &CPU::op_ld_dhl_d8},
      {0x37, &CPU::op_scf},
      {0x38, &CPU::op_jr_cc_r8},
      {0x39, &CPU::op_add_hl_rr},
      {0x3A, &CPU::op_ld_a_dhld},
      {0x3B, &CPU::op_dec_rr},
      {0x3C, &CPU::op_inc_hr},
      {0x3D, &CPU::op_dec_hr},
      {0x3E, &CPU::op_ld_hr_d8},
      {0x3F, &CPU::op_ccf},

      {0x40, &CPU::op_nop},

      // TODO: Think of a way to simplify these load instructions,
      //       the new way should be generic enough so all simple load
      //       instructions can use it.
      {0x41, &CPU::op_ld_b_c},
      {0x42, &CPU::op_ld_b_d},
      {0x43, &CPU::op_ld_b_e},
      {0x44, &CPU::op_ld_b_h},
      {0x45, &CPU::op_ld_b_l},
      {0x46, &CPU::op_ld_b_dhl},
      {0x47, &CPU::op_ld_b_a},

      {0x48, &CPU::op_ld_c_b},
      {0x49, &CPU::op_nop},
      {0x4a, &CPU::op_ld_c_d},
      {0x4b, &CPU::op_ld_c_e},
      {0x4c, &CPU::op_ld_c_h},
      {0x4d, &CPU::op_ld_c_l},
      {0x4e, &CPU::op_ld_c_dhl},
      {0x4f, &CPU::op_ld_c_a},

      {0x50, &CPU::op_ld_d_b},
      {0x51, &CPU::op_ld_d_c},
      {0x52, &CPU::op_nop},
      {0x53, &CPU::op_ld_d_e},
      {0x54, &CPU::op_ld_d_h},
      {0x55, &CPU::op_ld_d_l},
      {0x56, &CPU::op_ld_d_dhl},
      {0x57, &CPU::op_ld_d_a},

      {0x58, &CPU::op_ld_e_b},
      {0x59, &CPU::op_ld_e_c},
      {0x5A, &CPU::op_ld_e_d},
      {0x5B, &CPU::op_nop},
      {0x5C, &CPU::op_ld_e_h},
      {0x5D, &CPU::op_ld_e_l},
      {0x5E, &CPU::op_ld_e_dhl},
      {0x5F, &CPU::op_ld_e_a},

      {0x60, &CPU::op_ld_h_b},
      {0x61, &CPU::op_ld_h_c},
      {0x62, &CPU::op_ld_h_d},
      {0x63, &CPU::op_ld_h_e},
      {0x64, &CPU::op_nop},
      {0x65, &CPU::op_ld_h_l},
      {0x66, &CPU::op_ld_h_dhl},
      {0x67, &CPU::op_ld_h_a},

      {0x68, &CPU::op_ld_l_b},
      {0x69, &CPU::op_ld_l_c},
      {0x6A, &CPU::op_ld_l_d},
      {0x6B, &CPU::op_ld_l_e},
      {0x6C, &CPU::op_ld_l_h},
      {0x6D, &CPU::op_nop},
      {0x6E, &CPU::op_ld_l_dhl},
      {0x6F, &CPU::op_ld_l_a},

      {0x70, &CPU::op_ld_dhl_b},
      {0x71, &CPU::op_ld_dhl_c},
      {0x72, &CPU::op_ld_dhl_d},
      {0x73, &CPU::op_ld_dhl_e},
      {0x74, &CPU::op_ld_dhl_h},
      {0x75, &CPU::op_ld_dhl_l},
      {0x76, &CPU::op_nop},
      {0x77, &CPU::op_ld_dhl_a},

      {0x78, &CPU::op_ld_a_b},
      {0x79, &CPU::op_ld_a_c},
      {0x7A, &CPU::op_ld_a_d},
      {0x7B, &CPU::op_ld_a_e},
      {0x7C, &CPU::op_ld_a_h},
      {0x7D, &CPU::op_ld_a_l},
      {0x7E, &CPU::op_ld_a_dhl},
      {0x7F, &CPU::op_nop},

      {0x80, &CPU::op_add_a_b},
      {0x81, &CPU::op_add_a_c},
      {0x82, &CPU::op_add_a_d},
      {0x83, &CPU::op_add_a_e},
      {0x84, &CPU::op_add_a_h},
      {0x85, &CPU::op_add_a_l},
      {0x86, &CPU::op_add_a_dhl},
      {0x87, &CPU::op_add_a_a},

      {0x88, &CPU::op_adc_a_b},
      {0x89, &CPU::op_adc_a_c},
      {0x8A, &CPU::op_adc_a_d},
      {0x8B, &CPU::op_adc_a_e},
      {0x8C, &CPU::op_adc_a_h},
      {0x8D, &CPU::op_adc_a_l},
      {0x8E, &CPU::op_adc_a_dhl},
      {0x8F, &CPU::op_adc_a_a},

      {0xE0, &CPU::op_ld_da8_a},
  };
  // clang-format on
};
} // namespace gb

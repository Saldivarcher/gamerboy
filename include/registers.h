#pragma once

#include <cstdint>

namespace gb {

// 1-byte Register.
class Register {
public:
  Register() = default;
  Register(uint8_t v) : m_value(v) {}

  void set_register(uint8_t value) { m_value = value; }
  uint8_t get_register() { return m_value; }

  void set_bit(uint8_t bit, bool set) {
    auto set_bit = [&] { m_value = m_value | (1 << bit); };
    auto clear_bit = [&] { m_value = m_value & ~(1 << bit); };
    set ? set_bit() : clear_bit();
  }

private:
  uint8_t m_value = 0;
};

// 2-byte Register.
class DoubleRegister {
public:
  DoubleRegister() = default;
  ~DoubleRegister() = default;

  uint16_t get_word() const {
    return static_cast<uint16_t>((m_high << 8) | m_low);
  }
  uint8_t get_upper_register() const { return m_high; }
  uint8_t get_lower_register() const { return m_low; }

  void set_upper_register(uint8_t upper) { m_high = upper; }
  void set_lower_register(uint8_t lower) { m_low = lower; }
  void set_word(uint16_t word) {
    m_high = static_cast<uint8_t>((word & 0xFF00) >> 8);
    m_low = static_cast<uint8_t>(word & 0x00FF);
  }

  // Should only be used on the AF register
  void set_flag(uint8_t flag, bool set) {
    (set) ? m_low |= flag : m_low &= ~(flag);
  }

  void set_word(uint8_t upper, uint8_t lower) {
    m_high = upper;
    m_low = lower;
  }

  DoubleRegister &operator=(uint16_t word) {
    set_word(word);
    return *this;
  }

  DoubleRegister &operator++(int) {
    uint16_t word = get_word();
    set_word(++word);
    return *this;
  }

  DoubleRegister &operator--(int) {
    uint16_t word = get_word();
    set_word(--word);
    return *this;
  }

  DoubleRegister &operator+=(uint16_t value) {
    uint16_t word = get_word();
    word += value;
    set_word(word);
    return *this;
  }

  DoubleRegister &operator-=(uint16_t value) {
    uint16_t word = get_word();
    word -= value;
    set_word(word);
    return *this;
  }

  DoubleRegister &operator&=(uint16_t value) {
    uint16_t word = get_word();
    word &= value;
    set_word(word);
    return *this;
  }

  DoubleRegister &operator|=(uint16_t value) {
    uint16_t word = get_word();
    word |= value;
    set_word(word);
    return *this;
  }

  operator uint16_t() const { return get_word(); }

private:
  // TODO: Use `Register` class?
  uint8_t m_high = 0;
  uint8_t m_low = 0;
};

} // namespace gb

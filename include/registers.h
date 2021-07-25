#pragma once

#include <cstdint>

namespace gb {

class Register {
public:
  Register() = default;
  ~Register() = default;

  uint16_t get_word() const {
    return static_cast<uint16_t>((m_high << 8) | m_low);
  }
  uint8_t get_upper_value() const { return m_high; }
  uint8_t get_lower_value() const { return m_low; }

  void set_upper_value(uint8_t upper) { m_high = upper; }
  void set_lower_value(uint8_t lower) { m_low = lower; }
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

  Register &operator=(uint16_t word) {
    set_word(word);
    return *this;
  }

  Register &operator++(int) {
    uint16_t word = get_word();
    set_word(++word);
    return *this;
  }

  Register &operator--(int) {
    uint16_t word = get_word();
    set_word(--word);
    return *this;
  }

  Register &operator+=(uint16_t value) {
    uint16_t word = get_word();
    word += value;
    set_word(word);
    return *this;
  }

  Register &operator-=(uint16_t value) {
    uint16_t word = get_word();
    word -= value;
    set_word(word);
    return *this;
  }

  Register &operator&=(uint16_t value) {
    uint16_t word = get_word();
    word &= value;
    set_word(word);
    return *this;
  }

  Register &operator|=(uint16_t value) {
    uint16_t word = get_word();
    word |= value;
    set_word(word);
    return *this;
  }

  operator uint16_t() const { return get_word(); }

private:
  uint8_t m_high = 0;
  uint8_t m_low = 0;
};

} // namespace gb

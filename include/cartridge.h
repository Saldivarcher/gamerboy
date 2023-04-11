#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace gb {

class CartridgeInformation {
public:
  static constexpr std::array<uint32_t, 6> ram_sizes{0x0,    0x800,   0x2000,
                                                      0x8000, 0x20000, 0x10000};
  enum class CartridgeType : uint8_t {
    GB_NO_MBC,
    GB_MBC1,
    GB_MBC3,
  };

  CartridgeInformation(const std::vector<std::byte> &rom) {

    auto rom_cast = [&](uint16_t address) -> uint16_t {
      return static_cast<uint16_t>(rom[address]);
    };

    m_ram_size = ram_sizes[rom_cast(0x149)];
    m_version = rom_cast(0x14C);
  }

  uint32_t get_ram_size() { return m_ram_size; }
  uint8_t get_version() { return m_version; }

private:
  uint32_t m_ram_size;
  uint8_t m_version;
};

class Cartridge {
public:
  Cartridge(const std::vector<std::byte> &data)
      : m_rom_data(data), m_cartridge_info(m_rom_data){};

  virtual ~Cartridge() = default;

  virtual uint8_t read(uint16_t addr) = 0;
  virtual void write(uint16_t addr, uint8_t value) = 0;

protected:
  const std::vector<std::byte> &m_rom_data;
  const std::vector<std::byte> m_ram_data;

  CartridgeInformation m_cartridge_info;
};

class NoMbc : protected Cartridge {
public:
  NoMbc(const std::vector<std::byte> &data) : Cartridge(data) {}

  virtual uint8_t read(uint16_t addr) override;
  virtual void write(uint16_t addr, uint8_t value) override;
};

} // namespace gb

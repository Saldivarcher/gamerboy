#include "gameboy.h"

namespace gb {

Gameboy::Gameboy(const std::vector<std::byte> &v)
    : m_cartridge(v), m_cpu(m_cartridge, m_mem), m_mem(m_cartridge) {}

void Gameboy::run() { m_cpu.cycle(); }

} // namespace gb

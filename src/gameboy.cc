#include "gameboy.h"

namespace gb {

Gameboy::Gameboy(const std::vector<std::byte> &v)
    : cartridge(v), cpu(cartridge, mem), mem(cartridge) {}

void Gameboy::run() { cpu.cycle(); }

} // namespace gb

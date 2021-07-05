#include "gameboy.h"
#include "utility.h"

#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {

  if (argc < 2)
    gb::utility::error("Please pass in the path to the ROM", 1);

  gb::Gameboy gb(argv[1]);
  gb.on();

  return 0;
}

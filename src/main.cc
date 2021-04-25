#include "driver.h"
#include "utility.h"

#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {

  if (argc < 2)
    gb::utility::error("Please pass in the path to the ROM", 1);

  gb::Driver d(argv[1]);

  d.drive();

  return 0;
}

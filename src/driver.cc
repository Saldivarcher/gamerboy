#include "driver.h"

namespace gb {
void Driver::drive() {
  int i = 0;
  while (!did_quit() and i != this->rom_data.size()) {
    process();
    gameboy.run();
    i++;
  }
}

void Driver::process() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      this->did_close = true;
      break;
    }
  }
}

} // namespace gb

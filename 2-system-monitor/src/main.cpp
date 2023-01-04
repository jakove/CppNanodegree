#include <iostream>

#include "ncurses_display.h"
#include "system.h"

int main(int argc, char* argv[]) {
  System system;
  if (argc > 1) {
    std::string option{argv[1]};
    if (option == "-i") {
      system.Cpu().SetMode(Processor::Mode::kInfo);
    } else {
      std::cout << "Only option \"-i\" is currently supported\n"
                   "Usage: ./monitor -i"
                << std::endl;
      return -1;
    }
  }
  NCursesDisplay::Display(system);
}
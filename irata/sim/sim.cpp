#include <fstream>
#include <iostream>
#include <irata/sim/components/irata.hpp>
#include <irata/sim/components/memory/rom.hpp>

using namespace irata::sim::components;

int main(int argc, char **argv) {
  std::unique_ptr<memory::ROM> rom;
  if (argc > 1) {
    std::ifstream is(argv[1]);
    rom = std::make_unique<memory::ROM>(0x1000, is);
  } else {
    rom = std::make_unique<memory::ROM>(0x1000, std::cin);
  }
  Irata irata(std::move(rom));
  try {
    irata.tick_until_halt();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  std::cerr << "Simulation complete. Final state:" << std::endl;
  irata.serialize_all(std::cerr);
  return 0;
}

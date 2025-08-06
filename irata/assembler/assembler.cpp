#include <iostream>
#include <irata/assembler/assembler.hpp>
#include <sstream>
#include <string>

int main() {
  std::ostringstream input;
  input << std::cin.rdbuf();

  try {
    irata::assembler::Assembler assembler;
    assembler.assemble(input.str(), std::cout);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

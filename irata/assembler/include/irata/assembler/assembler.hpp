#pragma once

#include <irata/common/bytes/byte.hpp>
#include <string_view>
#include <vector>

namespace irata::assembler {

class Assembler final {
public:
  std::vector<common::bytes::Byte> assemble(std::string_view input) const;

  void assemble(std::string_view input, std::ostream &output) const;
};

} // namespace irata::assembler

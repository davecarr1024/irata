#pragma once

#include <irata/assembler/label_binder.hpp>
#include <irata/assembler/source_location.hpp>
#include <string>
#include <vector>

namespace irata::assembler {

struct DebugSymbol final {
  std::string label;
  common::bytes::Word address;
  SourceLocation source_location;

  bool operator==(const DebugSymbol &other) const;
  bool operator!=(const DebugSymbol &other) const;
};

class DebugSymbolEmitter final {
public:
  std::vector<DebugSymbol> emit(const LabelBinder::Program &program) const;
};

std::ostream &operator<<(std::ostream &os, const DebugSymbol &symbol);

} // namespace irata::assembler

#pragma once

#include <irata/assembler/label_binder.hpp>
#include <irata/assembler/source_location.hpp>
#include <string>
#include <vector>

namespace irata::assembler {

struct SourceLineSymbol final {
  common::bytes::Word address;
  SourceLocation source_location;

  bool operator==(const SourceLineSymbol &other) const;
  bool operator!=(const SourceLineSymbol &other) const;
};

struct LabelSymbol final {
  std::string label;
  common::bytes::Word address;
  SourceLocation source_location;

  bool operator==(const LabelSymbol &other) const;
  bool operator!=(const LabelSymbol &other) const;
};

struct DebugSymbols final {
  std::vector<SourceLineSymbol> source_lines;
  std::vector<LabelSymbol> labels;

  bool operator==(const DebugSymbols &other) const;
  bool operator!=(const DebugSymbols &other) const;
};

class DebugSymbolEmitter final {
public:
  DebugSymbols emit(const LabelBinder::Program &program) const;
};

std::ostream &operator<<(std::ostream &os, const SourceLineSymbol &symbol);
std::ostream &operator<<(std::ostream &os, const LabelSymbol &symbol);
std::ostream &operator<<(std::ostream &os, const DebugSymbols &symbols);

} // namespace irata::assembler

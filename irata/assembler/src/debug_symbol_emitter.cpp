#include <irata/assembler/debug_symbol_emitter.hpp>

namespace irata::assembler {

bool DebugSymbol::operator==(const DebugSymbol &other) const {
  return label == other.label && address == other.address &&
         source_location == other.source_location;
}

bool DebugSymbol::operator!=(const DebugSymbol &other) const {
  return !(*this == other);
}

std::vector<DebugSymbol>
DebugSymbolEmitter::emit(const LabelBinder::Program &program) const {
  std::vector<DebugSymbol> symbols;
  for (const auto &statement : program.statements()) {
    if (statement->type() == LabelBinder::Program::Statement::Type::Label) {
      const auto &label =
          dynamic_cast<const LabelBinder::Program::Label &>(*statement);
      symbols.push_back(
          {label.value(), label.address(), label.source_location()});
    }
  }
  return symbols;
}

std::ostream &operator<<(std::ostream &os, const DebugSymbol &symbol) {
  os << "DebugSymbol{label=" << symbol.label << ", address=" << symbol.address
     << ", source_location=" << symbol.source_location << "}";
  return os;
}

} // namespace irata::assembler

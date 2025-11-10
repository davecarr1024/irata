#include <irata/assembler/debug_symbol_emitter.hpp>

namespace irata::assembler {

bool SourceLineSymbol::operator==(const SourceLineSymbol &other) const {
  return address == other.address && source_location == other.source_location;
}

bool SourceLineSymbol::operator!=(const SourceLineSymbol &other) const {
  return !(*this == other);
}

bool LabelSymbol::operator==(const LabelSymbol &other) const {
  return label == other.label && address == other.address &&
         source_location == other.source_location;
}

bool LabelSymbol::operator!=(const LabelSymbol &other) const {
  return !(*this == other);
}

bool DebugSymbols::operator==(const DebugSymbols &other) const {
  return source_lines == other.source_lines && labels == other.labels;
}

bool DebugSymbols::operator!=(const DebugSymbols &other) const {
  return !(*this == other);
}

DebugSymbols
DebugSymbolEmitter::emit(const LabelBinder::Program &program) const {
  DebugSymbols symbols;
  
  for (const auto &statement : program.statements()) {
    symbols.source_lines.push_back(
        {statement->address(), statement->source_location()});
    
    if (statement->type() == LabelBinder::Program::Statement::Type::Label) {
      const auto &label =
          dynamic_cast<const LabelBinder::Program::Label &>(*statement);
      symbols.labels.push_back(
          {label.value(), label.address(), label.source_location()});
    }
  }
  
  return symbols;
}

std::ostream &operator<<(std::ostream &os, const SourceLineSymbol &symbol) {
  os << "SourceLineSymbol{address=" << symbol.address
     << ", source_location=" << symbol.source_location << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const LabelSymbol &symbol) {
  os << "LabelSymbol{label=" << symbol.label << ", address=" << symbol.address
     << ", source_location=" << symbol.source_location << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const DebugSymbols &symbols) {
  os << "DebugSymbols{source_lines=[";
  for (size_t i = 0; i < symbols.source_lines.size(); ++i) {
    if (i > 0) os << ", ";
    os << symbols.source_lines[i];
  }
  os << "], labels=[";
  for (size_t i = 0; i < symbols.labels.size(); ++i) {
    if (i > 0) os << ", ";
    os << symbols.labels[i];
  }
  os << "]}";
  return os;
}

} // namespace irata::assembler

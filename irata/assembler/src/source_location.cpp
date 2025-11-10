#include <irata/assembler/source_location.hpp>

namespace irata::assembler {

SourceLocation::SourceLocation(std::string filename, size_t line,
                               std::optional<size_t> column)
    : filename_(std::move(filename)), line_(line), column_(column) {}

const std::string &SourceLocation::filename() const { return filename_; }

size_t SourceLocation::line() const { return line_; }

std::optional<size_t> SourceLocation::column() const { return column_; }

bool SourceLocation::operator==(const SourceLocation &other) const {
  return filename_ == other.filename_ && line_ == other.line_ &&
         column_ == other.column_;
}

bool SourceLocation::operator!=(const SourceLocation &other) const {
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const SourceLocation &location) {
  os << location.filename() << ":" << location.line();
  if (location.column().has_value()) {
    os << ":" << location.column().value();
  }
  return os;
}

} // namespace irata::assembler

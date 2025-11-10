#pragma once

#include <optional>
#include <ostream>
#include <string>

namespace irata::assembler {

class SourceLocation {
public:
  SourceLocation(std::string filename, size_t line,
                 std::optional<size_t> column = std::nullopt);

  const std::string &filename() const;
  size_t line() const;
  std::optional<size_t> column() const;

  bool operator==(const SourceLocation &other) const;
  bool operator!=(const SourceLocation &other) const;

private:
  std::string filename_;
  size_t line_;
  std::optional<size_t> column_;
};

std::ostream &operator<<(std::ostream &os, const SourceLocation &location);

} // namespace irata::assembler

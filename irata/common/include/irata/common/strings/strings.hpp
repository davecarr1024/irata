#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace irata::common::strings {

static const std::string_view WHITESPACE = " \t\n\r";

std::string trim(std::string_view str, std::string_view chars = WHITESPACE);

std::vector<std::string> split(std::string_view str,
                               std::string_view delim = WHITESPACE);

template <typename Range>
std::string join(const Range &range, std::string_view delim) {
  std::ostringstream os;
  std::string sep;
  for (const auto &item : range) {
    os << sep << item;
    sep = delim;
  }
  return os.str();
}

std::string to_upper(std::string_view str);

std::string to_lower(std::string_view str);

bool starts_with(std::string_view s, std::string_view prefix);

} // namespace irata::common::strings

#include <algorithm>
#include <irata/common/strings/strings.hpp>

namespace irata::common::strings {

std::string trim(std::string_view str, std::string_view chars) {
  std::string result(str);
  result.erase(0, result.find_first_not_of(chars));
  result.erase(result.find_last_not_of(chars) + 1);
  return result;
}

namespace {

size_t position_of_any(std::string_view str, std::string_view chars) {
  for (size_t i = 0; i < str.size(); ++i) {
    if (chars.find(str[i]) != std::string::npos) {
      return i;
    }
  }
  return std::string::npos;
}

} // namespace

std::vector<std::string> split(std::string_view str, std::string_view delim) {
  if (str.empty()) {
    return {};
  }
  std::vector<std::string> result;
  size_t start = 0;

  while (start <= str.size()) {
    size_t end = position_of_any(str.substr(start), delim);
    if (end == std::string::npos) {
      if (const std::string_view value = str.substr(start); !value.empty()) {
        result.emplace_back(value);
      }
      break;
    };
    if (const std::string_view value = str.substr(start, end); !value.empty()) {
      result.emplace_back(value);
    }
    start += end + 1; // Move past the delimiter
  }

  return result;
}

std::string to_upper(std::string_view str) {
  std::string result(str);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

std::string to_lower(std::string_view str) {
  std::string result(str);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

bool starts_with(std::string_view s, std::string_view prefix) {
  return s.size() >= prefix.size() &&
         std::equal(prefix.begin(), prefix.end(), s.begin());
}

} // namespace irata::common::strings

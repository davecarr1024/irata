#include <irata/sim/microcode/table/entry.hpp>
#include <utility>

namespace irata::sim::microcode::table {

bool Entry::operator==(const Entry &other) const {
  return std::tie(instruction, step_index, statuses, controls) ==
         std::tie(other.instruction, other.step_index, other.statuses,
                  other.controls);
}

bool Entry::operator!=(const Entry &other) const { return !(*this == other); }

bool Entry::operator<(const Entry &other) const {
  return std::tie(instruction, step_index, statuses, controls) <
         std::tie(other.instruction, other.step_index, other.statuses,
                  other.controls);
}

std::ostream &operator<<(std::ostream &os, const Entry &entry) {
  os << "Entry(" << entry.instruction << ", step_index=" << entry.step_index
     << ", statuses={";
  for (const auto &[status, value] : entry.statuses) {
    os << status << "=" << value << ", ";
  }
  os << "}, controls={";
  for (const auto &control : entry.controls) {
    os << control << ", ";
  }
  os << "})";
  return os;
}

} // namespace irata::sim::microcode::table

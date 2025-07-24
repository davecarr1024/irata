#include <irata/sim/microcode/table/entry.hpp>
#include <utility>

namespace irata::sim::microcode::table {

bool operator==(const Entry &lhs, const Entry &rhs) {
  return std::tie(lhs.instruction, lhs.step_index, lhs.statuses,
                  lhs.controls) ==
         std::tie(rhs.instruction, rhs.step_index, rhs.statuses, rhs.controls);
}

bool operator!=(const Entry &lhs, const Entry &rhs) { return !(lhs == rhs); }

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

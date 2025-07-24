#include <irata/sim/microcode/table/table.hpp>

namespace irata::sim::microcode::table {

bool operator==(const Table &lhs, const Table &rhs) {
  return lhs.entries == rhs.entries;
}

bool operator!=(const Table &lhs, const Table &rhs) { return !(lhs == rhs); }

std::ostream &operator<<(std::ostream &os, const Table &table) {
  os << "Table(";
  for (const auto &entry : table.entries) {
    os << entry << ", ";
  }
  os << ")";
  return os;
}

} // namespace irata::sim::microcode::table

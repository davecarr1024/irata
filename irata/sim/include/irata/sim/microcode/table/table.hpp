#pragma once

#include <irata/sim/microcode/table/entry.hpp>
#include <ostream>
#include <set>

namespace irata::sim::microcode::table {

// The microcode table is a set of entries that represent compiled microcode,
// ready to be encoded into instruction memory.
struct Table {
  // The entries in the microcode table.
  const std::set<Entry> entries;
};

bool operator==(const Table &lhs, const Table &rhs);
bool operator!=(const Table &lhs, const Table &rhs);
std::ostream &operator<<(std::ostream &os, const Table &table);

} // namespace irata::sim::microcode::table

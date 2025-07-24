#include <irata/asm/instruction.hpp>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <map>
#include <set>
#include <string>

namespace irata::sim::microcode::table {

// Represents a single entry in a microcode table.
// Each entry associates an opcode, step index, and statuses with a set of
// controls. This is encoded by the controller into a rom that can be
// accessed by the CPU to determine the next set of controls to assert for a
// given opcode and step index.
struct Entry {
  // The declarative descriptor for the instruciton this entry belongs to.
  // This incldues the instruction's opcode.
  const asm_::Instruction &instruction;

  // The step index for this entry.
  const Byte step_index;

  // The statuses that must be true for this entry to be selected.
  // If a status is not present it is assumed to be don't-care.
  const std::map<const hdl::StatusDecl *, bool> statuses;

  // The controls that are asserted by this entry.
  const std::set<const hdl::ControlDecl *> controls;
};

bool operator==(const Entry &lhs, const Entry &rhs);
bool operator!=(const Entry &lhs, const Entry &rhs);
std::ostream &operator<<(std::ostream &os, const Entry &entry);

} // namespace irata::sim::microcode::table

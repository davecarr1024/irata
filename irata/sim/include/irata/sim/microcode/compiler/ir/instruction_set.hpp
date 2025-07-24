#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <set>

namespace irata::sim::microcode::compiler::ir {

class InstructionSet {
public:
  explicit InstructionSet(std::set<Instruction> instructions);

  explicit InstructionSet(const dsl::InstructionSet &instruction_set);

  const std::set<Instruction> &instructions() const;

  bool operator==(const InstructionSet &other) const;
  bool operator!=(const InstructionSet &other) const;
  bool operator<(const InstructionSet &other) const;

private:
  std::set<Instruction> instructions_;
};

std::ostream &operator<<(std::ostream &os,
                         const InstructionSet &instruction_set);

} // namespace irata::sim::microcode::compiler::ir

#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// Verify that all instructions in the asm instruction set have been
// implemented in the microcode instruction set.
class InstructionCoverageValidator : public Pass {
public:
  explicit InstructionCoverageValidator(asm_::InstructionSet instruction_set);

  static std::unique_ptr<InstructionCoverageValidator> irata();

  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;

private:
  const asm_::InstructionSet instruction_set_;
};

} // namespace irata::sim::microcode::compiler::passes

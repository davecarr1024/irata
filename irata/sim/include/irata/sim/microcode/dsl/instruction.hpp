#include <irata/asm/instruction.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <map>
#include <memory>
#include <vector>

namespace irata::sim::microcode::dsl {

class InstructionSet;
class Step;

// Instruction is IR container for a single instruction in the instruction
// set of the CPU. This is intended to be used as a fluent declarative API
// for defining the instruction set of the CPU.
// Consumption and validation of the instruction set is done by the
// microcode compiler.
// An instruction has multiple steps that are executed in order.
// Each step has a set of control lines that are asserted during that step.
// Steps may be optimized and collapsed by the microcode compiler.
// Steps may be executed in parallel if they do not conflict.
// An instruction may have multiple variants with different status flag values.
// Any instrution that specifies a status flag must be accompanied by another
// instruction in the instruction set with the opposite status flag value.
class Instruction {
public:
  // Constructs a new instruction with the given instruction descriptor and
  // instruction set. The instruction is owned by the instruction set.
  // This should only be called by InstructionSet::create_instruction.
  Instruction(const asm_::Instruction &descriptor,
              InstructionSet *instruction_set);

  Instruction(const Instruction &) = delete;
  Instruction &operator=(const Instruction &) = delete;

  // Returns the instruction descriptor for the instruction.
  const asm_::Instruction &descriptor() const;

  // Returns the instruction set that owns this instruction.
  InstructionSet *instruction_set() const;

  // Creates a new instruction with the given instruction descriptor. The new
  // instruction is owned by the same instruction set as this instruction.
  Instruction *create_instruction(const asm_::Instruction &instruction);

  // Creates a new step. The new step is owned by this instruction.
  Step *create_step();

  // Returns the steps in the instruction.
  const std::vector<std::unique_ptr<Step>> &steps() const;

  // Adds the given status flag to the instruction.
  // Throws an error if the status flag conflicts with the current status
  Instruction *with_status(const hdl::StatusDecl &status, bool value);

  // Returns the status flags for the instruction.
  const std::map<const hdl::StatusDecl *, bool> &statuses() const;

private:
  const asm_::Instruction &descriptor_;
  InstructionSet *const instruction_set_;
  std::vector<std::unique_ptr<Step>> steps_;
  std::map<const hdl::StatusDecl *, bool> statuses_;
};

} // namespace irata::sim::microcode::dsl

#include <irata/asm/instruction.hpp>
#include <irata/sim/hdl/alu_opcode.hpp>
#include <irata/sim/hdl/component_with_bus_decl.hpp>
#include <irata/sim/hdl/status_decl.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
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

  // Copy data from one bus-connected component to another.
  template <typename Bus>
  Instruction *copy(const hdl::ComponentWithBusDecl<Bus> &source,
                    const hdl::ComponentWithBusDecl<Bus> &dest) {
    validate_hdl_components_arent_the_same(source, dest);
    validate_hdl_components_in_same_tree(source, dest);
    validate_hdl_components_have_same_bus(source, dest);
    return create_step()
        ->with_control(source.write())
        ->with_control(dest.read())
        ->instruction();
  }

  // Reads the value from the memory at the address specified by the source
  // register into the destination register.
  Instruction *read_memory(const hdl::ComponentWithWordBusDecl &address_source,
                           const hdl::ComponentWithByteBusDecl &data_dest);

  // Read the value from the memory at the address in the program counter into
  // the destination register, and increment the program counter.
  Instruction *
  read_memory_at_pc(const hdl::ComponentWithByteBusDecl &data_dest);

  // Read the value from memory at the address in the next two bytes of the
  // program.
  Instruction *
  indirect_read_memory_at_pc(const hdl::ComponentWithByteBusDecl &data_dest);

  // Perform an ALU operation with the given opcode.
  // This assumes that the operands are ready in the alu lhs and rhs registers.
  // After this step, the result will be in the alu result register.
  Instruction *alu_operation(hdl::AluOpcode opcode);

  // Perform an ALU operation with the given opcode, lhs, rhs, and result.
  Instruction *
  binary_alu_operation(hdl::AluOpcode opcode,
                       const hdl::ComponentWithByteBusDecl &lhs,
                       const hdl::ComponentWithByteBusDecl &rhs,
                       const hdl::ComponentWithByteBusDecl &result);

  Instruction *unary_alu_operation(hdl::AluOpcode opcode,
                                   const hdl::ComponentWithByteBusDecl &operand,
                                   const hdl::ComponentWithByteBusDecl &result);

  Instruction *
  binary_alu_operation_no_result(hdl::AluOpcode opcode,
                                 const hdl::ComponentWithByteBusDecl &lhs,
                                 const hdl::ComponentWithByteBusDecl &rhs);

  Instruction *
  unary_alu_operation_no_result(hdl::AluOpcode opcode,
                                const hdl::ComponentWithByteBusDecl &operand);

  int stage() const;

  Instruction *next_stage();

private:
  const asm_::Instruction &descriptor_;
  int stage_ = 0;
  InstructionSet *const instruction_set_;
  std::vector<std::unique_ptr<Step>> steps_;
  std::map<const hdl::StatusDecl *, bool> statuses_;

  static void validate_hdl_components_in_same_tree(const hdl::ComponentDecl &a,
                                                   const hdl::ComponentDecl &b);

  static void
  validate_hdl_components_arent_the_same(const hdl::ComponentDecl &a,
                                         const hdl::ComponentDecl &b);

  template <typename Bus>
  static void validate_hdl_components_have_same_bus(
      const hdl::ComponentWithBusDecl<Bus> &a,
      const hdl::ComponentWithBusDecl<Bus> &b) {
    if (&a.bus() != &b.bus()) {
      std::ostringstream os;
      os << "source and dest must be on the same bus: source " << a
         << " is connected to bus " << a.bus() << " but dest " << b
         << " is connected to bus " << b.bus();
      throw std::invalid_argument(os.str());
    }
  }
};

} // namespace irata::sim::microcode::dsl

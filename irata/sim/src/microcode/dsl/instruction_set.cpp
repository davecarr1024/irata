#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

namespace irata::sim::microcode::dsl {

Instruction *
InstructionSet::create_instruction(const asm_::Instruction &descriptor) {
  auto *instruction =
      instructions_
          .emplace_back(std::make_unique<Instruction>(descriptor, this))
          .get();
  instruction =
      instruction->read_memory_at_pc(hdl::irata().cpu().controller().opcode());
  return instruction;
}

Instruction *InstructionSet::create_instruction(std::string_view name,
                                                asm_::AddressingMode mode) {
  return create_instruction(
      asm_::InstructionSet::irata().get_instruction(name, mode));
}

const std::vector<std::unique_ptr<Instruction>> &
InstructionSet::instructions() const {
  return instructions_;
}

namespace {

std::unique_ptr<const InstructionSet> build_irata() {
  auto instruction_set = std::make_unique<InstructionSet>();

  instruction_set->create_instruction("lda", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().a());

  return instruction_set;
}

} // namespace

const InstructionSet &InstructionSet::irata() {
  static const auto instruction_set = build_irata();
  return *instruction_set;
}

} // namespace irata::sim::microcode::dsl

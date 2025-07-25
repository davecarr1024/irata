#include <irata/asm/instruction_set.hpp>
#include <irata/common/strings/strings.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <sstream>
#include <stdexcept>

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

const Instruction &InstructionSet::get_instruction(
    const asm_::Instruction &descriptor,
    std::map<const hdl::StatusDecl *, bool> statuses) const {
  for (const auto &instruction : instructions_) {
    if (instruction->descriptor() == descriptor &&
        instruction->statuses() == statuses) {
      return *instruction;
    }
  }
  std::ostringstream os;
  std::vector<std::string> status_strs;
  for (const auto &[status, value] : statuses) {
    status_strs.push_back(status->path() + "=" + std::to_string(value));
  }
  os << "instruction not found: " << descriptor << " with statuses {"
     << common::strings::join(status_strs, ", ") << "}";
  throw std::invalid_argument(os.str());
}

const Instruction &InstructionSet::get_instruction(
    std::string_view name, asm_::AddressingMode mode,
    std::map<const hdl::StatusDecl *, bool> statuses) const {
  for (const auto &instruction : instructions_) {
    if (instruction->descriptor().name() == common::strings::to_upper(name) &&
        instruction->descriptor().addressing_mode() == mode &&
        instruction->statuses() == statuses) {
      return *instruction;
    }
  }
  std::ostringstream os;
  std::vector<std::string> status_strs;
  for (const auto &[status, value] : statuses) {
    status_strs.push_back(status->path() + "=" + std::to_string(value));
  }
  os << "instruction not found: " << name << " " << mode << " with statuses {"
     << common::strings::join(status_strs, ", ") << "}";
  throw std::invalid_argument(os.str());
}

} // namespace irata::sim::microcode::dsl

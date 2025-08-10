#include <irata/asm/instruction_set.hpp>
#include <irata/common/strings/strings.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
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
  instruction = instruction->next_stage();
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

  instruction_set->create_instruction("hlt", asm_::AddressingMode::NONE)
      ->create_step()
      ->with_control(hdl::irata().halt());

  instruction_set->create_instruction("crs", asm_::AddressingMode::NONE)
      ->create_step()
      ->with_control(hdl::irata().crash());

  instruction_set->create_instruction("nop", asm_::AddressingMode::NONE)
      ->create_step();

  instruction_set->create_instruction("lda", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().a());

  instruction_set->create_instruction("lda", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().a());

  instruction_set->create_instruction("cmp", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->create_step()
      ->with_control(hdl::irata().cpu().status_register().set_carry())
      ->instruction()
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Subtract);

  instruction_set->create_instruction("adc", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Add)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("adc", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Add)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("sbc", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().rhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::Subtract)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("sbc", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().alu().rhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::Subtract)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("and", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::And)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("and", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::And)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("ora", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Or)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("ora", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Or)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("eor", asm_::AddressingMode::IMMEDIATE)
      ->read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Xor)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("eor", asm_::AddressingMode::ABSOLUTE)
      ->indirect_read_memory_at_pc(hdl::irata().cpu().alu().lhs())
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Xor)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("asl", asm_::AddressingMode::NONE)
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::ShiftLeft)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("lsr", asm_::AddressingMode::NONE)
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::ShiftRight)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("rol", asm_::AddressingMode::NONE)
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::RotateLeft)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("ror", asm_::AddressingMode::NONE)
      ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().alu().lhs())
      ->alu_operation(hdl::AluOpcode::RotateRight)
      ->copy(hdl::irata().cpu().alu().result(), hdl::irata().cpu().a());

  instruction_set->create_instruction("jmp", asm_::AddressingMode::ABSOLUTE)
      ->read_memory_at_pc(hdl::irata().cpu().buffer().high())
      ->read_memory_at_pc(hdl::irata().cpu().buffer().low())
      ->copy(hdl::irata().cpu().buffer(), hdl::irata().cpu().pc());

  instruction_set
      ->create_instruction("jne", asm_::AddressingMode::ABSOLUTE)
      // Zero is true: increment PC twice to get to the next instruction.
      ->with_status(hdl::irata().cpu().status_register().zero_out(), true)
      ->create_step()
      ->with_control(hdl::irata().cpu().pc().increment())
      // Create a separate instruction stage to prevent merging the two
      // increment controls.
      ->next_stage()
      ->create_step()
      ->with_control(hdl::irata().cpu().pc().increment())
      // Zero is false: jump to the address at pc.
      ->create_instruction("jne", asm_::AddressingMode::ABSOLUTE)
      ->with_status(hdl::irata().cpu().status_register().zero_out(), false)
      ->read_memory_at_pc(hdl::irata().cpu().buffer().high())
      ->read_memory_at_pc(hdl::irata().cpu().buffer().low())
      ->copy(hdl::irata().cpu().buffer(), hdl::irata().cpu().pc());

  instruction_set
      ->create_instruction("jeq", asm_::AddressingMode::ABSOLUTE)
      // Zero is false: increment PC twice to get to the next instruction.
      ->with_status(hdl::irata().cpu().status_register().zero_out(), false)
      ->create_step()
      ->with_control(hdl::irata().cpu().pc().increment())
      // Create a separate instruction stage to prevent merging the two
      // increment controls.
      ->next_stage()
      ->create_step()
      ->with_control(hdl::irata().cpu().pc().increment())
      // Zero is true: jump to the address at pc.
      ->create_instruction("jeq", asm_::AddressingMode::ABSOLUTE)
      ->with_status(hdl::irata().cpu().status_register().zero_out(), true)
      ->read_memory_at_pc(hdl::irata().cpu().buffer().high())
      ->read_memory_at_pc(hdl::irata().cpu().buffer().low())
      ->copy(hdl::irata().cpu().buffer(), hdl::irata().cpu().pc());

  instruction_set->create_instruction("sec", asm_::AddressingMode::NONE)
      ->create_step()
      ->with_control(hdl::irata().cpu().status_register().set_carry());

  instruction_set->create_instruction("clc", asm_::AddressingMode::NONE)
      ->create_step()
      ->with_control(hdl::irata().cpu().status_register().clear_carry());

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

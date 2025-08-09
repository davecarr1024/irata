#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::dsl {

Instruction::Instruction(const asm_::Instruction &descriptor,
                         InstructionSet *instruction_set)
    : descriptor_(descriptor), instruction_set_(instruction_set) {
  if (instruction_set == nullptr) {
    throw std::invalid_argument("instruction_set cannot be null");
  }
}

const asm_::Instruction &Instruction::descriptor() const { return descriptor_; }

InstructionSet *Instruction::instruction_set() const {
  return instruction_set_;
}

Instruction *
Instruction::create_instruction(const asm_::Instruction &instruction) {
  return instruction_set_->create_instruction(instruction);
}

Instruction *
Instruction::create_instruction(std::string_view name,
                                asm_::AddressingMode addressing_mode) {
  return instruction_set_->create_instruction(name, addressing_mode);
}

Step *Instruction::create_step() {
  return steps_.emplace_back(std::make_unique<Step>(this, stage_)).get();
}

const std::vector<std::unique_ptr<Step>> &Instruction::steps() const {
  return steps_;
}

Instruction *Instruction::with_status(const hdl::StatusDecl &status,
                                      bool value) {
  if (auto it = statuses_.find(&status); it != statuses_.end()) {
    if (const auto &[_, current_value] = *it; current_value != value) {
      throw std::invalid_argument("status conflict replacing " + status.path() +
                                  " that has value " +
                                  std::to_string(current_value) +
                                  " with value " + std::to_string(value));
    }
  }
  statuses_[&status] = value;
  return this;
}

const std::map<const hdl::StatusDecl *, bool> &Instruction::statuses() const {
  return statuses_;
}

void Instruction::validate_hdl_components_in_same_tree(
    const hdl::ComponentDecl &a, const hdl::ComponentDecl &b) {
  if (a.root() != b.root()) {
    std::ostringstream os;
    os << "components must be in the same tree: " << a.path() << " and "
       << b.path();
    throw std::invalid_argument(os.str());
  }
}

void Instruction::validate_hdl_components_arent_the_same(
    const hdl::ComponentDecl &a, const hdl::ComponentDecl &b) {
  if (&a == &b) {
    std::ostringstream os;
    os << "components cannot be the same: " << a.path();
    throw std::invalid_argument(os.str());
  }
}

Instruction *
Instruction::read_memory(const hdl::ComponentWithWordBusDecl &address_source,
                         const hdl::ComponentWithByteBusDecl &data_dest) {
  auto *instruction = this;
  instruction =
      instruction->copy(address_source, hdl::irata().memory().address())
          ->copy(hdl::irata().memory(), data_dest);
  return instruction;
}

Instruction *
Instruction::read_memory_at_pc(const hdl::ComponentWithByteBusDecl &data_dest) {
  return read_memory(hdl::irata().cpu().pc(), data_dest)
      ->create_step()
      ->with_control(hdl::irata().cpu().pc().increment())
      ->instruction();
}

Instruction *Instruction::indirect_read_memory_at_pc(
    const hdl::ComponentWithByteBusDecl &data_dest) {
  return read_memory_at_pc(hdl::irata().cpu().buffer().high())
      ->read_memory_at_pc(hdl::irata().cpu().buffer().low())
      ->copy(hdl::irata().cpu().buffer(), hdl::irata().memory().address())
      ->copy(hdl::irata().memory(), data_dest);
}

int Instruction::stage() const { return stage_; }

Instruction *Instruction::next_stage() {
  stage_++;
  return this;
}

Instruction *Instruction::alu_operation(hdl::AluOpcode opcode) {
  auto *instruction = this;
  auto *step = instruction->create_step();
  for (const auto &control :
       hdl::irata().cpu().alu().opcode_controls_for_opcode(opcode)) {
    step = step->with_control(*control);
  }
  step->with_control(hdl::irata().cpu().status_register().latch());
  return step->instruction();
}

Instruction *
Instruction::binary_alu_operation(hdl::AluOpcode opcode,
                                  const hdl::ComponentWithByteBusDecl &lhs,
                                  const hdl::ComponentWithByteBusDecl &rhs,
                                  const hdl::ComponentWithByteBusDecl &result) {
  return copy(lhs, hdl::irata().cpu().alu().lhs())
      ->copy(rhs, hdl::irata().cpu().alu().rhs())
      ->alu_operation(opcode)
      ->copy(hdl::irata().cpu().alu().result(), result);
}

Instruction *
Instruction::unary_alu_operation(hdl::AluOpcode opcode,
                                 const hdl::ComponentWithByteBusDecl &operand,
                                 const hdl::ComponentWithByteBusDecl &result) {
  return copy(operand, hdl::irata().cpu().alu().lhs())
      ->alu_operation(opcode)
      ->copy(hdl::irata().cpu().alu().result(), result);
}

Instruction *Instruction::binary_alu_operation_no_result(
    hdl::AluOpcode opcode, const hdl::ComponentWithByteBusDecl &lhs,
    const hdl::ComponentWithByteBusDecl &rhs) {
  return copy(lhs, hdl::irata().cpu().alu().lhs())
      ->copy(rhs, hdl::irata().cpu().alu().rhs())
      ->alu_operation(opcode);
}

Instruction *Instruction::unary_alu_operation_no_result(
    hdl::AluOpcode opcode, const hdl::ComponentWithByteBusDecl &operand) {
  return copy(operand, hdl::irata().cpu().alu().lhs())->alu_operation(opcode);
}

} // namespace irata::sim::microcode::dsl

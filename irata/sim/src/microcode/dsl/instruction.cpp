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

Instruction *Instruction::with_control(const hdl::ControlDecl &control) {
  return create_step()->with_control(control)->instruction();
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
Instruction::write_memory(const hdl::ComponentWithWordBusDecl &address_source,
                          const hdl::ComponentWithByteBusDecl &data_source) {
  return copy(address_source, hdl::irata().memory().address())
      ->copy(data_source, hdl::irata().memory());
}

Instruction *
Instruction::read_memory_at_pc(const hdl::ComponentWithByteBusDecl &data_dest) {
  return read_memory(hdl::irata().cpu().pc(), data_dest)
      ->with_control(hdl::irata().cpu().pc().increment());
}

Instruction *
Instruction::write_memory_at_pc(const hdl::ComponentWithByteBusDecl &source) {
  return write_memory(hdl::irata().cpu().pc(), source)
      ->with_control(hdl::irata().cpu().pc().increment());
}

Instruction *
Instruction::read_word_at_pc(const hdl::ComponentWithByteBusDecl &high_dest,
                             const hdl::ComponentWithByteBusDecl &low_dest) {
  return read_memory_at_pc(high_dest)
      ->with_control(hdl::irata().memory().address().increment())
      ->copy(hdl::irata().memory(), low_dest)
      ->with_control(hdl::irata().cpu().pc().increment());
}

Instruction *
Instruction::read_word_at_pc(const hdl::ComponentWithWordBusDecl &dest) {
  return read_word_at_pc(hdl::irata().cpu().buffer().high(),
                         hdl::irata().cpu().buffer().low())
      ->copy(hdl::irata().cpu().buffer(), dest);
}

namespace {

// Read the next two bytes of the program into MAR.
Instruction *read_address_at_pc(Instruction *instruction) {
  return instruction->read_word_at_pc(hdl::irata().memory().address());
}

} // namespace

Instruction *Instruction::indirect_read_memory_at_pc(
    const hdl::ComponentWithByteBusDecl &data_dest) {
  auto *instruction = this;
  instruction = read_address_at_pc(instruction);
  instruction = instruction->copy(hdl::irata().memory(), data_dest);
  return instruction;
}

Instruction *Instruction::indirect_write_memory_at_pc(
    const hdl::ComponentWithByteBusDecl &data_source) {
  auto *instruction = this;
  instruction = read_address_at_pc(instruction);
  instruction = instruction->copy(data_source, hdl::irata().memory());
  return instruction;
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

namespace {

// Increment the stack pointer.
Instruction *increment_sp(Instruction *instruction) {
  return instruction->create_step()
      ->with_control(hdl::irata().cpu().stack_pointer().increment())
      ->instruction();
}

// Decrement the stack pointer.
Instruction *decrement_sp(Instruction *instruction) {
  return instruction->create_step()
      ->with_control(hdl::irata().cpu().stack_pointer().decrement())
      ->instruction();
}

// Copy the stack pointer to the low byte of the memory address register.
Instruction *copy_sp_to_mar_low(Instruction *instruction) {
  return instruction->copy(hdl::irata().cpu().stack_pointer(),
                           hdl::irata().memory().address().low());
}

// Set the high byte of the memory address register to the stack page.
Instruction *set_mar_high_to_stack_page(Instruction *instruction) {
  return instruction->create_step()
      ->with_control(hdl::irata().memory().address().high().reset())
      ->with_control(hdl::irata().memory().address().high().increment())
      ->instruction();
}

} // namespace

Instruction *Instruction::push(const hdl::ComponentWithByteBusDecl &source) {
  Instruction *instruction = this;
  instruction = copy_sp_to_mar_low(instruction);
  instruction = set_mar_high_to_stack_page(instruction);
  instruction = decrement_sp(instruction);
  instruction = copy(source, hdl::irata().memory());
  return instruction;
}

Instruction *Instruction::pop(const hdl::ComponentWithByteBusDecl &dest) {
  Instruction *instruction = this;
  instruction = increment_sp(instruction);
  instruction = set_mar_high_to_stack_page(instruction);
  instruction = copy_sp_to_mar_low(instruction);
  instruction = copy(hdl::irata().memory(), dest);
  return instruction;
}

namespace {

Instruction *set_mar_high_to_zero_page(Instruction *instruction) {
  return instruction->create_step()
      ->with_control(hdl::irata().memory().address().high().reset())
      ->instruction();
}

Instruction *
set_mar_to_zero_page(Instruction *instruction,
                     const hdl::ComponentWithByteBusDecl &address_source) {
  instruction =
      instruction->copy(address_source, hdl::irata().memory().address().low());
  instruction = set_mar_high_to_zero_page(instruction);
  return instruction;
}

} // namespace

Instruction *Instruction::read_memory_zero_page(
    const hdl::ComponentWithByteBusDecl &address_source,
    const hdl::ComponentWithByteBusDecl &data_dest) {
  Instruction *instruction = this;
  instruction = set_mar_to_zero_page(instruction, address_source);
  instruction = copy(hdl::irata().memory(), data_dest);
  return instruction;
}

Instruction *Instruction::write_memory_zero_page(
    const hdl::ComponentWithByteBusDecl &address_source,
    const hdl::ComponentWithByteBusDecl &data_source) {
  Instruction *instruction = this;
  instruction = set_mar_to_zero_page(instruction, address_source);
  instruction = copy(data_source, hdl::irata().memory());
  return instruction;
}

namespace {

Instruction *read_zero_page_address_at_pc(Instruction *instruction) {
  // Read next byte of program to get zero page address.
  // Note that this is reading from memory into the MAR, which is ok because the
  // memory write to the bus happens during the write phase when the MAR hasn't
  // been changed, and the read from the bus happens during the read phase.
  instruction =
      instruction->read_memory_at_pc(hdl::irata().memory().address().low());
  // Set MAR high to zero page.
  instruction = set_mar_high_to_zero_page(instruction);
  return instruction;
}

} // namespace

Instruction *Instruction::indirect_read_memory_zero_page_at_pc(
    const hdl::ComponentWithByteBusDecl &data_dest) {
  Instruction *instruction = this;
  // Set MAR to zero page address based on the next byte of the program.
  instruction = read_zero_page_address_at_pc(instruction);
  // Read from memory.
  instruction = instruction->copy(hdl::irata().memory(), data_dest);
  return instruction;
}

Instruction *Instruction::indirect_write_memory_zero_page_at_pc(
    const hdl::ComponentWithByteBusDecl &data_source) {
  Instruction *instruction = this;
  // Set MAR to zero page address based on the next byte of the program.
  instruction = read_zero_page_address_at_pc(instruction);
  // Write to memory.
  instruction = instruction->copy(data_source, hdl::irata().memory());
  return instruction;
}

} // namespace irata::sim::microcode::dsl

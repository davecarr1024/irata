#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
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

Step *Instruction::create_step() {
  return steps_.emplace_back(std::make_unique<Step>(this)).get();
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

Instruction *Instruction::copy(const hdl::ConnectedByteRegisterDecl &source,
                               const hdl::ConnectedByteRegisterDecl &dest) {
  if (&source == &dest) {
    throw std::invalid_argument("source and dest cannot be the same");
  }
  if (&source.bus() != &dest.bus()) {
    throw std::invalid_argument("source and dest must be on the same bus");
  }
  return create_step()
      ->with_control(source.write())
      ->with_control(dest.read())
      ->instruction();
}

Instruction *Instruction::copy(const hdl::ConnectedWordRegisterDecl &source,
                               const hdl::ConnectedWordRegisterDecl &dest) {
  if (&source == &dest) {
    throw std::invalid_argument("source and dest cannot be the same");
  }
  if (&source.bus() != &dest.bus()) {
    throw std::invalid_argument("source and dest must be on the same bus");
  }
  return create_step()
      ->with_control(source.write())
      ->with_control(source.read())
      ->instruction();
}

Instruction *Instruction::copy(const hdl::MemoryDecl &source,
                               const hdl::ConnectedByteRegisterDecl &dest) {
  if (&source.data_bus() != &dest.bus()) {
    throw std::invalid_argument("source and dest must be on the same bus");
  }
  return create_step()
      ->with_control(source.read())
      ->with_control(dest.write())
      ->instruction();
}

Instruction *Instruction::copy(const hdl::ConnectedByteRegisterDecl &source,
                               const hdl::MemoryDecl &dest) {
  if (&source.bus() != &dest.data_bus()) {
    throw std::invalid_argument("source and dest must be on the same bus");
  }
  return create_step()
      ->with_control(source.read())
      ->with_control(dest.write())
      ->instruction();
}

Instruction *
Instruction::read_memory(const hdl::ConnectedWordRegisterDecl &address_source,
                         const hdl::ConnectedByteRegisterDecl &data_dest) {
  auto *instruction = this;
  instruction = instruction->copy(address_source,
                                  hdl::irata().memory().address_register());
  instruction = instruction->copy(hdl::irata().memory(), data_dest);
  return instruction;
}

Instruction *Instruction::read_memory_at_pc(
    const hdl::ConnectedByteRegisterDecl &data_dest) {
  return read_memory(hdl::irata().cpu().program_counter(), data_dest)
      ->create_step()
      ->with_control(hdl::irata().cpu().program_counter().increment())
      ->instruction();
}

} // namespace irata::sim::microcode::dsl

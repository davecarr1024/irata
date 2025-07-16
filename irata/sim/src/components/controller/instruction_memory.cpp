#include <irata/sim/components/controller/instruction_memory.hpp>
#include <stdexcept>
#include <utility>

namespace irata::sim::components::controller {

namespace {

std::map<uint16_t, uint32_t>
build_instruction_memory(const microcode::InstructionSet &instruction_set,
                         const InstructionAddressEncoder &address_encoder,
                         const ControlEncoder &control_encoder) {
  std::map<uint16_t, uint32_t> data;
  for (const auto &instruction : instruction_set.instructions) {
    for (const auto &program : instruction.programs) {
      for (uint8_t step_index = 0; step_index < program.statements.size();
           ++step_index) {
        auto &statement = program.statements[step_index];
        for (const auto address : address_encoder.permute_statuses_and_encode(
                 instruction.instruction.opcode, step_index,
                 program.statuses)) {
          const auto entry = control_encoder.encode(statement.controls);
          data[address] = entry;
        }
      }
    }
  }
  return data;
}

} // namespace

InstructionMemory::InstructionMemory(
    const microcode::InstructionSet &instruction_set, std::string_view name,
    Component *parent)
    : Component(name, parent), address_encoder_(instruction_set),
      control_encoder_(instruction_set),
      data_(build_instruction_memory(instruction_set, address_encoder_,
                                     control_encoder_)) {}

std::set<std::string> InstructionMemory::statuses() const {
  return address_encoder_.statuses();
}

std::set<std::string> InstructionMemory::controls() const {
  return control_encoder_.controls();
}

std::set<std::string>
InstructionMemory::get_controls(uint8_t opcode, uint8_t step_index,
                                std::map<std::string, bool> statuses) const {
  const auto all_statuses = this->statuses();
  // If any statuses are missing, throw an error.
  for (const auto &status : all_statuses) {
    if (statuses.find(status) == statuses.end()) {
      throw std::invalid_argument("missing status: " + status);
    }
  }
  // If any statuses are unknown, throw an error.
  for (const auto &[key, _] : statuses) {
    if (all_statuses.find(key) == all_statuses.end()) {
      throw std::invalid_argument("unknown status: " + key);
    }
  }
  const auto address =
      address_encoder_.encode(opcode, step_index, std::move(statuses));
  const auto it = data_.find(address);
  if (it == data_.end()) {
    throw std::invalid_argument(
        "unknown address for opcode=" + std::to_string(opcode) +
        ", step=" + std::to_string(step_index));
  }
  return control_encoder_.decode(it->second);
}

} // namespace irata::sim::components::controller

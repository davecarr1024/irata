#include <irata/sim/components/controller/instruction_address_encoder.hpp>

namespace irata::sim::components::controller {

namespace {

uint8_t get_max_opcode(const microcode::InstructionSet &instruction_set) {
  uint8_t max_opcode = 0;
  for (const auto &instruction : instruction_set.instructions) {
    max_opcode = std::max(max_opcode, instruction.instruction.opcode);
  }
  return max_opcode;
}

size_t get_max_step_index(const microcode::InstructionSet &instruction_set) {
  size_t max_step_index = 0;
  for (const auto &instruction : instruction_set.instructions) {
    for (const auto &program : instruction.programs) {
      max_step_index = std::max(max_step_index, program.statements.size() - 1);
    }
  }
  return max_step_index;
}

size_t bits_to_represent(size_t value) {
  size_t bits = 0;
  while ((1ULL << bits) <= value) {
    ++bits;
  }
  return bits;
}

} // namespace

InstructionAddressEncoder::InstructionAddressEncoder(
    const microcode::InstructionSet &instruction_set)
    : status_encoder_(instruction_set),
      max_opcode_(get_max_opcode(instruction_set)),
      opcode_bit_count_(bits_to_represent(max_opcode_)),
      max_step_index_(get_max_step_index(instruction_set)),
      step_index_bit_count_(bits_to_represent(max_step_index_)) {
  if (address_bit_count() > 16) {
    throw std::runtime_error("instruction address too large");
  }
}

uint8_t InstructionAddressEncoder::max_opcode() const { return max_opcode_; }

size_t InstructionAddressEncoder::opcode_bit_count() const {
  return opcode_bit_count_;
}

size_t InstructionAddressEncoder::max_step_index() const {
  return max_step_index_;
}

size_t InstructionAddressEncoder::step_index_bit_count() const {
  return step_index_bit_count_;
}

size_t InstructionAddressEncoder::status_bit_count() const {
  return status_encoder_.status_count();
}

size_t InstructionAddressEncoder::address_bit_count() const {
  return opcode_bit_count_ + step_index_bit_count_ + status_bit_count();
}

uint16_t
InstructionAddressEncoder::encode(uint8_t opcode, uint8_t step_index,
                                  std::map<std::string, bool> statuses) const {
  if (opcode > max_opcode_) {
    throw std::invalid_argument("opcode too large");
  }
  if (step_index > max_step_index_) {
    throw std::invalid_argument("step index too large");
  }
  const uint8_t status_encoded = status_encoder_.encode(statuses);
  const size_t status_bit_count = this->status_bit_count();
  return (opcode << (step_index_bit_count_ + status_bit_count)) |
         (step_index << status_bit_count) | status_encoded;
}

std::tuple<uint8_t, uint8_t, std::map<std::string, bool>>
InstructionAddressEncoder::decode(uint16_t encoded) const {
  const size_t status_bit_count = this->status_bit_count();
  const uint8_t status_encoded = encoded & ((1 << status_bit_count) - 1);
  const uint8_t step_index =
      (encoded >> status_bit_count) & ((1 << step_index_bit_count_) - 1);
  const uint8_t opcode = encoded >> (step_index_bit_count_ + status_bit_count);
  const auto statuses = status_encoder_.decode(status_encoded);
  return {opcode, step_index, statuses};
}

std::vector<uint16_t> InstructionAddressEncoder::permute_statuses_and_encode(
    uint8_t opcode, uint8_t step_index,
    std::map<std::string, bool> partial_statuses) const {
  std::vector<uint16_t> result;
  for (const auto &statuses :
       status_encoder_.permute_statuses(partial_statuses)) {
    result.push_back(encode(opcode, step_index, statuses));
  }
  return result;
}

} // namespace irata::sim::components::controller

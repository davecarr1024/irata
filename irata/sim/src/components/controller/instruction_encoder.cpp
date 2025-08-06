#include <algorithm>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/components/controller/instruction_encoder.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::components::controller {

namespace {

uint8_t get_max_opcode(const microcode::table::Table &table) {
  uint8_t max_opcode = 0;
  for (const auto &entry : table.entries) {
    max_opcode =
        std::max(max_opcode, entry.instruction.opcode().unsigned_value());
  }
  return max_opcode;
}

uint8_t get_max_step_index(const microcode::table::Table &table) {
  uint8_t max_step_index = 0;
  for (const auto &entry : table.entries) {
    max_step_index =
        std::max(max_step_index, entry.step_index.unsigned_value());
  }
  return max_step_index;
}

size_t num_bits_to_represent(uint8_t max_value) {
  size_t num_bits = 0;
  while (max_value > 0) {
    max_value >>= 1;
    ++num_bits;
  }
  return num_bits;
}

} // namespace

InstructionEncoder::InstructionEncoder(const microcode::table::Table &table)
    : status_encoder_(table), control_encoder_(table),
      max_opcode_(get_max_opcode(table)),
      num_opcode_bits_(num_bits_to_represent(max_opcode_)),
      max_step_index_(get_max_step_index(table)),
      num_step_index_bits_(num_bits_to_represent(max_step_index_)) {
  if (num_address_bits() > 16) {
    throw std::invalid_argument("Too many address bits to encode");
  }
}

uint16_t InstructionEncoder::encode_address(uint8_t opcode,
                                            const CompleteStatuses &statuses,
                                            uint8_t step_index) const {
  if (opcode >= (1 << num_opcode_bits())) {
    std::ostringstream os;
    os << "Opcode 0x" << std::hex << int(opcode) << " out of bounds 0x"
       << std::hex << (1 << num_opcode_bits());
    throw std::invalid_argument(os.str());
  }
  if (step_index >= (1 << num_step_index_bits())) {
    std::ostringstream os;
    os << "Step index 0x" << std::hex << int(step_index) << " out of bounds 0x"
       << (1 << num_step_index_bits()) << " for opcode 0x" << std::hex
       << int(opcode);
    throw std::invalid_argument(os.str());
  }
  return (opcode << (num_status_bits() + num_step_index_bits())) |
         (status_encoder_.encode(statuses) << num_step_index_bits()) |
         step_index;
}

std::vector<uint16_t> InstructionEncoder::encode_address(
    uint8_t opcode, const PartialStatuses &statuses, uint8_t step_index) const {
  std::vector<uint16_t> addresses;
  for (const auto &complete_statuses : status_encoder_.permute(statuses)) {
    addresses.push_back(encode_address(opcode, complete_statuses, step_index));
  }
  return addresses;
}

std::vector<uint16_t>
InstructionEncoder::encode_address(const microcode::table::Entry &entry) const {
  return encode_address(entry.instruction.opcode().unsigned_value(),
                        PartialStatuses(entry.statuses),
                        entry.step_index.unsigned_value());
}

std::tuple<uint8_t, CompleteStatuses, uint8_t>
InstructionEncoder::decode_address(uint16_t address) const {
  const uint8_t opcode = address >> (num_status_bits() + num_step_index_bits());
  const uint8_t encoded_statuses =
      (address >> num_step_index_bits()) & ((1 << num_status_bits()) - 1);
  const auto statuses = status_encoder_.decode(encoded_statuses);
  const uint8_t step_index = address & ((1 << num_step_index_bits()) - 1);
  return {opcode, statuses, step_index};
}

uint32_t InstructionEncoder::encode_value(
    const std::set<const hdl::ControlDecl *> &controls) const {
  return control_encoder_.encode(controls);
}

uint32_t
InstructionEncoder::encode_value(const microcode::table::Entry &entry) const {
  return encode_value(entry.controls);
}

std::set<const hdl::ControlDecl *>
InstructionEncoder::decode_value(uint32_t value) const {
  return control_encoder_.decode(value);
}

size_t InstructionEncoder::num_address_bits() const {
  return num_opcode_bits() + num_status_bits() + num_step_index_bits();
}

size_t InstructionEncoder::num_value_bits() const {
  return control_encoder_.num_controls();
}

size_t InstructionEncoder::num_statuses() const {
  return status_encoder_.num_statuses();
}

size_t InstructionEncoder::num_status_bits() const { return num_statuses(); }

size_t InstructionEncoder::num_controls() const {
  return control_encoder_.num_controls();
}

uint8_t InstructionEncoder::max_opcode() const { return max_opcode_; }

size_t InstructionEncoder::num_opcode_bits() const { return num_opcode_bits_; }

size_t InstructionEncoder::max_step_index() const { return max_step_index_; }

size_t InstructionEncoder::num_step_index_bits() const {
  return num_step_index_bits_;
}

const StatusEncoder &InstructionEncoder::status_encoder() const {
  return status_encoder_;
}

const ControlEncoder &InstructionEncoder::control_encoder() const {
  return control_encoder_;
}

} // namespace irata::sim::components::controller

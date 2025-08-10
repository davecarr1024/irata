#pragma once

#include <cstdint>
#include <irata/asm/instruction.hpp>
#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/components/controller/partial_statuses.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <map>
#include <set>
#include <tuple>

namespace irata::sim::components::controller {

// InstructionEncoder is responsible for encoding the instruction memory
// of the CPU. Addresses in the instruction memory are composed of:
//   - up to 8 bits for the opcode
//   - up to 8 bits for the statuses
//   - up to 8 bits for the step index
// The actual widths of each field are determined by the microcode table.
// The instruction memory is encoded as a map from addresses to values.
// The values are the encoded controls for the given instruction, statuses, and
// step index.
class InstructionEncoder {
public:
  explicit InstructionEncoder(const microcode::table::Table &table);
  InstructionEncoder(const InstructionEncoder &) = delete;
  InstructionEncoder &operator=(const InstructionEncoder &) = delete;

  uint16_t encode_address(uint8_t opcode, const CompleteStatuses &statuses,
                          uint8_t step_index) const;

  std::vector<uint16_t> encode_address(uint8_t opcode,
                                       const PartialStatuses &statuses,
                                       uint8_t step_index) const;

  std::vector<uint16_t>
  encode_address(const microcode::table::Entry &entry) const;

  // Decodes the given address into an opcode, statuses, and step index.
  std::tuple<uint8_t, CompleteStatuses, uint8_t>
  decode_address(uint16_t address) const;

  // Encodes the given controls into a value.
  uint64_t
  encode_value(const std::set<const hdl::ControlDecl *> &controls) const;

  // Encodes the given entry into a value.
  uint64_t encode_value(const microcode::table::Entry &entry) const;

  // Decodes the given value into a set of controls.
  std::set<const hdl::ControlDecl *> decode_value(uint64_t value) const;

  // The number of statuses that are used in the microcode table.
  size_t num_statuses() const;

  // The number of bits that are used to encode the statuses.
  size_t num_status_bits() const;

  // The number of controls that are used in the microcode table.
  size_t num_controls() const;

  // The maximum opcode value in the microcode table.
  uint8_t max_opcode() const;

  // The number of bits that are used to encode the opcode.
  size_t num_opcode_bits() const;

  // The maximum step index value in the microcode table.
  size_t max_step_index() const;

  // The number of bits that are used to encode the step index.
  size_t num_step_index_bits() const;

  // Returns the number of address bits in the instruction memory.
  size_t num_address_bits() const;

  // Returns the number of value bits in the instruction memory.
  size_t num_value_bits() const;

  // Returns the status encoder.
  const StatusEncoder &status_encoder() const;

  // Returns the control encoder.
  const ControlEncoder &control_encoder() const;

private:
  const StatusEncoder status_encoder_;
  const ControlEncoder control_encoder_;
  const uint8_t max_opcode_;
  const size_t num_opcode_bits_;
  const uint8_t max_step_index_;
  const size_t num_step_index_bits_;
};

} // namespace irata::sim::components::controller
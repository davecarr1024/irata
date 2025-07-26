#pragma once

#include <cstdint>
#include <irata/asm/instruction.hpp>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <irata/sim/hdl/hdl.hpp>
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

  // Encodes the given instruction, statuses, and step index into an address.
  uint32_t encode_address(const asm_::Instruction &instruction,
                          std::map<const hdl::StatusDecl *, bool> statuses,
                          uint8_t step_index) const;

  uint32_t encode_address(uint8_t opcode,
                          std::map<const hdl::StatusDecl *, bool> statuses,
                          uint8_t step_index) const;

  // Encodes the given microcode table entry into an address.
  uint32_t encode_address(const microcode::table::Entry &entry) const;

  // Decodes the given address into an opcode, statuses, and step index.
  std::tuple<uint8_t, std::map<const hdl::StatusDecl *, bool>, uint8_t>
  decode_address(uint32_t address) const;

  // Encodes the given controls into a value.
  uint32_t
  encode_value(const std::set<const hdl::ControlDecl *> &controls) const;

  // Decodes the given value into a set of controls.
  std::set<const hdl::ControlDecl *> decode_value(uint32_t value) const;

  size_t num_statuses() const;

  size_t num_status_bits() const;

  size_t num_controls() const;

  uint8_t max_opcode() const;

  size_t num_opcode_bits() const;

  size_t max_step_index() const;

  size_t num_step_index_bits() const;

  // Returns the number of address bits in the instruction memory.
  size_t num_address_bits() const;

  // Returns the number of value bits in the instruction memory.
  size_t num_value_bits() const;

private:
  const StatusEncoder status_encoder_;
  const ControlEncoder control_encoder_;
  const uint8_t max_opcode_;
  const size_t num_opcode_bits_;
  const uint8_t max_step_index_;
  const size_t num_step_index_bits_;
};

} // namespace irata::sim::components::controller
#pragma once

#include <cstdint>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace irata::sim::components::controller {

// The instruction address encoder is used to encode the address of a
// microcode instruction in instruction memory.
class InstructionAddressEncoder {
public:
  explicit InstructionAddressEncoder(
      const microcode::InstructionSet &instruction_set);
  virtual ~InstructionAddressEncoder() = default;

  // Encodes the given opcode, step index, and statuses into a 16-bit address.
  uint16_t encode(uint8_t opcode, uint8_t step_index,
                  std::map<std::string, bool> statuses) const;

  std::vector<uint16_t> permute_statuses_and_encode(
      uint8_t opcode, uint8_t step_index,
      std::map<std::string, bool> partial_statuses) const;

  // Decodes the given 16-bit address into an opcode, step index, and statuses.
  std::tuple<uint8_t, uint8_t, std::map<std::string, bool>>
  decode(uint16_t encoded) const;

  // Returns the number of bits used to encode the address.
  size_t address_bit_count() const;

  // Returns the number of bits used to encode the opcode.
  size_t bit_count() const;

  // Returns the maximum opcode in the instruction set.
  uint8_t max_opcode() const;

  // Returns the number of bits used to encode the opcode.
  size_t opcode_bit_count() const;

  // Returns the maximum program length in the instruction set.
  size_t max_step_index() const;

  // Returns the number of bits used to encode the step index.
  size_t step_index_bit_count() const;

  // Returns the number of bits used to encode the statuses.
  size_t status_bit_count() const;

  // Returns the set of all statuses in the instruction set.
  std::set<std::string> statuses() const;

private:
  const StatusEncoder status_encoder_;
  const uint8_t max_opcode_;
  const size_t opcode_bit_count_;
  const size_t max_step_index_;
  const size_t step_index_bit_count_;
};

} // namespace irata::sim::components::controller

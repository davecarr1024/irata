#pragma once

#include <cstdint>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace irata::sim::components::controller {

// The precondition encoder is used to encode the preconditions of a
// microcode program into a single byte.
class PreconditionEncoder {
public:
  // Creates a new precondition encoder for the given instruction set.
  explicit PreconditionEncoder(
      const microcode::InstructionSet &instruction_set);
  virtual ~PreconditionEncoder() = default;

  // Encodes the given preconditions into a single byte.
  uint8_t encode(const std::map<std::string, bool> &preconditions) const;

  // Decodes the given byte into a map of preconditions.
  std::map<std::string, bool> decode(uint8_t encoded) const;

  // Returns the set of all preconditions in the instruction set.
  std::set<std::string> preconditions() const;

  // Returns a map of precondition names to their bit indices.
  std::map<std::string, uint8_t> precondition_indices() const;

  // Returns the number of preconditions in the instruction set.
  size_t precondition_count() const;

  // Returns the bit index of the given precondition.
  uint8_t precondition_index(const std::string &precondition) const;

private:
  const std::map<std::string, uint8_t> precondition_indices_;
};

} // namespace irata::sim::components::controller
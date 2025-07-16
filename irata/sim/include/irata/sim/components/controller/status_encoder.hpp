#pragma once

#include <cstdint>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace irata::sim::components::controller {

// The status encoder is used to encode the statuses of a
// microcode program into a single byte.
class StatusEncoder {
public:
  // Creates a new status encoder for the given instruction set.
  explicit StatusEncoder(const microcode::InstructionSet &instruction_set);
  virtual ~StatusEncoder() = default;

  // Encodes the given statuses into a single byte.
  // Note that the statuses must be a complete set of statuses for the
  // instruction set. If a status is not provided, an exception will be thrown.
  // To generate all possible permutations of a partial set of statuses, use
  // the permute_statuses method.
  uint8_t encode(std::map<std::string, bool> statuses) const;

  // Encodes every possible permutation of the given partial statuses into a
  // vector of bytes.
  std::vector<uint8_t>
  permute_and_encode(std::map<std::string, bool> partial_statuses) const;

  // Decodes the given byte into a map of statuses.
  std::map<std::string, bool> decode(uint8_t encoded) const;

  // Returns the set of all statuses in the instruction set.
  std::set<std::string> statuses() const;

  // Returns a map of status names to their bit indices.
  std::map<std::string, uint8_t> status_indices() const;

  // Returns the number of statuses in the instruction set.
  size_t status_count() const;

  // Returns the bit index of the given status.
  uint8_t status_index(const std::string &status) const;

  // Returns all possible permutations of the given partial statuses.
  std::vector<std::map<std::string, bool>>
  permute_statuses(const std::map<std::string, bool> &partial_statuses) const;

private:
  const std::map<std::string, uint8_t> status_indices_;
};

} // namespace irata::sim::components::controller
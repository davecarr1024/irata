#pragma once

#include <irata/sim/microcode/table/table.hpp>

namespace irata::sim::components::controller {

class PartialStatuses;
class CompleteStatuses;

// StatusEncoder is responsible for encoding the statuses of the CPU into
// a single byte that can be used to index into the microcode table.
class StatusEncoder {
public:
  explicit StatusEncoder(const microcode::table::Table &table);
  StatusEncoder(const StatusEncoder &) = delete;
  StatusEncoder &operator=(const StatusEncoder &) = delete;

  // Permutes the given statuses by setting all unset statuses to true and false
  // and encodes the resulting statuses into a byte.
  std::vector<uint8_t> encode(const PartialStatuses &statuses) const;

  // Encodes the given statuses into a byte.
  uint8_t encode(const CompleteStatuses &statuses) const;

  // Decodes the given byte into a map of statuses.
  CompleteStatuses decode(uint8_t encoded_statuses) const;

  // Permutes the given statuses by setting all statuses to true and false
  // if they are not already set.
  std::vector<CompleteStatuses> permute(const PartialStatuses &statuses) const;

  // Returns the number of statuses that are used in the microcode table.
  size_t num_statuses() const;

  // Returns the set of all statuses that are used in the microcode table.
  std::set<const hdl::StatusDecl *> statuses() const;

private:
  const std::map<const hdl::StatusDecl *, size_t> indices_;
};

} // namespace irata::sim::components::controller

#pragma once

#include <irata/sim/microcode/table/table.hpp>

namespace irata::sim::components::controller {

// StatusEncoder is responsible for encoding the statuses of the CPU into
// a single byte that can be used to index into the microcode table.
class StatusEncoder {
public:
  explicit StatusEncoder(const microcode::table::Table &table);
  StatusEncoder(const StatusEncoder &) = delete;
  StatusEncoder &operator=(const StatusEncoder &) = delete;

  // Encodes the given statuses into a single byte.
  // Throws an exception if any of the statuses are unknown.
  uint8_t encode(const std::map<const hdl::StatusDecl *, bool> &statuses) const;

  // Decodes the given byte into a map of statuses.
  std::map<const hdl::StatusDecl *, bool>
  decode(uint8_t encoded_statuses) const;

  // Permutes the given statuses by setting all statuses to true and false
  // if they are not already set.
  std::vector<std::map<const hdl::StatusDecl *, bool>> permute_statuses(
      const std::map<const hdl::StatusDecl *, bool> &statuses) const;

  // Permutes the given statuses and encodes them into a vector of bytes.
  std::vector<uint8_t> permute_and_encode_statuses(
      const std::map<const hdl::StatusDecl *, bool> &statuses) const;

  size_t num_statuses() const;

private:
  const std::map<const hdl::StatusDecl *, size_t> indices_;
};

} // namespace irata::sim::components::controller

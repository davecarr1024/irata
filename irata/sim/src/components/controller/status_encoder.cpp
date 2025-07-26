#include <irata/sim/components/controller/status_encoder.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::components::controller {

namespace {

std::set<const hdl::StatusDecl *>
get_statuses(const microcode::table::Table &table) {
  std::set<const hdl::StatusDecl *> statuses;
  for (const auto &entry : table.entries) {
    for (const auto &[status, value] : entry.statuses) {
      statuses.insert(status);
    }
  }
  return statuses;
}

std::map<const hdl::StatusDecl *, size_t>
build_indices(const microcode::table::Table &table) {
  const auto statuses = get_statuses(table);
  std::map<const hdl::StatusDecl *, size_t> indices;
  size_t index = 0;
  for (const auto &status : statuses) {
    indices[status] = index++;
  }
  return indices;
}

} // namespace

StatusEncoder::StatusEncoder(const microcode::table::Table &table)
    : indices_(build_indices(table)) {
  if (indices_.size() > 8) {
    throw std::invalid_argument("Too many statuses to encode");
  }
}

Byte StatusEncoder::encode(
    const std::map<const hdl::StatusDecl *, bool> &statuses) const {
  // Throw an error if any statuses are not set.
  for (const auto &[status, _] : indices_) {
    if (statuses.find(status) == statuses.end()) {
      throw std::invalid_argument("Missing status: " + status->path());
    }
  }
  uint8_t encoded_statuses = 0;
  for (const auto &[status, value] : statuses) {
    if (const auto it = indices_.find(status); it != indices_.end()) {
      if (value) {
        encoded_statuses |= (1 << it->second);
      }
    } else {
      std::ostringstream os;
      os << "Unknown status: " << status->path();
      throw std::invalid_argument(os.str());
    }
  }
  return Byte(encoded_statuses);
}

std::map<const hdl::StatusDecl *, bool>
StatusEncoder::decode(Byte encoded_statuses) const {
  std::map<const hdl::StatusDecl *, bool> statuses;
  for (const auto &[status, index] : indices_) {
    statuses[status] = (encoded_statuses.unsigned_value() & (1 << index)) != 0;
  }
  return statuses;
}

std::vector<std::map<const hdl::StatusDecl *, bool>>
StatusEncoder::permute_statuses(
    const std::map<const hdl::StatusDecl *, bool> &partial_statuses) const {

  // Gather all known statuses
  std::vector<const hdl::StatusDecl *> unset_statuses;
  for (const auto &[status, _] : indices_) {
    if (partial_statuses.find(status) == partial_statuses.end()) {
      unset_statuses.push_back(status);
    }
  }

  // Number of permutations = 2^N for N unset statuses
  size_t count = 1ULL << unset_statuses.size();
  std::vector<std::map<const hdl::StatusDecl *, bool>> permutations;

  for (size_t i = 0; i < count; ++i) {
    std::map<const hdl::StatusDecl *, bool> full_status = partial_statuses;
    for (size_t bit = 0; bit < unset_statuses.size(); ++bit) {
      full_status[unset_statuses[bit]] = (i >> bit) & 1;
    }
    permutations.push_back(std::move(full_status));
  }

  return permutations;
}

std::vector<Byte> StatusEncoder::permute_and_encode_statuses(
    const std::map<const hdl::StatusDecl *, bool> &statuses) const {
  std::vector<Byte> encoded_statuses;
  for (const auto &permuted_statuses : permute_statuses(statuses)) {
    encoded_statuses.push_back(encode(permuted_statuses));
  }
  return encoded_statuses;
}

} // namespace irata::sim::components::controller

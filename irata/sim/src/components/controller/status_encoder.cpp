#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/partial_statuses.hpp>
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

uint8_t StatusEncoder::encode(const CompleteStatuses &statuses) const {
  const auto &status_values = statuses.statuses();
  // Throw an error if any statuses are not set.
  for (const auto &[status, _] : indices_) {
    if (status_values.find(status) == status_values.end()) {
      throw std::invalid_argument("Missing status: " + status->path());
    }
  }
  uint8_t encoded_statuses = 0;
  for (const auto &[status, value] : status_values) {
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
  return encoded_statuses;
}

std::vector<uint8_t>
StatusEncoder::encode(const PartialStatuses &statuses) const {
  std::vector<uint8_t> encoded_statuses;
  for (const auto &permuted_statuses : permute(statuses)) {
    encoded_statuses.push_back(encode(permuted_statuses));
  }
  return encoded_statuses;
}

CompleteStatuses StatusEncoder::decode(uint8_t encoded_statuses) const {
  std::map<const hdl::StatusDecl *, bool> statuses;
  for (const auto &[status, index] : indices_) {
    statuses[status] = (encoded_statuses & (1 << index)) != 0;
  }
  return CompleteStatuses(*this, statuses);
}

std::vector<CompleteStatuses>
StatusEncoder::permute(const PartialStatuses &partial_statuses) const {
  const auto &partial_status_values = partial_statuses.statuses();

  // Gather all known statuses
  std::vector<const hdl::StatusDecl *> unset_statuses;
  for (const auto &[status, _] : indices_) {
    if (partial_status_values.find(status) == partial_status_values.end()) {
      unset_statuses.push_back(status);
    }
  }

  // Number of permutations = 2^N for N unset statuses
  size_t count = 1ULL << unset_statuses.size();
  std::vector<CompleteStatuses> permutations;

  for (size_t i = 0; i < count; ++i) {
    std::map<const hdl::StatusDecl *, bool> full_status = partial_status_values;
    for (size_t bit = 0; bit < unset_statuses.size(); ++bit) {
      full_status[unset_statuses[bit]] = (i >> bit) & 1;
    }
    permutations.push_back(CompleteStatuses(*this, std::move(full_status)));
  }

  return permutations;
}

size_t StatusEncoder::num_statuses() const { return indices_.size(); }

std::set<const hdl::StatusDecl *> StatusEncoder::statuses() const {
  std::set<const hdl::StatusDecl *> statuses;
  for (const auto &[status, _] : indices_) {
    statuses.insert(status);
  }
  return statuses;
}

} // namespace irata::sim::components::controller

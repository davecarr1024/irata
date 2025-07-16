#include <irata/sim/components/controller/status_encoder.hpp>
#include <set>
#include <stdexcept>
#include <utility>

namespace irata::sim::components::controller {

namespace {

std::set<std::string>
get_statuses(const microcode::InstructionSet &instruction_set) {
  std::set<std::string> result;
  for (const auto &instruction : instruction_set.instructions) {
    for (const auto &program : instruction.programs) {
      for (const auto &[key, value] : program.statuses) {
        result.insert(key);
      }
    }
  }
  return result;
}

std::map<std::string, uint8_t>
get_status_indices(const microcode::InstructionSet &instruction_set) {
  std::map<std::string, uint8_t> result;
  uint8_t index = 0;
  for (const auto &status : get_statuses(instruction_set)) {
    result[status] = index++;
  }
  return result;
}

} // namespace

StatusEncoder::StatusEncoder(const microcode::InstructionSet &instruction_set)
    : status_indices_(get_status_indices(instruction_set)) {
  if (status_count() > 8) {
    throw std::invalid_argument("too many statuses");
  }
}

uint8_t StatusEncoder::encode(std::map<std::string, bool> statuses) const {
  // If any statuses are missing, throw an error.
  for (const auto &[key, _] : status_indices_) {
    if (statuses.find(key) == statuses.end()) {
      throw std::invalid_argument("missing status: " + key);
    }
  }
  uint8_t result = 0;
  for (const auto &[key, value] : statuses) {
    result |= value << status_index(key);
  }
  return result;
}

std::map<std::string, bool> StatusEncoder::decode(uint8_t encoded) const {
  std::map<std::string, bool> result;
  for (const auto &[key, index] : status_indices_) {
    result[key] = encoded & (1 << index);
  }
  return result;
}

uint8_t StatusEncoder::status_index(const std::string &status) const {
  if (const auto it = status_indices_.find(status);
      it != status_indices_.end()) {
    return it->second;
  }
  throw std::invalid_argument("unknown status: " + status);
}

size_t StatusEncoder::status_count() const { return status_indices_.size(); }

std::set<std::string> StatusEncoder::statuses() const {
  std::set<std::string> result;
  for (const auto &[key, _] : status_indices_) {
    result.insert(key);
  }
  return result;
}

std::map<std::string, uint8_t> StatusEncoder::status_indices() const {
  return status_indices_;
}

std::vector<std::map<std::string, bool>> StatusEncoder::permute_statuses(
    const std::map<std::string, bool> &partial_statuses) const {
  std::vector<std::map<std::string, bool>> result;

  // Get unspecified statuses
  std::vector<std::string> unspecified;
  for (const auto &[name, _] : status_indices_) {
    if (const auto it = partial_statuses.find(name);
        it == partial_statuses.end()) {
      unspecified.push_back(name);
    }
  }

  const size_t permutations = 1 << unspecified.size();
  result.reserve(permutations);

  // Generate all boolean combinations
  for (size_t i = 0; i < permutations; ++i) {
    std::map<std::string, bool> full = partial_statuses;
    for (size_t bit = 0; bit < unspecified.size(); ++bit) {
      full[unspecified[bit]] = (i >> bit) & 1;
    }
    result.push_back(std::move(full));
  }

  return result;
}

std::vector<uint8_t> StatusEncoder::permute_and_encode(
    std::map<std::string, bool> partial_statuses) const {
  std::vector<uint8_t> result;
  for (const auto &statuses : permute_statuses(partial_statuses)) {
    result.push_back(encode(statuses));
  }
  return result;
}

} // namespace irata::sim::components::controller

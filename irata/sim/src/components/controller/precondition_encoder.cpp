#include <irata/sim/components/controller/precondition_encoder.hpp>
#include <set>
#include <stdexcept>
#include <utility>

namespace irata::sim::components::controller {

namespace {

std::set<std::string>
get_preconditions(const microcode::InstructionSet &instruction_set) {
  std::set<std::string> result;
  for (const auto &instruction : instruction_set.instructions) {
    for (const auto &program : instruction.programs) {
      for (const auto &[key, value] : program.preconditions) {
        result.insert(key);
      }
    }
  }
  return result;
}

std::map<std::string, uint8_t>
get_precondition_indices(const microcode::InstructionSet &instruction_set) {
  std::map<std::string, uint8_t> result;
  uint8_t index = 0;
  for (const auto &precondition : get_preconditions(instruction_set)) {
    result[precondition] = index++;
  }
  return result;
}

} // namespace

PreconditionEncoder::PreconditionEncoder(
    const microcode::InstructionSet &instruction_set)
    : precondition_indices_(get_precondition_indices(instruction_set)) {
  if (precondition_count() > 8) {
    throw std::runtime_error("too many preconditions");
  }
}

uint8_t PreconditionEncoder::encode(
    const std::map<std::string, bool> &preconditions) const {
  uint8_t result = 0;
  for (const auto &[key, value] : preconditions) {
    result |= value << precondition_index(key);
  }
  return result;
}

std::map<std::string, bool> PreconditionEncoder::decode(uint8_t encoded) const {
  std::map<std::string, bool> result;
  for (const auto &[key, index] : precondition_indices_) {
    result[key] = encoded & (1 << index);
  }
  return result;
}

uint8_t
PreconditionEncoder::precondition_index(const std::string &precondition) const {
  if (const auto it = precondition_indices_.find(precondition);
      it != precondition_indices_.end()) {
    return it->second;
  }
  throw std::runtime_error("unknown precondition: " + precondition);
}

size_t PreconditionEncoder::precondition_count() const {
  return precondition_indices_.size();
}

std::set<std::string> PreconditionEncoder::preconditions() const {
  std::set<std::string> result;
  for (const auto &[key, _] : precondition_indices_) {
    result.insert(key);
  }
  return result;
}

std::map<std::string, uint8_t>
PreconditionEncoder::precondition_indices() const {
  return precondition_indices_;
}

} // namespace irata::sim::components::controller

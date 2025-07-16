#include <irata/sim/components/controller/control_encoder.hpp>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace irata::sim::components::controller {

namespace {

std::set<std::string>
get_controls(const microcode::InstructionSet &instruction_set) {
  std::set<std::string> result;
  for (const auto &instruction : instruction_set.instructions) {
    for (const auto &program : instruction.programs) {
      for (const auto &statement : program.statements) {
        for (const auto &control : statement.controls) {
          result.insert(control);
        }
      }
    }
  }
  return result;
}

std::map<std::string, uint8_t>
get_control_indices(const microcode::InstructionSet &instruction_set) {
  std::map<std::string, uint8_t> result;
  uint8_t index = 0;
  for (const auto &control : get_controls(instruction_set)) {
    result[control] = index++;
  }
  return result;
}

} // namespace

ControlEncoder::ControlEncoder(const microcode::InstructionSet &instruction_set)
    : control_indices_(get_control_indices(instruction_set)) {
  if (control_count() > 32) {
    throw std::runtime_error("too many controls to be represented in 32 bits");
  }
}

uint32_t
ControlEncoder::encode(const std::vector<std::string> &controls) const {
  uint32_t result = 0;
  for (const auto &control : controls) {
    result |= 1 << control_index(control);
  }
  return result;
}

std::vector<std::string> ControlEncoder::decode(uint32_t encoded) const {
  std::vector<std::string> result;
  for (const auto &[key, index] : control_indices_) {
    if (encoded & (1 << index)) {
      result.push_back(key);
    }
  }
  return result;
}

uint8_t ControlEncoder::control_index(const std::string &control) const {
  if (const auto it = control_indices_.find(control);
      it != control_indices_.end()) {
    return it->second;
  }
  throw std::runtime_error("unknown control: " + control);
}

size_t ControlEncoder::control_count() const { return control_indices_.size(); }

std::set<std::string> ControlEncoder::controls() const {
  std::set<std::string> result;
  for (const auto &[key, _] : control_indices_) {
    result.insert(key);
  }
  return result;
}

std::map<std::string, uint8_t> ControlEncoder::control_indices() const {
  return control_indices_;
}

} // namespace irata::sim::components::controller

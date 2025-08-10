#include <algorithm>
#include <irata/common/strings/strings.hpp>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

namespace irata::sim::components::controller {

namespace {

std::set<const hdl::ControlDecl *>
get_controls(const microcode::table::Table &table) {
  std::set<const hdl::ControlDecl *> controls;
  for (const auto &entry : table.entries) {
    controls.insert(entry.controls.begin(), entry.controls.end());
  }
  return controls;
}

std::map<const hdl::ControlDecl *, size_t>
build_indices(const microcode::table::Table &table) {
  const auto controls = get_controls(table);
  std::map<const hdl::ControlDecl *, size_t> indices;
  size_t index = 0;
  for (const auto &control : controls) {
    indices[control] = index++;
  }
  return indices;
}

} // namespace

ControlEncoder::ControlEncoder(const microcode::table::Table &table)
    : indices_(build_indices(table)) {
  if (num_controls() > 64) {
    std::vector<std::string> control_names;
    for (const auto &[control, _] : indices_) {
      control_names.push_back(control->path());
    }
    std::sort(control_names.begin(), control_names.end());
    std::ostringstream os;
    os << "Too many controls to encode: ["
       << common::strings::join(control_names, ", ") << "]";
    throw std::invalid_argument(os.str());
  }
}

uint64_t ControlEncoder::encode(
    const std::set<const hdl::ControlDecl *> &controls) const {
  uint64_t encoded_controls = 0;
  for (const auto &control : controls) {
    if (const auto it = indices_.find(control); it != indices_.end()) {
      encoded_controls |= (1ull << it->second);
    } else {
      std::ostringstream os;
      os << "Unknown control: " << control->path();
      throw std::invalid_argument(os.str());
    }
  }
  return encoded_controls;
}

std::set<const hdl::ControlDecl *>
ControlEncoder::decode(uint64_t encoded_controls) const {
  std::set<const hdl::ControlDecl *> controls;
  for (const auto &[control, index] : indices_) {
    if (encoded_controls & (1ull << index)) {
      controls.insert(control);
    }
  }
  return controls;
}

size_t ControlEncoder::num_controls() const { return indices_.size(); }

const std::map<const hdl::ControlDecl *, size_t> &
ControlEncoder::indices() const {
  return indices_;
}

} // namespace irata::sim::components::controller
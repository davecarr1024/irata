#include <irata/sim/components/controller/control_encoder.hpp>
#include <sstream>
#include <stdexcept>

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
  if (num_controls() > 32) {
    throw std::invalid_argument("Too many controls to encode");
  }
}

uint32_t ControlEncoder::encode(
    const std::set<const hdl::ControlDecl *> &controls) const {
  uint32_t encoded_controls = 0;
  for (const auto &control : controls) {
    if (const auto it = indices_.find(control); it != indices_.end()) {
      encoded_controls |= (1 << it->second);
    } else {
      std::ostringstream os;
      os << "Unknown control: " << control->path();
      throw std::invalid_argument(os.str());
    }
  }
  return encoded_controls;
}

std::set<const hdl::ControlDecl *>
ControlEncoder::decode(uint32_t encoded_controls) const {
  std::set<const hdl::ControlDecl *> controls;
  for (const auto &[control, index] : indices_) {
    if (encoded_controls & (1 << index)) {
      controls.insert(control);
    }
  }
  return controls;
}

size_t ControlEncoder::num_controls() const { return indices_.size(); }

} // namespace irata::sim::components::controller
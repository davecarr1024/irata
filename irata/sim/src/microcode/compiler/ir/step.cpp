#include <irata/sim/microcode/compiler/ir/step.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::microcode::compiler::ir {

Step::Step(std::set<const hdl::ControlDecl *> controls,
           std::set<const hdl::WriteControlDecl *> write_controls,
           std::set<const hdl::ReadControlDecl *> read_controls)
    : controls_(std::move(controls)),
      write_controls_(std::move(write_controls)),
      read_controls_(std::move(read_controls)) {
  for (const auto &control : controls_) {
    if (control == nullptr) {
      throw std::invalid_argument("Control cannot be null");
    }
  }
  for (const auto &control : write_controls_) {
    if (control == nullptr) {
      throw std::invalid_argument("Write control cannot be null");
    }
    if (controls_.find(control) == controls_.end()) {
      std::ostringstream os;
      os << "Write control " << control->path() << " must be in controls"
         << " in step " << *this;
      throw std::invalid_argument(os.str());
    }
  }
  for (const auto &control : read_controls_) {
    if (control == nullptr) {
      throw std::invalid_argument("Read control cannot be null");
    }
    if (controls_.find(control) == controls_.end()) {
      std::ostringstream os;
      os << "Read control " << control->path() << " must be in controls"
         << " in step " << *this;
      throw std::invalid_argument(os.str());
    }
  }
}

Step::Step(const dsl::Step &step)
    : Step(step.controls(), step.write_controls(), step.read_controls()) {}

bool Step::can_merge(const Step &other) const {
  return max_phase() <= other.min_phase();
}

Step Step::merge(const Step &other) const {
  if (!can_merge(other)) {
    throw std::invalid_argument("Cannot merge steps");
  }
  std::set<const hdl::ControlDecl *> controls = controls_;
  controls.insert(other.controls_.begin(), other.controls_.end());
  std::set<const hdl::WriteControlDecl *> write_controls = write_controls_;
  write_controls.insert(other.write_controls_.begin(),
                        other.write_controls_.end());
  std::set<const hdl::ReadControlDecl *> read_controls = read_controls_;
  read_controls.insert(other.read_controls_.begin(),
                       other.read_controls_.end());
  return Step(controls, write_controls, read_controls);
}

hdl::TickPhase Step::max_phase() const {
  // Note that this isn't control because no controls can be in this phase.
  hdl::TickPhase phase = hdl::TickPhase::Write;
  for (const auto &control : controls_) {
    phase = std::max(phase, control->phase());
  }
  return phase;
}

hdl::TickPhase Step::min_phase() const {
  hdl::TickPhase phase = hdl::TickPhase::Clear;
  for (const auto &control : controls_) {
    phase = std::min(phase, control->phase());
  }
  return phase;
}

bool Step::operator==(const Step &other) const {
  return std::tie(controls_, write_controls_, read_controls_) ==
         std::tie(other.controls_, other.write_controls_, other.read_controls_);
}

bool Step::operator!=(const Step &other) const { return !(*this == other); }

bool Step::operator<(const Step &other) const {
  return std::tie(controls_, write_controls_, read_controls_) <
         std::tie(other.controls_, other.write_controls_, other.read_controls_);
}

std::ostream &operator<<(std::ostream &os, const Step &step) {
  os << "Step(";
  os << "controls={";
  for (const auto &control : step.controls()) {
    os << *control << ", ";
  }
  os << "})";
  return os;
}

const std::set<const hdl::ControlDecl *> &Step::controls() const {
  return controls_;
}

const std::set<const hdl::WriteControlDecl *> &Step::write_controls() const {
  return write_controls_;
}

const std::set<const hdl::ReadControlDecl *> &Step::read_controls() const {
  return read_controls_;
}

} // namespace irata::sim::microcode::compiler::ir

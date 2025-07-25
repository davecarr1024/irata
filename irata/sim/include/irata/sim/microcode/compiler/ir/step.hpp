#pragma once

#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <ostream>
#include <set>

namespace irata::sim::microcode::compiler::ir {

// Intermediate representation of a step in a microcode instruction.
// This is separate from the DSL step to allow for mutation during compilation.
// Trivially copyable.
class Step {
public:
  // Constructs a step from a set of controls, write controls, and read
  // controls.
  // The controls must be non-null.
  // The write controls and read controls must be a subset of the controls.
  Step(std::set<const hdl::ControlDecl *> controls,
       std::set<const hdl::WriteControlDecl *> write_controls,
       std::set<const hdl::ReadControlDecl *> read_controls);

  // Constructs a step from a DSL step.
  explicit Step(const dsl::Step &step);

  bool operator==(const Step &other) const;
  bool operator!=(const Step &other) const;
  bool operator<(const Step &other) const;

  // Returns the controls in this step.
  const std::set<const hdl::ControlDecl *> &controls() const;

  // Returns the write controls in this step.
  // This is a subset of the controls in this step.
  const std::set<const hdl::WriteControlDecl *> &write_controls() const;

  // Returns the read controls in this step.
  // This is a subset of the controls in this step.
  const std::set<const hdl::ReadControlDecl *> &read_controls() const;

private:
  std::set<const hdl::ControlDecl *> controls_;
  std::set<const hdl::WriteControlDecl *> write_controls_;
  std::set<const hdl::ReadControlDecl *> read_controls_;
};

std::ostream &operator<<(std::ostream &os, const Step &step);

} // namespace irata::sim::microcode::compiler::ir

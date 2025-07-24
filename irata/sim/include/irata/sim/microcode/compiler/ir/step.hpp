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
  // Constructs a step from a DSL step.
  explicit Step(const dsl::Step &step);

  // Returns true if this step can be merged with another step.
  // This is true if the maximum phase of this step is less than or equal to
  // the minimum phase of the other step.
  // This ensures that all controls in this step are consumed before any
  // controls in the other step are consumed. Since controls are consumed
  // in tick-phase order, and all controls in a tick phase are assumed
  // to be consumed simultaneously, this is safe.
  bool can_merge(const Step &other) const;

  // Merges this step with another step. The other step must be mergable.
  Step merge(const Step &other) const;

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
  Step(std::set<const hdl::ControlDecl *> controls,
       std::set<const hdl::WriteControlDecl *> write_controls,
       std::set<const hdl::ReadControlDecl *> read_controls);

  // Returns the maximum phase of all controls in this step.
  hdl::TickPhase max_phase() const;

  // Returns the minimum phase of all controls in this step.
  hdl::TickPhase min_phase() const;

  std::set<const hdl::ControlDecl *> controls_;
  std::set<const hdl::WriteControlDecl *> write_controls_;
  std::set<const hdl::ReadControlDecl *> read_controls_;
};

std::ostream &operator<<(std::ostream &os, const Step &step);

} // namespace irata::sim::microcode::compiler::ir

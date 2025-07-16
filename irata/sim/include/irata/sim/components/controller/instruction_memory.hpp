#include <cstdint>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/components/controller/instruction_address_encoder.hpp>
#include <irata/sim/microcode/instruction_set.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace irata::sim::components::controller {

// The instruction memory is a read-only memory that stores the microcode
// instructions for the CPU. It is addressed by the instruction address
// decoder, which encodes the opcode, step index, and statuses into a single
// address. The instruction memory returns the controls for the given address.
class InstructionMemory : public Component {
public:
  // Creates a new instruction memory for the given instruction set.
  explicit InstructionMemory(const microcode::InstructionSet &instruction_set,
                             std::string_view name = "InstructionMemory",
                             Component *parent = nullptr);

  // Returns the set of statuses in the instruction set.
  std::set<std::string> statuses() const;

  // Returns the set of controls in the instruction set.
  std::set<std::string> controls() const;

  // Returns the decoded instruction memory entry for the given opcode, step
  // index, and statuses. Note that the statuses must be a complete set of
  // statuses for the instruction set.
  std::set<std::string>
  get_controls(uint8_t opcode, uint8_t step_index,
               std::map<std::string, bool> statuses) const;

private:
  const InstructionAddressEncoder address_encoder_;
  const ControlEncoder control_encoder_;
  const std::map<uint16_t, uint32_t> data_;
};

} // namespace irata::sim::components::controller
#include <irata/sim/components/controller/instruction_encoder.hpp>

namespace irata::sim::components::controller {

InstructionEncoder::InstructionEncoder(
    const microcode::InstructionSet &instruction_set)
    : control_encoder_(instruction_set), status_encoder_(instruction_set) {}

} // namespace irata::sim::components::controller
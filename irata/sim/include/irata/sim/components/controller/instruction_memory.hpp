#pragma once

#include <array>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/controller/instruction_encoder.hpp>
#include <irata/sim/components/memory/rom.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <map>
#include <memory>

namespace irata::sim::components::controller {

class InstructionMemory : public Component {
public:
  explicit InstructionMemory(const microcode::table::Table &table,
                             std::string_view name = "instruction_memory",
                             Component *parent = nullptr);
  virtual ~InstructionMemory() = default;

  const InstructionEncoder &encoder() const;

  std::set<const hdl::ControlDecl *> read(uint8_t opcode,
                                          const CompleteStatuses &statuses,
                                          uint8_t step_index) const;

  std::set<const hdl::ControlDecl *> read(const asm_::Instruction &instruction,
                                          const CompleteStatuses &statuses,
                                          uint8_t step_index) const;

private:
  const InstructionEncoder encoder_;
  std::array<memory::ROM, 4> roms_;

  uint32_t read(uint16_t address) const;
};

} // namespace irata::sim::components::controller

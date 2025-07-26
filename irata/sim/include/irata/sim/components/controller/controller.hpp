#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/controller/instruction_memory.hpp>
#include <irata/sim/components/counter.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/word_counter.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <string_view>

namespace irata::sim::components::controller {

class Controller : public Component {
public:
  Controller(const microcode::table::Table &table, Bus<Byte> &data_bus,
             std::string_view name = "controller", Component *parent = nullptr);
  virtual ~Controller() = default;

  const InstructionMemory &instruction_memory() const;
  InstructionMemory &instruction_memory();

  const Register &opcode_register() const;
  Register &opcode_register();
  Byte opcode() const;
  void set_opcode(Byte opcode);

  const Counter &step_counter_register() const;
  Counter &step_counter_register();
  Byte step_counter() const;
  void set_step_counter(Byte step_counter);

  void tick_control(Logger &logger) override;

private:
  InstructionMemory instruction_memory_;
  Register opcode_;
  Counter step_counter_;

  CompleteStatuses status_values() const;
  void set_control_values(const std::set<const hdl::ControlDecl *> &controls);
};

} // namespace irata::sim::components::controller
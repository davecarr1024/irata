#include <irata/sim/components/cpu.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>

namespace irata::sim::components {

Cpu::Cpu(microcode::table::Table microcode_table, ByteBus &data_bus,
         WordBus &address_bus, Component *parent)
    : Component("cpu", parent),
      controller_(microcode_table, data_bus, "controller", this),
      a_("a", &data_bus, this), x_("x", &data_bus, this),
      y_("y", &data_bus, this), pc_("pc", &address_bus, &data_bus, this),
      carry_("carry", this), alu_(*this, data_bus, carry_),
      status_register_(*this, data_bus, alu_.carry_out_status(),
                       alu_.negative_status(), alu_.overflow_status(),
                       alu_.zero_status(), carry_),
      buffer_("buffer", &address_bus, &data_bus, this),
      stack_pointer_("stack_pointer", &data_bus, this) {
  stack_pointer().set_value(0xFF);
}

Cpu Cpu::irata(ByteBus &data_bus, WordBus &address_bus, Component *parent) {
  return Cpu(microcode::compiler::Compiler::compile_irata(), data_bus,
             address_bus, parent);
}

hdl::ComponentType Cpu::type() const { return hdl::ComponentType::Cpu; }

const controller::Controller &Cpu::controller() const { return controller_; }
controller::Controller &Cpu::controller() { return controller_; }

const alu::ALU &Cpu::alu() const { return alu_; }
alu::ALU &Cpu::alu() { return alu_; }

const Register &Cpu::a() const { return a_; }
Register &Cpu::a() { return a_; }

const Counter &Cpu::x() const { return x_; }
Counter &Cpu::x() { return x_; }

const Counter &Cpu::y() const { return y_; }
Counter &Cpu::y() { return y_; }

const WordCounter &Cpu::pc() const { return pc_; }
WordCounter &Cpu::pc() { return pc_; }

const StatusRegister &Cpu::status_register() const { return status_register_; }
StatusRegister &Cpu::status_register() { return status_register_; }

const Status &Cpu::carry() const { return carry_; }
Status &Cpu::carry() { return carry_; }

const WordRegister &Cpu::buffer() const { return buffer_; }
WordRegister &Cpu::buffer() { return buffer_; }

const Counter &Cpu::stack_pointer() const { return stack_pointer_; }
Counter &Cpu::stack_pointer() { return stack_pointer_; }

} // namespace irata::sim::components

#include <irata/sim/components/cpu.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>

namespace irata::sim::components {

Cpu::Cpu(microcode::table::Table microcode_table, ByteBus &data_bus,
         WordBus &address_bus, Component *parent, std::string_view name)
    : Component(name, parent),
      controller_(microcode_table, data_bus, "controller", this),
      alu_(*this, data_bus), a_("a", &data_bus, this), x_("x", &data_bus, this),
      y_("y", &data_bus, this), pc_("pc", &address_bus, &data_bus, this),
      status_register_(*this, data_bus, alu_.carry_out_status(),
                       alu_.negative_status(), alu_.overflow_status(),
                       alu_.zero_status()) {}

Cpu Cpu::irata(ByteBus &data_bus, WordBus &address_bus, Component *parent,
               std::string_view name) {
  return Cpu(microcode::compiler::Compiler::compile_irata(), data_bus,
             address_bus, parent, name);
}

hdl::ComponentType Cpu::type() const { return hdl::ComponentType::Cpu; }

const controller::Controller &Cpu::controller() const { return controller_; }
controller::Controller &Cpu::controller() { return controller_; }

const alu::ALU &Cpu::alu() const { return alu_; }
alu::ALU &Cpu::alu() { return alu_; }

const Register &Cpu::a() const { return a_; }
Register &Cpu::a() { return a_; }

const Register &Cpu::x() const { return x_; }
Register &Cpu::x() { return x_; }

const Register &Cpu::y() const { return y_; }
Register &Cpu::y() { return y_; }

const WordCounter &Cpu::pc() const { return pc_; }
WordCounter &Cpu::pc() { return pc_; }

const StatusRegister &Cpu::status_register() const { return status_register_; }
StatusRegister &Cpu::status_register() { return status_register_; }

} // namespace irata::sim::components

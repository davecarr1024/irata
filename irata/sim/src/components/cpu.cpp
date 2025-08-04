#include <irata/sim/components/cpu.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>

namespace irata::sim::components {

Cpu::Cpu(microcode::table::Table microcode_table, ByteBus &data_bus,
         WordBus &address_bus, Component *parent, std::string_view name)
    : Component(name, parent),
      controller_(microcode_table, data_bus, "controller", this),
      a_("a", &data_bus, this), x_("x", &data_bus, this),
      y_("y", &data_bus, this), pc_("pc", &address_bus, &data_bus, this) {}

Cpu Cpu::irata(ByteBus &data_bus, WordBus &address_bus, Component *parent,
               std::string_view name) {
  return Cpu(microcode::compiler::Compiler::compile_irata(), data_bus,
             address_bus, parent, name);
}

hdl::ComponentType Cpu::type() const { return hdl::ComponentType::Cpu; }

const controller::Controller &Cpu::controller() const { return controller_; }
controller::Controller &Cpu::controller() { return controller_; }

const Register &Cpu::a() const { return a_; }
Register &Cpu::a() { return a_; }

const Register &Cpu::x() const { return x_; }
Register &Cpu::x() { return x_; }

const Register &Cpu::y() const { return y_; }
Register &Cpu::y() { return y_; }

const WordCounter &Cpu::pc() const { return pc_; }
WordCounter &Cpu::pc() { return pc_; }

} // namespace irata::sim::components

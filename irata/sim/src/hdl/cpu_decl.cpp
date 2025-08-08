#include <irata/sim/hdl/cpu_decl.hpp>

namespace irata::sim::hdl {

CpuDecl::CpuDecl(const ComponentDecl &parent, const WordBusDecl &address_bus,
                 const ByteBusDecl &data_bus)
    : ComponentWithParentDecl<ComponentType::Cpu>("cpu", parent),
      ComponentWithTypeDecl<ComponentType::Cpu>("cpu"),
      address_bus_(address_bus), data_bus_(data_bus), a_("a", *this, data_bus_),
      x_("x", *this, data_bus_), y_("y", *this, data_bus_),
      pc_("pc", *this, address_bus_, data_bus_),
      controller_("controller", *this, data_bus_), alu_(*this, data_bus_),
      status_register_(*this, data_bus_, alu_.carry_out(), alu_.negative(),
                       alu_.overflow(), alu_.zero()) {}

void CpuDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Cpu>::verify(component);
  verify_child(a_, component);
  verify_child(x_, component);
  verify_child(y_, component);
  verify_child(pc_, component);
  verify_child(controller_, component);
  verify_child(alu_, component);
  verify_child(status_register_, component);
}

const WordBusDecl &CpuDecl::address_bus() const { return address_bus_; }

const ByteBusDecl &CpuDecl::data_bus() const { return data_bus_; }

const ConnectedByteRegisterDecl &CpuDecl::a() const { return a_; }

const ConnectedByteRegisterDecl &CpuDecl::x() const { return x_; }

const ConnectedByteRegisterDecl &CpuDecl::y() const { return y_; }

const ProgramCounterDecl &CpuDecl::pc() const { return pc_; }

const ControllerDecl &CpuDecl::controller() const { return controller_; }

const AluDecl &CpuDecl::alu() const { return alu_; }

const StatusRegisterDecl &CpuDecl::status_register() const {
  return status_register_;
}

} // namespace irata::sim::hdl
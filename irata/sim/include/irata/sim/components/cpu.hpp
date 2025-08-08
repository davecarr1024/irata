#pragma once

#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/controller/controller.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/components/status_register.hpp>

namespace irata::sim::components {

class Cpu final : public Component {
public:
  Cpu(microcode::table::Table microcode_table, ByteBus &data_bus,
      WordBus &address_bus, Component *parent = nullptr,
      std::string_view name = "cpu");

  static Cpu irata(ByteBus &data_bus, WordBus &address_bus,
                   Component *parent = nullptr, std::string_view name = "cpu");

  hdl::ComponentType type() const override final;

  const controller::Controller &controller() const;
  controller::Controller &controller();

  const Register &a() const;
  Register &a();

  const Register &x() const;
  Register &x();

  const Register &y() const;
  Register &y();

  const WordCounter &pc() const;
  WordCounter &pc();

  const Status &carry() const;
  Status &carry();

  const alu::ALU &alu() const;
  alu::ALU &alu();

  const StatusRegister &status_register() const;
  StatusRegister &status_register();

private:
  controller::Controller controller_;
  Register a_;
  Register x_;
  Register y_;
  WordCounter pc_;
  Status carry_;
  alu::ALU alu_;
  StatusRegister status_register_;
};

} // namespace irata::sim::components

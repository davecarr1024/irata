#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/status.hpp>

namespace irata::sim::components {

class StatusRegister final : public Register {
public:
  StatusRegister(Component &parent, ByteBus &bus, const Status &carry_in,
                 const Status &negative_in, const Status &overflow_in,
                 const Status &zero_in, Status &carry_out);

  hdl::ComponentType type() const override final;

  const Status &carry_in() const;

  const Status &negative_in() const;

  const Status &overflow_in() const;

  const Status &zero_in() const;

  Status &carry_out();
  const Status &carry_out() const;

  Status &negative_out();
  const Status &negative_out() const;

  Status &overflow_out();
  const Status &overflow_out() const;

  Status &zero_out();
  const Status &zero_out() const;

  Control &latch();
  const Control &latch() const;

  const std::map<const Status *, int> &status_indices() const;

  Control &set_carry();
  const Control &set_carry() const;

  Control &clear_carry();
  const Control &clear_carry() const;

protected:
  void tick_process(Logger &logger) override final;

  void tick_clear(Logger &logger) override final;

private:
  const Status &carry_in_;
  const Status &negative_in_;
  const Status &overflow_in_;
  const Status &zero_in_;
  Status &carry_out_;
  Status negative_out_;
  Status overflow_out_;
  Status zero_out_;
  Control latch_;
  std::map<const Status *, int> status_indices_;
  Control set_carry_;
  Control clear_carry_;
};

} // namespace irata::sim::components
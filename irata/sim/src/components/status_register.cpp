#include <irata/sim/components/status_register.hpp>

namespace irata::sim::components {

StatusRegister::StatusRegister(Component &parent, ByteBus &bus,
                               const Status &carry_in,
                               const Status &negative_in,
                               const Status &overflow_in, const Status &zero_in,
                               Status &carry_out)
    : Register("status_register", &bus, &parent), carry_in_(carry_in),
      negative_in_(negative_in), overflow_in_(overflow_in), zero_in_(zero_in),
      carry_out_(carry_out), negative_out_("negative", this),
      overflow_out_("overflow", this), zero_out_("zero", this),
      latch_("latch", hdl::TickPhase::Clear, this, /*auto_clear=*/false),
      status_indices_({
          {&carry_in_, 0},
          {&negative_in_, 7},
          {&overflow_in_, 6},
          {&zero_in_, 1},
      }) {}

hdl::ComponentType StatusRegister::type() const {
  return hdl::ComponentType::StatusRegister;
}

const Status &StatusRegister::carry_in() const { return carry_in_; }

const Status &StatusRegister::negative_in() const { return negative_in_; }

const Status &StatusRegister::overflow_in() const { return overflow_in_; }

const Status &StatusRegister::zero_in() const { return zero_in_; }

Status &StatusRegister::carry_out() { return carry_out_; }
const Status &StatusRegister::carry_out() const { return carry_out_; }

Status &StatusRegister::negative_out() { return negative_out_; }
const Status &StatusRegister::negative_out() const { return negative_out_; }

Status &StatusRegister::overflow_out() { return overflow_out_; }
const Status &StatusRegister::overflow_out() const { return overflow_out_; }

Status &StatusRegister::zero_out() { return zero_out_; }
const Status &StatusRegister::zero_out() const { return zero_out_; }

Control &StatusRegister::latch() { return latch_; }
const Control &StatusRegister::latch() const { return latch_; }

const std::map<const Status *, int> &StatusRegister::status_indices() const {
  return status_indices_;
}

void StatusRegister::tick_clear(Logger &logger) {
  if (latch_.value()) {
    logger << "latching status";
    // Encode status values into the register value.
    uint8_t value = 0x00;
    for (const auto &[status, index] : status_indices_) {
      logger << "status " << status->name() << " = " << status->value();
      value |= status->value() << index;
    }
    set_value(value);
    logger << "status value = " << this->value();
    // Also latch status values into the outgoing status lines.
    carry_out().set_value(carry_in().value());
    negative_out().set_value(negative_in().value());
    overflow_out().set_value(overflow_in().value());
    zero_out().set_value(zero_in().value());
    // Manually clear the latch control line - it is not auto-cleared.
    latch_.set_value(false);
  }
}

} // namespace irata::sim::components

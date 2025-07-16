#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/register.hpp>
#include <string_view>

namespace irata::sim::components {

class Counter : public Register {
public:
  explicit Counter(std::string_view name, Bus<Byte> &bus,
                   Component *parent = nullptr);
  virtual ~Counter() = default;

  bool increment() const;
  void set_increment(bool increment);

  void tick_process(Logger &logger) override;

private:
  Control increment_;
};

} // namespace irata::sim::components

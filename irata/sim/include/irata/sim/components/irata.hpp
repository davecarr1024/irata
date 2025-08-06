#pragma once

#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/cpu.hpp>
#include <irata/sim/components/memory/memory.hpp>
#include <memory>

namespace irata::sim::components {

// Irata is the top-level component of the Irata computer.
// It contains a data bus, an address bus, a memory, a CPU, and a halt control.
// It also provides facilities for running the computer until it halts.
class Irata final : public Component {
public:
  enum class Result { Halt, Crash };

  explicit Irata(std::unique_ptr<memory::ROM> cartridge = nullptr);

  hdl::ComponentType type() const override final;

  const ByteBus &data_bus() const;
  ByteBus &data_bus();

  const WordBus &address_bus() const;
  WordBus &address_bus();

  const memory::Memory &memory() const;
  memory::Memory &memory();

  const Cpu &cpu() const;
  Cpu &cpu();

  const Control &halt() const;
  Control &halt();

  const Control &crash() const;
  Control &crash();

  Result tick_until_halt();

protected:
  void tick_process(Logger &logger) override final;

private:
  ByteBus data_bus_;
  WordBus address_bus_;
  memory::Memory memory_;
  Cpu cpu_;
  Control halt_;
  Control crash_;
  bool halt_received_ = false;
  bool crash_received_ = false;
};

} // namespace irata::sim::components

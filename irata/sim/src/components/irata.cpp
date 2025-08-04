#include <irata/sim/components/irata.hpp>

namespace irata::sim::components {

Irata::Irata(std::unique_ptr<memory::ROM> cartridge)
    : Component("irata", nullptr), data_bus_("data_bus", this),
      address_bus_("address_bus", this),
      memory_(memory::Memory::irata(address_bus_, data_bus_,
                                    std::move(cartridge), this)),
      cpu_(Cpu::irata(data_bus_, address_bus_, this)),
      halt_("halt", hdl::TickPhase::Process, this) {
  cpu_.pc().set_value(Word(0x8000));
}

hdl::ComponentType Irata::type() const { return hdl::ComponentType::Irata; }

const memory::Memory &Irata::memory() const { return memory_; }
memory::Memory &Irata::memory() { return memory_; }

const Cpu &Irata::cpu() const { return cpu_; }
Cpu &Irata::cpu() { return cpu_; }

const ByteBus &Irata::data_bus() const { return data_bus_; }
ByteBus &Irata::data_bus() { return data_bus_; }

const WordBus &Irata::address_bus() const { return address_bus_; }
WordBus &Irata::address_bus() { return address_bus_; }

void Irata::tick_process(Logger &logger) {
  if (halt_.value()) {
    logger << "halted";
    halt_received_ = true;
  }
}

void Irata::tick_until_halt() {
  int i = 0;
  while (!halt_received_ && i++ < 10) {
    std::cout << "ticking irata:\n  pc = " << cpu_.pc().value()
              << "\n  opcode = " << cpu_.controller().opcode()
              << "\n  step counter = " << cpu_.controller().step_counter()
              << "\n";
    tick();
  }
}

} // namespace irata::sim::components

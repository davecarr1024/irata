#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/cpu.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/cpu_decl.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>

namespace irata::sim::hdl {

namespace {

class CpuDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl irata_decl = {ComponentType::Irata, "irata"};
  const ByteBusDecl data_bus_decl = ByteBusDecl("data_bus", irata_decl);
  const WordBusDecl address_bus_decl = WordBusDecl("address_bus", irata_decl);
  const CpuDecl cpu_decl = CpuDecl(irata_decl, address_bus_decl, data_bus_decl);

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::ByteBus data_bus =
      components::ByteBus("data_bus", &irata_component);
  components::WordBus address_bus =
      components::WordBus("address_bus", &irata_component);
};

} // namespace

TEST_F(CpuDeclTest, CpuDecl_Validate) {
  auto cpu = components::Cpu::irata(data_bus, address_bus, &irata_component);
  EXPECT_NO_THROW(cpu_decl.verify(&cpu));
}

} // namespace irata::sim::hdl

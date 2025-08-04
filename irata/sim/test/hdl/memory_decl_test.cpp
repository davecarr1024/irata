#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/memory/memory.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>
#include <irata/sim/hdl/memory_decl.hpp>

namespace irata::sim::hdl {

namespace {

class MemoryDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl irata_decl = {ComponentType::Irata, "irata"};
  const ByteBusDecl data_bus_decl = ByteBusDecl("data_bus", irata_decl);
  const WordBusDecl address_bus_decl = WordBusDecl("address_bus", irata_decl);
  const MemoryDecl memory_decl =
      MemoryDecl("memory", irata_decl, address_bus_decl, data_bus_decl);

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::ByteBus data_bus =
      components::ByteBus("data_bus", &irata_component);
  components::WordBus address_bus =
      components::WordBus("address_bus", &irata_component);
};

} // namespace

TEST_F(MemoryDeclTest, MemoryDecl_Validate) {
  components::memory::Memory memory_component("memory", {}, address_bus,
                                              data_bus, &irata_component);
  EXPECT_NO_THROW(memory_decl.verify(&memory_component));
}

} // namespace irata::sim::hdl

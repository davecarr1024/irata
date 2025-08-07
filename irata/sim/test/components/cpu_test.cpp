#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/cpu.hpp>

namespace irata::sim::components {

namespace {

class CpuTest : public ::testing::Test {
protected:
  Component root = Component("root");
  ByteBus data_bus = ByteBus("data_bus", &root);
  WordBus address_bus = WordBus("address_bus", &root);
  Cpu cpu = Cpu::irata(data_bus, address_bus, &root);
};

} // namespace

TEST_F(CpuTest, Type) { EXPECT_EQ(cpu.type(), hdl::ComponentType::Cpu); }

TEST_F(CpuTest, Constructor) {
  EXPECT_EQ(cpu.name(), "cpu");
  EXPECT_EQ(cpu.controller().parent(), &cpu);
  EXPECT_EQ(cpu.controller().name(), "controller");
  EXPECT_EQ(cpu.alu().parent(), &cpu);
  EXPECT_EQ(cpu.alu().name(), "alu");
  EXPECT_EQ(cpu.a().parent(), &cpu);
  EXPECT_EQ(cpu.a().name(), "a");
  EXPECT_EQ(cpu.x().parent(), &cpu);
  EXPECT_EQ(cpu.x().name(), "x");
  EXPECT_EQ(cpu.y().parent(), &cpu);
  EXPECT_EQ(cpu.y().name(), "y");
  EXPECT_EQ(cpu.pc().parent(), &cpu);
  EXPECT_EQ(cpu.pc().name(), "pc");
}

} // namespace irata::sim::components

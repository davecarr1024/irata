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

TEST_F(CpuTest, Properties) {
  EXPECT_EQ(cpu.name(), "cpu");
  EXPECT_EQ(cpu.type(), hdl::ComponentType::Cpu);
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
  EXPECT_EQ(cpu.status_register().parent(), &cpu);
  EXPECT_EQ(cpu.status_register().name(), "status_register");
  EXPECT_EQ(cpu.buffer().parent(), &cpu);
  EXPECT_EQ(cpu.buffer().name(), "buffer");
  EXPECT_EQ(cpu.stack_pointer().parent(), &cpu);
  EXPECT_EQ(cpu.stack_pointer().name(), "stack_pointer");
}

} // namespace irata::sim::components

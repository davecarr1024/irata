#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/add.hpp>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class AddTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  ALU alu = ALU(irata, data_bus);
  const Add &add = dynamic_cast<const Add &>(*alu.module(hdl::AluOpcode::Add));
};

} // namespace

TEST_F(AddTest, Properties) {
  EXPECT_EQ(add.name(), "add");
  EXPECT_EQ(add.opcode(), hdl::AluOpcode::Add);
  EXPECT_EQ(add.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(add.parent(), &alu);
}

TEST_F(AddTest, Add) {
  EXPECT_EQ(add.apply(false, Byte(0x01), Byte(0x02)), (Module::Result{
                                                          .value = 0x03,
                                                      }));
}

TEST_F(AddTest, AddWithCarry) {
  EXPECT_EQ(add.apply(true, Byte(0x01), Byte(0x02)), (Module::Result{
                                                         .value = 0x04,
                                                     }));
}

TEST_F(AddTest, AddOverflow) {
  EXPECT_EQ(add.apply(false, Byte(0xFF), Byte(0x01)), (Module::Result{
                                                          .value = 0x00,
                                                          .carry = true,
                                                          .zero = true,
                                                      }));
}

} // namespace irata::sim::components::alu

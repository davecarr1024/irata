#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/address_add.hpp>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>

namespace irata::sim::components::alu {

namespace {

class AddressAddTest : public ::testing::Test {
protected:
  FakeComponent irata = {hdl::ComponentType::Irata, "irata"};
  ByteBus data_bus = ByteBus("data_bus", &irata);
  Status carry = Status("carry", &irata);
  ALU alu = ALU(irata, data_bus, carry);
  AddressAdd &address_add =
      dynamic_cast<AddressAdd &>(*alu.module(hdl::AluOpcode::AddressAdd));
};

} // namespace

TEST_F(AddressAddTest, Properties) {
  EXPECT_EQ(address_add.name(), "address_add");
  EXPECT_EQ(address_add.opcode(), hdl::AluOpcode::AddressAdd);
  EXPECT_EQ(address_add.type(), hdl::ComponentType::AluModule);
  EXPECT_EQ(address_add.parent(), &alu);
}

TEST_F(AddressAddTest, Add) {
  EXPECT_EQ(address_add.apply(false, Byte(0x01), Byte(0x02)),
            (Module::Result{
                .value = 0x03,
                .set_flags = false,
            }));
}

TEST_F(AddressAddTest, AddIgnoresCarryIn) {
  EXPECT_EQ(address_add.apply(true, Byte(0x01), Byte(0x02)),
            (Module::Result{
                .value = 0x03,
                .set_flags = false,
            }));
}

TEST_F(AddressAddTest, AddOverflow) {
  EXPECT_EQ(address_add.apply(false, Byte(0xFF), Byte(0x01)),
            (Module::Result{
                .value = 0x00,
                .set_flags = false,
            }));
}

} // namespace irata::sim::components::alu

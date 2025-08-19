#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/memory/address.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/word_register.hpp>
#include <irata/sim/hdl/component_type.hpp>

namespace irata::sim::components::memory {

namespace {

class AddressTest : public ::testing::Test {
protected:
  FakeComponent root = FakeComponent(hdl::ComponentType::Unknown, "root");
  WordBus address_bus = WordBus("address_bus", &root);
  ByteBus data_bus = ByteBus("data_bus", &root);
  Register data_register = Register("data_register", &data_bus, &root);
  WordRegister address_register =
      WordRegister("address_register", &address_bus, &data_bus, &root);
  Address address = Address(root, address_bus, data_bus);
};

} // namespace

TEST_F(AddressTest, Properties) {
  EXPECT_EQ(address.name(), "address");
  EXPECT_EQ(address.type(), hdl::ComponentType::MemoryAddress);
  EXPECT_EQ(&address.address_bus(), &address_bus);
  EXPECT_EQ(&address.data_bus(), &data_bus);
  EXPECT_EQ(address.read_control().name(), "read");
  EXPECT_EQ(address.read_control().parent(), &address);
  EXPECT_EQ(address.write_control().name(), "write");
  EXPECT_EQ(address.write_control().parent(), &address);
  EXPECT_EQ(address.reset_control().name(), "reset");
  EXPECT_EQ(address.reset_control().parent(), &address);
  EXPECT_EQ(address.high_register().name(), "high");
  EXPECT_EQ(address.high_register().parent(), &address);
  EXPECT_EQ(address.low_register().name(), "low");
  EXPECT_EQ(address.low_register().parent(), &address);
}

TEST_F(AddressTest, Value) {
  EXPECT_EQ(address.value(), 0x0000);
  EXPECT_EQ(address.high().value(), 0x00);
  EXPECT_EQ(address.low().value(), 0x00);
  address.set_value(0x1234);
  EXPECT_EQ(address.value(), 0x1234);
  EXPECT_EQ(address.high().value(), 0x12);
  EXPECT_EQ(address.low().value(), 0x34);
}

TEST_F(AddressTest, Idle) {
  address.set_value(0x1234);
  address.tick();
  EXPECT_EQ(address.value(), 0x1234);
}

TEST_F(AddressTest, Write) {
  address.set_value(0x1234);
  address.set_write(true);
  address_register.set_read(true);
  address.tick();
  EXPECT_EQ(address_register.value(), 0x1234);
}

TEST_F(AddressTest, Read) {
  address_register.set_value(0x1234);
  address_register.set_write(true);
  address.set_read(true);
  address.tick();
  EXPECT_EQ(address.value(), 0x1234);
}

TEST_F(AddressTest, Reset) {
  address.set_value(0x1234);
  address.set_reset(true);
  address.tick();
  EXPECT_EQ(address.value(), 0x0000);
}

TEST_F(AddressTest, StackPointerWrite) {
  // Set MAR high to 0x01.
  address.high_register().set_reset(true);
  address.high_register().set_increment(true);
  address.tick();
  EXPECT_EQ(address.high(), 0x01);

  // Copy SP to MAR low.
  data_register.set_value(0x23);
  data_register.set_write(true);
  address.low_register().set_read(true);
  address.tick();
  EXPECT_EQ(address.low(), 0x23);

  // MAR = 0x0100 + SP.
  EXPECT_EQ(address.value(), 0x0123);
}

TEST_F(AddressTest, Increment) {
  address.set_value(0x1234);
  address.set_increment(true);
  address.tick();
  EXPECT_EQ(address.value(), 0x1235);
}

TEST_F(AddressTest, Decrement) {
  address.set_value(0x1234);
  address.set_decrement(true);
  address.tick();
  EXPECT_EQ(address.value(), 0x1233);
}

} // namespace irata::sim::components::memory

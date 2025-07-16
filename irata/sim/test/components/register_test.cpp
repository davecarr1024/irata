#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/register.hpp>
#include <stdexcept>

using ::testing::IsNull;
using ::testing::NotNull;

namespace irata::sim {

TEST(RegisterTest, Value) {
  Bus bus("bus");
  Register reg("reg", bus, &bus);
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0));
  reg.set_value(Byte::from_unsigned(0xAB));
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0xAB));
}

TEST(RegisterTest, Idle) {
  Bus bus("bus");
  Register reg("reg", bus, &bus);
  // Set register value.
  reg.set_value(Byte::from_unsigned(0xAB));
  // Value is set on register.
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0xAB));
  // Bus has no value.
  EXPECT_EQ(bus.value(), std::nullopt);
  // Tick.
  bus.tick();
  // Value is still set on register.
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0xAB));
  // Bus still has no value.
  EXPECT_EQ(bus.value(), std::nullopt);
}

TEST(RegisterTest, WriteThroughBus) {
  Bus bus("bus");
  Register from("from", bus, &bus);
  Register to("to", bus, &bus);
  // Set value on from register.
  from.set_value(Byte::from_unsigned(0xAB));
  // Value is set on from register.
  EXPECT_EQ(from.value(), Byte::from_unsigned(0xAB));
  // Value is not set on to register.
  EXPECT_EQ(to.value(), Byte::from_unsigned(0));
  // Write from register to bus.
  from.set_write(true);
  // Read to register from bus.
  to.set_read(true);
  // Tick.
  bus.tick();
  // Value is still set on from register.
  EXPECT_EQ(from.value(), Byte::from_unsigned(0xAB));
  // Value is set on to register.
  EXPECT_EQ(to.value(), Byte::from_unsigned(0xAB));
  // Write is cleared.
  EXPECT_FALSE(from.write());
  // Read is cleared.
  EXPECT_FALSE(to.read());
  // Bus is cleared.
  EXPECT_EQ(bus.value(), std::nullopt);
}

TEST(RegisterTest, Reset) {
  Bus bus("bus");
  Register reg("reg", bus, &bus);
  // Set value on register.
  reg.set_value(Byte::from_unsigned(0xAB));
  // Value is set on register.
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0xAB));
  // Reset register.
  reg.set_reset(true);
  // Tick.
  bus.tick();
  // Value is reset on register.
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0));
  // Reset is cleared.
  EXPECT_FALSE(reg.reset());
}

TEST(RegisterTest, ReadOpenBus) {
  Bus bus("bus");
  Register reg("reg", bus, &bus);
  // Set read on register.
  reg.set_read(true);
  // Tick. Expect an error since nothing is writing to the bus.
  EXPECT_THROW(bus.tick(), std::logic_error);
}

TEST(RegisterTest, ControlsExistInComponentTree) {
  Component root("root");
  Bus bus("bus", &root);
  Register reg("reg", bus, &root);
  EXPECT_THAT(root.child("/reg/write"), NotNull());
  EXPECT_THAT(root.child("/reg/read"), NotNull());
}

} // namespace irata::sim
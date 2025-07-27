#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/register.hpp>
#include <stdexcept>

using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Optional;

namespace irata::sim::components {

TEST(RegisterTest, Value) {
  Bus<Byte> bus("bus");
  Register reg("reg", &bus, &bus);
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0));
  reg.set_value(Byte::from_unsigned(0xAB));
  EXPECT_EQ(reg.value(), Byte::from_unsigned(0xAB));
}

TEST(RegisterTest, Idle) {
  Bus<Byte> bus("bus");
  Register reg("reg", &bus, &bus);
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
  Bus<Byte> bus("bus");
  Register from("from", &bus, &bus);
  Register to("to", &bus, &bus);
  // Set value on from register.
  from.set_value(Byte::from_unsigned(0xAB));
  // Value is set on from register.
  EXPECT_EQ(from.value(), Byte::from_unsigned(0xAB));
  // Value is not set on to register.
  EXPECT_EQ(to.value(), Byte::from_unsigned(0));
  // Write from register to bus.
  from.set_write(true);
  EXPECT_THAT(from.write(), Optional(true));
  // Read to register from bus.
  to.set_read(true);
  EXPECT_THAT(to.read(), Optional(true));
  // Tick.
  bus.tick();
  // Value is still set on from register.
  EXPECT_EQ(from.value(), Byte::from_unsigned(0xAB));
  // Value is set on to register.
  EXPECT_EQ(to.value(), Byte::from_unsigned(0xAB));
  // Write is cleared.
  EXPECT_THAT(from.write(), Optional(false));
  // Read is cleared.
  EXPECT_THAT(to.read(), Optional(false));
  // Bus is cleared.
  EXPECT_EQ(bus.value(), std::nullopt);
}

TEST(RegisterTest, Reset) {
  Bus<Byte> bus("bus");
  Register reg("reg", &bus, &bus);
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
  Bus<Byte> bus("bus");
  Register reg("reg", &bus, &bus);
  // Set read on register.
  reg.set_read(true);
  // Tick. Expect an error since nothing is writing to the bus.
  EXPECT_THROW(bus.tick(), std::logic_error);
}

TEST(RegisterTest, ControlsCreatedWhenConnectedToBus) {
  Component root("root");
  Bus<Byte> bus("bus", &root);
  Register reg("reg", &bus, &root);
  EXPECT_THAT(root.child("/reg/write"), NotNull());
  EXPECT_THAT(root.child("/reg/read"), NotNull());
  EXPECT_TRUE(reg.has_read());
  EXPECT_TRUE(reg.has_write());
}

TEST(RegisterTest, ControlsNotCreatedWhenNotConnectedToBus) {
  Component root("root");
  Register reg("reg", nullptr, &root);
  EXPECT_THAT(reg.bus(), IsNull());
  EXPECT_THAT(root.child("/reg/write"), IsNull());
  EXPECT_THAT(root.child("/reg/read"), IsNull());
  EXPECT_FALSE(reg.has_read());
  EXPECT_FALSE(reg.has_write());
  EXPECT_EQ(reg.read(), std::nullopt);
  EXPECT_EQ(reg.write(), std::nullopt);
  EXPECT_THROW(reg.set_read(true), std::logic_error);
  EXPECT_THROW(reg.set_write(true), std::logic_error);
}

TEST(RegisterTest, Serialize) {
  Register reg("reg");
  reg.set_value(Byte::from_unsigned(0xAB));
  std::ostringstream os;
  reg.serialize_all(os);
  EXPECT_EQ(os.str(), "value: 0xAB\n"
                      "reset\n");
}

} // namespace irata::sim::components
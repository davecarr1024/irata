#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>

using ::testing::Optional;

namespace irata::sim::components {

TEST(BusTest, Type) {
  ByteBus byte_bus;
  EXPECT_EQ(byte_bus.type(), hdl::ComponentType::ByteBus);
  WordBus word_bus;
  EXPECT_EQ(word_bus.type(), hdl::ComponentType::WordBus);
}

TEST(BusTest, Empty) {
  Bus<Byte> bus;
  EXPECT_EQ(bus.value(), std::nullopt);
  EXPECT_EQ(bus.setter(), std::nullopt);
}

TEST(BusTest, Write) {
  Bus<Byte> bus;
  Component component("component", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component);
  EXPECT_EQ(bus.value(), Byte::from_unsigned(0x12));
  EXPECT_THAT(bus.setter(), Optional(component.path()));
}

TEST(BusTest, WriteConflict) {
  Bus<Byte> bus;
  Component component1("component1", &bus);
  Component component2("component2", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component1);
  EXPECT_THROW(bus.set_value(Byte::from_unsigned(0x34), component2),
               std::logic_error);
}

TEST(BusTest, ClearOnTick) {
  Bus<Byte> bus;
  Component component("component", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component);
  EXPECT_EQ(bus.value(), Byte::from_unsigned(0x12));
  EXPECT_THAT(bus.setter(), Optional(component.path()));
  bus.tick();
  EXPECT_EQ(bus.value(), std::nullopt);
  EXPECT_EQ(bus.setter(), std::nullopt);
}

} // namespace irata::sim::components
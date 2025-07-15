#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>

using namespace irata::sim;
using ::testing::Optional;

TEST(BusTest, Empty) {
  Bus bus("bus");
  EXPECT_EQ(bus.value(), std::nullopt);
  EXPECT_EQ(bus.setter(), std::nullopt);
}

TEST(BusTest, Write) {
  Bus bus("bus");
  Component component("component", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component);
  EXPECT_EQ(bus.value(), Byte::from_unsigned(0x12));
  EXPECT_THAT(bus.setter(), Optional(component.path()));
}

TEST(BusTest, WriteConflict) {
  Bus bus("bus");
  Component component1("component1", &bus);
  Component component2("component2", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component1);
  EXPECT_THROW(bus.set_value(Byte::from_unsigned(0x34), component2),
               std::logic_error);
}

TEST(BusTest, ClearOnTick) {
  Bus bus("bus");
  Component component("component", &bus);
  bus.set_value(Byte::from_unsigned(0x12), component);
  EXPECT_EQ(bus.value(), Byte::from_unsigned(0x12));
  EXPECT_THAT(bus.setter(), Optional(component.path()));
  bus.tick();
  EXPECT_EQ(bus.value(), std::nullopt);
  EXPECT_EQ(bus.setter(), std::nullopt);
}
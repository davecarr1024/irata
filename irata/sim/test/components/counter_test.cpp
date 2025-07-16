#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/counter.hpp>

namespace irata::sim::components {

TEST(CounterTest, SetIncrement) {
  Bus bus;
  Counter counter("counter", bus);
  EXPECT_FALSE(counter.increment());
  counter.set_increment(true);
  EXPECT_TRUE(counter.increment());
}

TEST(CounterTest, Idle) {
  Bus bus;
  Counter counter("counter", bus);
  counter.set_increment(false);
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
}

TEST(CounterTest, Increment) {
  Bus bus;
  Counter counter("counter", bus);
  counter.set_increment(true);
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_FALSE(counter.increment());
}

TEST(CounterTest, Overflow) {
  Bus bus;
  Counter counter("counter", bus);
  counter.set_increment(true);
  counter.set_value(Byte::from_unsigned(255));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(255));
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
}

TEST(CounterTest, Reset) {
  Bus bus;
  Counter counter("counter", bus);
  counter.set_reset(true);
  counter.set_value(Byte::from_unsigned(1));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_TRUE(counter.reset());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.reset());
}

TEST(CounterTest, ResetOverridesIncrement) {
  Bus bus;
  Counter counter("counter", bus);
  counter.set_reset(true);
  counter.set_increment(true);
  counter.set_value(Byte::from_unsigned(1));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_TRUE(counter.reset());
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.reset());
  EXPECT_FALSE(counter.increment());
}

} // namespace irata::sim::components
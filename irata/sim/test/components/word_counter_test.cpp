#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/word_counter.hpp>

namespace irata::sim::components {

TEST(WordCounterTest, Idle) {
  WordCounter counter("counter");
  EXPECT_EQ(counter.value(), Word(0));
  EXPECT_FALSE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Word(0));
  EXPECT_FALSE(counter.increment());
}

TEST(WordCounterTest, Reset) {
  WordCounter counter("counter");
  counter.set_value(Word(1));
  EXPECT_EQ(counter.value(), Word(1));
  counter.set_reset(true);
  EXPECT_TRUE(counter.reset());
  counter.tick();
  EXPECT_EQ(counter.value(), Word(0));
  EXPECT_FALSE(counter.reset());
}

TEST(WordCounterTest, Increment) {
  WordCounter counter("counter");
  EXPECT_EQ(counter.value(), Word(0));
  counter.set_increment(true);
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Word(1));
  EXPECT_FALSE(counter.increment());
}

TEST(WordCounterTest, IncrementOverflow) {
  WordCounter counter("counter");
  counter.set_value(Word(0xFFFF));
  EXPECT_EQ(counter.value(), Word(0xFFFF));
  counter.set_increment(true);
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Word(0));
  EXPECT_FALSE(counter.increment());
}

TEST(WordCounterTest, ResetOverridesIncrement) {
  WordCounter counter("counter");
  counter.set_value(Word(1));
  EXPECT_EQ(counter.value(), Word(1));
  counter.set_reset(true);
  EXPECT_TRUE(counter.reset());
  counter.set_increment(true);
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Word(0));
  EXPECT_FALSE(counter.reset());
  EXPECT_FALSE(counter.increment());
}

} // namespace irata::sim::components

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/components/status_register.hpp>

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components {

namespace {

class StatusRegisterTest : public ::testing::Test {
protected:
  FakeComponent root = {hdl::ComponentType::Unknown, "root"};
  ByteBus bus = ByteBus("bus", &root);
  Status carry_in = Status("carry_in", &root);
  Status negative_in = Status("negative_in", &root);
  Status overflow_in = Status("overflow_in", &root);
  Status zero_in = Status("zero_in", &root);
  Status carry_out = Status("carry_out", &root);
  StatusRegister status_register = StatusRegister(
      root, bus, carry_in, negative_in, overflow_in, zero_in, carry_out);
};

} // namespace

TEST_F(StatusRegisterTest, Properties) {
  EXPECT_EQ(status_register.name(), "status_register");
  EXPECT_EQ(status_register.type(), hdl::ComponentType::StatusRegister);
  EXPECT_EQ(status_register.parent(), &root);
  EXPECT_EQ(status_register.carry_in().parent(), &root);
  EXPECT_EQ(status_register.carry_in().name(), "carry_in");
  EXPECT_EQ(status_register.negative_in().parent(), &root);
  EXPECT_EQ(status_register.negative_in().name(), "negative_in");
  EXPECT_EQ(status_register.overflow_in().parent(), &root);
  EXPECT_EQ(status_register.overflow_in().name(), "overflow_in");
  EXPECT_EQ(status_register.zero_in().parent(), &root);
  EXPECT_EQ(status_register.zero_in().name(), "zero_in");
  EXPECT_EQ(status_register.carry_out().parent(), &root);
  EXPECT_EQ(status_register.carry_out().name(), "carry_out");
  EXPECT_EQ(status_register.negative_out().parent(), &status_register);
  EXPECT_EQ(status_register.negative_out().name(), "negative");
  EXPECT_EQ(status_register.overflow_out().parent(), &status_register);
  EXPECT_EQ(status_register.overflow_out().name(), "overflow");
  EXPECT_EQ(status_register.zero_out().parent(), &status_register);
  EXPECT_EQ(status_register.zero_out().name(), "zero");
  EXPECT_EQ(status_register.latch().parent(), &status_register);
  EXPECT_EQ(status_register.latch().name(), "latch");
  EXPECT_THAT(status_register.status_indices(),
              UnorderedElementsAre(Pair(&carry_in, 0), Pair(&negative_in, 7),
                                   Pair(&overflow_in, 6), Pair(&zero_in, 1)));
}

TEST_F(StatusRegisterTest, DontLatch) {
  carry_in.set_value(true);
  negative_in.set_value(true);
  overflow_in.set_value(true);
  zero_in.set_value(true);
  status_register.latch().set_value(false);
  status_register.tick();
  EXPECT_FALSE(status_register.carry_out().value());
  EXPECT_FALSE(status_register.negative_out().value());
  EXPECT_FALSE(status_register.overflow_out().value());
  EXPECT_FALSE(status_register.zero_out().value());
  EXPECT_EQ(status_register.value(), 0x00);
}

TEST_F(StatusRegisterTest, Latch) {
  carry_in.set_value(true);
  negative_in.set_value(true);
  overflow_in.set_value(true);
  zero_in.set_value(true);
  status_register.latch().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.carry_out().value());
  EXPECT_TRUE(status_register.negative_out().value());
  EXPECT_TRUE(status_register.overflow_out().value());
  EXPECT_TRUE(status_register.zero_out().value());
  EXPECT_EQ(status_register.value(), 0xC3);
  EXPECT_FALSE(status_register.latch().value());
}

TEST_F(StatusRegisterTest, Latch_Carry) {
  carry_in.set_value(true);
  status_register.latch().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.carry_out().value());
  EXPECT_EQ(status_register.value(), 0x01);
}

TEST_F(StatusRegisterTest, Latch_Negative) {
  negative_in.set_value(true);
  status_register.latch().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.negative_out().value());
  EXPECT_EQ(status_register.value(), 0x80);
}

TEST_F(StatusRegisterTest, Latch_Overflow) {
  overflow_in.set_value(true);
  status_register.latch().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.overflow_out().value());
  EXPECT_EQ(status_register.value(), 0x40);
}

TEST_F(StatusRegisterTest, Latch_Zero) {
  zero_in.set_value(true);
  status_register.latch().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.zero_out().value());
  EXPECT_EQ(status_register.value(), 0x02);
}

TEST_F(StatusRegisterTest, SetCarry) {
  status_register.set_carry().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.carry_out().value());
  EXPECT_EQ(status_register.value(), 0x01);
}

TEST_F(StatusRegisterTest, ClearCarry) {
  status_register.set_carry().set_value(true);
  status_register.tick();
  EXPECT_TRUE(status_register.carry_out().value());
  EXPECT_EQ(status_register.value(), 0x01);
  status_register.clear_carry().set_value(true);
  status_register.tick();
  EXPECT_FALSE(status_register.carry_out().value());
  EXPECT_EQ(status_register.value(), 0x00);
}

} // namespace irata::sim::components

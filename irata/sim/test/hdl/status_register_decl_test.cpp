#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/components/status_register.hpp>
#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>
#include <irata/sim/hdl/status_register_decl.hpp>

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::hdl {

namespace {

class StatusRegisterDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl root = {ComponentType::Unknown, "root"};
  const ByteBusDecl bus = {"bus", root};
  const StatusDecl carry_out = {"carry_out", root};
  const StatusDecl carry_in = {"carry", root};
  const StatusDecl negative_in = {"negative", root};
  const StatusDecl overflow_in = {"overflow", root};
  const StatusDecl zero_in = {"zero", root};
  const StatusRegisterDecl status_register = {
      root, bus, carry_in, negative_in, overflow_in, zero_in, carry_out};
};

} // namespace

TEST_F(StatusRegisterDeclTest, Properties) {
  EXPECT_EQ(status_register.name(), "status_register");
  EXPECT_EQ(status_register.type(), ComponentType::StatusRegister);
  EXPECT_EQ(status_register.parent(), &root);
  EXPECT_EQ(&status_register.bus(), &bus);
  EXPECT_EQ(&status_register.carry_in(), &carry_in);
  EXPECT_EQ(&status_register.negative_in(), &negative_in);
  EXPECT_EQ(&status_register.overflow_in(), &overflow_in);
  EXPECT_EQ(&status_register.zero_in(), &zero_in);
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

TEST_F(StatusRegisterDeclTest, Verify) {
  components::FakeComponent root_component = {ComponentType::Unknown, "root"};
  components::ByteBus bus_component("bus", &root_component);
  components::Status carry_in_component("carry", &root_component);
  components::Status negative_in_component("negative", &root_component);
  components::Status overflow_in_component("overflow", &root_component);
  components::Status zero_in_component("zero", &root_component);
  components::Status carry_out_component("carry_out", &root_component);
  components::StatusRegister status_register_component(
      root_component, bus_component, carry_in_component, negative_in_component,
      overflow_in_component, zero_in_component, carry_out_component);
  EXPECT_NO_THROW(status_register.verify(&status_register_component));
}

} // namespace irata::sim::hdl

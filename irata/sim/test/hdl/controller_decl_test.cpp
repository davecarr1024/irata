#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/controller/controller.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/hdl/controller_decl.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>

namespace irata::sim::hdl {

namespace {

class ControllerDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl irata_decl = {ComponentType::Irata, "irata"};
  const ByteBusDecl data_bus_decl = ByteBusDecl("data_bus", irata_decl);
  const ControllerDecl controller_decl =
      ControllerDecl("controller", irata_decl, data_bus_decl);

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::ByteBus data_bus =
      components::ByteBus("data_bus", &irata_component);
};

} // namespace

TEST_F(ControllerDeclTest, ControllerDecl_Validate) {
  components::controller::Controller controller_component =
      components::controller::Controller::irata(data_bus, "controller",
                                                &irata_component);
  EXPECT_NO_THROW(controller_decl.verify(&controller_component));
}

} // namespace irata::sim::hdl

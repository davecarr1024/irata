#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/controller/controller.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <stdexcept>

namespace irata::sim::components::controller {

namespace {

class ControllerTest : public ::testing::Test {
protected:
  const hdl::StatusDecl status1_decl = hdl::StatusDecl("status1", hdl::irata());
  const hdl::ProcessControlDecl control1_decl =
      hdl::ProcessControlDecl("control1", hdl::irata());
  const hdl::ProcessControlDecl control2_decl =
      hdl::ProcessControlDecl("control2", hdl::irata());
  const asm_::Instruction instruction1 = asm_::Instruction(
      "instruction", Byte(0x10), asm_::AddressingMode::IMMEDIATE,
      "instruction1 is a sequential instruction with two steps that set "
      "control1 and then control2.");
  const asm_::Instruction instruction2 = asm_::Instruction(
      "instruction", Byte(0x20), asm_::AddressingMode::IMMEDIATE,
      "instruction2 is a branching instruction with one step that sets "
      "control1 if status1 is true and control2 if status1 is false.");
  const microcode::table::Table table = {{
      {
          .instruction = instruction1,
          .step_index = Byte(0x00),
          .controls = {&control1_decl},
      },
      {
          .instruction = instruction1,
          .step_index = Byte(0x01),
          .controls = {&control2_decl},
      },
      {
          .instruction = instruction2,
          .step_index = Byte(0x00),
          .statuses = {{&status1_decl, true}},
          .controls = {&control1_decl},
      },
      {
          .instruction = instruction2,
          .step_index = Byte(0x00),
          .statuses = {{&status1_decl, false}},
          .controls = {&control2_decl},
      },
  }};
  Component root = Component("root");
  Bus<Byte> bus = Bus<Byte>("bus", &root);
  Controller controller = Controller(table, bus, "controller", &root);
  Status status1 = Status("status1", &root);
  // Don't set controls to autoclear so they can be inspected after a tick.
  // Note that this is not a realistic use case for a control.
  // Note that this means the controls need to be manually cleared.
  Control control1 = Control("control1", hdl::TickPhase::Process, &root, false);
  Control control2 = Control("control2", hdl::TickPhase::Process, &root, false);
};

} // namespace

TEST_F(ControllerTest, UnknownOpcode) {
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());
  controller.set_opcode(Byte(0x30));
  controller.set_step_counter(Byte(0x00));
  controller.tick();
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());
}

TEST_F(ControllerTest, UnknownStep) {
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());
  controller.set_opcode(Byte(0x20));
  controller.set_step_counter(Byte(0x01));
  controller.tick();
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());
}

TEST_F(ControllerTest, NonBranchingInstruction) {
  // controls are off to start
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());

  // Run instruction1 step 0: sets control1
  controller.set_opcode(Byte(0x10));
  controller.set_step_counter(Byte(0x00));
  controller.tick();
  EXPECT_TRUE(control1.value());
  EXPECT_FALSE(control2.value());

  // reset control1
  control1.set_clear(true);
  controller.tick();
  EXPECT_FALSE(control1.value());

  // Run instruction1 step 1: sets control2
  controller.set_opcode(Byte(0x10));
  controller.set_step_counter(Byte(0x01));
  controller.tick();
  EXPECT_FALSE(control1.value());
  EXPECT_TRUE(control2.value());
}

TEST_F(ControllerTest, BranchingInstruction) {
  // controls are off to start
  EXPECT_FALSE(control1.value());
  EXPECT_FALSE(control2.value());

  // Run instruction2 step 0 with status1 true: sets control1
  controller.set_opcode(Byte(0x20));
  controller.set_step_counter(Byte(0x00));
  status1.set_value(true);
  controller.tick();
  EXPECT_TRUE(control1.value());
  EXPECT_FALSE(control2.value());

  // reset control1
  control1.set_clear(true);
  controller.tick();
  EXPECT_FALSE(control1.value());

  // Run instruction2 step 0 with status1 false: sets control2
  controller.set_opcode(Byte(0x20));
  controller.set_step_counter(Byte(0x00));
  status1.set_value(false);
  controller.tick();
  EXPECT_FALSE(control1.value());
  EXPECT_TRUE(control2.value());
}

} // namespace irata::sim::components::controller

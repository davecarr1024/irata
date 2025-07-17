#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/controller/controller.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/microcode/instruction_set.hpp>

namespace irata::sim::components::controller {

TEST(ControllerTest, SetAndGetOpcode) {
  Bus<Byte> bus;
  const auto instruction_set = microcode::InstructionSet{};
  Controller controller(instruction_set, &bus);
  EXPECT_EQ(controller.opcode(), Byte::from_unsigned(0));
  controller.set_opcode(Byte::from_unsigned(1));
  EXPECT_EQ(controller.opcode(), Byte::from_unsigned(1));
}

TEST(ControllerTest, SetAndGetStepIndex) {
  Bus<Byte> bus;
  const auto instruction_set = microcode::InstructionSet{};
  Controller controller(instruction_set, &bus);
  EXPECT_EQ(controller.step_index(), Byte::from_unsigned(0));
  controller.set_step_index(Byte::from_unsigned(1));
  EXPECT_EQ(controller.step_index(), Byte::from_unsigned(1));
}

TEST(ControllerTest, AbsoluteNoStatusProgram) {
  // Create a simple instruction set with a single instruction that loads
  // the value of register b into register a.
  // Note that there are no statuses in this instruction set, so the instruction
  // will always be executed. Note that the control paths are absolute.
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .instruction =
              asm_::Instruction{
                  .name = "lda",
                  .opcode = 0x01,
                  .description = "load a from b",
              },
          .programs = {
              microcode::Program{.statements = {microcode::Statement{
                                     .controls = {"/a/read", "/b/write"}}}}}}}};

  // Build up a simple component tree with a bus, and two registers a and b, and
  // a controller.
  Component root("root");
  Bus<Byte> bus("bus", &root);
  Register a("a", &bus, &root);
  Register b("b", &bus, &root);
  Controller controller(instruction_set, &bus, "controller", &root);

  // Set the opcode and step index to the first instruction in the instruction
  // Note that this is done directly and doesn't happen through any microcode
  // preamble or fetch-execute.
  controller.set_opcode(Byte::from_unsigned(0x01));
  controller.set_step_index(Byte::from_unsigned(0));

  // Set the value of register b to 0x12.
  b.set_value(Byte::from_unsigned(0x12));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x12));
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));

  // Tick!
  root.tick();

  // The value of register b was copied into register a.
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x12));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x12));
}

TEST(ControllerTest, RelativeNoStatusProgram) {
  // Microcode uses relative control paths — will be prefixed by controller.
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .instruction =
              asm_::Instruction{
                  .name = "lda",
                  .opcode = 0x01,
                  .description = "load a from b",
              },
          .programs = {
              microcode::Program{.statements = {microcode::Statement{
                                     .controls = {"a/read", "b/write"}}}}}}}};

  // Root component tree:
  //   /root
  //     /ctl
  //       /a
  //       /b
  Component root("root");
  Bus<Byte> bus("bus", &root);
  Component ctl("ctl", &root);
  Register a("a", &bus, &ctl);
  Register b("b", &bus, &ctl);
  Controller controller(instruction_set, &bus, "controller", &ctl, "/ctl/");

  // Set initial opcode and step index manually.
  controller.set_opcode(Byte::from_unsigned(0x01));
  controller.set_step_index(Byte::from_unsigned(0));

  // Set b = 0x12, a = 0x00
  b.set_value(Byte::from_unsigned(0x12));
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x12));

  // Tick the full tree
  root.tick();

  // a should now have b’s value
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x12));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x12));
}

TEST(ControllerTest, AbsoluteWithStatusProgram) {
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .instruction =
              asm_::Instruction{
                  .name = "lda",
                  .opcode = 0x01,
                  .description = "load a from b (if /lda_enable)",
              },
          .programs = {microcode::Program{
                           // branch 1: lda_enable is true so do the copy
                           .statements = {microcode::Statement{
                               .controls = {"/a/read", "/b/write"}}},
                           .statuses = {{"/lda_enable", true}},
                       },
                       microcode::Program{
                           // branch 2: lda_enable is false so do nop
                           .statements = {microcode::Statement{}},
                           .statuses = {{"/lda_enable", false}},
                       }}}}};

  Component root("root");
  Bus<Byte> bus("bus", &root);
  Register a("a", &bus, &root);
  Register b("b", &bus, &root);
  Status lda_enable("lda_enable", &root);
  Controller controller(instruction_set, &bus, "controller", &root);

  // Set initial opcode and step index manually.
  controller.set_opcode(Byte::from_unsigned(0x01));
  controller.set_step_index(Byte::from_unsigned(0));

  // lda_enable = false
  lda_enable.set_value(false);
  EXPECT_FALSE(lda_enable.value());

  // Set b = 0x12
  b.set_value(Byte::from_unsigned(0x56));
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x56));

  // Tick the full tree
  root.tick();

  // a should not have changed since lda_enable is false
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x56));

  // lda_enable = true
  lda_enable.set_value(true);
  EXPECT_TRUE(lda_enable.value());

  // Tick the full tree
  root.tick();

  // a should now have b’s value
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x56));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x56));
}

TEST(ControllerTest, RelativeWithStatusProgram) {
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .instruction =
              asm_::Instruction{
                  .name = "lda",
                  .opcode = 0x01,
                  .description = "load a from b (if ctl/lda_enable)",
              },
          .programs = {microcode::Program{
                           // branch 1: lda_enable is true so do the copy
                           .statements = {microcode::Statement{
                               .controls = {"a/read", "b/write"}}},
                           .statuses = {{"lda_enable", true}},
                       },
                       microcode::Program{
                           // branch 2: lda_enable is false so do nop
                           .statements = {microcode::Statement{}},
                           .statuses = {{"lda_enable", false}},
                       }}}}};

  // Root component tree:
  //   /root
  //     /ctl
  //       /a
  //       /b
  //       /lda_enable
  Component root("root");
  Bus<Byte> bus("bus", &root);
  Component ctl("ctl", &root);
  Register a("a", &bus, &ctl);
  Register b("b", &bus, &ctl);
  Status lda_enable("lda_enable", &ctl);
  Controller controller(instruction_set, &bus, "controller", &ctl, "/ctl/");

  // Set initial opcode and step index manually.
  controller.set_opcode(Byte::from_unsigned(0x01));
  controller.set_step_index(Byte::from_unsigned(0));

  // lda_enable = false
  lda_enable.set_value(false);
  EXPECT_FALSE(lda_enable.value());

  // Set b = 0x34
  b.set_value(Byte::from_unsigned(0x34));
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x34));

  // Tick the full tree
  root.tick();

  // a should not have changed since lda_enable is false
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x00));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x34));

  // lda_enable = true
  lda_enable.set_value(true);
  EXPECT_TRUE(lda_enable.value());

  // Tick the full tree
  root.tick();

  // a should now have b’s value
  EXPECT_EQ(a.value(), Byte::from_unsigned(0x34));
  EXPECT_EQ(b.value(), Byte::from_unsigned(0x34));
}

} // namespace irata::sim::components::controller

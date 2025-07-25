#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/microcode/compiler/ir/step.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::compiler::ir {

namespace {

class MicrocodeIrStepTest : public ::testing::Test {
protected:
  const dsl::Step &dsl_step_ =
      *dsl::InstructionSet::irata()
           .get_instruction("lda", asm_::AddressingMode::IMMEDIATE)
           .steps()
           .front();
};

} // namespace

TEST_F(MicrocodeIrStepTest, ConstructFromDsl) {
  Step ir_step(dsl_step_);
  EXPECT_EQ(ir_step.controls(), dsl_step_.controls());
  EXPECT_EQ(ir_step.write_controls(), dsl_step_.write_controls());
  EXPECT_EQ(ir_step.read_controls(), dsl_step_.read_controls());
}

TEST_F(MicrocodeIrStepTest, ConstructFromControls) {
  const hdl::ProcessControlDecl &process_control =
      hdl::irata().cpu().program_counter().increment();
  const hdl::WriteControlDecl &write_control =
      hdl::irata().cpu().program_counter().write();
  const hdl::ReadControlDecl &read_control =
      hdl::irata().cpu().program_counter().read();
  Step ir_step({&process_control, &write_control, &read_control},
               {&write_control}, {&read_control});
  EXPECT_THAT(
      ir_step.controls(),
      UnorderedElementsAre(&process_control, &write_control, &read_control));
  EXPECT_THAT(ir_step.write_controls(), UnorderedElementsAre(&write_control));
  EXPECT_THAT(ir_step.read_controls(), UnorderedElementsAre(&read_control));
}

TEST_F(MicrocodeIrStepTest, Equality) {
  Step ir_step1(dsl_step_);
  Step ir_step2(dsl_step_);
  EXPECT_EQ(ir_step1, ir_step2);
  EXPECT_FALSE(ir_step1 != ir_step2);
}

} // namespace irata::sim::microcode::compiler::ir

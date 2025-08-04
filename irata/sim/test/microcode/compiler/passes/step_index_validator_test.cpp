#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_validator.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

namespace irata::sim::microcode::compiler::passes {

namespace {

class StepIndexValidatorTest : public ::testing::Test {
protected:
  void validate(const ir::InstructionSet &instruction_set) {
    validator_.run(instruction_set);
  }

  void validate(const dsl::InstructionSet &instruction_set) {
    validate(ir::InstructionSet(instruction_set));
  }

  StepIndexValidator validator_;
  const asm_::Instruction &instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  const hdl::ProcessControlDecl &increment_step_index_ =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_step_index_ =
      hdl::irata().cpu().controller().step_counter().reset();
};

} // namespace

TEST_F(StepIndexValidatorTest, EmptyInstructionSet) {
  ir::InstructionSet instruction_set(std::set<ir::Instruction>{});
  EXPECT_NO_THROW(validate(instruction_set));
}

TEST_F(StepIndexValidatorTest, EmptyInstruction) {
  ir::InstructionSet instruction_set(
      {ir::Instruction(instruction_descriptor_, {}, {})});
  EXPECT_NO_THROW(validate(instruction_set));
}

TEST_F(StepIndexValidatorTest, OneStepWithReset) {
  ir::InstructionSet instruction_set(
      {ir::Instruction(instruction_descriptor_,
                       {ir::Step({&reset_step_index_}, {}, {}, 0)}, {})});
  EXPECT_NO_THROW(validate(instruction_set));
}

TEST_F(StepIndexValidatorTest, OneStepWithoutReset) {
  ir::InstructionSet instruction_set({ir::Instruction(
      instruction_descriptor_, {ir::Step({}, {}, {}, 0)}, {})});
  EXPECT_THROW(validate(instruction_set), std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, TwoStepsWithIncrementAndReset) {
  ir::InstructionSet instruction_set(
      {ir::Instruction(instruction_descriptor_,
                       {ir::Step({&increment_step_index_}, {}, {}, 0),
                        ir::Step({&reset_step_index_}, {}, {}, 0)},
                       {})});
  EXPECT_NO_THROW(validate(instruction_set));
}

TEST_F(StepIndexValidatorTest, TwoStepsWithIncrementWithoutReset) {
  ir::InstructionSet instruction_set({ir::Instruction(
      instruction_descriptor_,
      {ir::Step({&increment_step_index_}, {}, {}, 0), ir::Step({}, {}, {}, 0)},
      {})});
  EXPECT_THROW(validate(instruction_set), std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, TwoStepsWithoutIncrementWithReset) {
  ir::InstructionSet instruction_set({ir::Instruction(
      instruction_descriptor_,
      {ir::Step({}, {}, {}, 0), ir::Step({&reset_step_index_}, {}, {}, 0)},
      {})});
  EXPECT_THROW(validate(instruction_set), std::invalid_argument);
}

} // namespace irata::sim::microcode::compiler::passes

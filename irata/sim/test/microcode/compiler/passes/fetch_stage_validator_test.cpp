#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/ir/step.hpp>
#include <irata/sim/microcode/compiler/passes/fetch_stage_validator.hpp>

namespace irata::sim::microcode::compiler::passes {

namespace {

class FetchStageValidatorTest : public ::testing::Test {
protected:
  const asm_::Instruction &descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);
  const hdl::ProcessControlDecl control1 = {"control1", hdl::irata()};
  const hdl::ProcessControlDecl control2 = {"control2", hdl::irata()};
  const hdl::ProcessControlDecl control3 = {"control3", hdl::irata()};

  FetchStageValidator validator;
};

} // namespace

TEST_F(FetchStageValidatorTest, EmptyInstructionSet) {
  ir::InstructionSet instruction_set(std::set<ir::Instruction>{});
  EXPECT_NO_THROW(validator.run(instruction_set));
}

TEST_F(FetchStageValidatorTest, InstructionsWithSameFetchStage) {
  ir::Step step1({&control1}, {}, {}, 0);
  ir::Step step2({&control2}, {}, {}, 0);
  ir::Instruction instruction1(descriptor, {step1, step2}, {});
  ir::Instruction instruction2(descriptor, {step1, step2}, {});
  ir::InstructionSet instruction_set({instruction1, instruction2});
  EXPECT_NO_THROW(validator.run(instruction_set));
}

TEST_F(FetchStageValidatorTest, InstructionsWithDifferentFetchStage) {
  ir::Step step1({&control1}, {}, {}, 0);
  ir::Step step2({&control2}, {}, {}, 0);
  ir::Step step3({&control3}, {}, {}, 0);
  ir::Instruction instruction1(descriptor, {step1, step2}, {});
  ir::Instruction instruction2(descriptor, {step1, step3}, {});
  ir::InstructionSet instruction_set({instruction1, instruction2});
  EXPECT_THROW(validator.run(instruction_set), std::invalid_argument);
}

TEST_F(FetchStageValidatorTest, InstructionsWithSmallerFetchStage) {
  ir::Step step1({&control1}, {}, {}, 0);
  ir::Step step2({&control2}, {}, {}, 0);
  ir::Step step3({&control3}, {}, {}, 0);
  ir::Instruction instruction1(descriptor, {step1, step2}, {});
  ir::Instruction instruction2(descriptor, {step1}, {});
  ir::InstructionSet instruction_set({instruction1, instruction2});
  EXPECT_THROW(validator.run(instruction_set), std::invalid_argument);
}

TEST_F(FetchStageValidatorTest, InstructionsWithLargerFetchStage) {
  ir::Step step1({&control1}, {}, {}, 0);
  ir::Step step2({&control2}, {}, {}, 0);
  ir::Step step3({&control3}, {}, {}, 0);
  ir::Instruction instruction1(descriptor, {step1, step2}, {});
  ir::Instruction instruction2(descriptor, {step1, step2, step3}, {});
  ir::InstructionSet instruction_set({instruction1, instruction2});
  EXPECT_THROW(validator.run(instruction_set), std::invalid_argument);
}

TEST_F(FetchStageValidatorTest,
       InstructionsWithSameFetchStageButDifferentLaterStages) {
  ir::Step step1({&control1}, {}, {}, 0);
  ir::Step step2({&control2}, {}, {}, 1);
  ir::Step step3({&control3}, {}, {}, 1);
  ir::Instruction instruction1(descriptor, {step1, step2}, {});
  ir::Instruction instruction2(descriptor, {step1, step3}, {});
  ir::InstructionSet instruction_set({instruction1, instruction2});
  EXPECT_NO_THROW(validator.run(instruction_set));
}

} // namespace irata::sim::microcode::compiler::passes

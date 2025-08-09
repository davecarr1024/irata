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
    validator.run(instruction_set);
  }

  void validate(const dsl::InstructionSet &instruction_set) {
    validate(ir::InstructionSet(instruction_set));
  }

  void validate(
      std::vector<std::set<const hdl::ProcessControlDecl *>> control_sets) {
    validate(instruction_set(control_sets));
  }

  StepIndexValidator validator;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  const hdl::ProcessControlDecl &increment_step_index =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_step_index =
      hdl::irata().cpu().controller().step_counter().reset();

  // Build an instruction set with the given control sets assigned to steps.
  ir::InstructionSet instruction_set(
      std::vector<std::set<const hdl::ProcessControlDecl *>> control_sets) {
    std::vector<ir::Step> steps;
    for (auto &controls : control_sets) {
      std::set<const hdl::ControlDecl *> control_set;
      std::move(controls.begin(), controls.end(),
                std::inserter(control_set, control_set.end()));
      steps.push_back(ir::Step(std::move(control_set), {}, {}, 0));
    }
    return ir::InstructionSet(
        {ir::Instruction(instruction_descriptor, steps, {})});
  }
};

} // namespace

TEST_F(StepIndexValidatorTest, LastStepDoesntReset) {
  EXPECT_THROW(validate({{}}), std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, LastStepIncrements) {
  EXPECT_THROW(validate({{&increment_step_index}}), std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, LastStepResetsAndIncrements) {
  EXPECT_THROW(validate({{&increment_step_index, &reset_step_index}}),
               std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, LastStepResets) {
  EXPECT_NO_THROW(validate({{&reset_step_index}}));
}

TEST_F(StepIndexValidatorTest, IncrementThenNoReset) {
  EXPECT_THROW(validate({{&increment_step_index}, {}}), std::invalid_argument);
}

TEST_F(StepIndexValidatorTest, IncrementThenReset) {
  EXPECT_NO_THROW(validate({{&increment_step_index}, {&reset_step_index}}));
}

TEST_F(StepIndexValidatorTest, NoIncrementThenReset) {
  EXPECT_THROW(validate({{}, {&reset_step_index}}), std::invalid_argument);
}

} // namespace irata::sim::microcode::compiler::passes

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/step_index_transformer.hpp>

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::compiler::passes {

namespace {

class StepIndexTransformerTest : public ::testing::Test {
protected:
  StepIndexTransformer transformer_;
  const hdl::ProcessControlDecl &increment_control_ =
      hdl::irata().cpu().controller().step_counter().increment();
  const hdl::ProcessControlDecl &reset_control_ =
      hdl::irata().cpu().controller().step_counter().reset();
  const hdl::ProcessControlDecl &other_control_ =
      hdl::irata().cpu().pc().increment();
  const asm_::Instruction &instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);

  ir::InstructionSet transform(const ir::InstructionSet &instruction_set) {
    return transformer_.run(instruction_set);
  }

  ir::Instruction instruction_with_controls(
      std::vector<std::set<const hdl::ControlDecl *>> controls) {
    std::vector<ir::Step> steps;
    for (const auto &control_set : controls) {
      steps.push_back(ir::Step(control_set, {}, {}));
    }
    return ir::Instruction(instruction_descriptor_, steps, {});
  }

  template <typename Matcher>
  auto InstructionSetHasInstructions(Matcher matcher) {
    return Property("instructions", &ir::InstructionSet::instructions, matcher);
  }

  template <typename Matcher> auto InstructionHasSteps(Matcher matcher) {
    return Property("steps", &ir::Instruction::steps, matcher);
  }

  template <typename Matcher> auto InstructionHasDescriptor(Matcher matcher) {
    return Property("descriptor", &ir::Instruction::descriptor, matcher);
  }

  template <typename Matcher> auto StepHasControls(Matcher matcher) {
    return Property("controls", &ir::Step::controls, matcher);
  }
};

} // namespace

TEST_F(StepIndexTransformerTest, EmptyInstructionSet) {
  const ir::InstructionSet instruction_set(std::set<ir::Instruction>{});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(transformed_instruction_set,
              InstructionSetHasInstructions(IsEmpty()));
}

TEST_F(StepIndexTransformerTest, EmptyInstruction) {
  const ir::InstructionSet instruction_set({instruction_with_controls({})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(transformed_instruction_set,
              InstructionSetHasInstructions(UnorderedElementsAre(
                  AllOf(InstructionHasDescriptor(instruction_descriptor_),
                        InstructionHasSteps(IsEmpty())))));
}

TEST_F(StepIndexTransformerTest, OneStepWithoutReset) {
  const ir::InstructionSet instruction_set({instruction_with_controls({{}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(transformed_instruction_set,
              InstructionSetHasInstructions(UnorderedElementsAre(AllOf(
                  InstructionHasDescriptor(instruction_descriptor_),
                  InstructionHasSteps(UnorderedElementsAre(StepHasControls(
                      UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, OneStepWithReset) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{&reset_control_}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(transformed_instruction_set,
              InstructionSetHasInstructions(UnorderedElementsAre(
                  AllOf(InstructionHasDescriptor(instruction_descriptor_),
                        InstructionHasSteps(ElementsAre(StepHasControls(
                            UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, TwoStepsWithoutIncrementWithoutReset) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{}, {}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(
      transformed_instruction_set,
      InstructionSetHasInstructions(UnorderedElementsAre(AllOf(
          InstructionHasDescriptor(instruction_descriptor_),
          InstructionHasSteps(ElementsAre(
              StepHasControls(UnorderedElementsAre(&increment_control_)),
              StepHasControls(UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, TwoStepsWithIncrementWithoutReset) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{&increment_control_}, {}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(
      transformed_instruction_set,
      InstructionSetHasInstructions(UnorderedElementsAre(AllOf(
          InstructionHasDescriptor(instruction_descriptor_),
          InstructionHasSteps(ElementsAre(
              StepHasControls(UnorderedElementsAre(&increment_control_)),
              StepHasControls(UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, TwoStepsWithoutIncrementWithReset) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{}, {&reset_control_}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(
      transformed_instruction_set,
      InstructionSetHasInstructions(UnorderedElementsAre(AllOf(
          InstructionHasDescriptor(instruction_descriptor_),
          InstructionHasSteps(ElementsAre(
              StepHasControls(UnorderedElementsAre(&increment_control_)),
              StepHasControls(UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, TwoStepsWithIncrementWithReset) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{&increment_control_}, {&reset_control_}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(
      transformed_instruction_set,
      InstructionSetHasInstructions(UnorderedElementsAre(AllOf(
          InstructionHasDescriptor(instruction_descriptor_),
          InstructionHasSteps(ElementsAre(
              StepHasControls(UnorderedElementsAre(&increment_control_)),
              StepHasControls(UnorderedElementsAre(&reset_control_))))))));
}

TEST_F(StepIndexTransformerTest, TwoStepsWithOtherControl) {
  const ir::InstructionSet instruction_set(
      {instruction_with_controls({{&other_control_}, {&other_control_}})});
  const auto transformed_instruction_set = transform(instruction_set);
  EXPECT_THAT(transformed_instruction_set,
              InstructionSetHasInstructions(UnorderedElementsAre(
                  AllOf(InstructionHasDescriptor(instruction_descriptor_),
                        InstructionHasSteps(ElementsAre(
                            StepHasControls(UnorderedElementsAre(
                                &increment_control_, &other_control_)),
                            StepHasControls(UnorderedElementsAre(
                                &reset_control_, &other_control_))))))));
}

} // namespace irata::sim::microcode::compiler::passes

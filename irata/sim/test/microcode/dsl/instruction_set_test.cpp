#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::dsl {

namespace {

template <typename InstructionDescriptorMatcher>
auto InstructionHasDescriptor(InstructionDescriptorMatcher matcher) {
  return Property("instruction", &Instruction::descriptor, matcher);
}

template <typename StepMatcher> auto InstructionHasSteps(StepMatcher matcher) {
  return Property("steps", &Instruction::steps, matcher);
}

template <typename StatusMatcher>
auto InstructionHasStatuses(StatusMatcher matcher) {
  return Property("statuses", &Instruction::statuses, matcher);
}

template <typename Matcher> auto StepHasControls(Matcher matcher) {
  return Property("controls", &Step::controls, matcher);
}

} // namespace

TEST(InstructionSetTest, Empty) {
  InstructionSet instruction_set;
  EXPECT_THAT(instruction_set.instructions(), IsEmpty());
}

TEST(InstructionSetTest, CreateInstruction) {
  InstructionSet instruction_set;
  const asm_::Instruction &descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  instruction_set.create_instruction(descriptor)
      ->create_step()
      ->with_control(hdl::irata().memory().write())
      ->with_control(hdl::irata().cpu().a().read());
  EXPECT_THAT(
      instruction_set.instructions(),
      UnorderedElementsAre(Pointee(AllOf(
          InstructionHasDescriptor(descriptor),
          InstructionHasSteps(ElementsAre(Pointee(StepHasControls(
              UnorderedElementsAre(&hdl::irata().memory().write(),
                                   &hdl::irata().cpu().a().read())))))))));
}

} // namespace irata::sim::microcode::dsl

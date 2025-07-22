#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/microcode/ir/instruction.hpp>
#include <irata/sim/microcode/ir/instruction_set.hpp>
#include <irata/sim/microcode/ir/step.hpp>

using ::testing::AllOf;
using ::testing::IsEmpty;
using ::testing::Property;

namespace irata::sim::microcode::ir {

namespace {

template <typename InstructionDescriptorMatcher>
auto InstructionHasDescriptor(InstructionDescriptorMatcher matcher) {
  return Property("instruction", &Instruction::instruction, matcher);
}

template <typename StepMatcher> auto InstructionHasSteps(StepMatcher matcher) {
  return Property("steps", &Instruction::steps, matcher);
}

template <typename StatusMatcher>
auto InstructionHasStatuses(StatusMatcher matcher) {
  return Property("statuses", &Instruction::statuses, matcher);
}

} // namespace

TEST(InstructionSetTest, Empty) {
  InstructionSet instruction_set;
  EXPECT_THAT(instruction_set.instructions(), IsEmpty());
}

TEST(InstructionSetTest, CreateInstruction) {
  const asm_::Instruction instruction_descriptor1{.name = "instruction1"};
  const asm_::Instruction instruction_descriptor2{.name = "instruction2"};
  InstructionSet instruction_set;
  auto *instruction1 =
      instruction_set.create_instruction(instruction_descriptor1);
  auto *instruction2 =
      instruction1->create_instruction(instruction_descriptor2);
  EXPECT_EQ(instruction1->instruction_set(), &instruction_set);
  EXPECT_EQ(instruction2->instruction_set(), &instruction_set);
  EXPECT_THAT(
      instruction_set.instructions(),
      ElementsAre(
          Pointee(AllOf(InstructionHasDescriptor(instruction_descriptor1),
                        InstructionHasSteps(IsEmpty()),
                        InstructionHasStatuses(IsEmpty()))),
          Pointee(AllOf(InstructionHasDescriptor(instruction_descriptor2),
                        InstructionHasSteps(IsEmpty()),
                        InstructionHasStatuses(IsEmpty())))));
}

} // namespace irata::sim::microcode::ir

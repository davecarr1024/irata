#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

using ::testing::AllOf;
using ::testing::IsEmpty;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::dsl {

namespace {

template <typename Matcher> auto InstructionHasDescriptor(Matcher matcher) {
  return Property("descriptor", &Instruction::descriptor, matcher);
}

template <typename Matcher> auto InstructionHasInstructionSet(Matcher matcher) {
  return Property("instruction_set", &Instruction::instruction_set, matcher);
}

} // namespace

TEST(MicrocodeDslInstructionSetTest, Empty) {
  InstructionSet instruction_set;
  EXPECT_THAT(instruction_set.instructions(), IsEmpty());
}

TEST(MicrocodeDslInstructionSetTest, CreateInstruction) {
  InstructionSet instruction_set;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  instruction_set.create_instruction(instruction_descriptor);
  EXPECT_THAT(instruction_set.instructions(),
              UnorderedElementsAre(Pointee(
                  AllOf(InstructionHasDescriptor(instruction_descriptor),
                        InstructionHasInstructionSet(&instruction_set)))));
}

TEST(MicrocodeDslInstructionSetTest, CreateInstructionByName) {
  InstructionSet instruction_set;
  instruction_set.create_instruction("lda", asm_::AddressingMode::IMMEDIATE);
  EXPECT_THAT(instruction_set.instructions(),
              UnorderedElementsAre(Pointee(
                  AllOf(InstructionHasDescriptor(
                            asm_::InstructionSet::irata().get_instruction(
                                "lda", asm_::AddressingMode::IMMEDIATE)),
                        InstructionHasInstructionSet(&instruction_set)))));
}

} // namespace irata::sim::microcode::dsl

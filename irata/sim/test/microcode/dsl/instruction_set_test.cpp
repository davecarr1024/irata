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

TEST(MicrocodeDslInstructionSetTest, GetInstructionByDescriptor) {
  InstructionSet instruction_set;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  auto *instruction =
      instruction_set.create_instruction(instruction_descriptor);
  EXPECT_EQ(&instruction_set.get_instruction(instruction_descriptor),
            instruction);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByDescriptorNotFound) {
  InstructionSet instruction_set;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  EXPECT_THROW(instruction_set.get_instruction(instruction_descriptor),
               std::invalid_argument);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByDescriptorWithStatus) {
  InstructionSet instruction_set;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  auto *instruction1 =
      instruction_set.create_instruction(instruction_descriptor)
          ->with_status(hdl::irata().cpu().status_analyzer().zero(), true);
  auto *instruction2 =
      instruction_set.create_instruction(instruction_descriptor)
          ->with_status(hdl::irata().cpu().status_analyzer().zero(), false);
  EXPECT_EQ(&instruction_set.get_instruction(
                instruction_descriptor,
                {{&hdl::irata().cpu().status_analyzer().zero(), true}}),
            instruction1);
  EXPECT_EQ(&instruction_set.get_instruction(
                instruction_descriptor,
                {{&hdl::irata().cpu().status_analyzer().zero(), false}}),
            instruction2);
}

TEST(MicrocodeDslInstructionSetTest,
     GetInstructionByDescriptorWithStatusNotFound) {
  InstructionSet instruction_set;
  const asm_::Instruction &instruction_descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  instruction_set.create_instruction(instruction_descriptor)
      ->with_status(hdl::irata().cpu().status_analyzer().zero(), true);
  EXPECT_THROW(instruction_set.get_instruction(
                   instruction_descriptor,
                   {{&hdl::irata().cpu().status_analyzer().zero(), false}}),
               std::invalid_argument);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByName) {
  InstructionSet instruction_set;
  auto *instruction = instruction_set.create_instruction(
      "lda", asm_::AddressingMode::IMMEDIATE);
  EXPECT_EQ(
      &instruction_set.get_instruction("lda", asm_::AddressingMode::IMMEDIATE),
      instruction);
  EXPECT_EQ(
      &instruction_set.get_instruction("LDA", asm_::AddressingMode::IMMEDIATE),
      instruction);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByNameNotFound) {
  InstructionSet instruction_set;
  EXPECT_THROW(
      instruction_set.get_instruction("lda", asm_::AddressingMode::IMMEDIATE),
      std::invalid_argument);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByNameWithStatus) {
  InstructionSet instruction_set;
  auto *instruction1 =
      instruction_set
          .create_instruction("lda", asm_::AddressingMode::IMMEDIATE)
          ->with_status(hdl::irata().cpu().status_analyzer().zero(), true);
  auto *instruction2 =
      instruction_set
          .create_instruction("lda", asm_::AddressingMode::IMMEDIATE)
          ->with_status(hdl::irata().cpu().status_analyzer().zero(), false);
  EXPECT_EQ(&instruction_set.get_instruction(
                "lda", asm_::AddressingMode::IMMEDIATE,
                {{&hdl::irata().cpu().status_analyzer().zero(), true}}),
            instruction1);
  EXPECT_EQ(&instruction_set.get_instruction(
                "lda", asm_::AddressingMode::IMMEDIATE,
                {{&hdl::irata().cpu().status_analyzer().zero(), false}}),
            instruction2);
}

TEST(MicrocodeDslInstructionSetTest, GetInstructionByNameWithStatusNotFound) {
  InstructionSet instruction_set;
  instruction_set.create_instruction("lda", asm_::AddressingMode::IMMEDIATE)
      ->with_status(hdl::irata().cpu().status_analyzer().zero(), true);
  EXPECT_THROW(instruction_set.get_instruction(
                   "lda", asm_::AddressingMode::IMMEDIATE,
                   {{&hdl::irata().cpu().status_analyzer().zero(), false}}),
               std::invalid_argument);
}

TEST(MicrocodeDslInstructionSetTest, Irata) {
  const auto &instruction_set = InstructionSet::irata();
  EXPECT_NO_THROW(
      instruction_set.get_instruction("lda", asm_::AddressingMode::IMMEDIATE));
}

} // namespace irata::sim::microcode::dsl

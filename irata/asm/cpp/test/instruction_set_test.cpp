#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <stdexcept>

using ::testing::IsEmpty;

namespace irata::asm_ {

TEST(InstructionSetTest, Empty) {
  InstructionSet instruction_set({});
  EXPECT_THAT(instruction_set.instructions(), IsEmpty());
}

TEST(InstructionSetTest, GetInstructionByNameAndAddressingMode) {
  const auto &instruction =
      InstructionSet::irata().get_instruction("LDA", AddressingMode::Immediate);
  EXPECT_EQ(instruction.name(), "LDA");
  EXPECT_EQ(instruction.addressing_mode(), AddressingMode::Immediate);
  EXPECT_EQ(instruction.opcode(), Byte(0xA9));
  EXPECT_EQ(instruction.description(), "Load accumulator from constant");
}

TEST(InstructionSetTest, GetInstructionByOpcode) {
  const auto &instruction = InstructionSet::irata().get_instruction(Byte(0xA9));
  EXPECT_EQ(instruction.name(), "LDA");
  EXPECT_EQ(instruction.addressing_mode(), AddressingMode::Immediate);
  EXPECT_EQ(instruction.opcode(), Byte(0xA9));
  EXPECT_EQ(instruction.description(), "Load accumulator from constant");
}

TEST(InstructionSetTest, GetInstructionByNameAndAddressingModeNotFound) {
  EXPECT_THROW(InstructionSet::irata().get_instruction(
                   "unknown_instruction", AddressingMode::Immediate),
               std::invalid_argument);
}

TEST(InstructionSetTest, GetInstructionByOpcodeNotFound) {
  EXPECT_THROW(InstructionSet::irata().get_instruction(Byte(0xFF)),
               std::invalid_argument);
}

TEST(InstructionSetTest, GetInstructionByNameAndAddressingModeCaseInsensitive) {
  EXPECT_EQ(InstructionSet::irata()
                .get_instruction("lda", AddressingMode::Immediate)
                .name(),
            "LDA");
  EXPECT_EQ(InstructionSet::irata()
                .get_instruction("LDA", AddressingMode::Immediate)
                .name(),
            "LDA");
}

} // namespace irata::asm_

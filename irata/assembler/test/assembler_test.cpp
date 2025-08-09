#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/assembler.hpp>

using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace irata::assembler {

namespace {

class AssemblerTest : public ::testing::Test {
protected:
  Assembler assembler;

  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "hlt", asm_::AddressingMode::NONE);
  const asm_::Instruction &lda_immediate =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  const asm_::Instruction &lda_absolute =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::ABSOLUTE);
};

} // namespace

TEST_F(AssemblerTest, Assemble_Empty) {
  EXPECT_THAT(assembler.assemble(""), IsEmpty());
}

TEST_F(AssemblerTest, Assemble_Comment) {
  EXPECT_THAT(assembler.assemble("; comment"), IsEmpty());
}

TEST_F(AssemblerTest, Assemble_Label) {
  EXPECT_THAT(assembler.assemble("label:"), IsEmpty());
}

TEST_F(AssemblerTest, Assemble_LabelAndComment) {
  EXPECT_THAT(assembler.assemble("label: ; comment"), IsEmpty());
}

TEST_F(AssemblerTest, Assemble_SingleInstruction_None) {
  EXPECT_THAT(assembler.assemble("hlt"), ElementsAre(hlt.opcode()));
}

TEST_F(AssemblerTest, Assemble_SingleInstruction_Immediate) {
  EXPECT_THAT(assembler.assemble("lda #$12"),
              ElementsAre(lda_immediate.opcode(), 0x12));
}

TEST_F(AssemblerTest, Assemble_SingleInstruction_AbsoluteLiteral) {
  EXPECT_THAT(assembler.assemble("lda $1234"),
              ElementsAre(lda_absolute.opcode(), 0x12, 0x34));
}

TEST_F(AssemblerTest, Assemble_SingleInstruction_AbsoluteLabel) {
  // Note 0x8000 offset to account for cartridge memory mapping.
  EXPECT_THAT(assembler.assemble("label: lda label"),
              ElementsAre(lda_absolute.opcode(), 0x80, 0x00));
}

TEST_F(AssemblerTest, Assemble_MultipleInstructions) {
  EXPECT_THAT(assembler.assemble(R"(
        ; this is my program
        hlt ; start with hlt
        label: ; a label that's at 0x8001 because hlt is 1 byte and the cartridge is mapped to 0x8000
        lda #$12 ; load immediate
        another_unused_label_on_this_line: lda $1234 ; load absolute literal
        two_labels: on_one_line: lda label ; load absolute label
        )"),
              ElementsAre(hlt.opcode(), lda_immediate.opcode(), 0x12,
                          lda_absolute.opcode(), 0x12, 0x34,
                          lda_absolute.opcode(), 0x80, 0x01));
}

TEST_F(AssemblerTest, Assemble_InvalidInstruction) {
  EXPECT_THROW(assembler.assemble("invalid"), std::invalid_argument);
}

TEST_F(AssemblerTest, Assemble_DuplicateLabel) {
  EXPECT_THROW(assembler.assemble("label: label:"), std::invalid_argument);
}

TEST_F(AssemblerTest, Assemble_UnknownLabel) {
  EXPECT_THROW(assembler.assemble("lda unknown_label"), std::invalid_argument);
}

TEST_F(AssemblerTest, Assemble_InvalidImmediate) {
  EXPECT_THROW(assembler.assemble("lda #$123"), std::invalid_argument);
}

TEST_F(AssemblerTest, Assemble_InvalidAbsoluteLiteral) {
  EXPECT_THROW(assembler.assemble("lda $12345"), std::invalid_argument);
}

TEST_F(AssemblerTest, AssembleToStream_Empty) {
  std::ostringstream os;
  assembler.assemble("", os);
  EXPECT_EQ(os.str(), "");
}

TEST_F(AssemblerTest, AssembleToStream_MultipleInstructions) {
  std::ostringstream os;
  assembler.assemble(R"(
        lda #$12
        hlt
    )",
                     os);
  const auto expected =
      std::string() + static_cast<char>(lda_immediate.opcode()) +
      static_cast<char>(0x12) + static_cast<char>(hlt.opcode());
  EXPECT_EQ(os.str(), expected);
}

TEST_F(AssemblerTest, AssembleToStream_SingleInstruction_Immediate) {
  std::ostringstream os;
  assembler.assemble("lda #$12", os);
  const auto expected = std::string() +
                        static_cast<char>(lda_immediate.opcode()) +
                        static_cast<char>(0x12);
}

} // namespace irata::assembler

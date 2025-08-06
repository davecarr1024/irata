#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/byte_encoder.hpp>

using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::assembler {

namespace {

class ByteEncoderTest : public ::testing::Test {
protected:
  ByteEncoder encoder;

  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "HLT", asm_::AddressingMode::NONE);
  const asm_::Instruction &lda_immediate =
      asm_::InstructionSet::irata().get_instruction(
          "LDA", asm_::AddressingMode::IMMEDIATE);
  const asm_::Instruction &lda_absolute =
      asm_::InstructionSet::irata().get_instruction(
          "LDA", asm_::AddressingMode::ABSOLUTE);
};

} // namespace

TEST_F(ByteEncoderTest, Encode_Empty) {
  const auto program = LabelBinder::Program({});
  EXPECT_THAT(encoder.encode(program), IsEmpty());
}

TEST_F(ByteEncoderTest, Encode_Instruction_None) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>()));
  const auto program = LabelBinder::Program(std::move(statements));
  EXPECT_THAT(encoder.encode(program),
              UnorderedElementsAre(Pair(0x1234, hlt.opcode())));
}

TEST_F(ByteEncoderTest, Encode_Instruction_Immediate) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_immediate,
      std::make_unique<LabelBinder::Program::Instruction::Immediate>(0x56)));
  const auto program = LabelBinder::Program(std::move(statements));
  EXPECT_THAT(encoder.encode(program),
              UnorderedElementsAre(Pair(0x1234, lda_immediate.opcode()),
                                   Pair(0x1235, 0x56)));
}

TEST_F(ByteEncoderTest, Encode_Instruction_Absolute) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0x5678)));
  const auto program = LabelBinder::Program(std::move(statements));
  EXPECT_THAT(encoder.encode(program),
              UnorderedElementsAre(Pair(0x1234, lda_absolute.opcode()),
                                   Pair(0x1235, 0x56), Pair(0x1236, 0x78)));
}

TEST_F(ByteEncoderTest, Encode_Instruction_DuplicateAddress) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_immediate,
      std::make_unique<LabelBinder::Program::Instruction::Immediate>(0x56)));
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0x5678)));
  const auto program = LabelBinder::Program(std::move(statements));
  EXPECT_THROW(
      {
        try {
          encoder.encode(program);
        } catch (const std::invalid_argument &e) {
          EXPECT_THAT(e.what(), HasSubstr("duplicate address: 0x1234"));
          throw;
        }
      },
      std::invalid_argument);
}

} // namespace irata::assembler

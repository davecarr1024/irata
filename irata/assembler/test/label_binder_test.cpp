#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/label_binder.hpp>
#include <irata/assembler/source_location.hpp>

using ::testing::HasSubstr;

namespace irata::assembler {

namespace {

class LabelBinderTest : public ::testing::Test {
protected:
  static SourceLocation test_loc(size_t line = 1) {
    return SourceLocation("<test>", line);
  }

  LabelBinder binder;

  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "hlt", asm_::AddressingMode::None);
  const asm_::Instruction &lda_immediate =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);
  const asm_::Instruction &lda_absolute =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Absolute);
  const asm_::Instruction &lda_zero_page_x =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::ZeroPageX);
  const asm_::Instruction &lda_absolute_x =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::AbsoluteX);
};

} // namespace

TEST_F(LabelBinderTest, Instruction_None_Properties) {
  const auto arg = LabelBinder::Program::Instruction::None();
  EXPECT_EQ(arg.type(), LabelBinder::Program::Instruction::Arg::Type::None);
}

TEST_F(LabelBinderTest, Instruction_None_Equality) {
  EXPECT_EQ(LabelBinder::Program::Instruction::None(),
            LabelBinder::Program::Instruction::None());
  EXPECT_NE(LabelBinder::Program::Instruction::None(),
            LabelBinder::Program::Instruction::Immediate(0x00));
}

TEST_F(LabelBinderTest, Instruction_Immediate_Properties) {
  const auto arg = LabelBinder::Program::Instruction::Immediate(0x12);
  EXPECT_EQ(arg.type(),
            LabelBinder::Program::Instruction::Arg::Type::Immediate);
  EXPECT_EQ(arg.value(), 0x12);
}

TEST_F(LabelBinderTest, Instruction_Immediate_Equality) {
  EXPECT_EQ(LabelBinder::Program::Instruction::Immediate(0x12),
            LabelBinder::Program::Instruction::Immediate(0x12));
  EXPECT_NE(LabelBinder::Program::Instruction::Immediate(0x12),
            LabelBinder::Program::Instruction::Immediate(0x34));
  EXPECT_NE(LabelBinder::Program::Instruction::Immediate(0x12),
            LabelBinder::Program::Instruction::None());
}

TEST_F(LabelBinderTest, Instruction_Absolute_Properties) {
  const auto arg = LabelBinder::Program::Instruction::Absolute(0x1234);
  EXPECT_EQ(arg.type(), LabelBinder::Program::Instruction::Arg::Type::Absolute);
  EXPECT_EQ(arg.value(), 0x1234);
}

TEST_F(LabelBinderTest, Instruction_Absolute_Equality) {
  EXPECT_EQ(LabelBinder::Program::Instruction::Absolute(0x1234),
            LabelBinder::Program::Instruction::Absolute(0x1234));
  EXPECT_NE(LabelBinder::Program::Instruction::Absolute(0x1234),
            LabelBinder::Program::Instruction::Absolute(0x5678));
  EXPECT_NE(LabelBinder::Program::Instruction::Absolute(0x1234),
            LabelBinder::Program::Instruction::None());
}

TEST_F(LabelBinderTest, Instruction_ZeroPageIndexed_Properties) {
  const auto arg =
      LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12);
  EXPECT_EQ(arg.type(),
            LabelBinder::Program::Instruction::Arg::Type::ZeroPageIndexed);
  EXPECT_EQ(arg.index(), Index::X);
  EXPECT_EQ(arg.value(), 0x12);
}

TEST_F(LabelBinderTest, Instruction_ZeroPageIndexed_Equality) {
  EXPECT_EQ(LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12));
  EXPECT_NE(LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            LabelBinder::Program::Instruction::ZeroPageIndexed(Index::Y, 0x12));
  EXPECT_NE(LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x34));
  EXPECT_NE(LabelBinder::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            LabelBinder::Program::Instruction::None());
}

TEST_F(LabelBinderTest, Instruction_AbsoluteIndexed_Properties) {
  const auto arg =
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234);
  EXPECT_EQ(arg.type(),
            LabelBinder::Program::Instruction::Arg::Type::AbsoluteIndexed);
  EXPECT_EQ(arg.index(), Index::X);
  EXPECT_EQ(arg.value(), 0x1234);
}

TEST_F(LabelBinderTest, Instruction_AbsoluteIndexed_Equality) {
  EXPECT_EQ(
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234));
  EXPECT_NE(
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::Y, 0x1234));
  EXPECT_NE(
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x5678));
  EXPECT_NE(
      LabelBinder::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
      LabelBinder::Program::Instruction::None());
}

TEST_F(LabelBinderTest, Instruction_Properties) {
  const auto instruction = LabelBinder::Program::Instruction(
      0x1234, hlt, std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc());
  EXPECT_EQ(instruction.type(),
            LabelBinder::Program::Statement::Type::Instruction);
  EXPECT_EQ(instruction.address(), 0x1234);
  EXPECT_EQ(instruction.instruction(), hlt);
  EXPECT_EQ(instruction.arg(), LabelBinder::Program::Instruction::None());
  EXPECT_EQ(instruction.source_location(), test_loc());
}

TEST_F(LabelBinderTest, Instruction_Equality) {
  EXPECT_EQ(LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()),
            LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
  EXPECT_NE(LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()),
            LabelBinder::Program::Instruction(
                0x5678, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
  EXPECT_NE(LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()),
            LabelBinder::Program::Instruction(
                0x1234, lda_immediate,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
  EXPECT_NE(LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()),
            LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::Immediate>(
                    0x12), test_loc()));
  EXPECT_NE(LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc(1)),
            LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc(2)));
}

TEST_F(LabelBinderTest, Literal_Properties) {
  const auto literal = LabelBinder::Program::Literal(
      0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc());
  EXPECT_EQ(literal.type(), LabelBinder::Program::Statement::Type::Literal);
  EXPECT_EQ(literal.address(), 0x1234);
  EXPECT_EQ(literal.values(), (std::vector<common::bytes::Byte>{0x12, 0x34}));
  EXPECT_EQ(literal.source_location(), test_loc());
}

TEST_F(LabelBinderTest, Literal_Equality) {
  EXPECT_EQ(LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc()),
            LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc()));
  EXPECT_NE(LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc()),
            LabelBinder::Program::Literal(
                0x5678, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc()));
  EXPECT_NE(LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc()),
            LabelBinder::Program::Literal(0x1234, {}, test_loc()));
  EXPECT_NE(LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc(1)),
            LabelBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}, test_loc(2)));
  EXPECT_NE(LabelBinder::Program::Literal(0x1234, {}, test_loc()),
            LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
}

TEST_F(LabelBinderTest, Label_Properties) {
  const auto label = LabelBinder::Program::Label(0x1234, "my_label", test_loc());
  EXPECT_EQ(label.type(), LabelBinder::Program::Statement::Type::Label);
  EXPECT_EQ(label.address(), 0x1234);
  EXPECT_EQ(label.value(), "my_label");
  EXPECT_EQ(label.source_location(), test_loc());
}

TEST_F(LabelBinderTest, Label_Equality) {
  EXPECT_EQ(LabelBinder::Program::Label(0x1234, "my_label", test_loc()),
            LabelBinder::Program::Label(0x1234, "my_label", test_loc()));
  EXPECT_NE(LabelBinder::Program::Label(0x1234, "my_label", test_loc()),
            LabelBinder::Program::Label(0x5678, "my_label", test_loc()));
  EXPECT_NE(LabelBinder::Program::Label(0x1234, "my_label", test_loc()),
            LabelBinder::Program::Label(0x1234, "other_label", test_loc()));
  EXPECT_NE(LabelBinder::Program::Label(0x1234, "my_label", test_loc(1)),
            LabelBinder::Program::Label(0x1234, "my_label", test_loc(2)));
  EXPECT_NE(LabelBinder::Program::Label(0x1234, "my_label", test_loc()),
            LabelBinder::Program::Instruction(
                0x1234, hlt,
                std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
}

TEST_F(LabelBinderTest, Program_Equality) {
  {
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> lhs;
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> rhs;
    EXPECT_EQ(LabelBinder::Program(std::move(lhs)),
              LabelBinder::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> lhs;
    lhs.push_back(std::make_unique<LabelBinder::Program::Instruction>(
        0x1234, hlt,
        std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> rhs;
    EXPECT_NE(LabelBinder::Program(std::move(lhs)),
              LabelBinder::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> lhs;
    lhs.push_back(std::make_unique<LabelBinder::Program::Instruction>(
        0x1234, hlt,
        std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
    std::vector<std::unique_ptr<LabelBinder::Program::Statement>> rhs;
    rhs.push_back(std::make_unique<LabelBinder::Program::Instruction>(
        0x1234, hlt,
        std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
    EXPECT_EQ(LabelBinder::Program(std::move(lhs)),
              LabelBinder::Program(std::move(rhs)));
  }
}

TEST_F(LabelBinderTest, Bind_Empty) {
  const auto program = binder.bind(InstructionBinder::Program({}));
  EXPECT_EQ(program, LabelBinder::Program({}));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_None) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, hlt,
          std::make_unique<InstructionBinder::Program::Instruction::None>(), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_Immediate) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::Immediate>(
              0x12), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_immediate,
      std::make_unique<LabelBinder::Program::Instruction::Immediate>(0x12), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_AbsoluteLiteral) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, lda_absolute,
          std::make_unique<
              InstructionBinder::Program::Instruction::AbsoluteLiteral>(
              0x5678), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0x5678), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_AbsoluteLabel) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Label>(
      0x5678, "label", test_loc()));
  statements.push_back(std::make_unique<
                       InstructionBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<InstructionBinder::Program::Instruction::AbsoluteLabel>(
          "label"), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  // Label is propagated for debug symbol generation
  expected.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x5678, "label", test_loc()));
  // Note the expected value is offset 0x8000 to account for cartridge memory
  // mapping.
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0xD678), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_AbsoluteLabel_NotFound) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<
                       InstructionBinder::Program::Instruction>(
      0x1234, lda_absolute,
      std::make_unique<InstructionBinder::Program::Instruction::AbsoluteLabel>(
          "unknown_label"), test_loc()));
  EXPECT_THROW(
      {
        try {
          binder.bind(InstructionBinder::Program(std::move(statements)));
        } catch (const std::invalid_argument &e) {
          EXPECT_THAT(e.what(), HasSubstr("label not found: unknown_label"));
          throw;
        }
      },
      std::invalid_argument);
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_ZeroPageIndexed) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, lda_zero_page_x,
          std::make_unique<
              InstructionBinder::Program::Instruction::ZeroPageIndexed>(
              Index::X, 0x12), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_zero_page_x,
      std::make_unique<LabelBinder::Program::Instruction::ZeroPageIndexed>(
          Index::X, 0x12), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SingleInstruction_AbsoluteIndexed) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, lda_absolute_x,
          std::make_unique<
              InstructionBinder::Program::Instruction::AbsoluteIndexed>(
              Index::X, 0x1234), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, lda_absolute_x,
      std::make_unique<LabelBinder::Program::Instruction::AbsoluteIndexed>(
          Index::X, 0x1234), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_DuplicateLabel) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Label>(
      0x5678, "label", test_loc()));
  statements.push_back(std::make_unique<InstructionBinder::Program::Label>(
      0x9ABC, "label", test_loc()));
  EXPECT_THROW(
      {
        try {
          binder.bind(InstructionBinder::Program(std::move(statements)));
        } catch (const std::invalid_argument &e) {
          EXPECT_THAT(e.what(), HasSubstr("duplicate label: label"));
          throw;
        }
      },
      std::invalid_argument);
}

TEST_F(LabelBinderTest, Bind_MultipleInstructions) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Label>(
      0x4321, "label", test_loc()));
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, hlt,
          std::make_unique<InstructionBinder::Program::Instruction::None>(), test_loc()));
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1235, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::Immediate>(
              0x12), test_loc()));
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1236, lda_absolute,
          std::make_unique<
              InstructionBinder::Program::Instruction::AbsoluteLiteral>(
              0x5678), test_loc()));
  statements.push_back(std::make_unique<
                       InstructionBinder::Program::Instruction>(
      0x1237, lda_absolute,
      std::make_unique<InstructionBinder::Program::Instruction::AbsoluteLabel>(
          "label"), test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  // Label is propagated for debug symbol generation
  expected.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4321, "label", test_loc()));
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(), test_loc()));
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1235, lda_immediate,
      std::make_unique<LabelBinder::Program::Instruction::Immediate>(0x12), test_loc()));
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1236, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0x5678), test_loc()));
  // Note 0x8000 offset to account for cartridge memory mapping.
  expected.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1237, lda_absolute,
      std::make_unique<LabelBinder::Program::Instruction::Absolute>(0xC321), test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_Literal) {
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Literal>(
      0x1234, std::vector<common::bytes::Byte>{0xBE, 0xEF}, test_loc()));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> expected;
  expected.push_back(std::make_unique<LabelBinder::Program::Literal>(
      0x1234, std::vector<common::bytes::Byte>{0xBE, 0xEF}, test_loc()));
  EXPECT_EQ(program, LabelBinder::Program(std::move(expected)));
}

TEST_F(LabelBinderTest, Bind_SourceLocationPropagation) {
  // This test verifies that source locations from instruction binder statements
  // are correctly propagated through the label binder
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Label>(
      0x4321, "start", test_loc(20)));
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x1234, hlt,
          std::make_unique<InstructionBinder::Program::Instruction::None>(),
          test_loc(21)));
  statements.push_back(std::make_unique<InstructionBinder::Program::Literal>(
      0x5678, std::vector<common::bytes::Byte>{0xDE, 0xAD}, test_loc(22)));
  const auto program =
      binder.bind(InstructionBinder::Program(std::move(statements)));
  
  // Verify each statement has the correct source location
  // Note: Label is included in output for debug symbol generation
  ASSERT_EQ(program.statements().size(), 3);
  EXPECT_EQ(program.statements()[0]->source_location(), test_loc(20)); // Label
  EXPECT_EQ(program.statements()[1]->source_location(), test_loc(21)); // Instruction
  EXPECT_EQ(program.statements()[2]->source_location(), test_loc(22)); // Literal
}

} // namespace irata::assembler

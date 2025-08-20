#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/instruction_binder.hpp>

namespace irata::assembler {

namespace {

class InstructionBinderTest : public ::testing::Test {
protected:
  InstructionBinder binder;

  const asm_::Instruction &lda_immediate =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);
  const asm_::Instruction &lda_absolute =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Absolute);
  const asm_::Instruction &lda_zero_page_x =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::ZeroPageX);
  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "hlt", asm_::AddressingMode::None);
};

} // namespace

TEST_F(InstructionBinderTest, Label_Properties) {
  const auto label = InstructionBinder::Program::Label(0x1234, "label");
  EXPECT_EQ(label.address(), 0x1234);
  EXPECT_EQ(label.value(), "label");
  EXPECT_EQ(label.type(), InstructionBinder::Program::Statement::Type::Label);
  EXPECT_EQ(label.size(), 0);
}

TEST_F(InstructionBinderTest, Label_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Label(0x1234, "label"),
            InstructionBinder::Program::Label(0x1234, "label"));
  EXPECT_NE(InstructionBinder::Program::Label(0x1234, "label"),
            InstructionBinder::Program::Label(0x1234, "label2"));
  EXPECT_NE(InstructionBinder::Program::Label(0x1234, "label"),
            InstructionBinder::Program::Label(0x1235, "label"));
  EXPECT_NE(
      InstructionBinder::Program::Label(0x1234, "label"),
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()));
}

TEST_F(InstructionBinderTest, Instruction_Arg_None_Properties) {
  const auto arg = InstructionBinder::Program::Instruction::None();
  EXPECT_EQ(arg.type(),
            InstructionBinder::Program::Instruction::Arg::Type::None);
  EXPECT_EQ(arg.size(), 0);
}

TEST_F(InstructionBinderTest, Instruction_Arg_None_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Instruction::None(),
            InstructionBinder::Program::Instruction::None());
  EXPECT_NE(InstructionBinder::Program::Instruction::None(),
            InstructionBinder::Program::Instruction::Immediate(0x12));
}

TEST_F(InstructionBinderTest, Instruction_Arg_Immediate_Properties) {
  const auto arg = InstructionBinder::Program::Instruction::Immediate(0x12);
  EXPECT_EQ(arg.type(),
            InstructionBinder::Program::Instruction::Arg::Type::Immediate);
  EXPECT_EQ(arg.value(), 0x12);
  EXPECT_EQ(arg.size(), 1);
}

TEST_F(InstructionBinderTest, Instruction_Arg_Immediate_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Instruction::Immediate(0x12),
            InstructionBinder::Program::Instruction::Immediate(0x12));
  EXPECT_NE(InstructionBinder::Program::Instruction::Immediate(0x12),
            InstructionBinder::Program::Instruction::Immediate(0x13));
  EXPECT_NE(InstructionBinder::Program::Instruction::Immediate(0x12),
            InstructionBinder::Program::Instruction::None());
}

TEST_F(InstructionBinderTest, Instruction_Arg_AbsoluteLiteral_Properties) {
  const auto arg =
      InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1234);
  EXPECT_EQ(
      arg.type(),
      InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLiteral);
  EXPECT_EQ(arg.value(), 0x1234);
  EXPECT_EQ(arg.size(), 2);
}

TEST_F(InstructionBinderTest, Instruction_Arg_AbsoluteLiteral_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1234),
            InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1234));
  EXPECT_NE(InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1234),
            InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1235));
  EXPECT_NE(InstructionBinder::Program::Instruction::AbsoluteLiteral(0x1234),
            InstructionBinder::Program::Instruction::None());
}

TEST_F(InstructionBinderTest, Instruction_Arg_AbsoluteLabel_Properties) {
  const auto arg =
      InstructionBinder::Program::Instruction::AbsoluteLabel("label");
  EXPECT_EQ(arg.type(),
            InstructionBinder::Program::Instruction::Arg::Type::AbsoluteLabel);
  EXPECT_EQ(arg.value(), "label");
  EXPECT_EQ(arg.size(), 2);
}

TEST_F(InstructionBinderTest, Instruction_Arg_AbsoluteLabel_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Instruction::AbsoluteLabel("label"),
            InstructionBinder::Program::Instruction::AbsoluteLabel("label"));
  EXPECT_NE(InstructionBinder::Program::Instruction::AbsoluteLabel("label"),
            InstructionBinder::Program::Instruction::AbsoluteLabel("label2"));
  EXPECT_NE(InstructionBinder::Program::Instruction::AbsoluteLabel("label"),
            InstructionBinder::Program::Instruction::None());
}

TEST_F(InstructionBinderTest, Instruction_Arg_ZeroPageIndexed_Properties) {
  const auto arg = InstructionBinder::Program::Instruction::ZeroPageIndexed(
      InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X, 0x12);
  EXPECT_EQ(
      arg.type(),
      InstructionBinder::Program::Instruction::Arg::Type::ZeroPageIndexed);
  EXPECT_EQ(arg.index(),
            InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X);
  EXPECT_EQ(arg.value(), 0x12);
  EXPECT_EQ(arg.size(), 1);
}

TEST_F(InstructionBinderTest, Instruction_Arg_ZeroPageIndexed_Equality) {
  EXPECT_EQ(
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12),
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12));
  EXPECT_NE(
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12),
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::Y,
          0x12));
  EXPECT_NE(
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12),
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x34));
  EXPECT_NE(
      InstructionBinder::Program::Instruction::ZeroPageIndexed(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12),
      InstructionBinder::Program::Instruction::None());
}

TEST_F(InstructionBinderTest, Instruction_Properties) {
  const auto instruction = InstructionBinder::Program::Instruction(
      0x1234, lda_immediate,
      std::make_unique<InstructionBinder::Program::Instruction::None>());
  EXPECT_EQ(instruction.address(), 0x1234);
  EXPECT_EQ(instruction.instruction(), lda_immediate);
  EXPECT_EQ(instruction.arg().type(),
            InstructionBinder::Program::Instruction::Arg::Type::None);
  EXPECT_EQ(instruction.size(), 1);
}

TEST_F(InstructionBinderTest, Instruction_Equality) {
  EXPECT_EQ(
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()),
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()));
  EXPECT_NE(
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()),
      InstructionBinder::Program::Instruction(
          0x1235, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()));
  EXPECT_NE(
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()),
      InstructionBinder::Program::Instruction(
          0x1234, hlt,
          std::make_unique<InstructionBinder::Program::Instruction::None>()));
  EXPECT_NE(
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()),
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::Immediate>(
              0x12)));
  EXPECT_NE(
      InstructionBinder::Program::Instruction(
          0x1234, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::None>()),
      InstructionBinder::Program::Label(0x1234, "label"));
}

TEST_F(InstructionBinderTest, Literal_Properties) {
  const auto literal = InstructionBinder::Program::Literal(
      0x1234, std::vector<common::bytes::Byte>{0x12, 0x34});
  EXPECT_EQ(literal.address(), 0x1234);
  EXPECT_EQ(literal.values(), (std::vector<common::bytes::Byte>{0x12, 0x34}));
  EXPECT_EQ(literal.type(),
            InstructionBinder::Program::Statement::Type::Literal);
}

TEST_F(InstructionBinderTest, Literal_Equality) {
  EXPECT_EQ(InstructionBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}),
            InstructionBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}));
  EXPECT_NE(InstructionBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}),
            InstructionBinder::Program::Literal(
                0x1235, std::vector<common::bytes::Byte>{0x12, 0x34}));
  EXPECT_NE(InstructionBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x34}),
            InstructionBinder::Program::Literal(
                0x1234, std::vector<common::bytes::Byte>{0x12, 0x35}));
  EXPECT_NE(InstructionBinder::Program::Literal(0x1234, {}),
            InstructionBinder::Program::Label(0x1234, "label"));
}

TEST_F(InstructionBinderTest, Program_Properties) {
  {
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> lhs;
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> rhs;
    EXPECT_EQ(InstructionBinder::Program(std::move(lhs)),
              InstructionBinder::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> lhs;
    lhs.push_back(
        std::make_unique<InstructionBinder::Program::Label>(0x1234, "label"));
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> rhs;
    EXPECT_NE(InstructionBinder::Program(std::move(lhs)),
              InstructionBinder::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> lhs;
    lhs.push_back(
        std::make_unique<InstructionBinder::Program::Label>(0x1234, "label"));
    std::vector<std::unique_ptr<InstructionBinder::Program::Statement>> rhs;
    rhs.push_back(
        std::make_unique<InstructionBinder::Program::Label>(0x1234, "label"));
    EXPECT_EQ(InstructionBinder::Program(std::move(lhs)),
              InstructionBinder::Program(std::move(rhs)));
  }
}

TEST_F(InstructionBinderTest, Bind_Empty) {
  const auto parser_program = Parser::Program({});
  const auto program = binder.bind(parser_program);
  EXPECT_EQ(program, InstructionBinder::Program({}));
}

TEST_F(InstructionBinderTest, Bind_Label) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(
      std::make_unique<Parser::Program::Label>("label"));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Label>(0x0000, "label"));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_Instruction_None) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "hlt", std::make_unique<Parser::Program::Instruction::None>()));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x0000, hlt,
          std::make_unique<InstructionBinder::Program::Instruction::None>()));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_Instruction_Immediate) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12)));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x0000, lda_immediate,
          std::make_unique<InstructionBinder::Program::Instruction::Immediate>(
              0x12)));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_Instruction_AbsoluteLiteral) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234)));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(
      std::make_unique<InstructionBinder::Program::Instruction>(
          0x0000, lda_absolute,
          std::make_unique<
              InstructionBinder::Program::Instruction::AbsoluteLiteral>(
              0x1234)));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_Instruction_AbsoluteLabel) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLabel>("label")));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<
                       InstructionBinder::Program::Instruction>(
      0x0000, lda_absolute,
      std::make_unique<InstructionBinder::Program::Instruction::AbsoluteLabel>(
          "label")));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_Instruction_ZeroPageIndexed) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::ZeroPageIndexed>(
          Parser::Program::Instruction::ZeroPageIndexed::Index::X, 0x12)));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<
                       InstructionBinder::Program::Instruction>(
      0x0000, lda_zero_page_x,
      std::make_unique<
          InstructionBinder::Program::Instruction::ZeroPageIndexed>(
          InstructionBinder::Program::Instruction::ZeroPageIndexed::Index::X,
          0x12)));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

TEST_F(InstructionBinderTest, Bind_ByteDirective) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> parser_statements;
  parser_statements.push_back(
      std::make_unique<Parser::Program::ByteDirective>(0x12));
  const auto parser_program = Parser::Program(std::move(parser_statements));
  const auto program = binder.bind(parser_program);
  std::vector<std::unique_ptr<InstructionBinder::Program::Statement>>
      statements;
  statements.push_back(std::make_unique<InstructionBinder::Program::Literal>(
      0x0000, std::vector<common::bytes::Byte>{0x12}));
  EXPECT_EQ(program, InstructionBinder::Program(std::move(statements)));
}

} // namespace irata::assembler

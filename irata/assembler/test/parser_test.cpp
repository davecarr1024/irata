#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/assembler/parser.hpp>

namespace irata::assembler {

namespace {

class ParserTest : public ::testing::Test {
protected:
  Parser parser;
};

} // namespace

TEST_F(ParserTest, Comment_Value) {
  EXPECT_EQ(Parser::Program::Comment("hello").value(), "hello");
}

TEST_F(ParserTest, Comment_Equality) {
  EXPECT_EQ(Parser::Program::Comment("hello"),
            Parser::Program::Comment("hello"));
  EXPECT_NE(Parser::Program::Comment("hello"),
            Parser::Program::Comment("world"));
  EXPECT_NE(Parser::Program::Comment("hello"), Parser::Program::Label("hello"));
}

TEST_F(ParserTest, Label_Value) {
  EXPECT_EQ(Parser::Program::Label("hello").value(), "hello");
}

TEST_F(ParserTest, Label_Equality) {
  EXPECT_EQ(Parser::Program::Label("hello"), Parser::Program::Label("hello"));
  EXPECT_NE(Parser::Program::Label("hello"), Parser::Program::Label("world"));
  EXPECT_NE(Parser::Program::Label("hello"), Parser::Program::Comment("hello"));
}

TEST_F(ParserTest, Instruction_NullArg) {
  EXPECT_THROW(Parser::Program::Instruction("lda", nullptr),
               std::runtime_error);
}

TEST_F(ParserTest, None_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::None(),
            Parser::Program::Instruction::None());
  EXPECT_NE(Parser::Program::Instruction::None(),
            Parser::Program::Instruction::Immediate(0x12));
}

TEST_F(ParserTest, Immediate_Value) {
  EXPECT_EQ(Parser::Program::Instruction::Immediate(0x12).value(), 0x12);
}

TEST_F(ParserTest, Immediate_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::Immediate(0x12),
            Parser::Program::Instruction::Immediate(0x12));
  EXPECT_NE(Parser::Program::Instruction::Immediate(0x12),
            Parser::Program::Instruction::Immediate(0x34));
  EXPECT_NE(Parser::Program::Instruction::Immediate(0x12),
            Parser::Program::Instruction::None());
}

TEST_F(ParserTest, AbsoluteLiteral_Value) {
  EXPECT_EQ(Parser::Program::Instruction::AbsoluteLiteral(0x1234).value(),
            0x1234);
}

TEST_F(ParserTest, AbsoluteLiteral_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::AbsoluteLiteral(0x1234),
            Parser::Program::Instruction::AbsoluteLiteral(0x1234));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteLiteral(0x1234),
            Parser::Program::Instruction::AbsoluteLiteral(0x5678));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteLiteral(0x1234),
            Parser::Program::Instruction::None());
}

TEST_F(ParserTest, AbsoluteLabel_Value) {
  EXPECT_EQ(Parser::Program::Instruction::AbsoluteLabel("hello").value(),
            "hello");
}

TEST_F(ParserTest, AbsoluteLabel_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::AbsoluteLabel("hello"),
            Parser::Program::Instruction::AbsoluteLabel("hello"));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteLabel("hello"),
            Parser::Program::Instruction::AbsoluteLabel("world"));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteLabel("hello"),
            Parser::Program::Instruction::None());
}

TEST_F(ParserTest, Program_NullStatement) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(nullptr);
  EXPECT_THROW(Parser::Program(std::move(statements)), std::runtime_error);
}

TEST_F(ParserTest, Program_Equality) {
  EXPECT_EQ(Parser::Program({}), Parser::Program({}));
  {
    std::vector<std::unique_ptr<Parser::Program::Statement>> lhs;
    std::vector<std::unique_ptr<Parser::Program::Statement>> rhs;
    rhs.push_back(std::make_unique<Parser::Program::Comment>("hello"));
    EXPECT_NE(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<Parser::Program::Statement>> lhs;
    lhs.push_back(std::make_unique<Parser::Program::Comment>("hello"));
    std::vector<std::unique_ptr<Parser::Program::Statement>> rhs;
    rhs.push_back(std::make_unique<Parser::Program::Comment>("hello"));
    EXPECT_EQ(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<Parser::Program::Statement>> lhs;
    lhs.push_back(std::make_unique<Parser::Program::Comment>("hello"));
    std::vector<std::unique_ptr<Parser::Program::Statement>> rhs;
    rhs.push_back(std::make_unique<Parser::Program::Label>("hello"));
    EXPECT_NE(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
}

TEST_F(ParserTest, Parse_EmptyInput) {
  EXPECT_EQ(parser.parse(""), Parser::Program({}));
}

TEST_F(ParserTest, Parse_SingleComment) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Comment>("hello"));
  EXPECT_EQ(parser.parse("; hello"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleLabel) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Label>("hello"));
  EXPECT_EQ(parser.parse("hello:"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_NoneArg) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::None>()));
  EXPECT_EQ(parser.parse("lda"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_ImmediateArg_Decimal) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12)));
  EXPECT_EQ(parser.parse("lda #18"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_ImmediateArg_Hex) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12)));
  EXPECT_EQ(parser.parse("lda #$12"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLiteralArg_Decimal) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234)));
  EXPECT_EQ(parser.parse("lda 4660"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLiteralArg_Hex) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234)));
  EXPECT_EQ(parser.parse("lda $1234"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLabelArg) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLabel>("hello")));
  EXPECT_EQ(parser.parse("lda hello"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_MultipleStatements_SeparateLines) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Comment>("a comment"));
  statements.push_back(std::make_unique<Parser::Program::Label>("my_label"));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "nop", std::make_unique<Parser::Program::Instruction::None>()));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12)));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "sta",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234)));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "jmp",
      std::make_unique<Parser::Program::Instruction::AbsoluteLabel>("hello")));
  EXPECT_EQ(parser.parse(R"(
    ; a comment
    my_label:
    nop
    lda #$12
    sta $1234
    jmp hello
    )"),
            Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_MultipleStatements_SameLine) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Label>("my_label"));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "nop", std::make_unique<Parser::Program::Instruction::None>()));
  statements.push_back(std::make_unique<Parser::Program::Comment>("a comment"));
  EXPECT_EQ(parser.parse("my_label: nop ; a comment"),
            Parser::Program(std::move(statements)));
}

} // namespace irata::assembler

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/assembler/parser.hpp>
#include <irata/assembler/source_location.hpp>

namespace irata::assembler {

namespace {

class ParserTest : public ::testing::Test {
protected:
  static SourceLocation test_loc(size_t line = 1) {
    return SourceLocation("<test>", line);
  }

  Parser parser;
};

} // namespace

TEST_F(ParserTest, Comment_Value) {
  EXPECT_EQ(Parser::Program::Comment("hello", test_loc()).value(), "hello");
}

TEST_F(ParserTest, Comment_Equality) {
  EXPECT_EQ(Parser::Program::Comment("hello", test_loc()),
            Parser::Program::Comment("hello", test_loc()));
  EXPECT_NE(Parser::Program::Comment("hello", test_loc()),
            Parser::Program::Comment("world", test_loc()));
  EXPECT_NE(Parser::Program::Comment("hello", test_loc()),
            Parser::Program::Label("hello", test_loc()));
}

TEST_F(ParserTest, Label_Value) {
  EXPECT_EQ(Parser::Program::Label("hello", test_loc()).value(), "hello");
}

TEST_F(ParserTest, Label_Equality) {
  EXPECT_EQ(Parser::Program::Label("hello", test_loc()),
            Parser::Program::Label("hello", test_loc()));
  EXPECT_NE(Parser::Program::Label("hello", test_loc()),
            Parser::Program::Label("world", test_loc()));
  EXPECT_NE(Parser::Program::Label("hello", test_loc()),
            Parser::Program::Comment("hello", test_loc()));
}

TEST_F(ParserTest, Instruction_NullArg) {
  EXPECT_THROW(Parser::Program::Instruction("lda", nullptr, test_loc()),
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

TEST_F(ParserTest, ZeroPageIndexed_Properties) {
  auto arg = Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12);
  EXPECT_EQ(arg.index(), Index::X);
  EXPECT_EQ(arg.value(), 0x12);
  EXPECT_EQ(arg.addressing_mode(), asm_::AddressingMode::ZeroPageX);
  EXPECT_EQ(arg.type(),
            Parser::Program::Instruction::Arg::Type::ZeroPageIndexed);
}

TEST_F(ParserTest, ZeroPageIndexed_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12));
  EXPECT_NE(Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            Parser::Program::Instruction::ZeroPageIndexed(Index::Y, 0x12));
  EXPECT_NE(Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x34));
  EXPECT_NE(Parser::Program::Instruction::ZeroPageIndexed(Index::X, 0x12),
            Parser::Program::Instruction::None());
}

TEST_F(ParserTest, AbsoluteIndexed_Properties) {
  auto arg = Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234);
  EXPECT_EQ(arg.index(), Index::X);
  EXPECT_EQ(arg.value(), 0x1234);
  EXPECT_EQ(arg.addressing_mode(), asm_::AddressingMode::AbsoluteX);
  EXPECT_EQ(arg.type(),
            Parser::Program::Instruction::Arg::Type::AbsoluteIndexed);
}

TEST_F(ParserTest, AbsoluteIndexed_Equality) {
  EXPECT_EQ(Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
            Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
            Parser::Program::Instruction::AbsoluteIndexed(Index::Y, 0x1234));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
            Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x5678));
  EXPECT_NE(Parser::Program::Instruction::AbsoluteIndexed(Index::X, 0x1234),
            Parser::Program::Instruction::None());
}

TEST_F(ParserTest, ByteDirective_Properties) {
  const Parser::Program::ByteDirective byte_directive(0x12, test_loc());
  EXPECT_EQ(byte_directive.value(), 0x12);
  EXPECT_EQ(byte_directive.type(),
            Parser::Program::Statement::Type::ByteDirective);
}

TEST_F(ParserTest, ByteDirective_Equality) {
  EXPECT_EQ(Parser::Program::ByteDirective(0x12, test_loc()),
            Parser::Program::ByteDirective(0x12, test_loc()));
  EXPECT_NE(Parser::Program::ByteDirective(0x12, test_loc()),
            Parser::Program::ByteDirective(0x34, test_loc()));
  EXPECT_NE(Parser::Program::ByteDirective(0x12, test_loc()),
            Parser::Program::Comment("hello", test_loc()));
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
    rhs.push_back(
        std::make_unique<Parser::Program::Comment>("hello", test_loc(1)));
    EXPECT_NE(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<Parser::Program::Statement>> lhs;
    lhs.push_back(
        std::make_unique<Parser::Program::Comment>("hello", test_loc(1)));
    std::vector<std::unique_ptr<Parser::Program::Statement>> rhs;
    rhs.push_back(
        std::make_unique<Parser::Program::Comment>("hello", test_loc(1)));
    EXPECT_EQ(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
  {
    std::vector<std::unique_ptr<Parser::Program::Statement>> lhs;
    lhs.push_back(
        std::make_unique<Parser::Program::Comment>("hello", test_loc(1)));
    std::vector<std::unique_ptr<Parser::Program::Statement>> rhs;
    rhs.push_back(
        std::make_unique<Parser::Program::Label>("hello", test_loc(1)));
    EXPECT_NE(Parser::Program(std::move(lhs)), Parser::Program(std::move(rhs)));
  }
}

TEST_F(ParserTest, Parse_EmptyInput) {
  EXPECT_EQ(parser.parse(""), Parser::Program({}));
}

TEST_F(ParserTest, Parse_SingleComment) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(
      std::make_unique<Parser::Program::Comment>("hello", test_loc(1)));
  EXPECT_EQ(parser.parse("; hello"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleLabel) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(
      std::make_unique<Parser::Program::Label>("hello", test_loc(1)));
  EXPECT_EQ(parser.parse("hello:"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_NoneArg) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::None>(),
      test_loc()));
  EXPECT_EQ(parser.parse("lda"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_ImmediateArg_Decimal) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12),
      test_loc()));
  EXPECT_EQ(parser.parse("lda #18"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_ImmediateArg_Hex) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12),
      test_loc()));
  EXPECT_EQ(parser.parse("lda #$12"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLiteralArg_Decimal) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234),
      test_loc()));
  EXPECT_EQ(parser.parse("lda 4660"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLiteralArg_Hex) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234),
      test_loc()));
  EXPECT_EQ(parser.parse("lda $1234"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteLabelArg) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda",
      std::make_unique<Parser::Program::Instruction::AbsoluteLabel>("hello"),
      test_loc()));
  EXPECT_EQ(parser.parse("lda hello"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_SingleInstruction_ZeroPageIndexedArg) {
  for (const auto &[index_str, index] :
       std::vector<std::pair<std::string, Index>>{
           {"x", Index::X},
           {"y", Index::Y},
       }) {
    for (const std::string &expr_base : {
             "$12,",
             "18,",
             "0x12,",
             // Word-formatted bytes: these are expected to work since the
             // choice between zp,x and abs,x is based on the value of the byte,
             // not the format.
             "$0012,",
             "0x0012,",
         }) {
      const auto expr = expr_base + index_str;
      std::vector<std::unique_ptr<Parser::Program::Statement>> expected;
      expected.push_back(std::make_unique<Parser::Program::Instruction>(
          "lda",
          std::make_unique<Parser::Program::Instruction::ZeroPageIndexed>(index,
                                                                          0x12),
          test_loc()));
      EXPECT_EQ(parser.parse("lda " + std::string(expr)),
                Parser::Program(std::move(expected)));
    }
  }
}

TEST_F(ParserTest, Parse_SingleInstruction_AbsoluteIndexedArg) {
  for (const auto &[index_str, index] :
       std::vector<std::pair<std::string, Index>>{
           {"x", Index::X},
           {"y", Index::Y},
       }) {
    for (const std::string &expr_base : {
             "$1234,",
             "4660,",
             "0x1234,",
         }) {
      const auto expr = expr_base + index_str;
      std::vector<std::unique_ptr<Parser::Program::Statement>> expected;
      expected.push_back(std::make_unique<Parser::Program::Instruction>(
          "lda",
          std::make_unique<Parser::Program::Instruction::AbsoluteIndexed>(
              index, 0x1234),
          test_loc()));
      EXPECT_EQ(parser.parse("lda " + std::string(expr)),
                Parser::Program(std::move(expected)));
    }
  }
}

TEST_F(ParserTest, Parse_MultipleStatements_SeparateLines) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(
      std::make_unique<Parser::Program::Comment>("a comment", test_loc(1)));
  statements.push_back(
      std::make_unique<Parser::Program::Label>("my_label", test_loc(1)));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "nop", std::make_unique<Parser::Program::Instruction::None>(),
      test_loc()));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "lda", std::make_unique<Parser::Program::Instruction::Immediate>(0x12),
      test_loc()));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "sta",
      std::make_unique<Parser::Program::Instruction::AbsoluteLiteral>(0x1234),
      test_loc()));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "jmp",
      std::make_unique<Parser::Program::Instruction::AbsoluteLabel>("hello"),
      test_loc()));
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
  statements.push_back(
      std::make_unique<Parser::Program::Label>("my_label", test_loc(1)));
  statements.push_back(std::make_unique<Parser::Program::Instruction>(
      "nop", std::make_unique<Parser::Program::Instruction::None>(),
      test_loc()));
  statements.push_back(
      std::make_unique<Parser::Program::Comment>("a comment", test_loc(1)));
  EXPECT_EQ(parser.parse("my_label: nop ; a comment"),
            Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_InvalidInstruction_InvalidNumericValue) {
  EXPECT_THROW(parser.parse("lda #$1234"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_InvalidInstruction_InvalidNumericFormat) {
  EXPECT_THROW(parser.parse("lda #1x"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_InvalidInstruction_TooManyTokens) {
  EXPECT_THROW(parser.parse("lda #12 #13"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_InvalidInstruction_EmptyNumericLiteral) {
  EXPECT_THROW(parser.parse("lda #"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Empty) {
  EXPECT_THROW(parser.parse("."), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Unknown) {
  EXPECT_THROW(parser.parse(".unknown"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Byte_Empty) {
  EXPECT_THROW(parser.parse(".byte"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Byte_TooManyTokens) {
  EXPECT_THROW(parser.parse(".byte 1 2"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Byte_InvalidNumericFormat) {
  EXPECT_THROW(parser.parse(".byte 1x"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Byte_InvalidNumericValue) {
  EXPECT_THROW(parser.parse(".byte $1234"), std::invalid_argument);
}

TEST_F(ParserTest, Parse_Directive_Byte_Decimal) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(
      std::make_unique<Parser::Program::ByteDirective>(0x12, test_loc(1)));
  EXPECT_EQ(parser.parse(".byte 18"), Parser::Program(std::move(statements)));
}

TEST_F(ParserTest, Parse_Directive_Byte_Hex) {
  std::vector<std::unique_ptr<Parser::Program::Statement>> statements;
  statements.push_back(
      std::make_unique<Parser::Program::ByteDirective>(0x12, test_loc(1)));
  EXPECT_EQ(parser.parse(".byte $12"), Parser::Program(std::move(statements)));
}

} // namespace irata::assembler

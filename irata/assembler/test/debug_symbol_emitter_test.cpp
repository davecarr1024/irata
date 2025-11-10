#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/debug_symbol_emitter.hpp>
#include <irata/assembler/source_location.hpp>

using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace irata::assembler {

namespace {

SourceLocation test_loc(int line = 1) {
  return SourceLocation("<test>", line);
}

class DebugSymbolEmitterTest : public ::testing::Test {
protected:
  DebugSymbolEmitter emitter;

  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "HLT", asm_::AddressingMode::None);
  const asm_::Instruction &lda_immediate =
      asm_::InstructionSet::irata().get_instruction(
          "LDA", asm_::AddressingMode::Immediate);
};

} // namespace

TEST_F(DebugSymbolEmitterTest, Emit_Empty) {
  const auto program = LabelBinder::Program({});
  EXPECT_THAT(emitter.emit(program), IsEmpty());
}

TEST_F(DebugSymbolEmitterTest, Emit_NoLabels) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(),
      test_loc(10)));
  statements.push_back(std::make_unique<LabelBinder::Program::Literal>(
      0x1235, std::vector<common::bytes::Byte>{0xDE, 0xAD}, test_loc(11)));
  const auto program = LabelBinder::Program(std::move(statements));
  EXPECT_THAT(emitter.emit(program), IsEmpty());
}

TEST_F(DebugSymbolEmitterTest, Emit_SingleLabel) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4000, "start", test_loc(5)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.size(), 1);
  EXPECT_EQ(symbols[0].label, "start");
  EXPECT_EQ(symbols[0].address, 0x4000);
  EXPECT_EQ(symbols[0].source_location, test_loc(5));
}

TEST_F(DebugSymbolEmitterTest, Emit_MultipleLabels) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4000, "start", test_loc(5)));
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x4000, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(),
      test_loc(6)));
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4001, "loop", test_loc(10)));
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x4001, lda_immediate,
      std::make_unique<LabelBinder::Program::Instruction::Immediate>(0x42),
      test_loc(11)));
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4003, "end", test_loc(15)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.size(), 3);
  
  EXPECT_EQ(symbols[0].label, "start");
  EXPECT_EQ(symbols[0].address, 0x4000);
  EXPECT_EQ(symbols[0].source_location, test_loc(5));
  
  EXPECT_EQ(symbols[1].label, "loop");
  EXPECT_EQ(symbols[1].address, 0x4001);
  EXPECT_EQ(symbols[1].source_location, test_loc(10));
  
  EXPECT_EQ(symbols[2].label, "end");
  EXPECT_EQ(symbols[2].address, 0x4003);
  EXPECT_EQ(symbols[2].source_location, test_loc(15));
}

TEST_F(DebugSymbolEmitterTest, DebugSymbol_Equality) {
  const DebugSymbol sym1{"label", 0x1000, test_loc(5)};
  const DebugSymbol sym2{"label", 0x1000, test_loc(5)};
  const DebugSymbol sym3{"other", 0x1000, test_loc(5)};
  const DebugSymbol sym4{"label", 0x2000, test_loc(5)};
  const DebugSymbol sym5{"label", 0x1000, test_loc(10)};
  
  EXPECT_EQ(sym1, sym2);
  EXPECT_NE(sym1, sym3);
  EXPECT_NE(sym1, sym4);
  EXPECT_NE(sym1, sym5);
}

TEST_F(DebugSymbolEmitterTest, Emit_MixedStatements) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Literal>(
      0x3FFF, std::vector<common::bytes::Byte>{0xCA, 0xFE}, test_loc(1)));
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4001, "main", test_loc(3)));
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x4001, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(),
      test_loc(4)));
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4002, "data", test_loc(8)));
  statements.push_back(std::make_unique<LabelBinder::Program::Literal>(
      0x4002, std::vector<common::bytes::Byte>{0xBE, 0xEF}, test_loc(9)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.size(), 2);
  EXPECT_EQ(symbols[0].label, "main");
  EXPECT_EQ(symbols[0].address, 0x4001);
  EXPECT_EQ(symbols[1].label, "data");
  EXPECT_EQ(symbols[1].address, 0x4002);
}

} // namespace irata::assembler

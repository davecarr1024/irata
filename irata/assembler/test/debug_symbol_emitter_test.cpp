#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/assembler/debug_symbol_emitter.hpp>
#include <irata/assembler/source_location.hpp>

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
  const auto symbols = emitter.emit(program);
  
  EXPECT_THAT(symbols.source_lines, IsEmpty());
  EXPECT_THAT(symbols.labels, IsEmpty());
}

TEST_F(DebugSymbolEmitterTest, Emit_InstructionsOnly) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x1234, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(),
      test_loc(10)));
  statements.push_back(std::make_unique<LabelBinder::Program::Literal>(
      0x1235, std::vector<common::bytes::Byte>{0xDE, 0xAD}, test_loc(11)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.source_lines.size(), 2);
  EXPECT_EQ(symbols.source_lines[0].address, 0x1234);
  EXPECT_EQ(symbols.source_lines[0].source_location, test_loc(10));
  EXPECT_EQ(symbols.source_lines[1].address, 0x1235);
  EXPECT_EQ(symbols.source_lines[1].source_location, test_loc(11));
  
  EXPECT_THAT(symbols.labels, IsEmpty());
}

TEST_F(DebugSymbolEmitterTest, Emit_SingleLabel) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4000, "start", test_loc(5)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.source_lines.size(), 1);
  EXPECT_EQ(symbols.source_lines[0].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[0].source_location, test_loc(5));
  
  ASSERT_EQ(symbols.labels.size(), 1);
  EXPECT_EQ(symbols.labels[0].label, "start");
  EXPECT_EQ(symbols.labels[0].address, 0x4000);
  EXPECT_EQ(symbols.labels[0].source_location, test_loc(5));
}

TEST_F(DebugSymbolEmitterTest, Emit_MixedStatementsWithMultipleLabels) {
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
  
  ASSERT_EQ(symbols.source_lines.size(), 5);
  EXPECT_EQ(symbols.source_lines[0].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[0].source_location, test_loc(5));
  EXPECT_EQ(symbols.source_lines[1].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[1].source_location, test_loc(6));
  EXPECT_EQ(symbols.source_lines[2].address, 0x4001);
  EXPECT_EQ(symbols.source_lines[2].source_location, test_loc(10));
  EXPECT_EQ(symbols.source_lines[3].address, 0x4001);
  EXPECT_EQ(symbols.source_lines[3].source_location, test_loc(11));
  EXPECT_EQ(symbols.source_lines[4].address, 0x4003);
  EXPECT_EQ(symbols.source_lines[4].source_location, test_loc(15));
  
  ASSERT_EQ(symbols.labels.size(), 3);
  EXPECT_EQ(symbols.labels[0].label, "start");
  EXPECT_EQ(symbols.labels[0].address, 0x4000);
  EXPECT_EQ(symbols.labels[0].source_location, test_loc(5));
  EXPECT_EQ(symbols.labels[1].label, "loop");
  EXPECT_EQ(symbols.labels[1].address, 0x4001);
  EXPECT_EQ(symbols.labels[1].source_location, test_loc(10));
  EXPECT_EQ(symbols.labels[2].label, "end");
  EXPECT_EQ(symbols.labels[2].address, 0x4003);
  EXPECT_EQ(symbols.labels[2].source_location, test_loc(15));
}

TEST_F(DebugSymbolEmitterTest, SourceLineSymbol_Equality) {
  const SourceLineSymbol sym1{0x1000, test_loc(5)};
  const SourceLineSymbol sym2{0x1000, test_loc(5)};
  const SourceLineSymbol sym3{0x2000, test_loc(5)};
  const SourceLineSymbol sym4{0x1000, test_loc(10)};
  
  EXPECT_EQ(sym1, sym2);
  EXPECT_NE(sym1, sym3);
  EXPECT_NE(sym1, sym4);
}

TEST_F(DebugSymbolEmitterTest, LabelSymbol_Equality) {
  const LabelSymbol sym1{"label", 0x1000, test_loc(5)};
  const LabelSymbol sym2{"label", 0x1000, test_loc(5)};
  const LabelSymbol sym3{"other", 0x1000, test_loc(5)};
  const LabelSymbol sym4{"label", 0x2000, test_loc(5)};
  const LabelSymbol sym5{"label", 0x1000, test_loc(10)};
  
  EXPECT_EQ(sym1, sym2);
  EXPECT_NE(sym1, sym3);
  EXPECT_NE(sym1, sym4);
  EXPECT_NE(sym1, sym5);
}

TEST_F(DebugSymbolEmitterTest, DebugSymbols_Equality) {
  DebugSymbols syms1;
  syms1.source_lines.push_back({0x1000, test_loc(1)});
  syms1.labels.push_back({"main", 0x1000, test_loc(1)});
  
  DebugSymbols syms2;
  syms2.source_lines.push_back({0x1000, test_loc(1)});
  syms2.labels.push_back({"main", 0x1000, test_loc(1)});
  
  DebugSymbols syms3;
  syms3.source_lines.push_back({0x2000, test_loc(1)});
  syms3.labels.push_back({"main", 0x1000, test_loc(1)});
  
  EXPECT_EQ(syms1, syms2);
  EXPECT_NE(syms1, syms3);
}

TEST_F(DebugSymbolEmitterTest, Emit_ConsecutiveLabelsAtSameAddress) {
  std::vector<std::unique_ptr<LabelBinder::Program::Statement>> statements;
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4000, "start", test_loc(1)));
  statements.push_back(std::make_unique<LabelBinder::Program::Label>(
      0x4000, "main", test_loc(2)));
  statements.push_back(std::make_unique<LabelBinder::Program::Instruction>(
      0x4000, hlt,
      std::make_unique<LabelBinder::Program::Instruction::None>(),
      test_loc(3)));
  const auto program = LabelBinder::Program(std::move(statements));
  const auto symbols = emitter.emit(program);
  
  ASSERT_EQ(symbols.source_lines.size(), 3);
  EXPECT_EQ(symbols.source_lines[0].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[0].source_location, test_loc(1));
  EXPECT_EQ(symbols.source_lines[1].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[1].source_location, test_loc(2));
  EXPECT_EQ(symbols.source_lines[2].address, 0x4000);
  EXPECT_EQ(symbols.source_lines[2].source_location, test_loc(3));
  
  ASSERT_EQ(symbols.labels.size(), 2);
  EXPECT_EQ(symbols.labels[0].label, "start");
  EXPECT_EQ(symbols.labels[0].address, 0x4000);
  EXPECT_EQ(symbols.labels[1].label, "main");
  EXPECT_EQ(symbols.labels[1].address, 0x4000);
}

} // namespace irata::assembler

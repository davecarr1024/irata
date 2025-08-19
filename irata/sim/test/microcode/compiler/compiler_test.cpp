#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>
#include <stdexcept>

using ::testing::AllOf;
using ::testing::Contains;
using ::testing::Each;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Property;

namespace irata::sim::microcode::compiler {

namespace {

std::vector<table::Entry>
table_entries_for_instruction(const asm_::Instruction &instruction) {
  std::vector<table::Entry> entries;
  for (const auto &entry : Compiler::compile_irata().entries) {
    if (entry.instruction == instruction) {
      entries.push_back(entry);
    }
  }
  return entries;
}

template <typename Matcher> auto EntryHasStatuses(Matcher matcher) {
  return Field("statuses", &table::Entry::statuses, matcher);
}

template <typename Matcher> auto EntryHasControls(Matcher matcher) {
  return Field("controls", &table::Entry::controls, matcher);
}

template <typename Matcher> auto EntryHasInstruction(Matcher matcher) {
  return Field("instruction", &table::Entry::instruction, matcher);
}

} // namespace

TEST(MicrocodeCompilerTest, NullPasses) {
  std::vector<std::unique_ptr<passes::Pass>> passes;
  passes.push_back(nullptr);
  EXPECT_THROW(Compiler(std::move(passes)), std::invalid_argument);
}

TEST(MicrocodeCompilerTest, LdaImm) {
  const auto table = Compiler::compile_irata();
  const auto &descriptor = asm_::InstructionSet::irata().get_instruction(
      "lda", asm_::AddressingMode::Immediate);
  auto entries = table_entries_for_instruction(descriptor);
  const auto &increment_step_counter =
      hdl::irata().cpu().controller().step_counter().increment();
  const auto &reset_step_counter =
      hdl::irata().cpu().controller().step_counter().reset();
  EXPECT_THAT(entries, Each(AllOf(EntryHasStatuses(IsEmpty()),
                                  EntryHasInstruction(descriptor))));
  for (size_t step_index = 0; step_index < entries.size(); ++step_index) {
    EXPECT_EQ(entries[step_index].step_index, Byte(step_index));
    EXPECT_THAT(entries[step_index].controls,
                Contains(step_index == entries.size() - 1
                             ? &reset_step_counter
                             : &increment_step_counter));
  }
}

TEST(MicrocodeCompilerTest, CmpHasOpcode1) {
  const auto table = Compiler::compile_irata();
  const auto &descriptor = asm_::InstructionSet::irata().get_instruction(
      "cmp", asm_::AddressingMode::Immediate);
  EXPECT_THAT(table.entries,
              Contains(AllOf(EntryHasInstruction(descriptor),
                             EntryHasControls(Contains(
                                 Property("path", &hdl::ComponentDecl::path,
                                          "/cpu/alu/opcode_1"))))));
  for (const auto &entry : table_entries_for_instruction(descriptor)) {
    std::cout << entry << std::endl;
  }
}

} // namespace irata::sim::microcode::compiler

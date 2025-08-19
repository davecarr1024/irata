#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>

using ::testing::Contains;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::compiler::ir {

namespace {

template <typename Matcher> auto InstructionHasDescriptor(Matcher matcher) {
  return Property("descriptor", &Instruction::descriptor, matcher);
}

} // namespace

TEST(MicrocodeIrInstructionSetTest, Empty) {
  InstructionSet instruction_set(std::set<Instruction>{});
  EXPECT_THAT(instruction_set.instructions(), IsEmpty());
}

TEST(MicrocodeIrInstructionSetTest, ConstructFromDsl) {
  InstructionSet instruction_set(dsl::InstructionSet::irata());
  EXPECT_EQ(instruction_set.instructions().size(),
            dsl::InstructionSet::irata().instructions().size());
  for (const auto &dsl_instruction :
       dsl::InstructionSet::irata().instructions()) {
    EXPECT_THAT(
        instruction_set.instructions(),
        Contains(InstructionHasDescriptor(dsl_instruction->descriptor())));
  }
}

TEST(MicrocodeIrInstructionSetTest, ConstructDirectly) {
  const asm_::Instruction &descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);
  InstructionSet instruction_set({Instruction(descriptor, {}, {})});
  EXPECT_THAT(instruction_set.instructions(),
              UnorderedElementsAre(InstructionHasDescriptor(descriptor)));
}

} // namespace irata::sim::microcode::compiler::ir

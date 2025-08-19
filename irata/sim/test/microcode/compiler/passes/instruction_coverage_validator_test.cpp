#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/microcode/compiler/passes/instruction_coverage_validator.hpp>

namespace irata::sim::microcode::compiler::passes {

TEST(InstructionCoverageValidatorTest, InstructionFound) {
  const asm_::Instruction instruction("instruction", Byte(0x12),
                                      asm_::AddressingMode::Immediate, "");
  InstructionCoverageValidator validator(asm_::InstructionSet({instruction}));
  const ir::Instruction ir_instruction(instruction, {}, {});
  EXPECT_NO_THROW(validator.run(ir::InstructionSet({ir_instruction})));
}

TEST(InstructionCoverageValidatorTest, InstructionNotFound) {
  const asm_::Instruction instruction("instruction", Byte(0x12),
                                      asm_::AddressingMode::Immediate, "");
  InstructionCoverageValidator validator(asm_::InstructionSet({instruction}));
  EXPECT_THROW(validator.run(ir::InstructionSet(std::set<ir::Instruction>{})),
               std::invalid_argument);
}

TEST(InstructionCoverageValidatorTest, Irata) {
  EXPECT_NO_THROW(InstructionCoverageValidator::irata()->run(
      ir::InstructionSet(dsl::InstructionSet::irata())));
}

} // namespace irata::sim::microcode::compiler::passes

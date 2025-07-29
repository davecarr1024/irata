#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/status_completeness_validator.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <stdexcept>

namespace irata::sim::microcode::compiler::passes {

namespace {

class StatusCompletenessValidatorTest : public ::testing::Test {
protected:
  void validate(const ir::InstructionSet &instruction_set) {
    validator_.run(instruction_set);
  }

  void validate(const dsl::InstructionSet &instruction_set) {
    validate(ir::InstructionSet(instruction_set));
  }

  void validate() { validate(instruction_set_); }

  dsl::InstructionSet instruction_set_;
  const asm_::Instruction &instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);

  StatusCompletenessValidator validator_;
  const hdl::StatusDecl status = {"status", hdl::irata()};
};

} // namespace

TEST_F(StatusCompletenessValidatorTest, EmptyInstructionSet) {
  EXPECT_NO_THROW(validate());
}

TEST_F(StatusCompletenessValidatorTest, NoStatuses) {
  instruction_set_.create_instruction(instruction_descriptor_);
  EXPECT_NO_THROW(validate());
}

TEST_F(StatusCompletenessValidatorTest, SingleStatus) {
  instruction_set_.create_instruction(instruction_descriptor_)
      ->with_status(status, true);
  EXPECT_THROW(validate(), std::invalid_argument);
}

TEST_F(StatusCompletenessValidatorTest, CompleteStatuses) {
  instruction_set_.create_instruction(instruction_descriptor_)
      ->with_status(status, true);
  instruction_set_.create_instruction(instruction_descriptor_)
      ->with_status(status, false);
  EXPECT_THAT(instruction_set_.instructions(),
              ::testing::UnorderedElementsAre(
                  ::testing::Pointee(::testing::Property(
                      "statuses", &dsl::Instruction::statuses,
                      ::testing::UnorderedElementsAre(
                          ::testing::Pair(&status, false)))),
                  ::testing::Pointee(::testing::Property(
                      "statuses", &dsl::Instruction::statuses,
                      ::testing::UnorderedElementsAre(
                          ::testing::Pair(&status, true))))));
  EXPECT_NO_THROW(validate());
}

} // namespace irata::sim::microcode::compiler::passes

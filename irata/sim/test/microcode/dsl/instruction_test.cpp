#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::dsl {

namespace {

class InstructionTest : public ::testing::Test {
protected:
  template <typename ControlMatcher>
  static auto StepHasControls(ControlMatcher matcher) {
    return Property("controls", &Step::controls, matcher);
  }

  template <typename StepMatcher>
  static auto InstructionHasSteps(StepMatcher matcher) {
    return Property("steps", &Instruction::steps, matcher);
  }

  template <typename StatusMatcher>
  static auto InstructionHasStatuses(StatusMatcher matcher) {
    return Property("statuses", &Instruction::statuses, matcher);
  }

  template <typename InstructionDescriptorMatcher>
  static auto InstructionHasDescriptor(InstructionDescriptorMatcher matcher) {
    return Property("descriptor", &Instruction::descriptor, matcher);
  }

  Instruction *create_instruction(const asm_::Instruction &instruction) {
    return instruction_set_.create_instruction(instruction);
  }

  Instruction *lda_imm() { return create_instruction(lda_imm_); }

  InstructionSet instruction_set_;

  const asm_::Instruction &lda_imm_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
};

} // namespace

TEST_F(InstructionTest, Empty) {
  auto *instruction = lda_imm();
  EXPECT_EQ(instruction->instruction_set(), &instruction_set_);
  EXPECT_THAT(instruction, AllOf(InstructionHasSteps(IsEmpty()),
                                 InstructionHasStatuses(IsEmpty()),
                                 InstructionHasDescriptor(lda_imm_)));
}

TEST_F(InstructionTest, CreateStep) {
  auto *instruction = lda_imm();
  const auto &control1 = hdl::irata().cpu().a().read();
  const auto &control2 = hdl::irata().cpu().a().write();
  instruction->create_step()->with_control(control1);
  instruction->create_step()->with_control(control2);
  EXPECT_THAT(
      instruction,
      AllOf(InstructionHasSteps(ElementsAre(
                Pointee(StepHasControls(UnorderedElementsAre(&control1))),
                Pointee(StepHasControls(UnorderedElementsAre(&control2))))),
            InstructionHasStatuses(IsEmpty()),
            InstructionHasDescriptor(lda_imm_)));
}

TEST_F(InstructionTest, WithStatus) {
  auto *instruction = lda_imm();
  const hdl::ComponentDecl root("root", nullptr);
  const hdl::StatusDecl status("status", root);
  instruction->with_status(status, true);
  EXPECT_THAT(instruction, AllOf(InstructionHasSteps(IsEmpty()),
                                 InstructionHasStatuses(
                                     UnorderedElementsAre(Pair(&status, true))),
                                 InstructionHasDescriptor(lda_imm_)));
}

TEST_F(InstructionTest, CreateInstruction) {
  auto *instruction1 = lda_imm();
  auto *instruction2 = instruction1->create_instruction(lda_imm_);
  EXPECT_EQ(instruction2->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction2->descriptor(), lda_imm_);
  EXPECT_THAT(instruction_set_.instructions(),
              ElementsAre(Pointee(InstructionHasDescriptor(lda_imm_)),
                          Pointee(InstructionHasDescriptor(lda_imm_))));
}

} // namespace irata::sim::microcode::dsl

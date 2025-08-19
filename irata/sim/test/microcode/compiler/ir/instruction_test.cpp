#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>

using ::testing::AllOf;
using ::testing::Property;

namespace irata::sim::microcode::compiler::ir {

namespace {

class MicrocodeIrInstructionTest : public ::testing::Test {
protected:
  const dsl::Instruction &dsl_instruction_ =
      dsl::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);

  template <typename Matcher> auto StepHasControls(Matcher matcher) {
    return Property("controls", &Step::controls, matcher);
  }

  template <typename Matcher> auto StepHasWriteControls(Matcher matcher) {
    return Property("write_controls", &Step::write_controls, matcher);
  }

  template <typename Matcher> auto StepHasReadControls(Matcher matcher) {
    return Property("read_controls", &Step::read_controls, matcher);
  }

  const hdl::StatusDecl status = hdl::StatusDecl("status", hdl::irata());
};

} // namespace

TEST_F(MicrocodeIrInstructionTest, ConstructFromDsl) {
  Instruction ir_instruction(dsl_instruction_);
  EXPECT_EQ(ir_instruction.descriptor(), dsl_instruction_.descriptor());
  EXPECT_EQ(ir_instruction.statuses(), dsl_instruction_.statuses());
  ASSERT_EQ(ir_instruction.steps().size(), dsl_instruction_.steps().size());
  for (size_t i = 0; i < ir_instruction.steps().size(); ++i) {
    EXPECT_THAT(
        ir_instruction.steps()[i],
        AllOf(
            StepHasControls(dsl_instruction_.steps()[i]->controls()),
            StepHasWriteControls(dsl_instruction_.steps()[i]->write_controls()),
            StepHasReadControls(dsl_instruction_.steps()[i]->read_controls())));
  }
}

TEST_F(MicrocodeIrInstructionTest, ConstructDirectly) {
  const asm_::Instruction &descriptor =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);
  const std::vector<Step> steps = {Step(
      {
          &hdl::irata().cpu().pc().increment(),
      },
      {}, {}, 0)};
  const std::map<const hdl::StatusDecl *, bool> statuses = {{&status, true}};
  Instruction ir_instruction(descriptor, steps, statuses);
  EXPECT_EQ(ir_instruction.descriptor(), descriptor);
  EXPECT_EQ(ir_instruction.steps(), steps);
  EXPECT_EQ(ir_instruction.statuses(), statuses);
}

} // namespace irata::sim::microcode::compiler::ir

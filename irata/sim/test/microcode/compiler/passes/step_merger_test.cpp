#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/compiler/passes/step_merger.hpp>

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::compiler::passes {

namespace {

class StepMergerTest : public ::testing::Test {
protected:
  StepMerger transformer_;
  const hdl::WriteControlDecl &write_control_ = hdl::irata().cpu().a().write();
  const hdl::ReadControlDecl &read_control_ = hdl::irata().cpu().a().read();
  const hdl::ProcessControlDecl &process_control_ =
      hdl::irata().cpu().pc().increment();
  const asm_::Instruction &instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);

  ir::InstructionSet transform(const ir::InstructionSet &instruction_set) {
    return transformer_.run(instruction_set);
  }

  ir::Instruction instruction_with_controls(
      std::vector<std::set<const hdl::ControlDecl *>> controls) {
    std::vector<ir::Step> steps;
    for (const auto &control_set : controls) {
      std::set<const hdl::WriteControlDecl *> write_controls;
      std::set<const hdl::ReadControlDecl *> read_controls;
      for (const auto &control : control_set) {
        if (const auto *write_control =
                dynamic_cast<const hdl::WriteControlDecl *>(control)) {
          write_controls.insert(write_control);
        }
        if (const auto *read_control =
                dynamic_cast<const hdl::ReadControlDecl *>(control)) {
          read_controls.insert(read_control);
        }
      }
      steps.push_back(ir::Step(control_set, write_controls, read_controls));
    }
    return ir::Instruction(instruction_descriptor_, steps, {});
  }

  template <typename Matcher>
  auto InstructionSetHasInstructions(Matcher matcher) {
    return Property("instructions", &ir::InstructionSet::instructions, matcher);
  }

  template <typename Matcher> auto InstructionHasSteps(Matcher matcher) {
    return Property("steps", &ir::Instruction::steps, matcher);
  }

  template <typename Matcher> auto InstructionHasDescriptor(Matcher matcher) {
    return Property("descriptor", &ir::Instruction::descriptor, matcher);
  }

  template <typename Matcher> auto StepHasControls(Matcher matcher) {
    return Property("controls", &ir::Step::controls, matcher);
  }

  template <typename Matcher> auto StepHasWriteControls(Matcher matcher) {
    return Property("write_controls", &ir::Step::write_controls, matcher);
  }

  template <typename Matcher> auto StepHasReadControls(Matcher matcher) {
    return Property("read_controls", &ir::Step::read_controls, matcher);
  }
};

} // namespace

TEST_F(StepMergerTest, EmptyInstruction) {
  const auto instruction_set =
      ir::InstructionSet({instruction_with_controls({})});
  EXPECT_THAT(transform(instruction_set),
              InstructionSetHasInstructions(UnorderedElementsAre(
                  AllOf(InstructionHasSteps(IsEmpty()),
                        InstructionHasDescriptor(instruction_descriptor_)))));
}

TEST_F(StepMergerTest, SingleStep) {
  const auto instruction_set = ir::InstructionSet({instruction_with_controls(
      {{&write_control_, &read_control_, &process_control_}})});
  EXPECT_THAT(
      transform(instruction_set),
      InstructionSetHasInstructions(
          UnorderedElementsAre(AllOf(InstructionHasSteps(ElementsAre(AllOf(
              StepHasControls(UnorderedElementsAre(
                  &write_control_, &read_control_, &process_control_)),
              StepHasWriteControls(UnorderedElementsAre(&write_control_)),
              StepHasReadControls(UnorderedElementsAre(&read_control_)))))))));
}

TEST_F(StepMergerTest, MergableSteps) {
  const auto instruction_set = ir::InstructionSet({instruction_with_controls(
      {{&write_control_, &read_control_}, {&process_control_}})});
  EXPECT_THAT(
      transform(instruction_set),
      InstructionSetHasInstructions(
          UnorderedElementsAre(AllOf(InstructionHasSteps(ElementsAre(AllOf(
              StepHasControls(UnorderedElementsAre(
                  &write_control_, &read_control_, &process_control_)),
              StepHasWriteControls(UnorderedElementsAre(&write_control_)),
              StepHasReadControls(UnorderedElementsAre(&read_control_)))))))));
}

TEST_F(StepMergerTest, NonMergableSteps) {
  const auto instruction_set = ir::InstructionSet({instruction_with_controls(
      {{&process_control_}, {&write_control_, &read_control_}})});
  EXPECT_THAT(
      transform(instruction_set),
      InstructionSetHasInstructions(
          UnorderedElementsAre(AllOf(InstructionHasSteps(ElementsAre(
              AllOf(StepHasControls(UnorderedElementsAre(&process_control_)),
                    StepHasWriteControls(IsEmpty()),
                    StepHasReadControls(IsEmpty())),
              AllOf(StepHasControls(
                        UnorderedElementsAre(&write_control_, &read_control_)),
                    StepHasWriteControls(UnorderedElementsAre(&write_control_)),
                    StepHasReadControls(
                        UnorderedElementsAre(&read_control_)))))))));
}

} // namespace irata::sim::microcode::compiler::passes

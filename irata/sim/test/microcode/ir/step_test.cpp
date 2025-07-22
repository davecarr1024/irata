#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/microcode/ir/instruction.hpp>
#include <irata/sim/microcode/ir/instruction_set.hpp>
#include <irata/sim/microcode/ir/step.hpp>
#include <stdexcept>

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::ir {

namespace {

class StepTest : public ::testing::Test {
protected:
  Step *create_step() { return instruction_->create_step(); }

  template <typename ControlMatcher>
  static auto StepHasControls(ControlMatcher matcher) {
    return Property("controls", &Step::controls, matcher);
  }

  template <typename BusMatcher> static auto StepHasBuses(BusMatcher matcher) {
    return Property("buses", &Step::buses, matcher);
  }

  template <typename ControlMatcher, typename BusMatcher>
  static auto StepHasControlsAndBuses(ControlMatcher control_matcher,
                                      BusMatcher bus_matcher) {
    return AllOf(StepHasControls(control_matcher), StepHasBuses(bus_matcher));
  }

  InstructionSet instruction_set_;
  const asm_::Instruction instruction_descriptor_{};
  Instruction *instruction_ =
      instruction_set_.create_instruction(instruction_descriptor_);
};

} // namespace

TEST_F(StepTest, Empty) {
  auto *step = create_step();
  EXPECT_EQ(step->instruction_set(), &instruction_set_);
  EXPECT_EQ(step->instruction(), instruction_);
  EXPECT_THAT(step, StepHasControlsAndBuses(IsEmpty(), IsEmpty()));
}

TEST_F(StepTest, NullInstruction) {
  EXPECT_THROW(Step(nullptr), std::invalid_argument);
}

TEST_F(StepTest, WithControl) {
  auto *step = create_step();
  const auto &control = hdl::irata().cpu().a().read();
  EXPECT_THAT(
      step->with_control(control),
      StepHasControlsAndBuses(UnorderedElementsAre(&control), IsEmpty()));
}

TEST_F(StepTest, WithBus) {
  auto *step = create_step();
  const auto &bus = hdl::irata().data_bus();
  EXPECT_THAT(step->with_bus(bus),
              StepHasControlsAndBuses(IsEmpty(), UnorderedElementsAre(&bus)));
}

TEST_F(StepTest, CreateInstruction) {
  auto *step = create_step();
  const asm_::Instruction instruction_descriptor{};
  auto *instruction = step->create_instruction(instruction_descriptor);
  EXPECT_EQ(instruction->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction->instruction(), instruction_descriptor);
  EXPECT_THAT(instruction, Property("steps", &Instruction::steps, IsEmpty()));
}

TEST_F(StepTest, CreateStep) {
  const auto &control1 = hdl::irata().cpu().a().read();
  const auto &control2 = hdl::irata().cpu().a().write();
  auto *step1 = create_step()->with_control(control1);
  auto *step2 = step1->create_step()->with_control(control2);
  EXPECT_EQ(step2->instruction(), instruction_);
  EXPECT_THAT(
      instruction_->steps(),
      ElementsAre(Pointee(StepHasControls(UnorderedElementsAre(&control1))),
                  Pointee(StepHasControls(UnorderedElementsAre(&control2)))));
}

} // namespace irata::sim::microcode::ir

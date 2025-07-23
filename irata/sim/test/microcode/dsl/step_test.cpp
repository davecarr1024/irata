#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/hdl/hdl.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::dsl {

namespace {

class StepTest : public ::testing::Test {
protected:
  Step *create_step() { return instruction_->create_step(); }

  InstructionSet instruction_set_;
  const asm_::Instruction instruction_descriptor_{};
  Instruction *instruction_ =
      instruction_set_.create_instruction(instruction_descriptor_);
  const hdl::ComponentDecl hw_root_ = hdl::ComponentDecl("root", nullptr);
  const hdl::ByteBusDecl bus_ = hdl::ByteBusDecl("bus", hw_root_);
};

} // namespace

TEST_F(StepTest, Empty) {
  auto *step = create_step();
  EXPECT_EQ(step->instruction_set(), &instruction_set_);
  EXPECT_EQ(step->instruction(), instruction_);
  EXPECT_THAT(step->controls(), IsEmpty());
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), IsEmpty());
}

TEST_F(StepTest, NullInstruction) {
  EXPECT_THROW(Step(nullptr), std::invalid_argument);
}

TEST_F(StepTest, WithControl) {
  auto *step = create_step();
  const hdl::ProcessControlDecl control("control", hw_root_);
  step = step->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), IsEmpty());
}

TEST_F(StepTest, WithWriteControl) {
  auto *step = create_step();
  const hdl::WriteControlDecl control("control", hw_root_, bus_);
  step = step->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->read_controls(), IsEmpty());
}

TEST_F(StepTest, WithReadControl) {
  auto *step = create_step();
  const hdl::ReadControlDecl control("control", hw_root_, bus_);
  step = step->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), UnorderedElementsAre(&control));
}

TEST_F(StepTest, WithMultipleControls) {
  auto *step = create_step();
  const hdl::ProcessControlDecl process_control("process_control", hw_root_);
  const hdl::WriteControlDecl write_control("write_control", hw_root_, bus_);
  const hdl::ReadControlDecl read_control("read_control", hw_root_, bus_);
  step = step->with_control(process_control)
             ->with_control(write_control)
             ->with_control(read_control);
  EXPECT_THAT(
      step->controls(),
      UnorderedElementsAre(&process_control, &write_control, &read_control));
  EXPECT_THAT(step->write_controls(), UnorderedElementsAre(&write_control));
  EXPECT_THAT(step->read_controls(), UnorderedElementsAre(&read_control));
}

TEST_F(StepTest, DuplicateControls) {
  auto *step = create_step();
  const hdl::ProcessControlDecl control("control", hw_root_);
  step = step->with_control(control)->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
}

TEST_F(StepTest, CreateInstruction) {
  auto *step = create_step();
  const asm_::Instruction instruction_descriptor{};
  auto *instruction = step->create_instruction(instruction_descriptor);
  EXPECT_EQ(instruction->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction->instruction(), instruction_descriptor);
  EXPECT_THAT(instruction->steps(), IsEmpty());
}

TEST_F(StepTest, CreateStep) {
  const auto &control1 = hdl::irata().cpu().a().read();
  const auto &control2 = hdl::irata().cpu().a().write();
  auto *step1 = create_step()->with_control(control1);
  auto *step2 = step1->create_step()->with_control(control2);
  EXPECT_EQ(step2->instruction(), instruction_);
  EXPECT_THAT(instruction_->steps(),
              ElementsAre(Pointee(Property("controls", &Step::controls,
                                           UnorderedElementsAre(&control1))),
                          Pointee(Property("controls", &Step::controls,
                                           UnorderedElementsAre(&control2)))));
}

} // namespace irata::sim::microcode::dsl

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

using ::testing::Contains;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

namespace irata::sim::microcode::dsl {

namespace {

class MicrocodeDslStepTest : public ::testing::Test {
protected:
  InstructionSet instruction_set_;
  const asm_::Instruction instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::IMMEDIATE);
  Instruction *instruction_ =
      instruction_set_.create_instruction(instruction_descriptor_);

  const hdl::TypedComponentDecl<hdl::ComponentType::Irata> different_tree_root =
      {"different_tree_root"};
  const hdl::ProcessControlDecl different_tree_process_control = {
      "different_tree_process_control", different_tree_root};
  const hdl::ByteBusDecl different_tree_bus = {"different_tree_bus",
                                               different_tree_root};
  const hdl::WriteControlDecl different_tree_write_control = {
      "different_tree_write_control", different_tree_root, different_tree_bus};
  const hdl::ReadControlDecl different_tree_read_control = {
      "different_tree_read_control", different_tree_root, different_tree_bus};
};

} // namespace

TEST_F(MicrocodeDslStepTest, Empty) {
  auto *step = instruction_->create_step();
  EXPECT_THAT(step->controls(), IsEmpty());
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), IsEmpty());
  EXPECT_EQ(step->instruction(), instruction_);
  EXPECT_EQ(step->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_->steps().back().get(), step);
}

TEST_F(MicrocodeDslStepTest, WithControl) {
  const hdl::ProcessControlDecl &control = hdl::irata().cpu().pc().increment();
  auto *step = instruction_->create_step()->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), IsEmpty());
}

TEST_F(MicrocodeDslStepTest, WithWriteControl) {
  const hdl::WriteControlDecl &control = hdl::irata().cpu().a().write();
  auto *step = instruction_->create_step()->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->read_controls(), IsEmpty());
}

TEST_F(MicrocodeDslStepTest, WithReadControl) {
  const hdl::ReadControlDecl &control = hdl::irata().cpu().a().read();
  auto *step = instruction_->create_step()->with_control(control);
  EXPECT_THAT(step->controls(), UnorderedElementsAre(&control));
  EXPECT_THAT(step->write_controls(), IsEmpty());
  EXPECT_THAT(step->read_controls(), UnorderedElementsAre(&control));
}

TEST_F(MicrocodeDslStepTest, CreateStep) {
  auto *step1 = instruction_->create_step();
  auto *step2 = step1->create_step();
  EXPECT_EQ(step2->instruction(), instruction_);
  EXPECT_EQ(step2->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_->steps().back().get(), step2);
}

TEST_F(MicrocodeDslStepTest, CreateInstruction) {
  auto *step = instruction_->create_step();
  auto *instruction2 = step->create_instruction(instruction_descriptor_);
  EXPECT_EQ(instruction2->descriptor(), instruction_descriptor_);
  EXPECT_EQ(instruction2->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_set_.instructions().back().get(), instruction2);
}

TEST_F(MicrocodeDslStepTest, NullInstruction) {
  EXPECT_THROW(Step(nullptr), std::invalid_argument);
}

TEST_F(MicrocodeDslStepTest, WithControlDifferentTree) {
  const hdl::ProcessControlDecl &same_tree_control =
      hdl::irata().cpu().pc().increment();
  auto *step = instruction_->create_step()->with_control(same_tree_control);
  EXPECT_THROW(step->with_control(different_tree_process_control),
               std::invalid_argument);
}

TEST_F(MicrocodeDslStepTest, WithWriteControlDifferentTree) {
  const hdl::ProcessControlDecl &same_tree_control =
      hdl::irata().cpu().pc().increment();
  auto *step = instruction_->create_step()->with_control(same_tree_control);
  EXPECT_THROW(step->with_control(different_tree_write_control),
               std::invalid_argument);
}

TEST_F(MicrocodeDslStepTest, WithReadControlDifferentTree) {
  const hdl::ProcessControlDecl &same_tree_control =
      hdl::irata().cpu().pc().increment();
  auto *step = instruction_->create_step()->with_control(same_tree_control);
  EXPECT_THROW(step->with_control(different_tree_read_control),
               std::invalid_argument);
}

} // namespace irata::sim::microcode::dsl

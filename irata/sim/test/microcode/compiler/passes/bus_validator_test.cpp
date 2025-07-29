#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/bus_validator.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

namespace irata::sim::microcode::compiler::passes {

namespace {

class BusValidatorTest : public ::testing::Test {
protected:
  void validate(const ir::InstructionSet &instruction_set) {
    validator_.run(instruction_set);
  }

  void validate(const dsl::InstructionSet &instruction_set) {
    validate(ir::InstructionSet(instruction_set));
  }

  void validate() { validate(instruction_set_); }

  BusValidator validator_;
  dsl::InstructionSet instruction_set_;
  dsl::Instruction *instruction_ = instruction_set_.create_instruction(
      "lda", asm_::AddressingMode::IMMEDIATE);
  dsl::Step *step_ = instruction_->create_step();

  const hdl::WriteControlDecl &write_control_1_ =
      hdl::irata().cpu().a().write();
  const hdl::WriteControlDecl &write_control_2_ =
      hdl::irata().cpu().x().write();
  const hdl::ReadControlDecl &read_control_1_ = hdl::irata().cpu().a().read();
  const hdl::ReadControlDecl &read_control_2_ = hdl::irata().cpu().x().read();

  const hdl::WriteControlDecl &different_bus_write_control_ =
      hdl::irata().cpu().pc().write();
  const hdl::ReadControlDecl &different_bus_read_control_ =
      hdl::irata().cpu().pc().read();
};

} // namespace

TEST_F(BusValidatorTest, ValidateIrata) {
  BusValidator validator;
  ir::InstructionSet instruction_set(dsl::InstructionSet::irata());
  EXPECT_NO_THROW(validator.run(instruction_set));
}

TEST_F(BusValidatorTest, ValidateEmpty) { validate(); }

TEST_F(BusValidatorTest, ValidateOneWriteOneRead) {
  step_ = step_->with_control(write_control_1_)->with_control(read_control_1_);
  validate();
}

TEST_F(BusValidatorTest, ValidateOneWriteTwoRead) {
  step_ = step_->with_control(write_control_1_)
              ->with_control(read_control_1_)
              ->with_control(read_control_2_);
  validate();
}

TEST_F(BusValidatorTest, ValidateTwoWriteOneRead) {
  step_ = step_->with_control(write_control_1_)
              ->with_control(write_control_2_)
              ->with_control(read_control_1_);
  EXPECT_THROW(validate(), std::invalid_argument);
}

TEST_F(BusValidatorTest, ValidateOneWriteNoRead) {
  step_ = step_->with_control(write_control_1_);
  EXPECT_THROW(validate(), std::invalid_argument);
}

TEST_F(BusValidatorTest, ValidateNoWriteOneRead) {
  step_ = step_->with_control(read_control_1_);
  EXPECT_THROW(validate(), std::invalid_argument);
}

TEST_F(BusValidatorTest, ValidateDifferentBus) {
  step_ = step_->with_control(different_bus_write_control_)
              ->with_control(different_bus_read_control_)
              ->with_control(write_control_1_)
              ->with_control(read_control_1_);
  validate();
}

} // namespace irata::sim::microcode::compiler::passes

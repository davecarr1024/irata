#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/instruction_memory.hpp>
#include <stdexcept>

using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class InstructionMemoryTest : public ::testing::Test {
protected:
  const asm_::Instruction instruction1 = asm_::Instruction(
      "instruction1", Byte(0x10), asm_::AddressingMode::IMMEDIATE,
      "instruction1 is a sequence instruction with two steps: the first step "
      "asserts control1 and the second step asserts control2.");
  const asm_::Instruction instruction2 = asm_::Instruction(
      "instruction2", Byte(0x20), asm_::AddressingMode::IMMEDIATE,
      "instruction2 is a branching instruction with two variants: the first "
      "variant is used when status1 is true and asserts control1, the second "
      "variant is used when status1 is false and asserts control2.");
  const hdl::StatusDecl status1 = hdl::StatusDecl("status1", hdl::irata());
  const hdl::ProcessControlDecl control1 =
      hdl::ProcessControlDecl("control1", hdl::irata());
  const hdl::ProcessControlDecl control2 =
      hdl::ProcessControlDecl("control2", hdl::irata());
  const microcode::table::Table table = {
      {
          {
              .instruction = instruction1,
              .step_index = Byte(0x00),
              .controls = {&control1},
          },
          {
              .instruction = instruction1,
              .step_index = Byte(0x01),
              .controls = {&control2},
          },
          {
              .instruction = instruction2,
              .step_index = Byte(0x00),
              .statuses = {{&status1, true}},
              .controls = {&control1},
          },
          {
              .instruction = instruction2,
              .step_index = Byte(0x00),
              .statuses = {{&status1, false}},
              .controls = {&control2},
          },
      },
  };
  const InstructionMemory instruction_memory = InstructionMemory(table);

  PartialStatuses partial(std::map<const hdl::StatusDecl *, bool> statuses) {
    return PartialStatuses(statuses);
  }

  CompleteStatuses complete(std::map<const hdl::StatusDecl *, bool> statuses) {
    return CompleteStatuses(instruction_memory.encoder().status_encoder(),
                            statuses);
  }
};

} // namespace

TEST_F(InstructionMemoryTest, ReadByOpcode) {
  // instruction1 is non-branching, so to look up its values we need to
  // permute all statuses.
  for (const auto &complete_statuses :
       instruction_memory.encoder().status_encoder().permute(partial({}))) {
    // step index 0x00 asserts control1
    EXPECT_THAT(instruction_memory.read(0x10, complete_statuses, 0x00),
                UnorderedElementsAre(&control1));
    // step index 0x01 asserts control2
    EXPECT_THAT(instruction_memory.read(0x10, complete_statuses, 0x01),
                UnorderedElementsAre(&control2));
  }
  // instruction2 is branching, so we can look up its values directly.
  // status1=true step index 0x00 asserts control1
  EXPECT_THAT(instruction_memory.read(0x20, complete({{&status1, true}}), 0x00),
              UnorderedElementsAre(&control1));
  // status1=false step index 0x00 asserts control2
  EXPECT_THAT(
      instruction_memory.read(0x20, complete({{&status1, false}}), 0x00),
      UnorderedElementsAre(&control2));
}

TEST_F(InstructionMemoryTest, ReadByInstruction) {
  // instruction1 is non-branching, so to look up its values we need to
  // permute all statuses.
  for (const auto &complete_statuses :
       instruction_memory.encoder().status_encoder().permute(partial({}))) {
    // step index 0x00 asserts control1
    EXPECT_THAT(instruction_memory.read(instruction1, complete_statuses, 0x00),
                UnorderedElementsAre(&control1));
    // step index 0x01 asserts control2
    EXPECT_THAT(instruction_memory.read(instruction1, complete_statuses, 0x01),
                UnorderedElementsAre(&control2));
  }
  // instruction2 is branching, so we can look up its values directly.
  // status1=true step index 0x00 asserts control1
  EXPECT_THAT(
      instruction_memory.read(instruction2, complete({{&status1, true}}), 0x00),
      UnorderedElementsAre(&control1));
  // status1=false step index 0x00 asserts control2
  EXPECT_THAT(instruction_memory.read(instruction2,
                                      complete({{&status1, false}}), 0x00),
              UnorderedElementsAre(&control2));
}

} // namespace irata::sim::components::controller

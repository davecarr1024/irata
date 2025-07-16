#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/instruction_memory.hpp>
#include <stdexcept>

using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {
static const auto
    instruction_set = microcode::InstructionSet{.instructions = {microcode::
                                                                     Instruction{
                                                                         .instruction =
                                                                             asm_::Instruction{
                                                                                 .opcode = 10},
                                                                         .programs =
                                                                             {
                                                                                 microcode::
                                                                                     Program{
                                                                                         .statements =
                                                                                             {microcode::Statement{.controls = {"C1", "C2"}}, microcode::
                                                                                                                                                  Statement{.controls = {"C3"}}},
                                                                                         .statuses =
                                                                                             {
                                                                                                 {"P1",
                                                                                                  true},
                                                                                             },

                                                                                     },
                                                                                 microcode::
                                                                                     Program{
                                                                                         .statements =
                                                                                             {microcode::
                                                                                                  Statement{
                                                                                                      .controls = {"C4"}},
                                                                                              microcode::
                                                                                                  Statement{.controls = {"C5"}}},
                                                                                         .statuses =
                                                                                             {
                                                                                                 {"P1",
                                                                                                  false},
                                                                                             },

                                                                                     },
                                                                             },
                                                                     }}};

}

TEST(InstructionMemoryTest, statuses) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THAT(memory.statuses(), UnorderedElementsAre("P1"));
}

TEST(InstructionMemoryTest, controls) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THAT(memory.controls(),
              UnorderedElementsAre("C1", "C2", "C3", "C4", "C5"));
}

TEST(InstructionMemoryTest, get_controls_missing_status) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THROW(memory.get_controls(10, 0, {}), std::invalid_argument);
}

TEST(InstructionMemoryTest, get_controls_unknown_status) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THROW(memory.get_controls(10, 0, {{"P2", true}}),
               std::invalid_argument);
}

TEST(InstructionMemoryTest, get_controls_unknown_address) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THROW(memory.get_controls(11, 0, {{"P1", true}}),
               std::invalid_argument);
}

TEST(InstructionMemoryTest, get_controls) {
  const auto memory = InstructionMemory(instruction_set);
  EXPECT_THAT(memory.get_controls(10, 0, {{"P1", true}}),
              UnorderedElementsAre("C1", "C2"));
  EXPECT_THAT(memory.get_controls(10, 1, {{"P1", true}}),
              UnorderedElementsAre("C3"));
  EXPECT_THAT(memory.get_controls(10, 0, {{"P1", false}}),
              UnorderedElementsAre("C4"));
  EXPECT_THAT(memory.get_controls(10, 1, {{"P1", false}}),
              UnorderedElementsAre("C5"));
}

} // namespace irata::sim::components::controller
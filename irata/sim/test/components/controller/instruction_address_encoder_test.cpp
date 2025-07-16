#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/instruction_address_encoder.hpp>

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

TEST(InstructionAddressEncoderTest, max_opcode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_EQ(encoder.max_opcode(), 10);
}

TEST(InstructionAddressEncoderTest, opcode_bit_count) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_EQ(encoder.opcode_bit_count(), 4);
}

TEST(InstructionAddressEncoderTest, max_step_index) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_EQ(encoder.max_step_index(), 1);
}

TEST(InstructionAddressEncoderTest, program_length_bit_count) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_EQ(encoder.step_index_bit_count(), 1);
}

TEST(InstructionAddressEncoderTest, status_bit_count) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_EQ(encoder.status_bit_count(), 1);
}

TEST(InstructionAddressEncoderTest, address_bit_count) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  // opcode: 4 bits
  // step index: 1 bit
  // status: 1 bit
  EXPECT_EQ(encoder.address_bit_count(), 6);
}

TEST(InstructionAddressEncoderTest, encode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  // format: opcode (4 bits) | step index (1 bit) | status (1 bit)
  EXPECT_EQ(encoder.encode(10, 0, {{"P1", true}}), 0b101001);
  EXPECT_EQ(encoder.encode(10, 0, {{"P1", false}}), 0b101000);
  EXPECT_EQ(encoder.encode(10, 1, {{"P1", true}}), 0b101011);
  EXPECT_EQ(encoder.encode(9, 0, {{"P1", true}}), 0b100101);
}

TEST(InstructionAddressEncoderTest, decode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  // format: opcode (4 bits) | step index (1 bit) | status (1 bit)
  EXPECT_EQ(encoder.decode(0b101001),
            std::make_tuple(10, 0, std::map<std::string, bool>{{"P1", true}}));
  EXPECT_EQ(encoder.decode(0b101000),
            std::make_tuple(10, 0, std::map<std::string, bool>{{"P1", false}}));
  EXPECT_EQ(encoder.decode(0b101011),
            std::make_tuple(10, 1, std::map<std::string, bool>{{"P1", true}}));
  EXPECT_EQ(encoder.decode(0b100101),
            std::make_tuple(9, 0, std::map<std::string, bool>{{"P1", true}}));
}

TEST(InstructionAddressEncoderTest, RoundTripEncodeDecode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  for (uint8_t step = 0; step <= encoder.max_step_index(); ++step) {
    for (bool status : {false, true}) {
      auto encoded = encoder.encode(10, step, {{"P1", status}});
      auto decoded = encoder.decode(encoded);
      EXPECT_EQ(decoded,
                std::make_tuple(10, step,
                                std::map<std::string, bool>{{"P1", status}}));
    }
  }
}

TEST(InstructionAddressEncoderTest, EncodeInvalidOpcode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_THROW(encoder.encode(11, 0, {{"P1", true}}), std::invalid_argument);
}

TEST(InstructionAddressEncoderTest, EncodeInvalidStepIndex) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_THROW(encoder.encode(10, 2, {{"P1", true}}), std::invalid_argument);
}

TEST(InstructionAddressEncoderTest, PermuteStatusesAndEncode) {
  const auto encoder = InstructionAddressEncoder(instruction_set);
  EXPECT_THAT(encoder.permute_statuses_and_encode(10, 0, {{"P1", true}}),
              UnorderedElementsAre(0b101001));
  EXPECT_THAT(encoder.permute_statuses_and_encode(10, 0, {{"P1", false}}),
              UnorderedElementsAre(0b101000));
  EXPECT_THAT(encoder.permute_statuses_and_encode(10, 0, {}),
              UnorderedElementsAre(0b101001, 0b101000));
}

} // namespace irata::sim::components::controller

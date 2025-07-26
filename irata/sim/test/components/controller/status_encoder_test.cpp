#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/status_encoder.hpp>

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class StatusEncoderTest : public ::testing::Test {
protected:
  const hdl::StatusDecl status1 = hdl::StatusDecl("status1", hdl::irata());
  const hdl::StatusDecl status2 = hdl::StatusDecl("status2", hdl::irata());
  const asm_::Instruction instruction = asm_::Instruction(
      "instruction", Byte(0x01), asm_::AddressingMode::IMMEDIATE, "");
  const microcode::table::Table table = {
      .entries = {
          {
              .instruction = instruction,
              .step_index = Byte(0x01),
              .statuses = {{&status1, false}, {&status2, true}},
              .controls = {},
          },
      }};
  const StatusEncoder encoder = StatusEncoder(table);
};

} // namespace

TEST_F(StatusEncoderTest, TooManyStatuses) {
  std::vector<std::unique_ptr<hdl::StatusDecl>> statuses;
  std::set<microcode::table::Entry> entries;
  for (size_t i = 0; i < 9; ++i) {
    auto status = std::make_unique<hdl::StatusDecl>(
        "status" + std::to_string(i), hdl::irata());
    entries.insert({
        .instruction = instruction,
        .step_index = Byte(i),
        .statuses = {{status.get(), false}},
        .controls = {},
    });
    statuses.push_back(std::move(status));
  }
  const microcode::table::Table table = {.entries = std::move(entries)};
  EXPECT_THROW((StatusEncoder(table)), std::invalid_argument);
}

TEST_F(StatusEncoderTest, NumStatuses) { EXPECT_EQ(encoder.num_statuses(), 2); }

TEST_F(StatusEncoderTest, Encode) {
  EXPECT_EQ(encoder.encode({{&status1, false}, {&status2, false}}), 0b00);
  EXPECT_EQ(encoder.encode({{&status1, false}, {&status2, true}}), 0b10);
  EXPECT_EQ(encoder.encode({{&status1, true}, {&status2, false}}), 0b01);
  EXPECT_EQ(encoder.encode({{&status1, true}, {&status2, true}}), 0b11);
}

TEST_F(StatusEncoderTest, EncodeMissingStatus) {
  EXPECT_THROW(encoder.encode({}), std::invalid_argument);
  EXPECT_THROW(encoder.encode({{&status1, false}}), std::invalid_argument);
  EXPECT_THROW(encoder.encode({{&status2, false}}), std::invalid_argument);
}

TEST_F(StatusEncoderTest, EncodeUnknownStatus) {
  const hdl::StatusDecl unknown_status("unknown_status", hdl::irata());
  EXPECT_THROW(encoder.encode({{&unknown_status, false}}),
               std::invalid_argument);
}

TEST_F(StatusEncoderTest, Decode) {
  EXPECT_THAT(encoder.decode(0b00),
              ElementsAre(Pair(&status1, false), Pair(&status2, false)));
  EXPECT_THAT(encoder.decode(0b10),
              ElementsAre(Pair(&status1, false), Pair(&status2, true)));
  EXPECT_THAT(encoder.decode(0b01),
              ElementsAre(Pair(&status1, true), Pair(&status2, false)));
  EXPECT_THAT(encoder.decode(0b11),
              ElementsAre(Pair(&status1, true), Pair(&status2, true)));
}

TEST_F(StatusEncoderTest, PermuteStatuses) {
  EXPECT_THAT(encoder.permute_statuses({}),
              UnorderedElementsAre(
                  ElementsAre(Pair(&status1, false), Pair(&status2, false)),
                  ElementsAre(Pair(&status1, false), Pair(&status2, true)),
                  ElementsAre(Pair(&status1, true), Pair(&status2, false)),
                  ElementsAre(Pair(&status1, true), Pair(&status2, true))));
  EXPECT_THAT(encoder.permute_statuses({{&status1, false}}),
              UnorderedElementsAre(
                  ElementsAre(Pair(&status1, false), Pair(&status2, false)),
                  ElementsAre(Pair(&status1, false), Pair(&status2, true))));
  EXPECT_THAT(encoder.permute_statuses({{&status2, false}}),
              UnorderedElementsAre(
                  ElementsAre(Pair(&status1, false), Pair(&status2, false)),
                  ElementsAre(Pair(&status1, true), Pair(&status2, false))));
  EXPECT_THAT(encoder.permute_statuses({{&status1, false}, {&status2, false}}),
              UnorderedElementsAre(
                  ElementsAre(Pair(&status1, false), Pair(&status2, false))));
}

TEST_F(StatusEncoderTest, PermuteAndEncodeStatuses) {
  EXPECT_THAT(encoder.permute_and_encode_statuses({}),
              UnorderedElementsAre(0b00, 0b10, 0b01, 0b11));
  EXPECT_THAT(encoder.permute_and_encode_statuses({{&status1, false}}),
              UnorderedElementsAre(0b00, 0b10));
  EXPECT_THAT(encoder.permute_and_encode_statuses({{&status2, false}}),
              UnorderedElementsAre(0b00, 0b01));
  EXPECT_THAT(encoder.permute_and_encode_statuses(
                  {{&status1, false}, {&status2, false}}),
              UnorderedElementsAre(0b00));
}

} // namespace irata::sim::components::controller

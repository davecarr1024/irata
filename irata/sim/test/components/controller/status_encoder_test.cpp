#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/partial_statuses.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <irata/sim/hdl/irata_decl.hpp>

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
      "instruction", Byte(0x01), asm_::AddressingMode::Immediate, "");
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

  PartialStatuses partial(std::map<const hdl::StatusDecl *, bool> statuses) {
    return PartialStatuses(statuses);
  }

  CompleteStatuses complete(std::map<const hdl::StatusDecl *, bool> statuses) {
    return CompleteStatuses(encoder, statuses);
  }
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

TEST_F(StatusEncoderTest, Statuses) {
  EXPECT_THAT(encoder.statuses(), UnorderedElementsAre(&status1, &status2));
}

TEST_F(StatusEncoderTest, EncodeComplete) {
  EXPECT_EQ(encoder.encode(complete({{&status1, false}, {&status2, false}})),
            0b00);
  EXPECT_EQ(encoder.encode(complete({{&status1, false}, {&status2, true}})),
            0b10);
  EXPECT_EQ(encoder.encode(complete({{&status1, true}, {&status2, false}})),
            0b01);
  EXPECT_EQ(encoder.encode(complete({{&status1, true}, {&status2, true}})),
            0b11);
}

TEST_F(StatusEncoderTest, EncodePartial) {
  EXPECT_THAT(encoder.encode(partial({})),
              UnorderedElementsAre(0b00, 0b10, 0b01, 0b11));
  EXPECT_THAT(encoder.encode(partial({{&status1, false}})),
              UnorderedElementsAre(0b00, 0b10));
  EXPECT_THAT(encoder.encode(partial({{&status1, true}})),
              UnorderedElementsAre(0b01, 0b11));
  EXPECT_THAT(encoder.encode(partial({{&status2, false}})),
              UnorderedElementsAre(0b00, 0b01));
  EXPECT_THAT(encoder.encode(partial({{&status2, true}})),
              UnorderedElementsAre(0b10, 0b11));
  EXPECT_THAT(encoder.encode(partial({{&status1, false}, {&status2, false}})),
              UnorderedElementsAre(0b00));
  EXPECT_THAT(encoder.encode(partial({{&status1, false}, {&status2, true}})),
              UnorderedElementsAre(0b10));
  EXPECT_THAT(encoder.encode(partial({{&status1, true}, {&status2, false}})),
              UnorderedElementsAre(0b01));
  EXPECT_THAT(encoder.encode(partial({{&status1, true}, {&status2, true}})),
              UnorderedElementsAre(0b11));
}

TEST_F(StatusEncoderTest, Decode) {
  EXPECT_EQ(encoder.decode(0b00),
            complete({{&status1, false}, {&status2, false}}));
  EXPECT_EQ(encoder.decode(0b10),
            complete({{&status1, false}, {&status2, true}}));
  EXPECT_EQ(encoder.decode(0b01),
            complete({{&status1, true}, {&status2, false}}));
  EXPECT_EQ(encoder.decode(0b11),
            complete({{&status1, true}, {&status2, true}}));
}

TEST_F(StatusEncoderTest, Permute) {
  EXPECT_THAT(
      encoder.permute(partial({})),
      UnorderedElementsAre(complete({{&status1, false}, {&status2, false}}),
                           complete({{&status1, false}, {&status2, true}}),
                           complete({{&status1, true}, {&status2, false}}),
                           complete({{&status1, true}, {&status2, true}})));
}

} // namespace irata::sim::components::controller

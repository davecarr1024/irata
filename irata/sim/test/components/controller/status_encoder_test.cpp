#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/status_encoder.hpp>

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

StatusEncoder build_encoder(std::vector<std::string> status_names) {
  std::map<std::string, bool> statuses;
  for (const auto &status_name : status_names) {
    statuses[status_name] = true;
  }
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .programs = {microcode::Program{.statuses = statuses}}}}};
  return StatusEncoder(instruction_set);
}

} // namespace

TEST(StatusEncoderTest, TooManyStatuses) {
  std::vector<std::string> status_names;
  for (int i = 0; i < 9; i++) {
    status_names.push_back("P" + std::to_string(i));
  }
  EXPECT_THROW(build_encoder(status_names), std::runtime_error);
}

TEST(StatusEncoderTest, Statuses) {
  const auto encoder = build_encoder({"P1", "P2", "P3"});
  EXPECT_THAT(encoder.statuses(), ElementsAre("P1", "P2", "P3"));
  EXPECT_THAT(encoder.status_indices(),
              ElementsAre(Pair("P1", 0), Pair("P2", 1), Pair("P3", 2)));
  EXPECT_THAT(encoder.status_count(), 3);
}

TEST(StatusEncoderTest, Encode) {
  const auto encoder = build_encoder({"P1", "P2", "P3"});
  EXPECT_THAT(encoder.encode({{"P1", true}}), 0b001);
  EXPECT_THAT(encoder.encode({{"P2", true}}), 0b010);
  EXPECT_THAT(encoder.encode({{"P3", false}}), 0b000);
  EXPECT_THAT(encoder.encode({{"P1", true}, {"P2", true}, {"P3", false}}),
              0b011);
}

TEST(StatusEncoderTest, Decode) {
  const auto encoder = build_encoder({"P1", "P2", "P3"});
  EXPECT_THAT(
      encoder.decode(0b001),
      ElementsAre(Pair("P1", true), Pair("P2", false), Pair("P3", false)));
  EXPECT_THAT(
      encoder.decode(0b010),
      ElementsAre(Pair("P1", false), Pair("P2", true), Pair("P3", false)));
  EXPECT_THAT(
      encoder.decode(0b000),
      ElementsAre(Pair("P1", false), Pair("P2", false), Pair("P3", false)));
  EXPECT_THAT(
      encoder.decode(0b011),
      ElementsAre(Pair("P1", true), Pair("P2", true), Pair("P3", false)));
}

TEST(StatusEncoderTest, UnknownStatus) {
  const auto encoder = build_encoder({"P1"});
  EXPECT_THROW(encoder.encode({{"P2", true}}), std::runtime_error);
}

TEST(StatusEncoderTest, PermuteStatuses) {
  const auto encoder = build_encoder({"P1", "P2"});
  EXPECT_THAT(
      encoder.permute_statuses({{"P1", true}, {"P2", true}}),
      UnorderedElementsAre(ElementsAre(Pair("P1", true), Pair("P2", true))));
  EXPECT_THAT(
      encoder.permute_statuses({{"P1", true}}),
      UnorderedElementsAre(ElementsAre(Pair("P1", true), Pair("P2", true)),
                           ElementsAre(Pair("P1", true), Pair("P2", false))));
  EXPECT_THAT(
      encoder.permute_statuses({}),
      UnorderedElementsAre(ElementsAre(Pair("P1", false), Pair("P2", false)),
                           ElementsAre(Pair("P1", true), Pair("P2", false)),
                           ElementsAre(Pair("P1", false), Pair("P2", true)),
                           ElementsAre(Pair("P1", true), Pair("P2", true))));
}

} // namespace irata::sim::components::controller
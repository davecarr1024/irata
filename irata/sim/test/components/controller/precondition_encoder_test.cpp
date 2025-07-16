#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/precondition_encoder.hpp>

using ::testing::ElementsAre;
using ::testing::Pair;

namespace irata::sim::components::controller {

namespace {

PreconditionEncoder build_encoder(std::vector<std::string> precondition_names) {
  std::map<std::string, bool> preconditions;
  for (const auto &precondition_name : precondition_names) {
    preconditions[precondition_name] = true;
  }
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .programs = {microcode::Program{.preconditions = preconditions}}}}};
  return PreconditionEncoder(instruction_set);
}

} // namespace

TEST(PreconditionEncoderTest, TooManyPreconditions) {
  std::vector<std::string> precondition_names;
  for (int i = 0; i < 9; i++) {
    precondition_names.push_back("P" + std::to_string(i));
  }
  EXPECT_THROW(build_encoder(precondition_names), std::runtime_error);
}

TEST(PreconditionEncoderTest, Preconditions) {
  const auto encoder = build_encoder({"P1", "P2", "P3"});
  EXPECT_THAT(encoder.preconditions(), ElementsAre("P1", "P2", "P3"));
  EXPECT_THAT(encoder.precondition_indices(),
              ElementsAre(Pair("P1", 0), Pair("P2", 1), Pair("P3", 2)));
  EXPECT_THAT(encoder.precondition_count(), 3);
}

TEST(PreconditionEncoderTest, Encode) {
  const auto encoder = build_encoder({"P1", "P2", "P3"});
  EXPECT_THAT(encoder.encode({{"P1", true}}), 0b001);
  EXPECT_THAT(encoder.encode({{"P2", true}}), 0b010);
  EXPECT_THAT(encoder.encode({{"P3", false}}), 0b000);
  EXPECT_THAT(encoder.encode({{"P1", true}, {"P2", true}, {"P3", false}}),
              0b011);
}

TEST(PreconditionEncoderTest, Decode) {
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

TEST(PreconditionEncoderTest, UnknownPrecondition) {
  const auto encoder = build_encoder({"P1"});
  EXPECT_THROW(encoder.encode({{"P2", true}}), std::runtime_error);
}

} // namespace irata::sim::components::controller
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/control_encoder.hpp>

using ::testing::ElementsAre;
using ::testing::Pair;

namespace irata::sim::components::controller {

namespace {

ControlEncoder build_encoder(std::vector<std::string> controls) {
  const auto instruction_set = microcode::InstructionSet{
      .instructions = {microcode::Instruction{
          .programs = {microcode::Program{
              .statements = {microcode::Statement{.controls = controls}}}}}}};
  return ControlEncoder(instruction_set);
}

} // namespace

TEST(ControlEncoderTest, TooManyControls) {
  std::vector<std::string> controls;
  for (int i = 0; i < 33; i++) {
    controls.push_back("C" + std::to_string(i));
  }
  EXPECT_THROW(build_encoder(controls), std::runtime_error);
}

TEST(ControlEncoderTest, Controls) {
  const auto encoder = build_encoder({"C1", "C2", "C3"});
  EXPECT_THAT(encoder.controls(), ElementsAre("C1", "C2", "C3"));
  EXPECT_THAT(encoder.control_indices(),
              ElementsAre(Pair("C1", 0), Pair("C2", 1), Pair("C3", 2)));
  EXPECT_THAT(encoder.control_count(), 3);
}

TEST(ControlEncoderTest, Encode) {
  const auto encoder = build_encoder({"C1", "C2", "C3"});
  EXPECT_THAT(encoder.encode({}), 0b000);
  EXPECT_THAT(encoder.encode({"C1"}), 0b001);
  EXPECT_THAT(encoder.encode({"C2"}), 0b010);
  EXPECT_THAT(encoder.encode({"C2", "C3"}), 0b110);
  EXPECT_THAT(encoder.encode({"C1", "C2", "C3"}), 0b111);
}

TEST(ControlEncoderTest, Decode) {
  const auto encoder = build_encoder({"C1", "C2", "C3"});
  EXPECT_THAT(encoder.decode(0b000), ElementsAre());
  EXPECT_THAT(encoder.decode(0b001), ElementsAre("C1"));
  EXPECT_THAT(encoder.decode(0b010), ElementsAre("C2"));
  EXPECT_THAT(encoder.decode(0b110), ElementsAre("C2", "C3"));
  EXPECT_THAT(encoder.decode(0b111), ElementsAre("C1", "C2", "C3"));
}

TEST(ControlEncoderTest, UnknownControl) {
  const auto encoder = build_encoder({"C1"});
  EXPECT_THROW(encoder.encode({"C2"}), std::runtime_error);
}

} // namespace irata::sim::components::controller

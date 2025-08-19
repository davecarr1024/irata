#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/hdl/irata_decl.hpp>

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

namespace irata::sim::components::controller {

namespace {

class ControlEncoderTest : public ::testing::Test {
protected:
  const hdl::ProcessControlDecl control1 =
      hdl::ProcessControlDecl("control1", hdl::irata());
  const hdl::ProcessControlDecl control2 =
      hdl::ProcessControlDecl("control2", hdl::irata());
  const asm_::Instruction instruction = asm_::Instruction(
      "instruction", Byte(0x01), asm_::AddressingMode::Immediate, "");
  const microcode::table::Table table = {.entries = {{
                                             .instruction = instruction,
                                             .step_index = Byte(0x01),
                                             .statuses = {},
                                             .controls = {&control1, &control2},
                                         }}};
  const ControlEncoder encoder = ControlEncoder(table);
};

} // namespace

TEST_F(ControlEncoderTest, Indices) {
  EXPECT_THAT(encoder.indices(),
              UnorderedElementsAre(Pair(&control1, 0), Pair(&control2, 1)));
}

TEST_F(ControlEncoderTest, TooManyControls) {
  std::vector<std::unique_ptr<hdl::ProcessControlDecl>> controls;
  std::set<const hdl::ControlDecl *> control_ptrs;
  for (size_t i = 0; i < 65; ++i) {
    auto control = std::make_unique<hdl::ProcessControlDecl>(
        "control" + std::to_string(i), hdl::irata());
    control_ptrs.insert(control.get());
    controls.push_back(std::move(control));
  }
  const microcode::table::Table table = {.entries = {{
                                             .instruction = instruction,
                                             .step_index = Byte(0x01),
                                             .statuses = {},
                                             .controls = control_ptrs,
                                         }}};
  EXPECT_THROW((ControlEncoder(table)), std::invalid_argument);
}

TEST_F(ControlEncoderTest, EncodeLotsOfControls) {
  std::vector<std::unique_ptr<hdl::ProcessControlDecl>> controls;
  std::set<const hdl::ControlDecl *> control_ptrs;
  for (size_t i = 0; i < 64; ++i) {
    auto control = std::make_unique<hdl::ProcessControlDecl>(
        "control" + std::to_string(i), hdl::irata());
    control_ptrs.insert(control.get());
    controls.push_back(std::move(control));
  }
  const microcode::table::Table table = {.entries = {{
                                             .instruction = instruction,
                                             .step_index = Byte(0x01),
                                             .statuses = {},
                                             .controls = control_ptrs,
                                         }}};
  const auto encoder = ControlEncoder(table);
  EXPECT_EQ(encoder.num_controls(), 64);
  EXPECT_EQ(encoder.encode(control_ptrs), 0xFFFFFFFFFFFFFFFF);
  EXPECT_THAT(encoder.decode(0xFFFFFFFFFFFFFFFF),
              UnorderedElementsAreArray(control_ptrs));
  EXPECT_THAT(encoder.encode({}), 0x0000000000000000);
  EXPECT_THAT(encoder.decode(0x0000000000000000), IsEmpty());
  EXPECT_THAT(encoder.encode({controls[0].get()}), 0x0000000000000001);
  EXPECT_THAT(encoder.decode(0x0000000000000001),
              UnorderedElementsAre(controls[0].get()));
  EXPECT_THAT(encoder.encode({controls[63].get()}), 0x8000000000000000);
  EXPECT_THAT(encoder.decode(0x8000000000000000),
              UnorderedElementsAre(controls[63].get()));
}

TEST_F(ControlEncoderTest, NumControls) {
  EXPECT_EQ(encoder.num_controls(), 2);
}

TEST_F(ControlEncoderTest, Encode) {
  EXPECT_EQ(encoder.encode({}), 0b00);
  EXPECT_EQ(encoder.encode({&control1}), 0b01);
  EXPECT_EQ(encoder.encode({&control2}), 0b10);
  EXPECT_EQ(encoder.encode({&control1, &control2}), 0b11);
}

TEST_F(ControlEncoderTest, EncodeUnknownControl) {
  const hdl::ProcessControlDecl unknown_control("unknown_control",
                                                hdl::irata());
  EXPECT_THROW(encoder.encode({&unknown_control}), std::invalid_argument);
}

TEST_F(ControlEncoderTest, Decode) {
  EXPECT_THAT(encoder.decode(0b00), IsEmpty());
  EXPECT_THAT(encoder.decode(0b01), UnorderedElementsAre(&control1));
  EXPECT_THAT(encoder.decode(0b10), UnorderedElementsAre(&control2));
  EXPECT_THAT(encoder.decode(0b11), UnorderedElementsAre(&control1, &control2));
}

} // namespace irata::sim::components::controller

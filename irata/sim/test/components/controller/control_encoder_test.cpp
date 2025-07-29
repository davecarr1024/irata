#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/hdl/irata_decl.hpp>

using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class ControlEncoderTest : public ::testing::Test {
protected:
  const hdl::ProcessControlDecl control1 =
      hdl::ProcessControlDecl("control1", hdl::irata());
  const hdl::ProcessControlDecl control2 =
      hdl::ProcessControlDecl("control2", hdl::irata());
  const asm_::Instruction instruction = asm_::Instruction(
      "instruction", Byte(0x01), asm_::AddressingMode::IMMEDIATE, "");
  const microcode::table::Table table = {.entries = {{
                                             .instruction = instruction,
                                             .step_index = Byte(0x01),
                                             .statuses = {},
                                             .controls = {&control1, &control2},
                                         }}};
  const ControlEncoder encoder = ControlEncoder(table);
};

} // namespace

TEST_F(ControlEncoderTest, TooManyControls) {
  std::vector<std::unique_ptr<hdl::ProcessControlDecl>> controls;
  std::set<const hdl::ControlDecl *> control_ptrs;
  for (size_t i = 0; i < 33; ++i) {
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

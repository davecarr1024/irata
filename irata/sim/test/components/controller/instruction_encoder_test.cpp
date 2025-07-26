#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/instruction_encoder.hpp>
#include <irata/sim/microcode/compiler/compiler.hpp>
#include <stdexcept>

using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class InstructionEncoderTest : public ::testing::Test {
protected:
  const hdl::StatusDecl status1 = hdl::StatusDecl("status1", hdl::irata());
  const hdl::StatusDecl status2 = hdl::StatusDecl("status2", hdl::irata());
  const hdl::ProcessControlDecl control1 =
      hdl::ProcessControlDecl("control1", hdl::irata());
  const hdl::ProcessControlDecl control2 =
      hdl::ProcessControlDecl("control2", hdl::irata());
  const asm_::Instruction instruction = asm_::Instruction(
      "instruction", Byte(0x0F), asm_::AddressingMode::IMMEDIATE, "");
  const microcode::table::Entry entry = {
      .instruction = instruction,
      .step_index = Byte(0x05),
      .statuses = {{&status1, false}, {&status2, true}},
      .controls = {&control1, &control2},
  };
  const microcode::table::Table table = {{entry}};
  // [opcode 4 bits][statuses 2 bits][step index 3 bits]
  const InstructionEncoder encoder = InstructionEncoder(table);
};

} // namespace

TEST_F(InstructionEncoderTest, num_statuses) {
  EXPECT_EQ(encoder.num_statuses(), 2);
}

TEST_F(InstructionEncoderTest, num_status_bits) {
  EXPECT_EQ(encoder.num_status_bits(), 2);
}

TEST_F(InstructionEncoderTest, num_controls) {
  EXPECT_EQ(encoder.num_controls(), 2);
}

TEST_F(InstructionEncoderTest, max_opcode) {
  EXPECT_EQ(encoder.max_opcode(), 0x0F);
}

TEST_F(InstructionEncoderTest, num_opcode_bits) {
  EXPECT_EQ(encoder.num_opcode_bits(), 4);
}

TEST_F(InstructionEncoderTest, max_step_index) {
  EXPECT_EQ(encoder.max_step_index(), 0x05);
}

TEST_F(InstructionEncoderTest, num_step_index_bits) {
  EXPECT_EQ(encoder.num_step_index_bits(), 3);
}

TEST_F(InstructionEncoderTest, num_address_bits) {
  EXPECT_EQ(encoder.num_address_bits(), 9);
}

TEST_F(InstructionEncoderTest, num_value_bits) {
  EXPECT_EQ(encoder.num_value_bits(), 2);
}

TEST_F(InstructionEncoderTest, EncodeAndDecodeIrata) {
  const auto table = microcode::compiler::Compiler::compile_irata();
  const InstructionEncoder encoder(table);
  for (const auto &entry : table.entries) {
    const auto address = encoder.encode_address(entry);
    const auto [opcode, statuses, step_index] = encoder.decode_address(address);
    EXPECT_EQ(opcode, entry.instruction.opcode().unsigned_value());
    EXPECT_EQ(statuses, entry.statuses);
    EXPECT_EQ(step_index, entry.step_index.unsigned_value());

    const auto value = encoder.encode_value(entry.controls);
    EXPECT_EQ(encoder.decode_value(value), entry.controls);
  }
}

TEST_F(InstructionEncoderTest, EncodeAddress) {
  for (uint8_t opcode = 0x00; opcode <= 0x0F; ++opcode) {
    for (uint8_t step_index = 0x00; step_index <= 0x05; ++step_index) {
      for (bool status1_value : {false, true}) {
        for (bool status2_value : {false, true}) {
          uint32_t expected = 0x0000;
          expected |= opcode << 5;
          expected |= (status1_value ? 0b01 : 0b00) << 3;
          expected |= (status2_value ? 0b10 : 0b00) << 3;
          expected |= step_index;
          EXPECT_EQ(encoder.encode_address(
                        opcode,
                        {{&status1, status1_value}, {&status2, status2_value}},
                        step_index),
                    expected);
        }
      }
    }
  }
}

TEST_F(InstructionEncoderTest, EncodeAddressInstruction) {
  for (uint8_t step_index = 0x00; step_index <= 0x05; ++step_index) {
    for (bool status1_value : {false, true}) {
      for (bool status2_value : {false, true}) {
        uint32_t expected = 0x0000;
        expected |= instruction.opcode().unsigned_value() << 5;
        expected |= (status1_value ? 0b01 : 0b00) << 3;
        expected |= (status2_value ? 0b10 : 0b00) << 3;
        expected |= step_index;
        EXPECT_EQ(encoder.encode_address(
                      instruction,
                      {{&status1, status1_value}, {&status2, status2_value}},
                      step_index),
                  expected);
      }
    }
  }
}

TEST_F(InstructionEncoderTest, EncodeAddressEntry) {
  EXPECT_EQ(encoder.encode_address(entry), 0b0111110101);
}

TEST_F(InstructionEncoderTest, DecodeAddress) {
  for (uint32_t address = 0x0000; address <= 0x01FF; ++address) {
    const auto [opcode, statuses, step_index] = encoder.decode_address(address);
    EXPECT_EQ(opcode, (address >> 5) & 0b1111);
    EXPECT_EQ(statuses.at(&status1), ((address >> 3) & 0b01) == 0b01);
    EXPECT_EQ(statuses.at(&status2), ((address >> 3) & 0b10) == 0b10);
    EXPECT_EQ(step_index, address & 0b111);
  }
}

TEST_F(InstructionEncoderTest, EncodeValue) {
  EXPECT_EQ(encoder.encode_value({}), 0b00);
  EXPECT_EQ(encoder.encode_value({&control1}), 0b01);
  EXPECT_EQ(encoder.encode_value({&control2}), 0b10);
  EXPECT_EQ(encoder.encode_value({&control1, &control2}), 0b11);
}

TEST_F(InstructionEncoderTest, DecodeValue) {
  EXPECT_THAT(encoder.decode_value(0b00), IsEmpty());
  EXPECT_THAT(encoder.decode_value(0b01), UnorderedElementsAre(&control1));
  EXPECT_THAT(encoder.decode_value(0b10), UnorderedElementsAre(&control2));
  EXPECT_THAT(encoder.decode_value(0b11),
              UnorderedElementsAre(&control1, &control2));
}

} // namespace irata::sim::components::controller

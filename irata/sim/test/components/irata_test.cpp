#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/components/irata.hpp>

namespace irata::sim::components {

namespace {

class IrataTest : public ::testing::Test {
protected:
  Irata irata(std::vector<Byte> data = {}) {
    std::map<Word, Byte> rom_data;
    for (size_t i = 0; i < data.size(); i++) {
      rom_data[Word(i)] = data[i];
    }
    return Irata(std::make_unique<memory::ROM>(1024, "cartridge", rom_data));
  }

  const asm_::Instruction &hlt = asm_::InstructionSet::irata().get_instruction(
      "HLT", asm_::AddressingMode::NONE);
  const asm_::Instruction &crs = asm_::InstructionSet::irata().get_instruction(
      "CRS", asm_::AddressingMode::NONE);
  const asm_::Instruction &lda = asm_::InstructionSet::irata().get_instruction(
      "LDA", asm_::AddressingMode::IMMEDIATE);
};

} // namespace

TEST_F(IrataTest, Type) {
  EXPECT_EQ(irata().type(), hdl::ComponentType::Irata);
}

TEST_F(IrataTest, Properties) {
  auto irata = this->irata();
  EXPECT_EQ(irata.data_bus().parent(), &irata);
  EXPECT_EQ(irata.data_bus().path(), "/data_bus");
  EXPECT_EQ(irata.address_bus().parent(), &irata);
  EXPECT_EQ(irata.address_bus().path(), "/address_bus");
  EXPECT_EQ(irata.memory().parent(), &irata);
  EXPECT_EQ(irata.memory().path(), "/memory");
  EXPECT_EQ(irata.cpu().parent(), &irata);
  EXPECT_EQ(irata.cpu().path(), "/cpu");
}

TEST_F(IrataTest, LoadCartridge) {
  auto irata = this->irata({Byte(0x12)});
  EXPECT_EQ(irata.memory().value(Word(0x8000)), Byte(0x12));
  EXPECT_EQ(irata.memory().value(Word(0x8001)), Byte(0x00));
}

TEST_F(IrataTest, TickUntilHalt) {
  auto irata = this->irata({hlt.opcode()});
  EXPECT_EQ(irata.tick_until_halt(), Irata::Result::Halt);
  EXPECT_EQ(irata.cpu().pc().value(), Word(0x8001));
}

TEST_F(IrataTest, TickUntilCrash) {
  auto irata = this->irata({crs.opcode()});
  EXPECT_EQ(irata.tick_until_halt(), Irata::Result::Crash);
  EXPECT_EQ(irata.cpu().pc().value(), Word(0x8001));
}

TEST_F(IrataTest, TickUntilHaltWithLda) {
  auto irata = this->irata({lda.opcode(), Byte(0x12), hlt.opcode()});
  EXPECT_EQ(irata.tick_until_halt(), Irata::Result::Halt);
  EXPECT_EQ(irata.cpu().pc().value(), Word(0x8003));
  EXPECT_EQ(irata.cpu().a().value(), Byte(0x12));
}

} // namespace irata::sim::components

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/hdl/component_type.hpp>
#include <sstream>

namespace irata::sim::hdl {

TEST(ComponentTypeTest, ToString) {
  const auto to_string = [](ComponentType type) {
    std::ostringstream os;
    os << type;
    return os.str();
  };
  EXPECT_EQ(to_string(ComponentType::Irata), "Irata");
  EXPECT_EQ(to_string(ComponentType::Cpu), "Cpu");
  EXPECT_EQ(to_string(ComponentType::Memory), "Memory");
  EXPECT_EQ(to_string(ComponentType::Register), "Register");
  EXPECT_EQ(to_string(ComponentType::WordRegister), "WordRegister");
  EXPECT_EQ(to_string(ComponentType::ByteBus), "ByteBus");
  EXPECT_EQ(to_string(ComponentType::WordBus), "WordBus");
  EXPECT_EQ(to_string(ComponentType::Control), "Control");
  EXPECT_EQ(to_string(ComponentType::Status), "Status");
  EXPECT_EQ(to_string(ComponentType::Controller), "Controller");
  EXPECT_EQ(to_string(ComponentType::Alu), "Alu");
  EXPECT_EQ(to_string(ComponentType::AluModule), "AluModule");
  EXPECT_EQ(to_string(ComponentType::StatusRegister), "StatusRegister");
  EXPECT_EQ(to_string(ComponentType::Unknown), "Unknown");
}

} // namespace irata::sim::hdl

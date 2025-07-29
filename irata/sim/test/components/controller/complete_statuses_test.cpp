#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/table/entry.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <stdexcept>

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class CompleteStatusesTest : public ::testing::Test {
protected:
  const hdl::StatusDecl status1 = hdl::StatusDecl("status1", hdl::irata());
  const hdl::StatusDecl status2 = hdl::StatusDecl("status2", hdl::irata());
  const microcode::table::Entry entry = {
      .instruction = asm_::Instruction("instruction", Byte(0x0F),
                                       asm_::AddressingMode::IMMEDIATE, ""),
      .step_index = Byte(0x05),
      .statuses = {{&status1, false}, {&status2, true}},
  };
  const microcode::table::Table table = {{entry}};
  const StatusEncoder encoder = StatusEncoder(table);
};

} // namespace

TEST_F(CompleteStatusesTest, CompleteStatuses) {
  EXPECT_NO_THROW(
      CompleteStatuses(encoder, {{&status1, false}, {&status2, true}}));
}

TEST_F(CompleteStatusesTest, NullStatus) {
  EXPECT_THROW(CompleteStatuses(encoder, {{nullptr, false}}),
               std::invalid_argument);
}

TEST_F(CompleteStatusesTest, MissingStatus) {
  EXPECT_THROW(CompleteStatuses(encoder, {{&status1, false}}),
               std::invalid_argument);
}

TEST_F(CompleteStatusesTest, ExtraStatus) {
  const hdl::StatusDecl status3 = hdl::StatusDecl("status3", hdl::irata());
  EXPECT_THROW(
      CompleteStatuses(
          encoder, {{&status1, false}, {&status2, true}, {&status3, false}}),
      std::invalid_argument);
}

TEST_F(CompleteStatusesTest, statuses) {
  const auto complete_statuses =
      CompleteStatuses(encoder, {{&status1, false}, {&status2, true}});
  EXPECT_THAT(
      complete_statuses.statuses(),
      UnorderedElementsAre(Pair(&status1, false), Pair(&status2, true)));
}

} // namespace irata::sim::components::controller
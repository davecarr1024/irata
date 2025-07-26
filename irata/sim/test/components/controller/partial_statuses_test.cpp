#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/controller/partial_statuses.hpp>
#include <stdexcept>

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::controller {

namespace {

class PartialStatusesTest : public ::testing::Test {
protected:
  const hdl::StatusDecl status1 = hdl::StatusDecl("status1", hdl::irata());
  const hdl::StatusDecl status2 = hdl::StatusDecl("status2", hdl::irata());
};

} // namespace

TEST_F(PartialStatusesTest, Empty) {
  PartialStatuses partial_statuses({});
  EXPECT_THAT(partial_statuses.statuses(), IsEmpty());
}

TEST_F(PartialStatusesTest, NullStatus) {
  EXPECT_THROW(PartialStatuses({{nullptr, true}}), std::invalid_argument);
}

TEST_F(PartialStatusesTest, Statuses) {
  PartialStatuses partial_statuses({{&status1, true}, {&status2, false}});
  EXPECT_THAT(
      partial_statuses.statuses(),
      UnorderedElementsAre(Pair(&status1, true), Pair(&status2, false)));
}

} // namespace irata::sim::components::controller
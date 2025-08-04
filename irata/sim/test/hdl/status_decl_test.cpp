#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/hdl/status_decl.hpp>

using ::testing::HasSubstr;

namespace irata::sim::hdl {

namespace {

class StatusDeclTest : public ::testing::Test {
protected:
  const ComponentWithTypeDecl<ComponentType::Irata> irata_decl = {"irata"};
  const StatusDecl status_decl = {"status", irata_decl};

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
};

} // namespace

TEST_F(StatusDeclTest, Status_Properties) {
  EXPECT_EQ(status_decl.name(), "status");
  EXPECT_EQ(status_decl.path(), "/status");
  EXPECT_EQ(status_decl.parent(), &irata_decl);
  EXPECT_EQ(status_decl.type(), ComponentType::Status);
}

TEST_F(StatusDeclTest, Status_ToString) {
  std::ostringstream os;
  os << status_decl;
  EXPECT_EQ(os.str(), "Status(/status)");
}

TEST_F(StatusDeclTest, Status_Validate_Null) {
  EXPECT_THROW(
      {
        try {
          status_decl.verify(nullptr);
        } catch (const std::invalid_argument &e) {
          EXPECT_THAT(e.what(),
                      HasSubstr("hdl component Status(/status) failed to find "
                                "a corresponding sim component"));
          throw;
        }
      },
      std::invalid_argument);
}

TEST_F(StatusDeclTest, Status_Validate_WrongType) {
  components::FakeComponent status_component = {ComponentType::Control,
                                                "status", &irata_component};
  EXPECT_THROW(
      {
        try {
          status_decl.verify(&status_component);
        } catch (const std::invalid_argument &e) {
          EXPECT_THAT(
              e.what(),
              HasSubstr("hdl component Status(/status) has type Status but "
                        "sim component /status has type Control"));
          throw;
        }
      },
      std::invalid_argument);
}

TEST_F(StatusDeclTest, Status_Validate) {
  components::Status status_component("status", &irata_component);
  EXPECT_NO_THROW(status_decl.verify(&status_component));
}

} // namespace irata::sim::hdl

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/irata.hpp>
#include <irata/sim/hdl/irata_decl.hpp>

namespace irata::sim::hdl {

TEST(IrataDeclTest, Verify) {
  components::Irata irata_component;
  IrataDecl irata_decl;
  EXPECT_NO_THROW(irata_decl.verify(&irata_component));
}

} // namespace irata::sim::hdl
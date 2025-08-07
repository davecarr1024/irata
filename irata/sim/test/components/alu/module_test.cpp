#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/alu/module.hpp>

namespace irata::sim::components::alu {

TEST(AluModuleTest, Result_Equality) {
  EXPECT_EQ((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }));
  EXPECT_NE((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x02,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }));
  EXPECT_NE((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x01,
                .carry = true,
                .zero = false,
                .negative = false,
                .overflow = false,
            }));
  EXPECT_NE((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = true,
                .negative = false,
                .overflow = false,
            }));
  EXPECT_NE((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = true,
                .overflow = false,
            }));
  EXPECT_NE((Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = false,
            }),
            (Module::Result{
                .value = 0x01,
                .carry = false,
                .zero = false,
                .negative = false,
                .overflow = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_Zero) {
  // 0x00  + 0x00 + 0 = 0x00
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0x00,
                                       /*rhs=*/0x00, /*result=*/0x00),
            (Module::Result{
                .value = 0x00,
                .zero = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_NoFlags) {
  // 0x00  + 0x02 + 0 = 0x03
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0x00,

                                       /*rhs=*/0x02, /*result=*/0x03),
            (Module::Result{
                .value = 0x03,
            }));
}

TEST(AluModuleTest, Result_ForResult_Carry) {
  // 0xFF + 0x01 + 0 = 0x100 + carry, zero
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0xFF,

                                       /*rhs=*/0x01, /*result=*/0x100),
            (Module::Result{
                .value = 0x00,
                .carry = true,
                .zero = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_Negative) {
  // 0x7F + 0x01 + 0 = 0x80 + negative, overflow
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0x7F,
                                       /*rhs=*/0x01, /*result=*/0x80),
            (Module::Result{
                .value = 0x80,
                .negative = true,
                .overflow = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_PositiveToNegative) {
  // 0x7F + 0x01 + 0 = 0x80
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0x7F,
                                       /*rhs=*/0x01, /*result=*/0x80),
            (Module::Result{
                .value = 0x80,
                .negative = true,
                .overflow = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_NegativeToPositive) {
  // 0x80 + 0x80 + 0 = 0x100 + carry, zero
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/false, /*lhs=*/0x80,
                                       /*rhs=*/0x80, /*result=*/0x100),
            (Module::Result{
                .value = 0x00,
                .carry = true,
                .zero = true,
                .overflow = true,
            }));
}

TEST(AluModuleTest, Result_ForResult_AddWithCarryIn) {
  // 0x01 + 0x01 + 1 = 0x03
  EXPECT_EQ(Module::Result::for_result(/*carry_in=*/true, /*lhs=*/0x01,
                                       /*rhs=*/0x01, /*result=*/0x03),
            (Module::Result{
                .value = 0x03,
            }));
}

} // namespace irata::sim::components::alu

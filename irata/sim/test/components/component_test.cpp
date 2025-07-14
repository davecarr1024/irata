#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/component.hpp>

using namespace irata::sim;

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(ComponentTest, Name) {
  const Component test_component("test");
  EXPECT_EQ(test_component.name(), "test");
}

TEST(ComponentTest, NullParent) {
  const Component test_component("test");
  EXPECT_EQ(test_component.parent(), nullptr);
}

TEST(ComponentTest, AddChild) {
  Component parent("parent");
  auto child = std::make_unique<Component>("child");
  EXPECT_EQ(child->parent(), nullptr);
  EXPECT_THAT(parent.children(), IsEmpty());
  EXPECT_EQ(parent.child("child"), nullptr);
  auto *child_ptr = child.get();
  parent.add_child(std::move(child));
  EXPECT_EQ(child_ptr->parent(), &parent);
  EXPECT_EQ(parent.child("child"), child_ptr);
  EXPECT_THAT(parent.children(),
              UnorderedElementsAre(Pair("child", child_ptr)));
}

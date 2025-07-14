#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/component.hpp>

using namespace irata::sim;

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(ComponentTest, Empty) {
  const Component test_component("test");
  EXPECT_EQ(test_component.name(), "test");
  EXPECT_THAT(test_component.children(), IsEmpty());
  EXPECT_EQ(test_component.parent(), nullptr);
}

TEST(ComponentTest, AddChild) {
  Component parent("parent");
  Component child("child");
  EXPECT_THAT(parent.children(), IsEmpty());
  EXPECT_EQ(child.parent(), nullptr);
  EXPECT_EQ(parent.child("child"), nullptr);
  parent.add_child(&child);
  EXPECT_THAT(parent.children(), UnorderedElementsAre(Pair("child", &child)));
  EXPECT_EQ(child.parent(), &parent);
  EXPECT_EQ(parent.child("child"), &child);
}

TEST(ComponentTest, AddChildWithSameName) {
  Component parent("parent");
  Component child1("child");
  Component child2("child");
  parent.add_child(&child1);
  EXPECT_THROW(parent.add_child(&child2), std::invalid_argument);
}

TEST(ComponentTest, AddNullChild) {
  Component parent("parent");
  EXPECT_THROW(parent.add_child(nullptr), std::invalid_argument);
}

TEST(ComponentTest, AddChildWithParent) {
  Component parent1("parent");
  Component parent2("parent");
  Component child("child");
  parent1.add_child(&child);
  EXPECT_THROW(parent2.add_child(&child), std::invalid_argument);
}

TEST(ComponentTest, GetRootByPath) {
  Component root("root");
  EXPECT_EQ(root.child("/"), &root);
  Component child("child");
  root.add_child(&child);
  EXPECT_EQ(child.child("/"), &root);
}

TEST(ComponentTest, GetChildByPath) {
  Component root("root");
  Component child("child");
  root.add_child(&child);
  EXPECT_EQ(root.child("child"), &child);
}

TEST(ComponentTest, GetGrandchildByPath) {
  Component root("root");
  Component parent("parent");
  Component child("child");
  root.add_child(&parent);
  parent.add_child(&child);
  EXPECT_EQ(root.child("parent/child"), &child);
}

TEST(ComponentTest, GetRootRelativeChildByPath) {
  Component root("root");
  Component child("child");
  root.add_child(&child);
  EXPECT_EQ(root.child("/child"), &child);
  EXPECT_EQ(child.child("/child"), &child);
}

TEST(ComponentTest, GetRootRelativeGrandchildByPath) {
  Component root("root");
  Component parent("parent");
  Component child("child");
  root.add_child(&parent);
  parent.add_child(&child);
  EXPECT_EQ(root.child("/parent/child"), &child);
  EXPECT_EQ(parent.child("/parent/child"), &child);
  EXPECT_EQ(child.child("/parent/child"), &child);
}

TEST(ComponentTest, GetNonExistentChildByPath) {
  Component root("root");
  EXPECT_EQ(root.child("child"), nullptr);
}

TEST(ComponentTest, GetParentByPath) {
  Component root("root");
  Component child("child");
  root.add_child(&child);
  EXPECT_EQ(child.child(".."), &root);
  EXPECT_EQ(child.child("../.."), nullptr);
  EXPECT_EQ(child.child("../child"), &child);
}

TEST(ComponentTest, GetPath) {
  Component root("root");
  Component parent("parent");
  Component child("child");
  root.add_child(&parent);
  parent.add_child(&child);
  EXPECT_EQ(root.path(), "/");
  EXPECT_EQ(parent.path(), "/parent");
  EXPECT_EQ(child.path(), "/parent/child");
}
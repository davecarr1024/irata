#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/component.hpp>
#include <stdexcept>

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

TEST(ComponentTest, WithParent) {
  Component parent("parent");
  Component child("child", &parent);
  EXPECT_EQ(child.parent(), &parent);
  EXPECT_THAT(parent.children(), UnorderedElementsAre(Pair("child", &child)));
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

namespace {
class MockComponent : public Component {
public:
  explicit MockComponent(std::string_view name) : Component(name) {}
  MOCK_METHOD(void, tick_control, (Component::Logger & logger), (override));
  MOCK_METHOD(void, tick_write, (Component::Logger & logger), (override));
  MOCK_METHOD(void, tick_read, (Component::Logger & logger), (override));
  MOCK_METHOD(void, tick_process, (Component::Logger & logger), (override));
  MOCK_METHOD(void, tick_clear, (Component::Logger & logger), (override));
};
} // namespace

TEST(ComponentTest, TickCallsTickPhases) {
  MockComponent root("root");
  const ::testing::InSequence s;
  EXPECT_CALL(root, tick_control(::testing::_));
  EXPECT_CALL(root, tick_write(::testing::_));
  EXPECT_CALL(root, tick_read(::testing::_));
  EXPECT_CALL(root, tick_process(::testing::_));
  EXPECT_CALL(root, tick_clear(::testing::_));
  root.tick();
}

TEST(ComponentTest, TickCallsTickPhasesForChildren) {
  MockComponent root("root");
  MockComponent child("child");
  root.add_child(&child);
  const ::testing::InSequence s;
  EXPECT_CALL(root, tick_control(::testing::_));
  EXPECT_CALL(child, tick_control(::testing::_));
  EXPECT_CALL(root, tick_write(::testing::_));
  EXPECT_CALL(child, tick_write(::testing::_));
  EXPECT_CALL(root, tick_read(::testing::_));
  EXPECT_CALL(child, tick_read(::testing::_));
  EXPECT_CALL(root, tick_process(::testing::_));
  EXPECT_CALL(child, tick_process(::testing::_));
  EXPECT_CALL(root, tick_clear(::testing::_));
  EXPECT_CALL(child, tick_clear(::testing::_));
  root.tick();
}

TEST(ComponentTest, Log) {
  Component root("root");
  MockComponent child("child");
  root.add_child(&child);
  std::ostringstream os;
  EXPECT_CALL(child, tick_process(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "Hello, world!"; });
  root.tick(os);
  EXPECT_EQ(os.str(), "[tick_process] /child: Hello, world!\n");
}

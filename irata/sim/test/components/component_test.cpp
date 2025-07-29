#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/component.hpp>
#include <stdexcept>

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components {

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
  EXPECT_CALL(child, tick_control(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "control msg"; });
  EXPECT_CALL(child, tick_write(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "write msg"; });
  EXPECT_CALL(child, tick_read(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "read msg"; });
  EXPECT_CALL(child, tick_process(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "process msg"; });
  EXPECT_CALL(child, tick_clear(::testing::_))
      .WillOnce([&](Component::Logger &logger) { logger << "clear msg"; });
  root.tick(os);
  EXPECT_EQ(os.str(), "[Control] /child: control msg\n"
                      "[Write] /child: write msg\n"
                      "[Read] /child: read msg\n"
                      "[Process] /child: process msg\n"
                      "[Clear] /child: clear msg\n");
}

namespace {

class ComponentWithSerializedProperty : public Component {
public:
  explicit ComponentWithSerializedProperty(int value, Byte byte_value,
                                           Word word_value,
                                           std::string_view name,
                                           Component *parent = nullptr)
      : Component(name, parent), value_(value), byte_value_(byte_value),
        word_value_(word_value) {}

protected:
  void serialize(Serializer &serializer) const override {
    serializer.property("value", value_);
    serializer.property("byte_value", byte_value_);
    serializer.property("word_value", word_value_);
  }

private:
  const int value_;
  const Byte byte_value_;
  const Word word_value_;
};

} // namespace

TEST(ComponentTest, Serialize) {
  Component root("root");
  Component parent("parent", &root);
  ComponentWithSerializedProperty child1(1, Byte(0x12), Word(0xDEAD), "child1",
                                         &parent);
  ComponentWithSerializedProperty child2(2, Byte(0x32), Word(0xBEEF), "child2",
                                         &parent);
  std::ostringstream os;
  root.serialize_all(os);
  EXPECT_EQ(os.str(), "parent\n"
                      "  child1\n"
                      "    value: 1\n"
                      "    byte_value: 0x12\n"
                      "    word_value: 0xDEAD\n"
                      "  child2\n"
                      "    value: 2\n"
                      "    byte_value: 0x32\n"
                      "    word_value: 0xBEEF\n");
}

} // namespace irata::sim::components

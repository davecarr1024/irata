#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/hdl/bus_decl.hpp>

namespace irata::sim::hdl {

namespace {

class BusDeclTest : public ::testing::Test {
public:
  const ComponentWithTypeDecl<ComponentType::Irata> irata = {"irata"};
  const ByteBusDecl data_bus = {"data_bus", irata};
  const WordBusDecl address_bus = {"address_bus", irata};
  const BusDecl *data_bus_ptr = &data_bus;
  const BusDecl *address_bus_ptr = &address_bus;
};

} // namespace

TEST_F(BusDeclTest, Type) {
  EXPECT_EQ(data_bus.type(), ComponentType::ByteBus);
  EXPECT_EQ(address_bus.type(), ComponentType::WordBus);
  EXPECT_EQ(data_bus_ptr->type(), ComponentType::ByteBus);
  EXPECT_EQ(address_bus_ptr->type(), ComponentType::WordBus);
}

TEST_F(BusDeclTest, Name) {
  EXPECT_EQ(data_bus.name(), "data_bus");
  EXPECT_EQ(address_bus.name(), "address_bus");
  EXPECT_EQ(data_bus_ptr->name(), "data_bus");
  EXPECT_EQ(address_bus_ptr->name(), "address_bus");
}

TEST_F(BusDeclTest, Parent) {
  EXPECT_EQ(data_bus.parent(), &irata);
  EXPECT_EQ(address_bus.parent(), &irata);
  EXPECT_EQ(data_bus_ptr->parent(), &irata);
  EXPECT_EQ(address_bus_ptr->parent(), &irata);
}

TEST_F(BusDeclTest, Path) {
  EXPECT_EQ(data_bus.path(), "/data_bus");
  EXPECT_EQ(address_bus.path(), "/address_bus");
  EXPECT_EQ(data_bus_ptr->path(), "/data_bus");
  EXPECT_EQ(address_bus_ptr->path(), "/address_bus");
}

TEST_F(BusDeclTest, Root) {
  EXPECT_EQ(data_bus.root(), &irata);
  EXPECT_EQ(address_bus.root(), &irata);
  EXPECT_EQ(data_bus_ptr->root(), &irata);
  EXPECT_EQ(address_bus_ptr->root(), &irata);
}

TEST_F(BusDeclTest, ByteBusVerify) {
  const components::FakeComponent component(ComponentType::ByteBus, "data_bus");
  const ByteBusDecl byte_bus("data_bus", irata);
}

} // namespace irata::sim::hdl

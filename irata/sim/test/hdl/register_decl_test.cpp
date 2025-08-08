#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/counter.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/word_counter.hpp>
#include <irata/sim/components/word_register.hpp>
#include <irata/sim/hdl/fake_component_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

namespace {

class RegisterDeclTest : public ::testing::Test {
protected:
  const FakeComponentDecl irata_decl = {ComponentType::Irata, "irata"};
  const DisconnectedByteRegisterDecl disconnected_byte_register_decl = {
      "disconnected_byte_register", irata_decl};
  const ByteBusDecl data_bus_decl = {"data_bus", irata_decl};
  const ConnectedByteRegisterDecl connected_byte_register_decl = {
      "connected_byte_register", irata_decl, data_bus_decl};
  const IncrementableConnectedByteRegisterDecl
      incrementable_connected_byte_register_decl = {
          "incrementable_connected_byte_register", irata_decl, data_bus_decl};
  const WordBusDecl address_bus_decl = {"address_bus", irata_decl};
  const ConnectedWordRegisterDecl connected_word_register_decl = {
      "connected_word_register", irata_decl, address_bus_decl};
  const DataAndAddressBusWordRegisterDecl
      data_and_address_bus_word_register_decl = {
          "data_and_address_bus_word_register", irata_decl, address_bus_decl,
          data_bus_decl};
  const ProgramCounterDecl program_counter_decl = {
      "program_counter", irata_decl, address_bus_decl, data_bus_decl};

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::ByteBus data_bus =
      components::ByteBus("data_bus", &irata_component);
  components::WordBus address_bus =
      components::WordBus("address_bus", &irata_component);
};

} // namespace

TEST_F(RegisterDeclTest, DisconnectedByteRegisterDecl_Validate) {
  components::Register disconnected_byte_register_component(
      "disconnected_byte_register", nullptr, &irata_component);
  EXPECT_NO_THROW(disconnected_byte_register_decl.verify(
      &disconnected_byte_register_component));
}

TEST_F(RegisterDeclTest, ConnectedByteRegisterDecl_Validate) {
  components::Register connected_byte_register_component(
      "connected_byte_register", &data_bus, &irata_component);
  EXPECT_NO_THROW(
      connected_byte_register_decl.verify(&connected_byte_register_component));
}

TEST_F(RegisterDeclTest, IncrementableConnectedByteRegisterDecl_Validate) {
  components::Counter incrementable_connected_byte_register_component(
      "incrementable_connected_byte_register", &data_bus, &irata_component);
  EXPECT_NO_THROW(incrementable_connected_byte_register_decl.verify(
      &incrementable_connected_byte_register_component));
}

TEST_F(RegisterDeclTest, ConnectedWordRegisterDecl_Validate) {
  components::WordRegister connected_word_register_component(
      "connected_word_register", &address_bus, nullptr, &irata_component);
  EXPECT_NO_THROW(
      connected_word_register_decl.verify(&connected_word_register_component));
}

TEST_F(RegisterDeclTest, DataAndAddressBusWordRegisterDecl_Validate) {
  components::WordRegister data_and_address_bus_word_register_component(
      "data_and_address_bus_word_register", &address_bus, &data_bus,
      &irata_component);
  EXPECT_NO_THROW(data_and_address_bus_word_register_decl.verify(
      &data_and_address_bus_word_register_component));
}

TEST_F(RegisterDeclTest, ProgramCounterDecl_Validate) {
  components::WordCounter program_counter_component(
      "program_counter", &address_bus, &data_bus, &irata_component);
  EXPECT_NO_THROW(program_counter_decl.verify(&program_counter_component));
}

} // namespace irata::sim::hdl

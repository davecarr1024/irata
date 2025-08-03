#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/fake_component.hpp>
#include <irata/sim/hdl/component_type.hpp>
#include <irata/sim/hdl/control_decl.hpp>

using ::testing::HasSubstr;

namespace irata::sim::hdl {

namespace {

class ControlDeclTest : public ::testing::Test {
protected:
  const ComponentWithTypeDecl<ComponentType::Irata> irata_decl = {"irata"};
  const ByteBusDecl data_bus_decl = {"data_bus", irata_decl};
  const WriteControlDecl write_control_decl = {"write", irata_decl,
                                               data_bus_decl};
  const ReadControlDecl read_control_decl = {"read", irata_decl, data_bus_decl};
  const ProcessControlDecl process_control_decl = {"process", irata_decl};
  const PersistentProcessControlDecl persistent_process_control_decl = {
      "persistent_process", irata_decl};
  const PersistentClearControlDecl persistent_clear_control_decl = {
      "persistent_clear", irata_decl};

  components::FakeComponent irata_component = {ComponentType::Irata, "irata"};
  components::FakeComponent data_bus_component = {ComponentType::ByteBus,
                                                  "data_bus", &irata_component};
};

} // namespace

TEST_F(ControlDeclTest, WriteControl_Properties) {
  EXPECT_EQ(write_control_decl.name(), "write");
  EXPECT_EQ(write_control_decl.path(), "/write");
  EXPECT_EQ(write_control_decl.parent(), &irata_decl);
  EXPECT_EQ(write_control_decl.type(), ComponentType::Control);
  EXPECT_EQ(write_control_decl.phase(), TickPhase::Write);
  EXPECT_EQ(write_control_decl.auto_clear(), true);
}

TEST_F(ControlDeclTest, WriteControl_ToString) {
  std::ostringstream os;
  os << write_control_decl;
  EXPECT_EQ(os.str(),
            "Control(path = /write, phase = Write, auto_clear = true)");
}

TEST_F(ControlDeclTest, WriteControl_Validate_Null) {
  EXPECT_THROW(write_control_decl.verify(nullptr), std::invalid_argument);
}

TEST_F(ControlDeclTest, WriteControl_Validate_UnableToCast) {
  components::FakeComponent write_control_component = {
      ComponentType::Control, "write", &irata_component};
  try {
    write_control_decl.verify(&write_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(),
                HasSubstr("unable to cast component /write to Control"));
  }
}

TEST_F(ControlDeclTest, WriteControl_Validate_PhaseMismatch) {
  components::Control write_control_component("write", TickPhase::Read,
                                              &irata_component, true);
  try {
    write_control_decl.verify(&write_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(
        e.what(),
        HasSubstr("control phase Read does not match decl phase Write"));
  }
}

// No way to create autoclear mismatch due to invariants in Control constructor.

TEST_F(ControlDeclTest, WriteControl_Validate) {
  components::Control write_control_component("write", TickPhase::Write,
                                              &irata_component, true);
  EXPECT_NO_THROW(write_control_decl.verify(&write_control_component));
}

TEST_F(ControlDeclTest, ReadControl_Properties) {
  EXPECT_EQ(read_control_decl.name(), "read");
  EXPECT_EQ(read_control_decl.path(), "/read");
  EXPECT_EQ(read_control_decl.parent(), &irata_decl);
  EXPECT_EQ(read_control_decl.type(), ComponentType::Control);
  EXPECT_EQ(read_control_decl.phase(), TickPhase::Read);
  EXPECT_EQ(read_control_decl.auto_clear(), true);
}

TEST_F(ControlDeclTest, ReadControl_ToString) {
  std::ostringstream os;
  os << read_control_decl;
  EXPECT_EQ(os.str(), "Control(path = /read, phase = Read, auto_clear = true)");
}

TEST_F(ControlDeclTest, ReadControl_Validate_Null) {
  EXPECT_THROW(read_control_decl.verify(nullptr), std::invalid_argument);
}

TEST_F(ControlDeclTest, ReadControl_Validate_UnableToCast) {
  components::FakeComponent read_control_component = {ComponentType::Control,
                                                      "read", &irata_component};
  try {
    read_control_decl.verify(&read_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(),
                HasSubstr("unable to cast component /read to Control"));
  }
}

TEST_F(ControlDeclTest, ReadControl_Validate_PhaseMismatch) {
  components::Control read_control_component("read", TickPhase::Write,
                                             &irata_component, true);
  try {
    read_control_decl.verify(&read_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(
        e.what(),
        HasSubstr("control phase Write does not match decl phase Read"));
  }
}

TEST_F(ControlDeclTest, ReadControl_Validate) {
  components::Control read_control_component("read", TickPhase::Read,
                                             &irata_component, true);
  EXPECT_NO_THROW(read_control_decl.verify(&read_control_component));
}

TEST_F(ControlDeclTest, ProcessControl_Properties) {
  EXPECT_EQ(process_control_decl.name(), "process");
  EXPECT_EQ(process_control_decl.path(), "/process");
  EXPECT_EQ(process_control_decl.parent(), &irata_decl);
  EXPECT_EQ(process_control_decl.type(), ComponentType::Control);
  EXPECT_EQ(process_control_decl.phase(), TickPhase::Process);
  EXPECT_EQ(process_control_decl.auto_clear(), true);
}

TEST_F(ControlDeclTest, ProcessControl_ToString) {
  std::ostringstream os;
  os << process_control_decl;
  EXPECT_EQ(os.str(),
            "Control(path = /process, phase = Process, auto_clear = true)");
}

TEST_F(ControlDeclTest, ProcessControl_Validate_Null) {
  EXPECT_THROW(process_control_decl.verify(nullptr), std::invalid_argument);
}

TEST_F(ControlDeclTest, ProcessControl_Validate_UnableToCast) {
  components::FakeComponent process_control_component = {
      ComponentType::Control, "process", &irata_component};
  try {
    process_control_decl.verify(&process_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(),
                HasSubstr("unable to cast component /process to Control"));
  }
}

TEST_F(ControlDeclTest, ProcessControl_Validate_PhaseMismatch) {
  components::Control process_control_component("process", TickPhase::Write,
                                                &irata_component, true);
  try {
    process_control_decl.verify(&process_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(
        e.what(),
        HasSubstr("control phase Write does not match decl phase Process"));
  }
}

TEST_F(ControlDeclTest, ProcessControl_Validate_AutoclearMismatch) {
  components::Control process_control_component("process", TickPhase::Process,
                                                &irata_component, false);
  try {
    process_control_decl.verify(&process_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(
        e.what(),
        HasSubstr(
            "control auto_clear false does not match decl auto_clear true"));
  }
}

TEST_F(ControlDeclTest, ProcessControl_Validate) {
  components::Control process_control_component("process", TickPhase::Process,
                                                &irata_component, true);
  EXPECT_NO_THROW(process_control_decl.verify(&process_control_component));
}

TEST_F(ControlDeclTest, PersistentProcessControl_Properties) {
  EXPECT_EQ(persistent_process_control_decl.name(), "persistent_process");
  EXPECT_EQ(persistent_process_control_decl.path(), "/persistent_process");
  EXPECT_EQ(persistent_process_control_decl.parent(), &irata_decl);
  EXPECT_EQ(persistent_process_control_decl.type(), ComponentType::Control);
  EXPECT_EQ(persistent_process_control_decl.phase(), TickPhase::Process);
  EXPECT_EQ(persistent_process_control_decl.auto_clear(), false);
}

TEST_F(ControlDeclTest, PersistentProcessControl_ToString) {
  std::ostringstream os;
  os << persistent_process_control_decl;
  EXPECT_EQ(os.str(), "Control(path = /persistent_process, phase = Process, "
                      "auto_clear = false)");
}

TEST_F(ControlDeclTest, PersistentProcessControl_Validate_Null) {
  EXPECT_THROW(persistent_process_control_decl.verify(nullptr),
               std::invalid_argument);
}

TEST_F(ControlDeclTest, PersistentProcessControl_Validate_UnableToCast) {
  components::FakeComponent persistent_process_control_component = {
      ComponentType::Control, "persistent_process", &irata_component};
  try {
    persistent_process_control_decl.verify(
        &persistent_process_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("unable to cast component "
                                    "/persistent_process to Control"));
  }
}

// No way to create phase mismatch due to invariants in Control constructor.

TEST_F(ControlDeclTest, PersistentProcessControl_Validate_AutoclearMismatch) {
  components::Control persistent_process_control_component(
      "persistent_process", TickPhase::Process, &irata_component, true);
  try {
    persistent_process_control_decl.verify(
        &persistent_process_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("control auto_clear true does not "
                                    "match decl auto_clear false"));
  }
}

TEST_F(ControlDeclTest, PersistentProcessControl_Validate) {
  components::Control persistent_process_control_component(
      "persistent_process", TickPhase::Process, &irata_component, false);
  EXPECT_NO_THROW(persistent_process_control_decl.verify(
      &persistent_process_control_component));
}

TEST_F(ControlDeclTest, PersistentClearControl_Properties) {
  EXPECT_EQ(persistent_clear_control_decl.name(), "persistent_clear");
  EXPECT_EQ(persistent_clear_control_decl.path(), "/persistent_clear");
  EXPECT_EQ(persistent_clear_control_decl.parent(), &irata_decl);
  EXPECT_EQ(persistent_clear_control_decl.type(), ComponentType::Control);
  EXPECT_EQ(persistent_clear_control_decl.phase(), TickPhase::Clear);
  EXPECT_EQ(persistent_clear_control_decl.auto_clear(), false);
}

TEST_F(ControlDeclTest, PersistentClearControl_ToString) {
  std::ostringstream os;
  os << persistent_clear_control_decl;
  EXPECT_EQ(os.str(), "Control(path = /persistent_clear, phase = Clear, "
                      "auto_clear = false)");
}

TEST_F(ControlDeclTest, PersistentClearControl_Validate_Null) {
  EXPECT_THROW(persistent_clear_control_decl.verify(nullptr),
               std::invalid_argument);
}

TEST_F(ControlDeclTest, PersistentClearControl_Validate_UnableToCast) {
  components::FakeComponent persistent_clear_control_component = {
      ComponentType::Control, "persistent_clear", &irata_component};
  try {
    persistent_clear_control_decl.verify(&persistent_clear_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("unable to cast component "
                                    "/persistent_clear to Control"));
  }
}

TEST_F(ControlDeclTest, PersistentClearControl_Validate_PhaseMismatch) {
  components::Control persistent_clear_control_component(
      "persistent_clear", TickPhase::Process, &irata_component, false);
  try {
    persistent_clear_control_decl.verify(&persistent_clear_control_component);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("control phase Process does not "
                                    "match decl phase Clear"));
  }
}

// No way to create autoclear mismatch due to invariants in Control

TEST_F(ControlDeclTest, PersistentClearControl_Validate) {
  components::Control persistent_clear_control_component(
      "persistent_clear", TickPhase::Clear, &irata_component, false);
  EXPECT_NO_THROW(persistent_clear_control_decl.verify(
      &persistent_clear_control_component));
}

} // namespace irata::sim::hdl

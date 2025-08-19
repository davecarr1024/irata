#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/asm/instruction.hpp>
#include <irata/asm/instruction_set.hpp>
#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/hdl/memory_decl.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/dsl/step.hpp>
#include <stdexcept>

using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::Property;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

namespace irata::sim::microcode::dsl {

namespace {

class MicrocodeDslInstructionTest : public ::testing::Test {
protected:
  InstructionSet instruction_set_;
  const asm_::Instruction instruction_descriptor_ =
      asm_::InstructionSet::irata().get_instruction(
          "lda", asm_::AddressingMode::Immediate);

  static void AssertInstructionEndsWithSteps(
      const Instruction &instruction,
      std::vector<std::set<const hdl::ControlDecl *>> controls) {
    const auto expected_controls = control_paths(controls);
    std::vector<std::set<std::string>> actual_controls;
    tail_control_paths(instruction, controls.size(), actual_controls);
    EXPECT_EQ(actual_controls, expected_controls);
  }

  const hdl::StatusDecl status = {"status", hdl::irata()};

  const hdl::ComponentWithTypeDecl<hdl::ComponentType::Irata>
      different_tree_root = {"different_tree_root"};
  const hdl::ByteBusDecl different_tree_data_bus = {"different_tree_bus",
                                                    different_tree_root};
  const hdl::WordBusDecl different_tree_address_bus = {"different_tree_bus",
                                                       different_tree_root};
  const hdl::MemoryDecl different_tree_memory = {
      "different_tree_memory", different_tree_root, different_tree_address_bus,
      different_tree_data_bus};
  const hdl::ConnectedByteRegisterDecl different_tree_byte_register = {
      "different_tree_byte_register", different_tree_root,
      different_tree_data_bus};
  const hdl::ConnectedWordRegisterDecl different_tree_word_register = {
      "different_tree_word_register", different_tree_root,
      different_tree_address_bus};

  const hdl::ByteBusDecl different_byte_bus = {"different_byte_bus",
                                               hdl::irata()};
  const hdl::WordBusDecl different_word_bus = {"different_word_bus",
                                               hdl::irata()};
  const hdl::ConnectedByteRegisterDecl different_bus_byte_register = {
      "different_bus_byte_register", hdl::irata(), different_byte_bus};
  const hdl::ConnectedWordRegisterDecl different_bus_word_register = {
      "different_bus_word_register", hdl::irata(), different_word_bus};
  const hdl::MemoryDecl different_memory = {
      "different_memory", hdl::irata(), different_word_bus, different_byte_bus};

private:
  static void tail_control_paths(const Instruction &instruction, size_t count,
                                 std::vector<std::set<std::string>> &result) {
    ASSERT_GE(instruction.steps().size(), count);
    for (size_t i = instruction.steps().size() - count;
         i < instruction.steps().size(); ++i) {
      const auto &step = instruction.steps()[i];
      std::set<std::string> paths;
      for (const auto *control : step->controls()) {
        paths.insert(control->path());
      }
      result.push_back(paths);
    }
  }

  static std::vector<std::set<std::string>> control_paths(
      const std::vector<std::set<const hdl::ControlDecl *>> &controls) {
    std::vector<std::set<std::string>> result;
    for (const auto &step : controls) {
      std::set<std::string> paths;
      for (const auto *control : step) {
        paths.insert(control->path());
      }
      result.push_back(paths);
    }
    return result;
  }
};

} // namespace

TEST_F(MicrocodeDslInstructionTest, NullInstructionSet) {
  EXPECT_THROW(Instruction(instruction_descriptor_, nullptr),
               std::invalid_argument);
}

TEST_F(MicrocodeDslInstructionTest, CreateInstructionFromInstructionSet) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_);
  EXPECT_EQ(instruction->descriptor(), instruction_descriptor_);
  EXPECT_EQ(instruction->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_set_.instructions().back().get(), instruction);
  EXPECT_THAT(instruction->statuses(), IsEmpty());
  AssertInstructionEndsWithSteps(
      *instruction, {
                        {&hdl::irata().cpu().pc().write(),
                         &hdl::irata().memory().address().read()},
                        {&hdl::irata().memory().write(),
                         &hdl::irata().cpu().controller().opcode().read()},
                        {&hdl::irata().cpu().pc().increment()},
                    });
}

TEST_F(MicrocodeDslInstructionTest, CreateInstructionFromInstruction) {
  auto *instruction1 =
      instruction_set_.create_instruction(instruction_descriptor_);
  auto *instruction2 =
      instruction1->create_instruction(instruction_descriptor_);
  EXPECT_EQ(instruction2->descriptor(), instruction_descriptor_);
  EXPECT_EQ(instruction2->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_set_.instructions().back().get(), instruction2);
  EXPECT_THAT(instruction2->statuses(), IsEmpty());
}

TEST_F(MicrocodeDslInstructionTest,
       CreateInstructionFromInstruction_WithNameAndAddressingMode) {
  auto *instruction1 =
      instruction_set_.create_instruction(instruction_descriptor_);
  auto *instruction2 = instruction1->create_instruction(
      instruction_descriptor_.name(),
      instruction_descriptor_.addressing_mode());
  EXPECT_EQ(instruction2->descriptor(), instruction_descriptor_);
  EXPECT_EQ(instruction2->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction_set_.instructions().back().get(), instruction2);
  EXPECT_THAT(instruction2->statuses(), IsEmpty());
}

TEST_F(MicrocodeDslInstructionTest, CreateStep) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_);
  auto *step = instruction->create_step();
  EXPECT_EQ(step->instruction(), instruction);
  EXPECT_EQ(step->instruction_set(), &instruction_set_);
  EXPECT_EQ(instruction->steps().back().get(), step);
}

TEST_F(MicrocodeDslInstructionTest, WithStatus) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_);
  instruction->with_status(status, true);
  EXPECT_THAT(instruction->statuses(),
              UnorderedElementsAre(Pair(&status, true)));
}

TEST_F(MicrocodeDslInstructionTest, WithStatusConflict) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_);
  instruction->with_status(status, true);
  EXPECT_THROW(instruction->with_status(status, false), std::invalid_argument);
}

using StepControlSet = std::vector<std::vector<const hdl::ControlDecl *>>;

TEST_F(MicrocodeDslInstructionTest, CopyByteRegisterToByteRegister) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().x());
  AssertInstructionEndsWithSteps(
      *instruction,
      {{&hdl::irata().cpu().a().write(), &hdl::irata().cpu().x().read()}});
  const auto &step = instruction->steps().back();
  EXPECT_THAT(step->controls(),
              UnorderedElementsAre(&hdl::irata().cpu().a().write(),
                                   &hdl::irata().cpu().x().read()));
}

TEST_F(MicrocodeDslInstructionTest, CopyByteRegisterToByteRegisterSame) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().a(), hdl::irata().cpu().a());
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components cannot be the same"));
  }
}

TEST_F(MicrocodeDslInstructionTest,
       CopyByteRegisterToByteRegisterDifferentTree) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().a(), different_tree_byte_register);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components must be in the same tree"));
  }
}

TEST_F(MicrocodeDslInstructionTest,
       CopyByteRegisterToByteRegisterDifferentBus) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().a(), different_bus_byte_register);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("source and dest must be on the same bus"));
  }
}

TEST_F(MicrocodeDslInstructionTest, CopyWordRegisterToWordRegister) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->copy(hdl::irata().cpu().pc(), hdl::irata().memory().address());
  AssertInstructionEndsWithSteps(*instruction,
                                 {{&hdl::irata().cpu().pc().write(),
                                   &hdl::irata().memory().address().read()}});
}

TEST_F(MicrocodeDslInstructionTest, CopyWordRegisterToWordRegisterSame) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().pc(), hdl::irata().cpu().pc());
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components cannot be the same"));
  }
}

TEST_F(MicrocodeDslInstructionTest,
       CopyWordRegisterToWordRegisterDifferentTree) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().pc(), different_tree_word_register);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components must be in the same tree"));
  }
}

TEST_F(MicrocodeDslInstructionTest,
       CopyWordRegisterToWordRegisterDifferentBus) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().pc(), different_bus_word_register);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("source and dest must be on the same bus"));
  }
}

TEST_F(MicrocodeDslInstructionTest, CopyMemoryToByteRegister) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->copy(hdl::irata().memory(), hdl::irata().cpu().a());
  const auto &step = instruction->steps().back();
  EXPECT_THAT(step->controls(),
              UnorderedElementsAre(&hdl::irata().memory().write(),
                                   &hdl::irata().cpu().a().read()));
}

TEST_F(MicrocodeDslInstructionTest, CopyMemoryToByteRegisterDifferentTree) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(different_tree_memory, hdl::irata().cpu().a());
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components must be in the same tree"));
  }
}

TEST_F(MicrocodeDslInstructionTest, CopyMemoryToByteRegisterDifferentBus) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(different_memory, hdl::irata().cpu().a());
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("source and dest must be on the same bus"));
  }
}

TEST_F(MicrocodeDslInstructionTest, CopyByteRegisterToMemory) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->copy(hdl::irata().cpu().a(), hdl::irata().memory());
  const auto &step = instruction->steps().back();
  EXPECT_THAT(step->controls(),
              UnorderedElementsAre(&hdl::irata().cpu().a().write(),
                                   &hdl::irata().memory().read()));
}

TEST_F(MicrocodeDslInstructionTest, CopyByteRegisterToMemoryDifferentTree) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().a(), different_tree_memory);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("components must be in the same tree"));
  }
}

TEST_F(MicrocodeDslInstructionTest, CopyByteRegisterToMemoryDifferentBus) {
  try {
    instruction_set_.create_instruction(instruction_descriptor_)
        ->copy(hdl::irata().cpu().a(), different_memory);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument &e) {
    EXPECT_THAT(e.what(), HasSubstr("source and dest must be on the same bus"));
  }
}

TEST_F(MicrocodeDslInstructionTest, ReadMemory) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->read_memory(hdl::irata().cpu().pc(), hdl::irata().cpu().a());
  AssertInstructionEndsWithSteps(
      *instruction,
      {{&hdl::irata().cpu().pc().write(),
        &hdl::irata().memory().address().read()},
       {&hdl::irata().memory().write(), &hdl::irata().cpu().a().read()}});
}

TEST_F(MicrocodeDslInstructionTest, ReadMemoryAtPc) {
  auto *instruction =
      instruction_set_.create_instruction(instruction_descriptor_)
          ->read_memory_at_pc(hdl::irata().cpu().a());
  AssertInstructionEndsWithSteps(
      *instruction,
      {{&hdl::irata().cpu().pc().write(),
        &hdl::irata().memory().address().read()},
       {&hdl::irata().memory().write(), &hdl::irata().cpu().a().read()},
       {&hdl::irata().cpu().pc().increment()}});
}

} // namespace irata::sim::microcode::dsl

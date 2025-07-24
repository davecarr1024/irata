// #include <gmock/gmock.h>
// #include <gtest/gtest.h>
// #include <irata/asm/asm.hpp>
// #include <irata/sim/microcode/compiler/compiler.hpp>
// #include <irata/sim/microcode/dsl/instruction.hpp>
// #include <irata/sim/microcode/dsl/instruction_set.hpp>
// #include <irata/sim/microcode/dsl/step.hpp>
// #include <irata/sim/microcode/table/table.hpp>
// #include <stdexcept>

// using ::testing::AllOf;
// using ::testing::ElementsAre;
// using ::testing::Field;
// using ::testing::IsEmpty;
// using ::testing::Pair;
// using ::testing::UnorderedElementsAre;

// namespace irata::sim::microcode::compiler {

// class CompilerTest : public ::testing::Test {
// protected:
//   template <typename Matcher>
//   static auto TableHasInstructions(Matcher matcher) {
//     return Field("instructions", &table::Table::instructions, matcher);
//   }

//   template <typename Matcher>
//   static auto InstructionHasDescriptor(Matcher matcher) {
//     return Field("descriptor", &table::Instruction::descriptor, matcher);
//   }

//   template <typename Matcher>
//   static auto InstructionHasVariants(Matcher matcher) {
//     return Field("variants", &table::Instruction::variants, matcher);
//   }

//   template <typename Matcher>
//   static auto InstructionVariantHasSteps(Matcher matcher) {
//     return Field("steps", &table::InstructionVariant::steps, matcher);
//   }

//   template <typename Matcher>
//   static auto InstructionVariantHasStatuses(Matcher matcher) {
//     return Field("statuses", &table::InstructionVariant::statuses, matcher);
//   }

//   template <typename Matcher> static auto StepHasControls(Matcher matcher) {
//     return Field("controls", &table::Step::controls, matcher);
//   }

//   template <typename Matcher>
//   static auto InstructionHasOneVariantWithSteps(Matcher matcher) {
//     return InstructionHasVariants(
//         UnorderedElementsAre(AllOf(InstructionVariantHasSteps(matcher),
//                                    InstructionVariantHasStatuses(IsEmpty()))));
//   }
// };

// TEST_F(CompilerTest, EmptyInstructionSet) {
//   const dsl::InstructionSet instruction_set;
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table.instructions, IsEmpty());
// }

// TEST_F(CompilerTest, EmptyInstruction) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor);
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table.instructions,
//               UnorderedElementsAre(InstructionHasDescriptor(descriptor)));
// }

// TEST_F(CompilerTest, NonBranchingInstuction) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   dsl::Instruction *instruction =
//       instruction_set.create_instruction(descriptor);
//   instruction->create_step()
//       ->with_control(hdl::irata().cpu().a().read())
//       ->with_control(hdl::irata().memory().write());
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table.instructions,
//               UnorderedElementsAre(AllOf(
//                   InstructionHasDescriptor(descriptor),
//                   InstructionHasOneVariantWithSteps(ElementsAre(StepHasControls(
//                       ElementsAre(hdl::irata().cpu().a().read().path(),
//                                   hdl::irata().memory().write().path())))))));
// }

// TEST_F(CompilerTest, OpenBus) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   dsl::Instruction *instruction =
//       instruction_set.create_instruction(descriptor);
//   instruction->create_step()->with_control(hdl::irata().cpu().a().read());
//   EXPECT_THROW(compile(instruction_set), std::invalid_argument);
// }

// TEST_F(CompilerTest, WriteConflict) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   dsl::Instruction *instruction =
//       instruction_set.create_instruction(descriptor);
//   instruction->create_step()
//       ->with_control(hdl::irata().cpu().a().write())
//       ->with_control(hdl::irata().cpu().x().write())
//       ->with_control(hdl::irata().cpu().y().read());
//   EXPECT_THROW(compile(instruction_set), std::invalid_argument);
// }

// TEST_F(CompilerTest, NoReaders) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   dsl::Instruction *instruction =
//       instruction_set.create_instruction(descriptor);
//   instruction->create_step()->with_control(hdl::irata().cpu().a().write());
//   EXPECT_THROW(compile(instruction_set), std::invalid_argument);
// }

// TEST_F(CompilerTest, BranchingInstruction) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   const hdl::StatusDecl status("status", hdl::irata());
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->with_status(status, true)
//       ->create_step()
//       ->with_control(hdl::irata().cpu().a().read())
//       ->with_control(hdl::irata().memory().write());
//   instruction_set.create_instruction(descriptor)->with_status(status, false);
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(
//       table,
//       TableHasInstructions(UnorderedElementsAre(AllOf(
//           InstructionHasDescriptor(descriptor),
//           InstructionHasVariants(UnorderedElementsAre(
//               AllOf(InstructionVariantHasSteps(ElementsAre(StepHasControls(
//                         ElementsAre(hdl::irata().cpu().a().read().path(),
//                                     hdl::irata().memory().write().path())))),
//                     InstructionVariantHasStatuses(
//                         UnorderedElementsAre(Pair(status.path(), true)))),
//               AllOf(InstructionVariantHasSteps(IsEmpty()),
//                     InstructionVariantHasStatuses(UnorderedElementsAre(
//                         Pair(status.path(), false))))))))));
// }

// TEST_F(CompilerTest, NonComplementaryVariants) {
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   const hdl::StatusDecl status("status", hdl::irata());
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)->with_status(status, true);
//   EXPECT_THROW(compile(instruction_set), std::invalid_argument);
// }

// TEST_F(CompilerTest, MergeInOrderSteps) {
//   // Merging steps that are in order is fine, since they don't conflict.
//   // In this example, the process control must be asserted before the clear
//   // control, so their effect is the same whether they are in the same step
//   // or different steps.
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   const hdl::ProcessControlDecl process_control("process", hdl::irata());
//   const hdl::ClearControlDecl clear_control("clear", hdl::irata());
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->create_step()
//       ->with_control(process_control)
//       ->create_step()
//       ->with_control(clear_control);
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table,
//               TableHasInstructions(UnorderedElementsAre(AllOf(
//                   InstructionHasDescriptor(descriptor),
//                   InstructionHasOneVariantWithSteps(
//                       ElementsAre(StepHasControls(UnorderedElementsAre(
//                           process_control.path(),
//                           clear_control.path()))))))));
// }

// TEST_F(CompilerTest, DontMergeOutOfOrderSteps) {
//   // Don't try to automatically merge steps that are out of order, since this
//   // violates the microcode execution order. In this example, the clear
//   control
//   // must be asserted after the process control, so they can't be merged into
//   // the same step. If they were merged, the clear control would be asserted
//   // before the process control, which is not the intended behavior.
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   const hdl::ProcessControlDecl process_control("process", hdl::irata());
//   const hdl::ClearControlDecl clear_control("clear", hdl::irata());
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->create_step()
//       ->with_control(clear_control)
//       ->create_step()
//       ->with_control(process_control);
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(
//       table,
//       TableHasInstructions(UnorderedElementsAre(
//           AllOf(InstructionHasDescriptor(descriptor),
//                 InstructionHasOneVariantWithSteps(ElementsAre(
//                     StepHasControls(UnorderedElementsAre(clear_control.path())),
//                     StepHasControls(
//                         UnorderedElementsAre(process_control.path()))))))));
// }

// TEST_F(CompilerTest, MergeSimultaneousSteps) {
//   // Merging steps that are simultaneous is fine, since they don't conflict.
//   // In this example, both steps use a control in the process phase, so they
//   // can be merged since all events during the process phase are considered
//   // simultaneous.
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   const hdl::ProcessControlDecl process_control1("process1", hdl::irata());
//   const hdl::ProcessControlDecl process_control2("process2", hdl::irata());
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->create_step()
//       ->with_control(process_control1)
//       ->create_step()
//       ->with_control(process_control2);
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table,
//               TableHasInstructions(UnorderedElementsAre(AllOf(
//                   InstructionHasDescriptor(descriptor),
//                   InstructionHasOneVariantWithSteps(ElementsAre(StepHasControls(
//                       UnorderedElementsAre(process_control1.path(),
//                                            process_control2.path()))))))));
// }

// TEST_F(CompilerTest, MergeBusStepsWithNonBusSteps) {
//   // Merging a bus move with a non-bus step is fine, since the bus move
//   // doesn't conflict with the non-bus step. In this example, copy pc to mar
//   // and then increment pc. This is ok since the copy is done in the write
//   and
//   // read phases, and the increment is done in the process phase. So it's the
//   // same to do them in the same step or different steps.
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->create_step()
//       ->with_control(hdl::irata().cpu().program_counter().write())
//       ->with_control(hdl::irata().memory().address_register().read())
//       ->create_step()
//       ->with_control(hdl::irata().cpu().program_counter().increment());
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(
//       table, TableHasInstructions(UnorderedElementsAre(AllOf(
//                  InstructionHasDescriptor(descriptor),
//                  InstructionHasOneVariantWithSteps(
//                      ElementsAre(StepHasControls(UnorderedElementsAre(
//                          hdl::irata().cpu().program_counter().write().path(),
//                          hdl::irata().memory().address_register().read().path(),
//                          hdl::irata()
//                              .cpu()
//                              .program_counter()
//                              .increment()
//                              .path()))))))));
// }

// TEST_F(CompilerTest, DontMergeBusMoves) {
//   // Don't try to automatically merge bus moves, since this is technically
//   // possible as a microcode programmer but requires a lot of care to ensure
//   // correctness. In this example, copy a to x on the data bus and pc to mar
//   on
//   // the address bus. This is theoretically possible since it doesn't require
//   // any bus conflicts, but it's not something that should be automatically
//   // merged since there could potentailly be some other subtle interaction.
//   const auto descriptor = asm_::Instruction{.name = "instruction"};
//   dsl::InstructionSet instruction_set;
//   instruction_set.create_instruction(descriptor)
//       ->create_step()
//       ->with_control(hdl::irata().cpu().a().write())
//       ->with_control(hdl::irata().cpu().x().read())
//       ->create_step()
//       ->with_control(hdl::irata().cpu().program_counter().write())
//       ->with_control(hdl::irata().memory().address_register().read());
//   const auto table = compile(instruction_set);
//   EXPECT_THAT(table,
//               TableHasInstructions(UnorderedElementsAre(AllOf(
//                   InstructionHasDescriptor(descriptor),
//                   InstructionHasOneVariantWithSteps(ElementsAre(
//                       StepHasControls(UnorderedElementsAre(
//                           hdl::irata().cpu().a().write().path(),
//                           hdl::irata().cpu().x().read().path())),
//                       StepHasControls(UnorderedElementsAre(
//                           hdl::irata().cpu().program_counter().write().path(),
//                           hdl::irata()
//                               .memory()
//                               .address_register()
//                               .read()
//                               .path()))))))));
// }

// } // namespace irata::sim::microcode::compiler

#include <irata/sim/hdl/irata_decl.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction_set.hpp>

#ifdef __has_include
#if __has_include(<sanitizer/lsan_interface.h>)
#include <sanitizer/lsan_interface.h>
#define HAS_LSAN 1
#endif
#endif

namespace irata::sim::microcode::dsl {

namespace {

// Create an instruction for an immediate mode ALU binary operation.
InstructionSet *immediate_alu_op(const hdl::IrataDecl &irata_hdl,
                                 InstructionSet *instruction_set,
                                 std::string_view name, hdl::AluOpcode opcode) {
  return instruction_set
      ->create_instruction(name, asm_::AddressingMode::Immediate)
      ->read_memory_at_pc(irata_hdl.cpu().alu().rhs())
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().alu().lhs())
      ->alu_operation(opcode)
      ->copy(irata_hdl.cpu().alu().result(), irata_hdl.cpu().a())
      ->instruction_set();
}

// Create an instruction for an absolute mode ALU binary operation.
InstructionSet *absolute_alu_op(const hdl::IrataDecl &irata_hdl,
                                InstructionSet *instruction_set,
                                std::string_view name, hdl::AluOpcode opcode) {
  return instruction_set
      ->create_instruction(name, asm_::AddressingMode::Absolute)
      ->indirect_read_memory_at_pc(irata_hdl.cpu().alu().rhs())
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().alu().lhs())
      ->alu_operation(opcode)
      ->copy(irata_hdl.cpu().alu().result(), irata_hdl.cpu().a())
      ->instruction_set();
}

// Create an instruction for a zero page mode ALU binary operation.
InstructionSet *zero_page_alu_op(const hdl::IrataDecl &irata_hdl,
                                 InstructionSet *instruction_set,
                                 std::string_view name, hdl::AluOpcode opcode) {
  return instruction_set
      ->create_instruction(name, asm_::AddressingMode::ZeroPage)
      ->indirect_read_memory_zero_page_at_pc(irata_hdl.cpu().alu().rhs())
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().alu().lhs())
      ->alu_operation(opcode)
      ->copy(irata_hdl.cpu().alu().result(), irata_hdl.cpu().a())
      ->instruction_set();
}

// Create instructions for an ALU binary operation in all addressing modes.
InstructionSet *alu_op(const hdl::IrataDecl &irata_hdl,
                       InstructionSet *instruction_set, std::string_view name,
                       hdl::AluOpcode opcode) {
  instruction_set = immediate_alu_op(irata_hdl, instruction_set, name, opcode);
  instruction_set = absolute_alu_op(irata_hdl, instruction_set, name, opcode);
  instruction_set = zero_page_alu_op(irata_hdl, instruction_set, name, opcode);
  return instruction_set;
}

// Returns the name of a load operation for a register.
std::string load_op_name(const hdl::RegisterDecl &reg) {
  return "ld" + reg.name();
}

// Create an instruction for an immediate mode load operation.
InstructionSet *immediate_load_op(InstructionSet *instruction_set,
                                  const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::Immediate)
      ->read_memory_at_pc(reg)
      ->instruction_set();
}

// Create an instruction for an absolute mode load operation.
InstructionSet *absolute_load_op(InstructionSet *instruction_set,
                                 const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::Absolute)
      ->indirect_read_memory_at_pc(reg)
      ->instruction_set();
}

// Create an instruction for a zero page mode load operation.
InstructionSet *zero_page_load_op(InstructionSet *instruction_set,
                                  const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::ZeroPage)
      ->indirect_read_memory_zero_page_at_pc(reg)
      ->instruction_set();
}

// Create an instruction for a zero page indexed by x mode load operation.
InstructionSet *zero_page_x_load_op(const hdl::IrataDecl &irata_hdl,
                                    InstructionSet *instruction_set,
                                    const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::ZeroPageX)
      ->read_memory_zero_page_indexed(irata_hdl.cpu().x(), reg)
      ->instruction_set();
}

// Create an instruction for a zero page indexed by y mode load operation.
InstructionSet *zero_page_y_load_op(const hdl::IrataDecl &irata_hdl,
                                    InstructionSet *instruction_set,
                                    const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::ZeroPageY)
      ->read_memory_zero_page_indexed(irata_hdl.cpu().y(), reg)
      ->instruction_set();
}

// Create an instruction for an absolute indexed by x mode load operation.
InstructionSet *absolute_x_load_op(const hdl::IrataDecl &irata_hdl,
                                   InstructionSet *instruction_set,
                                   const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::AbsoluteX)
      ->read_memory_absolute_indexed(irata_hdl.cpu().x(), reg)
      ->instruction_set();
}

// Create an instruction for an absolute indexed by y mode load operation.
InstructionSet *absolute_y_load_op(const hdl::IrataDecl &irata_hdl,
                                   InstructionSet *instruction_set,
                                   const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(load_op_name(reg), asm_::AddressingMode::AbsoluteY)
      ->read_memory_absolute_indexed(irata_hdl.cpu().y(), reg)
      ->instruction_set();
}

// Create instructions for a load operation in all addressing modes.
InstructionSet *load_op(InstructionSet *instruction_set,
                        const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = immediate_load_op(instruction_set, reg);
  instruction_set = absolute_load_op(instruction_set, reg);
  instruction_set = zero_page_load_op(instruction_set, reg);
  return instruction_set;
}

// Create instructions for indexed load operations.
InstructionSet *indexed_load_op(const hdl::IrataDecl &irata_hdl,
                                InstructionSet *instruction_set,
                                const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = zero_page_x_load_op(irata_hdl, instruction_set, reg);
  instruction_set = zero_page_y_load_op(irata_hdl, instruction_set, reg);
  instruction_set = absolute_x_load_op(irata_hdl, instruction_set, reg);
  instruction_set = absolute_y_load_op(irata_hdl, instruction_set, reg);
  return instruction_set;
}

// Returns the name of a store operation for a register.
std::string store_op_name(const hdl::RegisterDecl &reg) {
  return "st" + reg.name();
}

// Create an instruction for an absolute mode store operation.
InstructionSet *absolute_store_op(InstructionSet *instruction_set,
                                  const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::Absolute)
      ->indirect_write_memory_at_pc(reg)
      ->instruction_set();
}

// Create an instruction for a zero page mode store operation.
InstructionSet *zero_page_store_op(InstructionSet *instruction_set,
                                   const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::ZeroPage)
      ->indirect_write_memory_zero_page_at_pc(reg)
      ->instruction_set();
}

InstructionSet *zero_page_x_store_op(const hdl::IrataDecl &irata_hdl,
                                     InstructionSet *instruction_set,
                                     const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::ZeroPageX)
      ->write_memory_zero_page_indexed(irata_hdl.cpu().x(), reg)
      ->instruction_set();
}

InstructionSet *zero_page_y_store_op(const hdl::IrataDecl &irata_hdl,
                                     InstructionSet *instruction_set,
                                     const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::ZeroPageY)
      ->write_memory_zero_page_indexed(irata_hdl.cpu().y(), reg)
      ->instruction_set();
}

InstructionSet *absolute_x_store_op(const hdl::IrataDecl &irata_hdl,
                                    InstructionSet *instruction_set,
                                    const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::AbsoluteX)
      ->write_memory_absolute_indexed(irata_hdl.cpu().x(), reg)
      ->instruction_set();
}

InstructionSet *absolute_y_store_op(const hdl::IrataDecl &irata_hdl,
                                    InstructionSet *instruction_set,
                                    const hdl::RegisterWithByteBusDecl &reg) {
  return instruction_set
      ->create_instruction(store_op_name(reg), asm_::AddressingMode::AbsoluteY)
      ->write_memory_absolute_indexed(irata_hdl.cpu().y(), reg)
      ->instruction_set();
}

InstructionSet *indexed_store_op(const hdl::IrataDecl &irata_hdl,
                                 InstructionSet *instruction_set,
                                 const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = zero_page_x_store_op(irata_hdl, instruction_set, reg);
  instruction_set = zero_page_y_store_op(irata_hdl, instruction_set, reg);
  instruction_set = absolute_x_store_op(irata_hdl, instruction_set, reg);
  instruction_set = absolute_y_store_op(irata_hdl, instruction_set, reg);
  return instruction_set;
}

// Create instructions for a store operation in all addressing modes.
InstructionSet *store_op(InstructionSet *instruction_set,
                         const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = absolute_store_op(instruction_set, reg);
  instruction_set = zero_page_store_op(instruction_set, reg);
  return instruction_set;
}

// Create instructions for a load and store operation in all addressing modes.
InstructionSet *load_and_store_op(const hdl::IrataDecl &irata_hdl,
                                  InstructionSet *instruction_set,
                                  const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = load_op(instruction_set, reg);
  instruction_set = store_op(instruction_set, reg);
  return instruction_set;
}

// Create instructions for an indexed load and store operation.
InstructionSet *
indexed_load_and_store_op(const hdl::IrataDecl &irata_hdl,
                          InstructionSet *instruction_set,
                          const hdl::RegisterWithByteBusDecl &reg) {
  instruction_set = indexed_load_op(irata_hdl, instruction_set, reg);
  instruction_set = indexed_store_op(irata_hdl, instruction_set, reg);
  return instruction_set;
}

// Create an instruction for a unary ALU operation.
InstructionSet *unary_alu_op(const hdl::IrataDecl &irata_hdl,
                             InstructionSet *instruction_set,
                             std::string_view name, hdl::AluOpcode opcode) {
  return instruction_set->create_instruction(name, asm_::AddressingMode::None)
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().alu().lhs())
      ->alu_operation(opcode)
      ->copy(irata_hdl.cpu().alu().result(), irata_hdl.cpu().a())
      ->instruction_set();
}

// Create an instruction for a branch operation.
// Two instructions are created: one for taking the branch and one for not
// taking the branch. The branch is taken if the status flag is equal to
// branch_status_value, and not taken otherwise.
InstructionSet *branch_op(const hdl::IrataDecl &irata_hdl,
                          InstructionSet *instruction_set,
                          std::string_view name, const hdl::StatusDecl &status,
                          bool branch_status_value) {
  return instruction_set
      ->create_instruction(name, asm_::AddressingMode::Absolute)
      // Non-branch version. Just move forward to the next instruction by
      // incrementing the PC twice.
      ->with_status(status, !branch_status_value)
      ->with_control(irata_hdl.cpu().pc().increment())
      // Create a separate instruction stage to prevent merging the two
      // increment controls.
      ->next_stage()
      ->with_control(irata_hdl.cpu().pc().increment())
      // Branch version. Read the address from the program and jump to it.
      ->create_instruction(name, asm_::AddressingMode::Absolute)
      ->with_status(status, branch_status_value)
      ->read_word_at_pc(irata_hdl.cpu().pc())
      ->instruction_set();
}

std::unique_ptr<const InstructionSet>
build_irata(const hdl::IrataDecl &irata_hdl) {
  auto instruction_set_ptr = std::make_unique<InstructionSet>();
  auto *instruction_set = instruction_set_ptr.get();

  instruction_set->create_instruction("hlt", asm_::AddressingMode::None)
      ->with_control(irata_hdl.halt());

  instruction_set->create_instruction("crs", asm_::AddressingMode::None)
      ->with_control(irata_hdl.crash());

  instruction_set->create_instruction("nop", asm_::AddressingMode::None)
      ->create_step();

  instruction_set =
      load_and_store_op(irata_hdl, instruction_set, irata_hdl.cpu().a());
  instruction_set = indexed_load_and_store_op(irata_hdl, instruction_set,
                                              irata_hdl.cpu().a());
  instruction_set =
      load_and_store_op(irata_hdl, instruction_set, irata_hdl.cpu().x());
  instruction_set =
      load_and_store_op(irata_hdl, instruction_set, irata_hdl.cpu().y());

  instruction_set->create_instruction("tax", asm_::AddressingMode::None)
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().x());
  instruction_set->create_instruction("txa", asm_::AddressingMode::None)
      ->copy(irata_hdl.cpu().x(), irata_hdl.cpu().a());

  instruction_set->create_instruction("tay", asm_::AddressingMode::None)
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().y());
  instruction_set->create_instruction("tya", asm_::AddressingMode::None)
      ->copy(irata_hdl.cpu().y(), irata_hdl.cpu().a());

  instruction_set->create_instruction("inx", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().x().increment());
  instruction_set->create_instruction("dex", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().x().decrement());

  instruction_set->create_instruction("iny", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().y().increment());
  instruction_set->create_instruction("dey", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().y().decrement());

  instruction_set->create_instruction("cmp", asm_::AddressingMode::Immediate)
      ->read_memory_at_pc(irata_hdl.cpu().alu().lhs())
      ->with_control(irata_hdl.cpu().status_register().set_carry())
      ->copy(irata_hdl.cpu().a(), irata_hdl.cpu().alu().rhs())
      ->alu_operation(hdl::AluOpcode::Subtract);

  instruction_set =
      alu_op(irata_hdl, instruction_set, "adc", hdl::AluOpcode::Add);
  instruction_set =
      alu_op(irata_hdl, instruction_set, "sbc", hdl::AluOpcode::Subtract);
  instruction_set =
      alu_op(irata_hdl, instruction_set, "and", hdl::AluOpcode::And);
  instruction_set =
      alu_op(irata_hdl, instruction_set, "ora", hdl::AluOpcode::Or);
  instruction_set =
      alu_op(irata_hdl, instruction_set, "eor", hdl::AluOpcode::Xor);

  instruction_set = unary_alu_op(irata_hdl, instruction_set, "asl",
                                 hdl::AluOpcode::ShiftLeft);
  instruction_set = unary_alu_op(irata_hdl, instruction_set, "lsr",
                                 hdl::AluOpcode::ShiftRight);
  instruction_set = unary_alu_op(irata_hdl, instruction_set, "rol",
                                 hdl::AluOpcode::RotateLeft);
  instruction_set = unary_alu_op(irata_hdl, instruction_set, "ror",
                                 hdl::AluOpcode::RotateRight);

  instruction_set->create_instruction("jmp", asm_::AddressingMode::Absolute)
      ->read_word_at_pc(irata_hdl.cpu().pc());

  instruction_set =
      branch_op(irata_hdl, instruction_set, "jne",
                irata_hdl.cpu().status_register().zero_out(), false);
  instruction_set =
      branch_op(irata_hdl, instruction_set, "jeq",
                irata_hdl.cpu().status_register().zero_out(), true);

  instruction_set->create_instruction("sec", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().status_register().set_carry());

  instruction_set->create_instruction("clc", asm_::AddressingMode::None)
      ->with_control(irata_hdl.cpu().status_register().clear_carry());

  instruction_set->create_instruction("pha", asm_::AddressingMode::None)
      ->push(irata_hdl.cpu().a());
  instruction_set->create_instruction("pla", asm_::AddressingMode::None)
      ->pop(irata_hdl.cpu().a());

  instruction_set->create_instruction("phx", asm_::AddressingMode::None)
      ->push(irata_hdl.cpu().x());
  instruction_set->create_instruction("plx", asm_::AddressingMode::None)
      ->pop(irata_hdl.cpu().x());

  instruction_set->create_instruction("phy", asm_::AddressingMode::None)
      ->push(irata_hdl.cpu().y());
  instruction_set->create_instruction("ply", asm_::AddressingMode::None)
      ->pop(irata_hdl.cpu().y());

  instruction_set->create_instruction("php", asm_::AddressingMode::None)
      ->push(irata_hdl.cpu().status_register());
  instruction_set->create_instruction("plp", asm_::AddressingMode::None)
      ->pop(irata_hdl.cpu().status_register());

  instruction_set->create_instruction("jsr", asm_::AddressingMode::Absolute)
      ->read_memory_at_pc(irata_hdl.cpu().buffer().high())
      ->read_memory_at_pc(irata_hdl.cpu().buffer().low())
      ->push(irata_hdl.cpu().pc().high())
      ->push(irata_hdl.cpu().pc().low())
      ->copy(irata_hdl.cpu().buffer(), irata_hdl.cpu().pc());

  instruction_set->create_instruction("rts", asm_::AddressingMode::None)
      ->pop(irata_hdl.cpu().pc().low())
      ->pop(irata_hdl.cpu().pc().high());

  return instruction_set_ptr;
}

} // namespace

const InstructionSet &InstructionSet::irata() {
  // Initialize hdl::irata() FIRST to avoid re-entrant static initialization
  // This ensures the HDL singleton is fully constructed before build_irata()
  // runs
  const auto &irata_hdl = hdl::irata();
  // Use .release() to intentionally leak memory and avoid static destruction
  // order issues This singleton's lifetime should extend to program termination
  static const auto *instruction_set = build_irata(irata_hdl).release();
#ifdef HAS_LSAN
  __lsan_ignore_object(instruction_set);
#endif
  return *instruction_set;
}

} // namespace irata::sim::microcode::dsl
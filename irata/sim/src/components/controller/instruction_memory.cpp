#include <irata/sim/components/controller/instruction_memory.hpp>

namespace irata::sim::components::controller {

namespace {

std::map<Word, Byte> encode_rom(const InstructionEncoder &encoder,
                                const microcode::table::Table &table,
                                size_t value_offset) {
  std::map<Word, Byte> rom;
  for (const auto &entry : table.entries) {
    for (const auto &address : encoder.encode_address(entry)) {
      const auto value = encoder.encode_value(entry);
      const auto word_address = Word(address);
      if (rom.find(word_address) != rom.end()) {
        std::ostringstream os;
        os << "Duplicate address " << std::hex << address << " for entry "
           << entry << " in instruction memory";
        throw std::logic_error(os.str());
      }
      rom[word_address] = Byte((value >> value_offset) & 0xFF);
    }
  }
  return rom;
}

} // namespace

InstructionMemory::InstructionMemory(const microcode::table::Table &table,
                                     std::string_view name, Component *parent)
    : Component(name, parent), encoder_(table),
      high_rom_(1 << 16, "high_rom", encode_rom(encoder_, table, 8)),
      low_rom_(1 << 16, "low_rom", encode_rom(encoder_, table, 0)) {
  add_child(&high_rom_);
  add_child(&low_rom_);
}

uint16_t InstructionMemory::read(uint16_t address) const {
  const auto high = high_rom_.read(Word(address)).unsigned_value();
  const auto low = low_rom_.read(Word(address)).unsigned_value();
  return (high << 8) | low;
}

std::set<const hdl::ControlDecl *>
InstructionMemory::read(uint8_t opcode, const CompleteStatuses &statuses,
                        uint8_t step_index) const {
  const auto address = encoder_.encode_address(opcode, statuses, step_index);
  return encoder_.decode_value(read(address));
}

std::set<const hdl::ControlDecl *>
InstructionMemory::read(const asm_::Instruction &instruction,
                        const CompleteStatuses &statuses,
                        uint8_t step_index) const {
  return read(instruction.opcode().unsigned_value(), statuses, step_index);
}

const InstructionEncoder &InstructionMemory::encoder() const {
  return encoder_;
}

} // namespace irata::sim::components::controller

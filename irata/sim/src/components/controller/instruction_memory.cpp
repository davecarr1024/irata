#include <irata/common/strings/strings.hpp>
#include <irata/sim/components/controller/instruction_memory.hpp>

namespace irata::sim::components::controller {

namespace {

memory::ROM encode_rom(const InstructionEncoder &encoder,
                       const microcode::table::Table &table, size_t rom_index) {
  std::map<Word, Byte> data;
  for (const auto &entry : table.entries) {
    for (const auto &address : encoder.encode_address(entry)) {
      const auto value = encoder.encode_value(entry);
      if (data.find(address) != data.end()) {
        std::ostringstream os;
        os << "Duplicate address " << std::hex << address << " for entry "
           << entry << " in instruction memory";
        throw std::logic_error(os.str());
      }
      data[address] = Byte((value >> (8 * rom_index)) & 0xFF);
    }
  }
  return memory::ROM(1 << 16, "rom_" + std::to_string(rom_index), data);
}

std::array<memory::ROM, 8> encode_roms(const InstructionEncoder &encoder,
                                       const microcode::table::Table &table) {
  return {
      encode_rom(encoder, table, 0), encode_rom(encoder, table, 1),
      encode_rom(encoder, table, 2), encode_rom(encoder, table, 3),
      encode_rom(encoder, table, 4), encode_rom(encoder, table, 5),
      encode_rom(encoder, table, 6), encode_rom(encoder, table, 7),
  };
}

} // namespace

InstructionMemory::InstructionMemory(const microcode::table::Table &table,
                                     std::string_view name, Component *parent)
    : Component(name, parent), encoder_(table),
      roms_(encode_roms(encoder_, table)) {
  for (auto &rom : roms_) {
    add_child(&rom);
  }
}

uint64_t InstructionMemory::read(uint16_t address) const {
  uint64_t value = 0;
  for (size_t rom_index = 0; rom_index < roms_.size(); rom_index++) {
    const uint64_t rom_value = roms_[rom_index].read(address);
    const uint64_t shifted_rom_value = rom_value << (8 * rom_index);
    value |= shifted_rom_value;
  }
  return value;
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

#include <irata/assembler/byte_encoder.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::assembler {

namespace {

void append_byte(std::map<common::bytes::Word, common::bytes::Byte> &bytes,
                 common::bytes::Word address, common::bytes::Byte value) {
  if (bytes.find(address) != bytes.end()) {
    std::ostringstream os;
    os << "duplicate address: " << address;
    throw std::invalid_argument(os.str());
  }
  bytes[address] = value;
}

void append_bytes(std::map<common::bytes::Word, common::bytes::Byte> &bytes,
                  common::bytes::Word address,
                  std::vector<common::bytes::Byte> values) {
  for (size_t i = 0; i < values.size(); ++i) {
    append_byte(bytes, address + common::bytes::Word(i), values[i]);
  }
}

void encode_arg(std::map<common::bytes::Word, common::bytes::Byte> &bytes,
                common::bytes::Word address,
                const LabelBinder::Program::Instruction::Arg &arg) {
  switch (arg.type()) {
  case LabelBinder::Program::Instruction::Arg::Type::None:
    break;
  case LabelBinder::Program::Instruction::Arg::Type::Immediate: {
    append_byte(
        bytes, address,
        dynamic_cast<const LabelBinder::Program::Instruction::Immediate &>(arg)
            .value());
    break;
  }
  case LabelBinder::Program::Instruction::Arg::Type::Absolute: {
    const auto &value =
        dynamic_cast<const LabelBinder::Program::Instruction::Absolute &>(arg)
            .value();
    const auto &[high, low] = value.to_bytes();
    append_bytes(bytes, address, {high, low});

    break;
  }
  case LabelBinder::Program::Instruction::Arg::Type::ZeroPageIndexed: {
    const auto &value =
        dynamic_cast<
            const LabelBinder::Program::Instruction::ZeroPageIndexed &>(arg)
            .value();
    append_byte(bytes, address, value);
    break;
  }
  case LabelBinder::Program::Instruction::Arg::Type::AbsoluteIndexed: {
    const auto &value =
        dynamic_cast<
            const LabelBinder::Program::Instruction::AbsoluteIndexed &>(arg)
            .value();
    const auto &[high, low] = value.to_bytes();
    append_bytes(bytes, address, {high, low});
    break;
  }
  }
}

void encode_instruction(
    std::map<common::bytes::Word, common::bytes::Byte> &bytes,
    const LabelBinder::Program::Instruction &instruction) {
  append_byte(bytes, instruction.address(), instruction.instruction().opcode());
  encode_arg(bytes, instruction.address() + common::bytes::Word(1),
             instruction.arg());
}

void encode_statement(std::map<common::bytes::Word, common::bytes::Byte> &bytes,
                      const LabelBinder::Program::Statement &statement) {
  switch (statement.type()) {
  case LabelBinder::Program::Statement::Type::Instruction:
    encode_instruction(
        bytes,
        dynamic_cast<const LabelBinder::Program::Instruction &>(statement));
    break;
  case LabelBinder::Program::Statement::Type::Literal:
    append_bytes(bytes, statement.address(),
                 dynamic_cast<const LabelBinder::Program::Literal &>(statement)
                     .values());
    break;
  }
}

} // namespace

std::map<common::bytes::Word, common::bytes::Byte>
ByteEncoder::encode(const LabelBinder::Program &program) const {
  std::map<common::bytes::Word, common::bytes::Byte> bytes;
  for (const auto &statement : program.statements()) {
    encode_statement(bytes, *statement);
  }
  return bytes;
}

} // namespace irata::assembler

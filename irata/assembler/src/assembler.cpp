#include <iostream>
#include <irata/assembler/assembler.hpp>
#include <irata/assembler/byte_encoder.hpp>

namespace irata::assembler {

std::vector<common::bytes::Byte>
Assembler::assemble(std::string_view input) const {
  std::cerr << "assembling:" << std::endl << input << std::endl;
  const auto parser_output = Parser().parse(input);
  std::cerr << "parser output: " << parser_output << std::endl;
  const auto instruction_binder_output =
      InstructionBinder().bind(parser_output);
  std::cerr << "instruction binder output: " << instruction_binder_output
            << std::endl;
  const auto label_binder_output =
      LabelBinder().bind(instruction_binder_output);
  std::cerr << "label binder output: " << label_binder_output << std::endl;
  const auto byte_encoder_output = ByteEncoder().encode(label_binder_output);
  if (byte_encoder_output.empty()) {
    return {};
  }
  common::bytes::Word max_address = 0;
  for (const auto &[address, _] : byte_encoder_output) {
    if (address > max_address) {
      max_address = address;
    }
  }
  std::vector<common::bytes::Byte> output(max_address.value() + 1, 0);
  for (const auto &[address, byte] : byte_encoder_output) {
    output[address.value()] = byte;
  }
  return output;
}

void Assembler::assemble(std::string_view input, std::ostream &output) const {
  const auto bytes = assemble(input);
  std::vector<uint8_t> values;
  values.reserve(bytes.size());
  uint32_t address = 0;
  for (const auto &byte : bytes) {
    const auto value = byte.value();
    std::cerr << "writing byte at address " << common::bytes::Word(address++)
              << ": " << byte << std::endl;
    values.push_back(value);
    // values.push_back(byte.value());
  }
  output.write(reinterpret_cast<const char *>(values.data()), values.size());
}

} // namespace irata::assembler

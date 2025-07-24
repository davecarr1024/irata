#include <irata/sim/microcode/compiler/ir/instruction.hpp>
#include <irata/sim/microcode/dsl/instruction.hpp>

namespace irata::sim::microcode::compiler::ir {

Instruction::Instruction(const asm_::Instruction &descriptor,
                         std::vector<Step> steps,
                         std::map<const hdl::StatusDecl *, bool> statuses)
    : descriptor_(descriptor), steps_(std::move(steps)),
      statuses_(std::move(statuses)) {
  for (const auto &[status, _] : statuses_) {
    if (status == nullptr) {
      throw std::invalid_argument("Status cannot be null");
    }
  }
}

namespace {

std::vector<Step>
convert_dsl_steps(const std::vector<std::unique_ptr<dsl::Step>> &steps) {
  std::vector<Step> result;
  for (const auto &step : steps) {
    result.emplace_back(*step);
  }
  return result;
}

} // namespace

Instruction::Instruction(const dsl::Instruction &instruction)
    : Instruction(instruction.descriptor(),
                  convert_dsl_steps(instruction.steps()),
                  instruction.statuses()) {}

const asm_::Instruction &Instruction::descriptor() const { return descriptor_; }

const std::vector<Step> &Instruction::steps() const { return steps_; }

bool Instruction::operator==(const Instruction &other) const {
  return std::tie(descriptor_, steps_, statuses_) ==
         std::tie(other.descriptor_, other.steps_, other.statuses_);
}

bool Instruction::operator!=(const Instruction &other) const {
  return !(*this == other);
}

bool Instruction::operator<(const Instruction &other) const {
  return std::tie(descriptor_, steps_, statuses_) <
         std::tie(other.descriptor_, other.steps_, other.statuses_);
}

std::ostream &operator<<(std::ostream &os, const Instruction &instruction) {
  os << "Instruction(" << instruction.descriptor() << ", steps={";
  for (const auto &step : instruction.steps()) {
    os << step << ", ";
  }
  os << "}, statuses={";
  for (const auto &[status, value] : instruction.statuses()) {
    os << status->path() << "=" << value << ", ";
  }
  os << "})";
  return os;
}

} // namespace irata::sim::microcode::compiler::ir

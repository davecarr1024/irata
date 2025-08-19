#include <irata/sim/components/alu/add.hpp>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/and.hpp>
#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/components/alu/or.hpp>
#include <irata/sim/components/alu/rotate_left.hpp>
#include <irata/sim/components/alu/rotate_right.hpp>
#include <irata/sim/components/alu/shift_left.hpp>
#include <irata/sim/components/alu/shift_right.hpp>
#include <irata/sim/components/alu/subtract.hpp>
#include <irata/sim/components/alu/xor.hpp>
#include <stdexcept>

namespace irata::sim::components::alu {

namespace {

std::vector<std::unique_ptr<Module>> build_modules(ALU &alu) {
  std::vector<std::unique_ptr<Module>> modules;
  modules.push_back(std::make_unique<Add>(alu));
  modules.push_back(std::make_unique<Subtract>(alu));
  modules.push_back(std::make_unique<And>(alu));
  modules.push_back(std::make_unique<Or>(alu));
  modules.push_back(std::make_unique<Xor>(alu));
  modules.push_back(std::make_unique<ShiftLeft>(alu));
  modules.push_back(std::make_unique<ShiftRight>(alu));
  modules.push_back(std::make_unique<RotateLeft>(alu));
  modules.push_back(std::make_unique<RotateRight>(alu));
  return modules;
}

uint8_t get_max_opcode(const std::vector<std::unique_ptr<Module>> &modules) {
  uint8_t max_opcode = 0;
  for (const auto &module : modules) {
    max_opcode = std::max(max_opcode, static_cast<uint8_t>(module->opcode()));
  }
  return max_opcode;
}

size_t get_num_opcode_controls(uint8_t max_opcode) {
  size_t num_opcode_controls = 0;
  while (1 << num_opcode_controls < max_opcode) {
    ++num_opcode_controls;
  }
  return num_opcode_controls;
}

std::unique_ptr<Control> build_opcode_control(size_t index, ALU &alu) {
  return std::make_unique<Control>("opcode_" + std::to_string(index),
                                   hdl::TickPhase::Process, &alu);
}

std::vector<std::unique_ptr<Control>> build_opcode_controls(size_t count,
                                                            ALU &alu) {
  std::vector<std::unique_ptr<Control>> opcode_controls;
  opcode_controls.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    opcode_controls.push_back(build_opcode_control(i, alu));
  }
  return opcode_controls;
}

std::vector<std::unique_ptr<Control>>
build_opcode_controls(const std::vector<std::unique_ptr<Module>> &modules,
                      ALU &alu) {
  return build_opcode_controls(get_num_opcode_controls(get_max_opcode(modules)),
                               alu);
}

} // namespace

ALU::ALU(Component &parent, ByteBus &data_bus, const Status &carry_in)
    : Component("alu", &parent), modules_(build_modules(*this)),
      opcode_controls_(build_opcode_controls(modules_, *this)),
      lhs_("lhs", &data_bus, this), rhs_("rhs", &data_bus, this),
      result_("result", &data_bus, this), carry_in_(carry_in),
      carry_out_("carry", this), zero_("zero", this),
      negative_("negative", this), overflow_("overflow", this) {}

hdl::ComponentType ALU::type() const { return hdl::ComponentType::Alu; }

const std::vector<std::unique_ptr<Module>> &ALU::modules() const {
  return modules_;
}

bool ALU::carry_in() const { return carry_in_.value(); }
const Status &ALU::carry_in_status() const { return carry_in_; }

Byte ALU::lhs() const { return lhs_.value(); }
void ALU::set_lhs(Byte value) { lhs_.set_value(value); }
Register &ALU::lhs_register() { return lhs_; }
const Register &ALU::lhs_register() const { return lhs_; }

Byte ALU::rhs() const { return rhs_.value(); }
void ALU::set_rhs(Byte value) { rhs_.set_value(value); }
Register &ALU::rhs_register() { return rhs_; }
const Register &ALU::rhs_register() const { return rhs_; }

Byte ALU::result() const { return result_.value(); }
void ALU::set_result(Byte value) { result_.set_value(value); }
Register &ALU::result_register() { return result_; }
const Register &ALU::result_register() const { return result_; }

bool ALU::carry_out() const { return carry_out_.value(); }
void ALU::set_carry_out(bool value) { carry_out_.set_value(value); }
Status &ALU::carry_out_status() { return carry_out_; }
const Status &ALU::carry_out_status() const { return carry_out_; }

bool ALU::zero() const { return zero_.value(); }
void ALU::set_zero(bool value) { zero_.set_value(value); }
Status &ALU::zero_status() { return zero_; }
const Status &ALU::zero_status() const { return zero_; }

bool ALU::negative() const { return negative_.value(); }
void ALU::set_negative(bool value) { negative_.set_value(value); }
Status &ALU::negative_status() { return negative_; }
const Status &ALU::negative_status() const { return negative_; }

bool ALU::overflow() const { return overflow_.value(); }
void ALU::set_overflow(bool value) { overflow_.set_value(value); }
Status &ALU::overflow_status() { return overflow_; }
const Status &ALU::overflow_status() const { return overflow_; }

std::vector<int>
ALU::opcode_control_indices_for_opcode(hdl::AluOpcode opcode) const {
  std::vector<int> indices;
  const uint8_t opcode_value = static_cast<uint8_t>(opcode);
  for (size_t i = 0; i < opcode_controls_.size(); ++i) {
    if (opcode_value & (1 << i)) {
      indices.push_back(i);
    }
  }
  return indices;
}

const std::vector<std::unique_ptr<Control>> &ALU::opcode_controls() const {
  return opcode_controls_;
}

std::vector<Control *> ALU::opcode_controls_for_opcode(hdl::AluOpcode opcode) {
  const auto indices = opcode_control_indices_for_opcode(opcode);
  std::vector<Control *> controls;
  controls.reserve(indices.size());
  for (const auto index : indices) {
    controls.push_back(opcode_controls_[index].get());
  }
  return controls;
}

std::vector<const Control *>
ALU::opcode_controls_for_opcode(hdl::AluOpcode opcode) const {
  const auto indices = opcode_control_indices_for_opcode(opcode);
  std::vector<const Control *> controls;
  controls.reserve(indices.size());
  for (const auto index : indices) {
    controls.push_back(opcode_controls_[index].get());
  }
  return controls;
}

hdl::AluOpcode ALU::opcode() const {
  uint8_t opcode = 0;
  for (size_t i = 0; i < opcode_controls_.size(); ++i) {
    if (opcode_controls_[i]->value()) {
      opcode |= 1 << i;
    }
  }
  return static_cast<hdl::AluOpcode>(opcode);
}

void ALU::set_opcode(hdl::AluOpcode opcode) {
  const auto opcode_value = static_cast<uint8_t>(opcode);
  for (size_t i = 0; i < opcode_controls_.size(); ++i) {
    opcode_controls_[i]->set_value(opcode_value & (1 << i));
  }
}

namespace {
Module *module_for_opcode(const ALU &alu, hdl::AluOpcode opcode) {
  if (opcode == hdl::AluOpcode::Nop) {
    return nullptr;
  }
  for (const auto &module : alu.modules()) {
    if (module->opcode() == opcode) {
      return module.get();
    }
  }
  std::ostringstream os;
  os << "No alu module found for opcode " << std::to_string(int(opcode));
  throw std::runtime_error(os.str());
}
} // namespace

Module *ALU::module(hdl::AluOpcode opcode) {
  return module_for_opcode(*this, opcode);
}

const Module *ALU::module(hdl::AluOpcode opcode) const {
  return module_for_opcode(*this, opcode);
}

Module *ALU::module() { return module(opcode()); }

const Module *ALU::module() const { return module(opcode()); }

void ALU::tick_process(Logger &logger) {
  if (auto *module = this->module(); module != nullptr) {
    const auto carry_in = this->carry_in();
    const auto lhs = this->lhs();
    const auto rhs = this->rhs();
    const auto result = module->apply(carry_in, lhs, rhs);
    if (result.set_flags) {
      this->set_result(result.value);
      this->set_carry_out(result.carry);
      this->set_zero(result.zero);
      this->set_negative(result.negative);
      this->set_overflow(result.overflow);
    }
    logger << "ALU operation: " << module->name() << "(" << lhs << ", " << rhs
           << ") = " << result;
  }
}

} // namespace irata::sim::components::alu

#include <irata/sim/hdl/alu_decl.hpp>

namespace irata::sim::hdl {

namespace {

std::unique_ptr<ProcessControlDecl> opcode_control(const AluDecl &alu,
                                                   size_t index) {
  std::ostringstream os;
  os << "opcode_" << index;
  return std::make_unique<ProcessControlDecl>(os.str(), alu);
}

size_t get_num_opcode_controls(size_t max_opcode) {
  size_t num_opcode_controls = 1;
  while ((1 << num_opcode_controls) < max_opcode) {
    num_opcode_controls++;
  }
  return num_opcode_controls;
}

std::vector<std::unique_ptr<ProcessControlDecl>>
get_opcode_controls(const AluDecl &alu, size_t num_opcode_controls) {
  std::vector<std::unique_ptr<ProcessControlDecl>> opcode_controls;
  for (size_t i = 0; i < num_opcode_controls; i++) {
    opcode_controls.push_back(opcode_control(alu, i));
  }
  return opcode_controls;
}

uint8_t
get_max_opcode(const std::set<std::unique_ptr<AluDecl::ModuleDecl>> &modules) {
  uint8_t max_opcode = 0;
  for (const auto &module : modules) {
    max_opcode = std::max(max_opcode, module->opcode());
  }
  return max_opcode;
}

std::set<std::unique_ptr<AluDecl::ModuleDecl>> get_modules(const AluDecl &alu) {
  std::set<std::unique_ptr<AluDecl::ModuleDecl>> modules;
  modules.insert(std::make_unique<AluDecl::ModuleDecl>(alu, "cmp", 0x01));
  modules.insert(std::make_unique<AluDecl::ModuleDecl>(alu, "adc", 0x02));
  modules.insert(std::make_unique<AluDecl::ModuleDecl>(alu, "sbc", 0x03));
  return modules;
}

} // namespace

AluDecl::ModuleDecl::ModuleDecl(const AluDecl &parent, std::string_view name,
                                uint8_t opcode)
    : ComponentWithParentDecl<ComponentType::AluModule>(name, parent),
      ComponentWithTypeDecl<ComponentType::AluModule>(name), opcode_(opcode) {}

uint8_t AluDecl::ModuleDecl::opcode() const { return opcode_; }

AluDecl::AluDecl(const ComponentDecl &parent, const ByteBusDecl &data_bus)
    : ComponentWithParentDecl<ComponentType::Alu>("alu", parent),
      ComponentWithTypeDecl<ComponentType::Alu>("alu"),
      modules_(get_modules(*this)), max_opcode_(get_max_opcode(modules_)),
      opcode_controls_(
          get_opcode_controls(*this, get_num_opcode_controls(max_opcode_))),
      lhs_("lhs", *this, data_bus), rhs_("rhs", *this, data_bus),
      result_("result", *this, data_bus), carry_("carry", *this),
      zero_("zero", *this), negative_("negative", *this),
      overflow_("overflow", *this), half_carry_("half_carry", *this)

{
  std::set<uint8_t> seen_opcodes;
  std::set<std::string> seen_names;
  for (const auto &module : modules_) {
    if (module == nullptr) {
      throw std::invalid_argument("alu module cannot be null");
    }
    if (!seen_names.insert(std::string(module->name())).second) {
      throw std::invalid_argument("duplicate alu module name: " +
                                  module->name());
    }
    if (!seen_opcodes.insert(module->opcode()).second) {
      throw std::invalid_argument("duplicate alu module opcode: " +
                                  std::to_string(module->opcode()));
    }
    if (module->opcode() == 0x00) {
      throw std::invalid_argument("alu module opcode cannot be 0x00: " +
                                  module->name());
    }
  }
}

void AluDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Alu>::verify(component);
  for (const auto &module : modules_) {
    verify_child(*module, component);
  }
  for (const auto &opcode_control : opcode_controls_) {
    verify_child(*opcode_control, component);
  }
  verify_child(lhs_, component);
  verify_child(rhs_, component);
  verify_child(result_, component);
  verify_child(carry_, component);
  verify_child(zero_, component);
  verify_child(negative_, component);
  verify_child(overflow_, component);
  verify_child(half_carry_, component);
}

const std::set<std::unique_ptr<AluDecl::ModuleDecl>> &AluDecl::modules() const {
  return modules_;
}

uint8_t AluDecl::max_opcode() const { return max_opcode_; }

size_t AluDecl::num_opcode_controls() const { return opcode_controls_.size(); }

const std::vector<std::unique_ptr<ProcessControlDecl>> &
AluDecl::opcode_controls() const {
  return opcode_controls_;
}

std::vector<const ProcessControlDecl *>
AluDecl::opcode_controls_for_opcode(uint8_t opcode) const {
  bool found = false;
  for (const auto &module : modules_) {
    if (module->opcode() == opcode) {
      found = true;
    }
  }
  if (opcode != 0 && !found) {
    std::ostringstream os;
    os << "alu opcode not found: " << int(opcode);
    throw std::invalid_argument(os.str());
  }

  std::vector<const ProcessControlDecl *> controls;
  for (size_t i = 0; i < opcode_controls_.size(); i++) {
    if (opcode & (1 << i)) {
      controls.push_back(opcode_controls_[i].get());
    }
  }
  return controls;
}

const ConnectedByteRegisterDecl &AluDecl::lhs() const { return lhs_; }

const ConnectedByteRegisterDecl &AluDecl::rhs() const { return rhs_; }

const ConnectedByteRegisterDecl &AluDecl::result() const { return result_; }

const StatusDecl &AluDecl::carry() const { return carry_; }

const StatusDecl &AluDecl::zero() const { return zero_; }

const StatusDecl &AluDecl::negative() const { return negative_; }

const StatusDecl &AluDecl::overflow() const { return overflow_; }

const StatusDecl &AluDecl::half_carry() const { return half_carry_; }

} // namespace irata::sim::hdl

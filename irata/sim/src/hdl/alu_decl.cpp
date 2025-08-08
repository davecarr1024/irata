#include <irata/common/strings/strings.hpp>
#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/hdl/alu_decl.hpp>

namespace irata::sim::hdl {

AluDecl::ModuleDecl::ModuleDecl(const AluDecl &parent, std::string_view name,
                                AluOpcode opcode)
    : ComponentWithParentDecl<ComponentType::AluModule>(name, parent),
      ComponentWithTypeDecl<ComponentType::AluModule>(name), opcode_(opcode) {}

AluOpcode AluDecl::ModuleDecl::opcode() const { return opcode_; }

void AluDecl::ModuleDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::AluModule>::verify(component);
  const components::alu::Module &module =
      dynamic_cast<const components::alu::Module &>(*component);
  if (module.opcode() != opcode_) {
    std::ostringstream os;
    os << "alu module opcode mismatch: " << module.opcode()
       << " != " << opcode_;
    throw std::invalid_argument(os.str());
  }
}

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
    max_opcode = std::max(max_opcode, static_cast<uint8_t>(module->opcode()));
  }
  return max_opcode;
}

std::set<std::unique_ptr<AluDecl::ModuleDecl>> get_modules(const AluDecl &alu) {
  std::set<std::unique_ptr<AluDecl::ModuleDecl>> modules;
  for (const auto &[name, opcode] :
       std::vector<std::pair<std::string, AluOpcode>>{
           {"add", AluOpcode::Add},
           {"subtract", AluOpcode::Subtract},
           {"and", AluOpcode::And},
           {"or", AluOpcode::Or},
           {"xor", AluOpcode::Xor},
           {"rotate_left", AluOpcode::RotateLeft},
           {"rotate_right", AluOpcode::RotateRight},
           {"shift_left", AluOpcode::ShiftLeft},
           {"shift_right", AluOpcode::ShiftRight},
       }) {
    modules.insert(std::make_unique<AluDecl::ModuleDecl>(alu, name, opcode));
  }
  return modules;
}

} // namespace

AluDecl::AluDecl(const ComponentDecl &parent, const ByteBusDecl &data_bus,
                 const StatusDecl &carry_in)
    : ComponentWithParentDecl<ComponentType::Alu>("alu", parent),
      ComponentWithTypeDecl<ComponentType::Alu>("alu"),
      modules_(get_modules(*this)), max_opcode_(get_max_opcode(modules_)),
      opcode_controls_(
          get_opcode_controls(*this, get_num_opcode_controls(max_opcode_))),
      lhs_("lhs", *this, data_bus), rhs_("rhs", *this, data_bus),
      result_("result", *this, data_bus), carry_in_(carry_in),
      carry_out_("carry", *this), zero_("zero", *this),
      negative_("negative", *this), overflow_("overflow", *this)

{
  std::set<hdl::AluOpcode> seen_opcodes;
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
      std::ostringstream os;
      os << "duplicate alu module opcode: " << module->opcode();
      throw std::invalid_argument(os.str());
    }
    if (module->opcode() == AluOpcode::Nop) {
      throw std::invalid_argument("alu module opcode cannot be Nop: " +
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
  // Don't verify carry_in_, it's not a child of the ALU.
  verify_child(carry_out_, component);
  verify_child(zero_, component);
  verify_child(negative_, component);
  verify_child(overflow_, component);

  const auto &alu = dynamic_cast<const components::alu::ALU &>(*component);
  for (const auto &module : modules_) {
    const auto hdl_controls = opcode_controls_for_opcode(module->opcode());
    std::set<std::string> hdl_control_names;
    for (const auto &control : hdl_controls) {
      hdl_control_names.insert(control->name());
    }
    const auto sim_controls = alu.opcode_controls_for_opcode(module->opcode());
    std::set<std::string> sim_control_names;
    for (const auto &control : sim_controls) {
      sim_control_names.insert(control->name());
    }
    if (hdl_control_names != sim_control_names) {
      std::ostringstream os;
      os << "alu module " << module->name()
         << " opcode controls mismatch: hdl controls ["
         << common::strings::join(hdl_control_names, ", ")
         << "] != sim controls ["
         << common::strings::join(sim_control_names, ", ") << "]";
      throw std::invalid_argument(os.str());
    }
  }

  const auto verify_status_connection = [](const StatusDecl &expected,
                                           const components::alu::ALU
                                               &status_register,
                                           const components::Status &(
                                               components::alu::ALU::*func)()
                                               const) {
    const components::Status &actual = (status_register.*func)();
    if (actual.path() != expected.path()) {
      std::ostringstream os;
      os << "Status register status connection path does not match. Expected: "
         << expected.path() << ", actual: " << actual.path();
      throw std::invalid_argument(os.str());
    }
  };
  verify_status_connection(carry_in_, alu,
                           &components::alu::ALU::carry_in_status);
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
AluDecl::opcode_controls_for_opcode(AluOpcode opcode) const {
  const auto opcode_value = static_cast<uint8_t>(opcode);
  std::vector<const ProcessControlDecl *> controls;
  for (size_t i = 0; i < opcode_controls_.size(); i++) {
    if (opcode_value & (1 << i)) {
      controls.push_back(opcode_controls_[i].get());
    }
  }
  return controls;
}

const ConnectedByteRegisterDecl &AluDecl::lhs() const { return lhs_; }

const ConnectedByteRegisterDecl &AluDecl::rhs() const { return rhs_; }

const ConnectedByteRegisterDecl &AluDecl::result() const { return result_; }

const StatusDecl &AluDecl::carry_in() const { return carry_in_; }

const StatusDecl &AluDecl::carry_out() const { return carry_out_; }

const StatusDecl &AluDecl::zero() const { return zero_; }

const StatusDecl &AluDecl::negative() const { return negative_; }

const StatusDecl &AluDecl::overflow() const { return overflow_; }

} // namespace irata::sim::hdl

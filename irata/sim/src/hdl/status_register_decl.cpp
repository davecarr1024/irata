#include <irata/common/strings/strings.hpp>
#include <irata/sim/components/status_register.hpp>
#include <irata/sim/hdl/status_register_decl.hpp>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace irata::sim::hdl {

StatusRegisterDecl::StatusRegisterDecl(const ComponentDecl &parent,
                                       const ByteBusDecl &bus,
                                       const StatusDecl &carry_in,
                                       const StatusDecl &negative_in,
                                       const StatusDecl &overflow_in,
                                       const StatusDecl &zero_in)
    : ComponentWithParentDecl<ComponentType::StatusRegister>("status_register",
                                                             parent),
      ComponentWithTypeDecl<ComponentType::StatusRegister>("status_register"),
      RegisterWithByteBusDecl(bus), ComponentWithByteBusDecl(bus),
      carry_in_(carry_in), negative_in_(negative_in), overflow_in_(overflow_in),
      zero_in_(zero_in), carry_out_("carry", *this),
      negative_out_("negative", *this), overflow_out_("overflow", *this),
      zero_out_("zero", *this), latch_("latch", *this), status_indices_({
                                                            {&carry_in_, 0},
                                                            {&negative_in_, 7},
                                                            {&overflow_in_, 6},
                                                            {&zero_in_, 1},
                                                        }) {}

void StatusRegisterDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::StatusRegister>::verify(component);
  RegisterWithByteBusDecl::verify(component);
  verify_child(carry_out_, component);
  verify_child(negative_out_, component);
  verify_child(overflow_out_, component);
  verify_child(zero_out_, component);
  verify_child(latch_, component);
  const auto &status_register =
      dynamic_cast<const components::StatusRegister &>(*component);
  std::set<std::pair<std::string, int>> expected_status_indices;
  for (const auto &[status, index] : status_indices_) {
    expected_status_indices.emplace(status->name(), index);
  }
  std::set<std::pair<std::string, int>> actual_status_indices;
  for (const auto &[status, index] : status_register.status_indices()) {
    actual_status_indices.emplace(status->name(), index);
  }
  if (expected_status_indices != actual_status_indices) {
    std::vector<std::string> expected_status_indices_strs;
    for (const auto &[status, index] : expected_status_indices) {
      expected_status_indices_strs.push_back(status + ":" +
                                             std::to_string(index));
    }
    std::vector<std::string> actual_status_indices_strs;
    for (const auto &[status, index] : actual_status_indices) {
      actual_status_indices_strs.push_back(status + ":" +
                                           std::to_string(index));
    }
    std::ostringstream os;
    os << "Status register status indices do not match. Expected: ["
       << common::strings::join(expected_status_indices_strs, ", ")
       << "], actual: ["
       << common::strings::join(actual_status_indices_strs, ", ") << "]";
    throw std::invalid_argument(os.str());
  }
  // TODO: verify status indices
}

const StatusDecl &StatusRegisterDecl::carry_in() const { return carry_in_; }

const StatusDecl &StatusRegisterDecl::negative_in() const {
  return negative_in_;
}

const StatusDecl &StatusRegisterDecl::overflow_in() const {
  return overflow_in_;
}

const StatusDecl &StatusRegisterDecl::zero_in() const { return zero_in_; }

const StatusDecl &StatusRegisterDecl::carry_out() const { return carry_out_; }

const StatusDecl &StatusRegisterDecl::negative_out() const {
  return negative_out_;
}

const StatusDecl &StatusRegisterDecl::overflow_out() const {
  return overflow_out_;
}

const StatusDecl &StatusRegisterDecl::zero_out() const { return zero_out_; }

const PersistentClearControlDecl &StatusRegisterDecl::latch() const {
  return latch_;
}

const std::map<const StatusDecl *, int> &
StatusRegisterDecl::status_indices() const {
  return status_indices_;
}

} // namespace irata::sim::hdl

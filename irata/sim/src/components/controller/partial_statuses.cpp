#include <irata/sim/components/controller/partial_statuses.hpp>
#include <stdexcept>

namespace irata::sim::components::controller {

PartialStatuses::PartialStatuses(
    const std::map<const hdl::StatusDecl *, bool> &statuses)
    : statuses_(statuses) {
  for (const auto &[status, _] : statuses_) {
    if (status == nullptr) {
      throw std::invalid_argument("Status cannot be null");
    }
  }
}

const std::map<const hdl::StatusDecl *, bool> &
PartialStatuses::statuses() const {
  return statuses_;
}

} // namespace irata::sim::components::controller

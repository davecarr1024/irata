#include <irata/common/strings/strings.hpp>
#include <irata/sim/components/controller/complete_statuses.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace irata::sim::components::controller {

CompleteStatuses::CompleteStatuses(
    const StatusEncoder &encoder,
    const std::map<const hdl::StatusDecl *, bool> &statuses)
    : statuses_(statuses) {
  for (const auto &[status, _] : statuses_) {
    if (status == nullptr) {
      throw std::invalid_argument("status cannot be null");
    }
  }
  const auto encoder_statuses = encoder.statuses();
  for (const auto &status : encoder_statuses) {
    if (statuses_.find(status) == statuses_.end()) {
      std::ostringstream os;
      os << "expected status " << *status << " is not in " << *this;
      throw std::invalid_argument(os.str());
    }
  }
  for (const auto &[status, _] : statuses_) {
    if (encoder_statuses.find(status) == encoder_statuses.end()) {
      std::ostringstream os;
      os << "unexpected status " << *status << " in " << *this;
      throw std::invalid_argument(os.str());
    }
  }
}

const std::map<const hdl::StatusDecl *, bool> &
CompleteStatuses::statuses() const {
  return statuses_;
}

bool CompleteStatuses::operator==(const CompleteStatuses &other) const {
  return statuses_ == other.statuses_;
}

bool CompleteStatuses::operator!=(const CompleteStatuses &other) const {
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const CompleteStatuses &statuses) {
  std::vector<std::string> status_strings;
  for (const auto &[status, value] : statuses.statuses()) {
    status_strings.push_back(status->name() + "=" + (value ? "1" : "0"));
  }
  os << "CompleteStatuses(" << common::strings::join(status_strings, ", ")
     << ")";
  return os;
}

} // namespace irata::sim::components::controller

#include <irata/sim/components/status.hpp>

namespace irata::sim {

Status::Status(std::string_view name, Component *parent)
    : Component(name, parent) {}

bool Status::value() const { return value_; }

void Status::set_value(bool value) { value_ = value; }

std::vector<Status *> Status::statuses() {
  auto statuses = Component::statuses();
  statuses.push_back(this);
  return statuses;
}

} // namespace irata::sim

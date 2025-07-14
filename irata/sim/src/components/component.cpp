#include <irata/sim/components/component.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace irata::sim {
Component::Component(std::string name) : name_(std::move(name)) {}

std::string Component::name() const { return name_; }

Component *Component::parent() const { return parent_; }

} // namespace irata::sim
#include <irata/sim/components/component.h>
#include <string>
#include <utility>

namespace irata::sim {
Component::Component(std::string name) : name_(std::move(name)) {}

std::string Component::name() const { return name_; }
} // namespace irata::sim
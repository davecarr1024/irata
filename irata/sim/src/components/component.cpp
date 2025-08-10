#include <iostream>
#include <irata/sim/components/component.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace irata::sim::components {

Component::Component(std::string_view name, Component *parent)
    : name_(std::string(name)), parent_(nullptr) {
  if (parent != nullptr) {
    parent->add_child(this);
  }
}

hdl::ComponentType Component::type() const {
  return hdl::ComponentType::Unknown;
}

std::string Component::name() const { return name_; }

Component *Component::parent() const { return parent_; }

void Component::add_child(Component *child) {
  if (child == nullptr) {
    throw std::invalid_argument("child cannot be null");
  }
  if (child->parent_ != nullptr) {
    throw std::invalid_argument("child already has a parent");
  }
  if (const auto it = children_.find(child->name_); it != children_.end()) {
    if (it->second == child) {
      return;
    }
    std::ostringstream os;
    os << "trying to add child " << child->name() << " to component "
       << this->path() << " but a child with the same name already exists";
    throw std::invalid_argument(os.str());
  }
  if (children_.find(child->name_) != children_.end()) {
  }
  children_[child->name_] = child;
  child->parent_ = this;
}

std::map<std::string, Component *> Component::children() const {
  return children_;
}

const Component *Component::child(std::string_view path) const {
  if (path == "/") {
    return root();
  } else if (path == "..") {
    return parent_;
  } else if (path.find('/') == 0) {
    return root()->child(path.substr(1));
  } else if (const auto pos = path.find('/'); pos != std::string::npos) {
    const auto name = path.substr(0, pos);
    const auto rest = path.substr(pos + 1);
    return child(name)->child(rest);
  } else if (const auto it = children_.find(std::string(path));
             it != children_.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

Component *Component::child(std::string_view path) {
  return const_cast<Component *>(std::as_const(*this).child(path));
}

std::string Component::path() const {
  if (parent_ == nullptr) {
    return "/";
  } else if (parent_->parent_ == nullptr) {
    return "/" + name_;
  } else {
    return parent_->path() + "/" + name_;
  }
}

const Component *Component::root() const {
  if (parent_ == nullptr) {
    return this;
  } else {
    return parent_->root();
  }
}

Component *Component::root() {
  if (parent_ == nullptr) {
    return this;
  } else {
    return parent_->root();
  }
}

std::optional<hdl::TickPhase> Component::active_tick_phase() const {
  return active_tick_phase_;
}

void Component::set_active_tick_phase(
    std::optional<hdl::TickPhase> tick_phase) {
  active_tick_phase_ = tick_phase;
}

void Component::tick_traverse(std::ostream &log_output, hdl::TickPhase phase) {
  set_active_tick_phase(phase);
  {
    Logger logger(log_output, phase, *this);
    switch (phase) {
    case hdl::TickPhase::Control:
      tick_control(logger);
      break;
    case hdl::TickPhase::Write:
      tick_write(logger);
      break;
    case hdl::TickPhase::Read:
      tick_read(logger);
      break;
    case hdl::TickPhase::Process:
      tick_process(logger);
      break;
    case hdl::TickPhase::Clear:
      tick_clear(logger);
      break;
    default:
      throw std::logic_error("unknown tick phase " +
                             std::to_string(int(phase)));
    }
  }
  for (const auto &[_, child] : children_) {
    child->tick_traverse(log_output, phase);
  }
  set_active_tick_phase(std::nullopt);
}

void Component::tick(std::ostream &log_output) {
  auto *root = this->root();
  root->tick_traverse(log_output, hdl::TickPhase::Control);
  root->tick_traverse(log_output, hdl::TickPhase::Write);
  root->tick_traverse(log_output, hdl::TickPhase::Read);
  root->tick_traverse(log_output, hdl::TickPhase::Process);
  root->tick_traverse(log_output, hdl::TickPhase::Clear);
}

Component::Logger::Logger(std::ostream &output, hdl::TickPhase tick_phase,
                          const Component &component)
    : tick_phase_(tick_phase), component_(component), output_(output) {}

Component::Logger::~Logger() {
  if (!logged_) {
    return;
  }
  output_ << "[" << tick_phase_ << "] " << component_.path() << ": "
          << os_.str() << std::endl;
}

std::vector<Control *> Component::controls() {
  std::vector<Control *> result;
  for (const auto &[_, child] : children_) {
    for (const auto &control : child->controls()) {
      result.push_back(control);
    }
  }
  return result;
}

std::vector<const Control *> Component::controls() const {
  std::vector<const Control *> result;
  for (auto *control : const_cast<Component *>(this)->controls()) {
    result.push_back(control);
  }
  return result;
}

std::vector<Status *> Component::statuses() {
  std::vector<Status *> result;
  for (const auto &[_, child] : children_) {
    for (const auto &status : child->statuses()) {
      result.push_back(status);
    }
  }
  return result;
}

std::vector<const Status *> Component::statuses() const {
  std::vector<const Status *> result;
  for (auto *status : const_cast<Component *>(this)->statuses()) {
    result.push_back(status);
  }
  return result;
}

Component::Serializer::Serializer(std::ostream &os, size_t tabs)
    : os_(os), tabs_(tabs) {}

Component::Serializer Component::Serializer::with_tab() const {
  return Serializer(os_, tabs_ + 1);
}

void Component::Serializer::output(std::string_view value) {
  for (size_t i = 0; i < tabs_; ++i) {
    os_ << "  ";
  }
  os_ << value << std::endl;
}

void Component::serialize_all(std::ostream &os) const {
  Serializer serializer(os);
  this->root()->serialize_traverse(serializer);
}

void Component::serialize_traverse(Serializer &serializer) const {
  serialize(serializer);
  for (const auto &[name, child] : children_) {
    serializer.output(name);
    auto child_serializer = serializer.with_tab();
    child->serialize_traverse(child_serializer);
  }
}

} // namespace irata::sim::components

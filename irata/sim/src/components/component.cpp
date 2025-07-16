#include <iostream>
#include <irata/sim/components/component.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace irata::sim {

Component::Component(std::string_view name, Component *parent)
    : name_(std::string(name)), parent_(nullptr) {
  if (parent != nullptr) {
    parent->add_child(this);
  }
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
  if (children_.find(child->name_) != children_.end()) {
    throw std::invalid_argument("child with same name already exists");
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

void Component::tick(std::ostream &log_output) {
  auto *root = this->root();
  auto traverse_phase = [&](TickPhase tick_phase,
                            void (Component::*tick_fn)(Logger &)) {
    root->traverse([&](Component *component) {
      Logger logger(log_output, tick_phase);
      (component->*tick_fn)(logger.for_component(component));
    });
  };

  traverse_phase(TickPhase::Control, &Component::tick_control);
  traverse_phase(TickPhase::Write, &Component::tick_write);
  traverse_phase(TickPhase::Read, &Component::tick_read);
  traverse_phase(TickPhase::Process, &Component::tick_process);
  traverse_phase(TickPhase::Clear, &Component::tick_clear);
}

Component::Logger::Logger(std::ostream &output, TickPhase tick_phase)
    : output_(output), tick_phase_(tick_phase) {}

Component::Logger::~Logger() {
  if (!logged_) {
    return;
  }
  output_ << "[" << tick_phase_ << "] ";
  if (component_ != nullptr) {
    output_ << component_->path() << ": ";
  }
  output_ << os_.str() << std::endl;
}

Component::Logger &
Component::Logger::for_component(const Component *component) {
  component_ = component;
  return *this;
}

std::ostream &operator<<(std::ostream &os, Component::TickPhase phase) {
  switch (phase) {
  case Component::TickPhase::Control:
    return os << "tick_control";
  case Component::TickPhase::Write:
    return os << "tick_write";
  case Component::TickPhase::Read:
    return os << "tick_read";
  case Component::TickPhase::Process:
    return os << "tick_process";
  case Component::TickPhase::Clear:
    return os << "tick_clear";
  }
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

} // namespace irata::sim
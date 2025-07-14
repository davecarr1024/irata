#pragma once

#include <map>
#include <memory>
#include <string>

namespace irata::sim {

class Component {
public:
  explicit Component(std::string_view name);
  Component(const Component &) = delete;
  Component(Component &&) = delete;
  Component &operator=(const Component &) = delete;
  Component &operator=(Component &&) = delete;
  virtual ~Component() = default;

  std::string name() const;

  Component *parent() const;

  std::map<std::string, Component *> children() const;
  void add_child(std::unique_ptr<Component> child);
  Component *child(std::string_view name) const;

private:
  std::string name_;
  Component *parent_ = nullptr;

  std::map<std::string, std::unique_ptr<Component>> children_;
};

} // namespace irata::sim
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace irata::sim {

class Component {
public:
  explicit Component(std::string name);
  virtual ~Component() = default;

  std::string name() const;

  Component *parent() const;

  const std::vector<std::unique_ptr<Component>> &children() const;
  void add_child(std::unique_ptr<Component> child);

private:
  std::string name_;
  Component *parent_ = nullptr;

  std::vector<std::unique_ptr<Component>> children_;
};

} // namespace irata::sim
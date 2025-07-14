#pragma once

#include <string>

namespace irata::sim {

class Component {
public:
  Component(std::string name);
  virtual ~Component() = default;

  std::string name() const;

private:
  std::string name_;
};

} // namespace irata::sim
#pragma once

#include <irata/sim/components/component.hpp>

namespace irata::sim {

class Status : public Component {
public:
  explicit Status(std::string_view name);
  virtual ~Status() = default;

  bool value() const;
  void set_value(bool value);

  std::vector<Status *> statuses() override;

private:
  bool value_ = false;
};

} // namespace irata::sim

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>

namespace irata::sim::components {

template <typename T>
Bus<T>::Bus(std::string_view name, Component *parent)
    : Component(name, parent) {}

template <typename T> std::optional<T> Bus<T>::value() const { return value_; }

template <typename T> void Bus<T>::set_value(T value, Component &setter) {
  const auto setter_name = setter.path();
  if (setter_ != std::nullopt && setter_ != setter_name) {
    throw std::logic_error("write conflict: trying to write to bus by " +
                           setter_name + " but bus was already written by " +
                           *setter_);
  }
  setter_ = setter_name;
  value_ = value;
}

template <typename T> std::optional<std::string> Bus<T>::setter() const {
  return setter_;
}

template <typename T> void Bus<T>::tick_clear(Logger &logger) {
  if (setter_ != std::nullopt) {
    logger << "clearing bus value set by " << *setter_;
    setter_ = std::nullopt;
    value_ = std::nullopt;
  }
}

template class Bus<Byte>;
template class Bus<Word>;

} // namespace irata::sim::components
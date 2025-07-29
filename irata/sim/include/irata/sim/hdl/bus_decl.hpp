#pragma once

#include <irata/common/bytes/byte.hpp>
#include <irata/common/bytes/word.hpp>
#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

class BusDecl : public virtual ComponentDecl {};

template <typename DataType, ComponentType Type>
struct IsValidBus : std::false_type {};
template <> struct IsValidBus<Byte, ComponentType::ByteBus> : std::true_type {};
template <> struct IsValidBus<Word, ComponentType::WordBus> : std::true_type {};

template <typename DataType, ComponentType Type>
class TypedBusDecl final : public BusDecl,
                           public ComponentWithParentDecl<Type> {
public:
  static_assert(IsValidBus<DataType, Type>::value, "Invalid bus type");

  static constexpr ComponentType type_v = Type;

  TypedBusDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl<Type>(name, parent),
        TypedComponentDecl<Type>(name) {}
};

using ByteBusDecl = TypedBusDecl<Byte, ComponentType::ByteBus>;
using WordBusDecl = TypedBusDecl<Word, ComponentType::WordBus>;

} // namespace irata::sim::hdl

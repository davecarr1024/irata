#pragma once

#include <irata/common/bytes/byte.hpp>
#include <irata/common/bytes/word.hpp>
#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

class BusDecl : public virtual ComponentDecl {};

template <typename DataType>
class BusWithDataTypeDecl : public virtual BusDecl {};

class ByteBusDecl : public ComponentWithParentDecl<ComponentType::ByteBus>,
                    public BusWithDataTypeDecl<Byte> {
public:
  ByteBusDecl(std::string_view name, const ComponentDecl &parent);
};

class WordBusDecl : public ComponentWithParentDecl<ComponentType::WordBus>,
                    public BusWithDataTypeDecl<Word> {
public:
  WordBusDecl(std::string_view name, const ComponentDecl &parent);
};

} // namespace irata::sim::hdl

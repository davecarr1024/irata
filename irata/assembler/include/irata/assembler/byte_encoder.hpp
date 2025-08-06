#pragma once

#include <irata/assembler/label_binder.hpp>

namespace irata::assembler {

class ByteEncoder final {
public:
  std::map<common::bytes::Word, common::bytes::Byte>
  encode(const LabelBinder::Program &program) const;
};

} // namespace irata::assembler

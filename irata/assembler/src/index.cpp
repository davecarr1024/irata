#include <irata/assembler/index.hpp>

namespace irata::assembler {

std::ostream &operator<<(std::ostream &os, const Index &index_type) {
  switch (index_type) {
  case Index::X:
    return os << "X";
  case Index::Y:
    return os << "Y";
  }
}

} // namespace irata::assembler
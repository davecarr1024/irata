#include <ostream>

namespace irata::assembler {

// The type of indexing an indexed instruction uses.
enum class Index {
  // Indexed by the X register.
  X,
  // Indexed by the Y register.
  Y,
};

std::ostream &operator<<(std::ostream &os, const Index &index_type);

} // namespace irata::assembler

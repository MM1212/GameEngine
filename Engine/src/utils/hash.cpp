#include "utils/hash.h"

constexpr size_t Engine::Hash(const std::string_view str) {
  static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4);
  size_t h = 0;

  if constexpr (sizeof(size_t) == 8) {
    h = 1125899906842597L; // prime
  }
  else {
    h = 4294967291L;
  }

  int i = 0;
  while (str[i] != 0) {
    h = 31 * h + str[i++];
  }

  return h;
}

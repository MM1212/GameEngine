#pragma once

#include <string_view>

namespace Engine {
  constexpr size_t Hash(const std::string_view str) {
    static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4);
    size_t h = 0;

    if constexpr (sizeof(size_t) == 8) {
      h = 1125899906842597L; // prime
    }
    else {
      h = 4294967291L;
    }

    int i = 0;
    for (i = 0; i < str.size(); i++) {
      h = 31 * h + str[i];
    }
    return h;
  }

  template <typename T, typename... Rest>
  void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
    (HashCombine(seed, rest), ...);
  };
}
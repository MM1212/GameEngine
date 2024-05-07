#pragma once

#include <glm/glm.hpp>

namespace Engine {
  template <typename T>
  struct Size2D {
    union {
      glm::tvec2<T> size;
      struct {
        T width;
        T height;
      };
    };
    Size2D() : size(0) {}
    Size2D(T width, T height) : size(width, height) {}
    Size2D(const glm::tvec2<T>& size) : size(size) {}
  };
  template <typename T>
  struct Size3D {
    union {
      glm::tvec3<T> size;
      struct {
        T width;
        T height;
        T depth;
      };
    };
    Size3D() : size(0) {}
    Size3D(T width, T height, T depth) : size(width, height, depth) {}
    Size3D(const glm::tvec3<T>& size) : size(size) {}
  };
}
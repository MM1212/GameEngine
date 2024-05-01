#pragma once

#include <glm/glm.hpp>
#include <random>

namespace Engine {
  class Random {
  public:
    Random() = delete;

    static float Float();
    static float Float(float min, float max);
    template<typename T>
    static T Int() {
      return Random::Int(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }
    template <typename T>
    static T Int(T min, T max) {
      std::uniform_int_distribution<T> dist(min, max);
      return dist(Random::gen);
    }
    template <typename T>
    T IntNormalized() {
      return Random::Int<T>(0, 1);
    }
    static glm::vec2 Vec2();
    static glm::vec2 Vec2(float min, float max);
    static glm::vec3 Vec3();
    static glm::vec3 Vec3(float min, float max);
    static glm::vec4 Vec4();
    static glm::vec4 Vec4(float min, float max);
  private:
    static std::random_device rd;
    static std::mt19937 gen;
  };
}
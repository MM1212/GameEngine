#pragma once

#include <glm/glm.hpp>

namespace Engine::Coordinates {
  template<typename T>
  static constexpr T Up = {0, 1};

  template<>
  static constexpr glm::vec2 Up<glm::vec2> = {0, 1};

  template<>
  static constexpr glm::vec3 Up<glm::vec3> = {0, 1, 0};

  template<>
  static constexpr glm::vec4 Up<glm::vec4> = {0, 1, 0, 0};

  template<typename T>
  static constexpr T Right = {1, 0};

  template<>
  static constexpr glm::vec2 Right<glm::vec2> = {1, 0};
  
  template<>
  static constexpr glm::vec3 Right<glm::vec3> = {1, 0, 0};

  template<>
  static constexpr glm::vec4 Right<glm::vec4> = {1, 0, 0, 0};

  template<typename T>
  static constexpr T Forward = {0, 0};

  template<>
  static constexpr glm::vec2 Forward<glm::vec2> = {0, 0};

  template<>
  static constexpr glm::vec3 Forward<glm::vec3> = {0, 0, 1};

  template<>
  static constexpr glm::vec4 Forward<glm::vec4> = {0, 0, 1, 0};

}
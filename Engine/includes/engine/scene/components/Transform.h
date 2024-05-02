#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine::Components {
  struct Transform {
    union {
      glm::vec3 translation{};
      glm::vec3 position;
    };
    glm::vec3 scale{ 1.f, 1.f, 1.f };
    glm::vec3 rotation{};

    Transform() = default;
    Transform(const Transform&) = default;
    Transform& operator=(const Transform&) = default;

    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    explicit operator glm::mat4() const;
    glm::mat3 computeNormalMatrix() const;
    glm::vec3 forward() const;
    glm::vec3 right() const;
  };
}
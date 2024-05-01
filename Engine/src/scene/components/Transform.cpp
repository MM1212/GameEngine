#include "engine/scene/components/Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <engine/core/Coordinates.h>

using Engine::Components::Transform;

Transform::operator glm::mat4() const {
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  return glm::mat4{
      {
          scale.x * (c1 * c3 + s1 * s2 * s3),
          scale.x * (c2 * s3),
          scale.x * (c1 * s2 * s3 - c3 * s1),
          0.0f,
      },
      {
          scale.y * (c3 * s1 * s2 - c1 * s3),
          scale.y * (c2 * c3),
          scale.y * (c1 * c3 * s2 + s1 * s3),
          0.0f,
      },
      {
          scale.z * (c2 * s1),
          scale.z * (-s2),
          scale.z * (c1 * c2),
          0.0f,
      },
      {translation.x, translation.y, translation.z, 1.0f}
  };
}

glm::mat3 Transform::computeNormalMatrix() const {
  return glm::transpose(glm::inverse(static_cast<glm::mat4>(*this)));
}

glm::vec3 Transform::forward() const {
  return glm::normalize(glm::vec3{
      glm::sin(rotation.y) * glm::cos(rotation.x),
      glm::sin(rotation.x),
      glm::cos(rotation.y) * glm::cos(rotation.x)
    }) * glm::vec3{ 1, 1, -1 };
}

glm::vec3 Transform::right() const {
  return glm::normalize(glm::cross(forward(), Coordinates::Up<glm::vec3>));
}
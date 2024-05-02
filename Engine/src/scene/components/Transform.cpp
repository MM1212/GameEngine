#include "engine/scene/components/Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <engine/core/Coordinates.h>

using Engine::Components::Transform;

Transform::operator glm::mat4() const {
  // Transform::rotation is in euler angles in randians (XYZ)
  glm::mat4 rx = glm::rotate(glm::mat4{ 1.f }, this->rotation.x, Coordinates::Right<glm::vec3>);
  glm::mat4 ry = glm::rotate(glm::mat4{ 1.f }, this->rotation.y, Coordinates::Up<glm::vec3>);
  glm::mat4 rz = glm::rotate(glm::mat4{ 1.f }, this->rotation.z, Coordinates::Forward<glm::vec3>);

  glm::mat4 rotation = rx * ry * rz;
  glm::mat4 scale = glm::scale(glm::mat4{ 1.f }, this->scale);
  glm::mat4 transform = glm::translate(glm::mat4{ 1.f }, this->translation);
  return scale * rotation * transform;
}

glm::mat3 Transform::computeNormalMatrix() const {
  return glm::transpose(glm::inverse(static_cast<glm::mat4>(*this)));
}

glm::vec3 Transform::forward() const {
  return glm::normalize(glm::vec3{
    glm::sin(this->rotation.y) * glm::cos(this->rotation.x),
    glm::sin(this->rotation.x),
    glm::cos(this->rotation.y) * glm::cos(this->rotation.x)
  }) * Coordinates::Forward<glm::vec3>;
}

glm::vec3 Transform::right() const {
  return glm::normalize(glm::cross(this->forward(), Coordinates::Up<glm::vec3>));
}
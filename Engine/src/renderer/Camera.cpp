#include "renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

using Engine::Camera;

Camera::Camera() {
  this->computeProjection();
}

void Camera::setPerspective(float fov, float nearClip, float farClip) {
  this->projectionType = Projection::Perspective;
  this->perspectiveFov = fov;
  this->perspectiveNearClip = nearClip;
  this->perspectiveFarClip = farClip;
  this->computeProjection();
}
void Camera::setOrthographic(float size, float nearClip, float farClip) {
  this->projectionType = Projection::Orthographic;
  this->orthographicSize = size;
  this->orthographicNearClip = nearClip;
  this->orthographicFarClip = farClip;
  this->computeProjection();
}
void Camera::setViewportSize(uint32_t width, uint32_t height) {
  if (this->viewportSize.x == width && this->viewportSize.y == height)
    return;
  this->viewportSize = { width, height };
  this->aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  this->computeProjection();
}
void Camera::computeProjection() {
  switch (this->projectionType) {
    case Projection::Perspective:
      this->projection = glm::perspective(this->perspectiveFov, this->aspectRatio, this->perspectiveNearClip, this->perspectiveFarClip);
      break;
    case Projection::Orthographic:
      float orthoLeft = -this->orthographicSize * this->aspectRatio * 0.5f;
      float orthoRight = this->orthographicSize * this->aspectRatio * 0.5f;
      float orthoBottom = -this->orthographicSize * 0.5f;
      float orthoTop = this->orthographicSize * 0.5f;
      this->projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, this->orthographicNearClip, this->orthographicFarClip);
      break;
  };
  this->onProjectionUpdate();
}
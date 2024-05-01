#pragma once

#include <glm/glm.hpp>

namespace Engine {
  class Camera {
  public:
    enum class Projection {
      Orthographic, Ortho = Orthographic,
      Perspective, Persp = Perspective
    };
  public:
    Camera();
    Camera(const glm::mat4& projection) : projection(projection) {}
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
    virtual ~Camera() = default;

    void setPerspective(float fov, float nearClip, float farClip);
    void setOrthographic(float size, float nearClip, float farClip);

    void setViewportSize(uint32_t width, uint32_t height);

    float getPerspectiveFov() const { return this->perspectiveFov; }
    void setPerspectiveFov(float fov) { this->perspectiveFov = fov; this->computeProjection(); }
    float getPerspectiveNearClip() const { return this->perspectiveNearClip; }
    void setPerspectiveNearClip(float nearClip) { this->perspectiveNearClip = nearClip; this->computeProjection(); }
    float getPerspectiveFarClip() const { return this->perspectiveFarClip; }
    void setPerspectiveFarClip(float farClip) { this->perspectiveFarClip = farClip; this->computeProjection(); }

    float getOrthographicSize() const { return this->orthographicSize; }
    void setOrthographicSize(float size) { this->orthographicSize = size; this->computeProjection(); }
    float getOrthographicNearClip() const { return this->orthographicNearClip; }
    void setOrthographicNearClip(float nearClip) { this->orthographicNearClip = nearClip; this->computeProjection(); }
    float getOrthographicFarClip() const { return this->orthographicFarClip; }
    void setOrthographicFarClip(float farClip) { this->orthographicFarClip = farClip; this->computeProjection(); }

    float getAspectRatio() const { return this->aspectRatio; }
    void setAspectRatio(float aspectRatio) {
      if (this->aspectRatio == aspectRatio)
        return;
      this->aspectRatio = aspectRatio;
      this->computeProjection();
    }

    Projection getProjectionType() const { return this->projectionType; }
    void setProjectionType(Projection type) { this->projectionType = type; this->computeProjection(); }
  private:
    void computeProjection();
    virtual void onProjectionUpdate() {}
  protected:
    glm::mat4 projection{ 1.0f };

    Projection projectionType = Projection::Perspective;
    float perspectiveFov = glm::radians(45.0f);
    float perspectiveNearClip = 0.01f, perspectiveFarClip = 1000.0f;

    float orthographicSize = 10.0f;
    float orthographicNearClip = -1.0f, orthographicFarClip = 1.0f;

    glm::uvec2 viewportSize = { 0, 0 };
    float aspectRatio = 0.0f;
  };
}
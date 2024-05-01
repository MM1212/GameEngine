#pragma once

#include <glm/glm.hpp>

namespace Engine::Components {
  struct BaseLight {
  protected:
    BaseLight() = default;
    BaseLight(const BaseLight&) = default;
    BaseLight& operator=(const BaseLight&) = default;
    ~BaseLight() = default;

    BaseLight(float intensity, float range, glm::vec3 color) : color(glm::vec4{ color, intensity }), range(range) {}
    BaseLight(float intensity, glm::vec3 color) : color(glm::vec4{ color, intensity }) {}
    BaseLight(glm::vec4 color) : color(color) {}
    BaseLight(glm::vec4 color, float range) : color(color), range(range) {}

  public:
    glm::vec4 color = glm::vec4(1.0f);
    float range = 10.0f;
    uint8_t _pad0[12];
  };

  struct PointLight : public BaseLight {
    PointLight() = default;
    PointLight(const PointLight&) = default;
    PointLight& operator=(const PointLight&) = default;
    ~PointLight() = default;

    PointLight(float intensity, float range, glm::vec3 color) : BaseLight(intensity, range, color) {}
    PointLight(float intensity, glm::vec3 color) : BaseLight(intensity, color) {}
    PointLight(glm::vec4 color) : BaseLight(color) {}
    PointLight(glm::vec4 color, float range) : BaseLight(color, range) {}

  };

  // range unused
  struct GlobalLight : public BaseLight {
    GlobalLight() = default;
    GlobalLight(const GlobalLight&) = default;
    GlobalLight& operator=(const GlobalLight&) = default;
    ~GlobalLight() = default;

    GlobalLight(glm::vec3 color) : BaseLight(glm::vec4{color, 1.0f}) {}
    GlobalLight(float intensity, float range, glm::vec3 color) : BaseLight(intensity, range, color) {}
    GlobalLight(float intensity, glm::vec3 color) : BaseLight(intensity, color) {}
    GlobalLight(glm::vec4 color) : BaseLight(color) {}
    GlobalLight(glm::vec4 color, float range) : BaseLight(color, range) {}

  };
};
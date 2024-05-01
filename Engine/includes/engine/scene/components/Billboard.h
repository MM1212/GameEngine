#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Engine::Components {
  struct Billboard {  
    Billboard() = default;
    Billboard(const Billboard&) = default;
    Billboard& operator=(const Billboard&) = default;
    ~Billboard() = default;

    Billboard(glm::vec4 color, glm::vec2 size, bool rounded) : color(color), size(size), rounded(rounded) {}
    Billboard(glm::vec2 size) : size(size) {}
    glm::vec4 color = glm::vec4(1.0f);
    glm::vec2 size = glm::vec2(1.0f);
    bool rounded = false;
  };
}
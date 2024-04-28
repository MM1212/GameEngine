#pragma once

#include "defines.h"

#include <string_view>

namespace Engine::Renderers::Vulkan::Shaders {
  class Object : public Base {
  public:
    Object(Device& device);
    ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    void init();
  };
};
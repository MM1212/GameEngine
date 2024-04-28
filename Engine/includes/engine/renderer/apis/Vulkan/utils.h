#pragma once

#include <vulkan/vulkan.h>
#include <string_view>

namespace Engine::Renderers::Vulkan {
  bool IsCallResultSuccess(VkResult result);
  std::string_view CallResultToString(VkResult result, bool extended = false);
}
#pragma once

#include <vulkan/vulkan.h>
#include <utils/logger.h>
#include <utils/memory.h>
#include <renderer/FrameInfo.h>
#include "utils.h"

#ifdef _DEBUG
#define VK_CHECK(x) ASSERT_MSG(x == VK_SUCCESS, "Vulkan API Call failed with error code {}[{}] - {}", ::Engine::Renderers::Vulkan::CallResultToString(x), x, ::Engine::Renderers::Vulkan::CallResultToString(x, true))
#else
#define VK_CHECK(x) x
#endif

#ifndef VK_ENABLE_VALIDATION_LAYERS
# define VK_ENABLE_VALIDATION_LAYERS _DEBUG
#endif
#ifndef VK_ENABLE_DEBUG_MESSENGER
# define VK_ENABLE_DEBUG_MESSENGER VK_ENABLE_VALIDATION_LAYERS
#endif

namespace Engine::Renderers::Vulkan {
  class CommandBuffer;

  struct VkFrameInfo {
    FrameInfo& shared;

    uint32_t frameIndex;
    CommandBuffer& cmdBuffer;
    VkDescriptorSet globalDescriptorSet;
  };
  void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator
  );
  VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
  );
}
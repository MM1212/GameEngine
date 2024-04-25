#pragma once

#include <vulkan/vulkan.h>
#include <utils/logger.h>

#ifdef _DEBUG
#define VK_CHECK(x) { \
  VkResult result = x; \
  if (result != VK_SUCCESS) { \
    LOG_ERROR("Vulkan API Call {} failed with error code {}", #x, result); \
    _DEBUGBREAK(); \
  } \
}
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
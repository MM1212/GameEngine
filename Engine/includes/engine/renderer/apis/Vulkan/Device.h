#pragma once

#include "defines.h"
#include <vector>
#include <string_view>
#include <platform/Window.h>
#include <core/Application.h>

namespace Engine::Renderers::Vulkan {
  class Device {
  public:
    Device(ApplicationInfo& appInfo, Window& window);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    VkInstance getInstance() const { return this->instance; }
    const VkAllocationCallbacks* getAllocator() const { return this->allocator; }
  private:
    void createInstance();
#if VK_ENABLE_DEBUG_MESSENGER
    void setupDebugMessenger();
#endif
    void createSurface();
  private:
    std::vector<std::string_view> getRequiredExtensions();
    bool checkValidationLayerSupport();
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void checkHasWindowRequiredInstanceExtensions();
  private:
    ApplicationInfo& appInfo;
    Window& window;
    const VkAllocationCallbacks* allocator = nullptr;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    friend class Window;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<std::string_view> deviceExtensions = {};
  };
}
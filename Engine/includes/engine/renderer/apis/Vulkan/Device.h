#pragma once

#include "defines.h"
#include "CommandBuffer.h"
#include <platform/Window.h>
#include <core/Application.h>

#include <vector>
#include <string_view>

namespace Engine::Renderers::Vulkan {
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };
  struct QueueFamilyIndices {
    uint32_t graphicsFamily = static_cast<uint32_t>(-1);
    uint32_t presentFamily = static_cast<uint32_t>(-1);
    uint32_t computeFamily = static_cast<uint32_t>(-1);
    uint32_t transferFamily = static_cast<uint32_t>(-1);

    explicit operator bool() const {
      return graphicsFamily != -1 &&
        presentFamily != -1 &&
        computeFamily != -1 &&
        transferFamily != -1;
    }
  };
  struct PhysicalDeviceRequirements {
    bool graphics;
    bool compute;
    bool present;
    bool transfer;

    std::vector<std::string_view> extensions;
    bool sampleAnisotropy;
    bool discreteGpu;
  };
  struct PhysicalDeviceInfo {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    QueueFamilyIndices queueFamilyIndices;
    SwapChainSupportDetails swapChainSupport;
  };
  struct Queues {
    VkQueue graphics = VK_NULL_HANDLE;
    VkQueue present = VK_NULL_HANDLE;
    // VkQueue compute = VK_NULL_HANDLE;
    VkQueue transfer = VK_NULL_HANDLE;
  };

  class Fence;
  class Device {
  public:
    Device(ApplicationInfo& appInfo, Window& window);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    VkInstance getInstance() const { return this->instance; }
    operator VkDevice() const { return this->logicalDevice; }
    VkDevice getHandle() const { return this->logicalDevice; }
    const VkAllocationCallbacks* getAllocator() const { return this->allocator; }
    VkPhysicalDevice getPhysicalDevice() const { return this->physicalDevice; }
    VkDevice getLogicalDevice() const { return this->logicalDevice; }
    VkSurfaceKHR getSurface() const { return this->surface; }
    const PhysicalDeviceInfo& getPhysicalDeviceInfo() const { return this->physicalDeviceInfo; }
    const Queues& getQueues() const { return this->queues; }
    const QueueFamilyIndices getQueueFamilies() const { return this->physicalDeviceInfo.queueFamilyIndices; }
    const SwapChainSupportDetails& getSwapChainSupport() const { return this->physicalDeviceInfo.swapChainSupport; }
    void querySwapChainSupport();
    VkQueue getGraphicsQueue() const { return this->queues.graphics; }
    VkQueue getPresentQueue() const { return this->queues.present; }
    VkQueue getTransferQueue() const { return this->queues.transfer; }

    VkCommandPool getGraphicsCommandPool() const { return this->graphicsCommandPool; }

    VkResult waitIdle() const { return vkDeviceWaitIdle(this->logicalDevice); }
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    // Buffer Helper Functions
    void createBuffer(
      VkDeviceSize size,
      VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties,
      VkBuffer& buffer,
      VkDeviceMemory& bufferMemory,
      bool bindToBuffer = true
    );
    void copyBuffer(
      VkBuffer srcBuffer,
      VkBuffer dstBuffer,
      VkDeviceSize size,
      VkQueue queue = VK_NULL_HANDLE,
      VkCommandPool pool = VK_NULL_HANDLE,
      Fence* fence = nullptr,
      VkDeviceSize srcOffset = 0,
      VkDeviceSize dstOffset = 0
    );
    void copyBufferToImage(
      VkBuffer buffer,
      VkImage image,
      uint32_t width,
      uint32_t height,
      uint32_t layerCount,
      VkQueue queue = VK_NULL_HANDLE,
      VkCommandPool pool = VK_NULL_HANDLE,
      Fence* fence = nullptr
    );

    SingleTimeCommandBuffer createSingleTimeCmds(
      VkCommandPool pool,
      VkQueue queue,
      CommandBuffer::SubmitInfo info = {},
      bool isPrimary = true
    ) {
      ASSERT(pool, "Invalid command pool");
      ASSERT(queue, "Invalid queue");
      return SingleTimeCommandBuffer(*this, this->graphicsCommandPool, queue, info, isPrimary);
    }
    SingleTimeCommandBuffer createGraphicsSingleTimeCmds(CommandBuffer::SubmitInfo info = {}) {
      return this->createSingleTimeCmds(this->graphicsCommandPool, this->queues.graphics, info);
    }
  private:
    void createInstance();
#if VK_ENABLE_DEBUG_MESSENGER
    void setupDebugMessenger();
#endif
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createGraphicsCommandPool();

  private:
    std::vector<std::string_view> getRequiredExtensions() const;
    bool checkValidationLayerSupport() const;
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void checkHasWindowRequiredInstanceExtensions() const;
    bool isDeviceSuitable(VkPhysicalDevice device, const PhysicalDeviceRequirements& requirements, PhysicalDeviceInfo& properties) const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    SwapChainSupportDetails querySwapChainSupportForDevice(VkPhysicalDevice device) const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device, const PhysicalDeviceRequirements& requirements) const;
  private:
    ApplicationInfo& appInfo;
    Window& window;
    const VkAllocationCallbacks* allocator = nullptr;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    friend class Window;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    PhysicalDeviceInfo physicalDeviceInfo;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    Queues queues;

    VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<std::string_view> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  };
}
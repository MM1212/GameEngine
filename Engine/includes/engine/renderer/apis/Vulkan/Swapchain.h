#pragma once

#include "defines.h"
#include "Device.h"
#include "Image.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <memory>
#include <limits>

namespace Engine::Renderers::Vulkan {
  class Swapchain;
  struct SwapchainCreateInfo {
    VkExtent2D windowExtent{};
    std::shared_ptr<Swapchain> oldSwapchain = nullptr;
    bool vSync = false;
  };
  class Swapchain {
  public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    Swapchain(Device& device, SwapchainCreateInfo createInfo);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    VkSwapchainKHR getHandle() const { return this->handle; }
    VkExtent2D getExtent() const { return this->windowExtent; }
    uint32_t width() const { return this->windowExtent.width; }
    uint32_t height() const { return this->windowExtent.height; }
    VkFormat getImageFormat() const { return this->imageFormat.format; }
    VkImageView getImageView(uint32_t index) const { return this->imageViews[index]; }
    VkFormat getDepthFormat() const { return this->depthFormat; }
    VkImageView getDepthImageView(uint32_t index) const { return this->depthImages[index].getView(); }
    float getAspectRatio() const { return static_cast<float>(this->windowExtent.width) / static_cast<float>(this->windowExtent.height); }
    bool compareFormats(const Swapchain& other) const {
      return this->imageFormat.format == other.imageFormat.format &&
        this->imageFormat.colorSpace == other.imageFormat.colorSpace;
    }

    VkResult acquireNextImage(uint32_t* imageIndex, uint64_t timeout = std::numeric_limits<uint64_t>::max());
    VkResult presentImage(uint32_t imageIndex);
  private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choosePresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat findDepthFormat() const;
  private:
    Device& device;
    VkSwapchainKHR handle = VK_NULL_HANDLE;
    VkExtent2D windowExtent;
    bool vSync = false;
    std::shared_ptr<Swapchain> oldSwapchain = nullptr;

    VkSurfaceFormatKHR imageFormat;
    VkFormat depthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<Image> depthImages;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
  };
}
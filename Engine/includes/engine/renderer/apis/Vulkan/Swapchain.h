#pragma once

#include "defines.h"
#include "Device.h"
#include "Image.h"
#include "Framebuffer.h"
#include "RenderPass.h"

#include <engine/utils/memory.h>

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
    RenderPassCreateInfo mainRenderPassCreateInfo;
  };
  class Swapchain {
  public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    Swapchain(Device& device, const SwapchainCreateInfo& createInfo);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    operator VkSwapchainKHR() const { return this->handle; }
    VkSwapchainKHR getHandle() const { return this->handle; }
    VkExtent2D getExtent() const { return this->swapChainExtent; }
    uint32_t width() const { return this->swapChainExtent.width; }
    uint32_t height() const { return this->swapChainExtent.height; }
    RenderPass& getMainRenderPass() const { return *this->mainRenderPass; }
    uint32_t getImageCount() const { return static_cast<uint32_t>(this->images.size()); }
    VkFormat getImageFormat() const { return this->imageFormat.format; }
    VkImageView getImageView(uint32_t index) const { return this->imageViews[index]; }
    const std::vector<VkImageView>& getImageViews() const { return this->imageViews; }
    VkFormat getDepthFormat() const { return this->depthFormat; }
    VkImageView getDepthImageView(uint32_t index) const { return this->depthImages[index].getView(); }
    const std::vector<Image>& getDepthImages() const { return this->depthImages; }
    float getAspectRatio() const { return static_cast<float>(this->swapChainExtent.width) / static_cast<float>(this->windowExtent.height); }
    bool compareFormats(const Swapchain& other) const {
      return this->imageFormat.format == other.imageFormat.format &&
        this->imageFormat.colorSpace == other.imageFormat.colorSpace;
    }

    VkResult acquireNextImage(
      uint32_t* imageIndex,
      VkSemaphore imageAvailableSemaphore,
      VkFence fence = VK_NULL_HANDLE,
      uint64_t timeout = std::numeric_limits<uint64_t>::max()
    );
    VkResult presentImage(uint32_t imageIndex, VkSemaphore renderFinishedSemaphore);
  private:
    void init(const SwapchainCreateInfo& createInfo);
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createMainRenderPass(const RenderPassCreateInfo& createInfo);
    void createFramebuffers();

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

    Scope<RenderPass> mainRenderPass = nullptr;
    std::vector<Framebuffer> framebuffers;

    VkSurfaceFormatKHR imageFormat;
    VkFormat depthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<Image> depthImages;

    size_t currentFrame = 0;
  };
}
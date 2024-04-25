#include <renderer/apis/Vulkan/Device.h>
#include <core/EngineInfo.h>
#include <renderer/logger.h>
#include <vulkan/vulkan.h>

#include <unordered_set>

using namespace Engine::Renderers::Vulkan;

Device::Device(ApplicationInfo& appInfo, Window& window)
  : appInfo(appInfo), window(window) {
  this->createInstance();
#ifdef VK_ENABLE_DEBUG_MESSENGER
  this->setupDebugMessenger();
#endif
  this->createSurface();
  this->pickPhysicalDevice();
  this->createLogicalDevice();
  LOG_RENDERER_INFO("Vulkan device created.");
}

Device::~Device() {
  vkDestroyDevice(this->logicalDevice, this->allocator);
  vkDestroySurfaceKHR(this->instance, this->surface, this->allocator);
#ifdef VK_ENABLE_DEBUG_MESSENGER
  DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, this->allocator);
#endif
  vkDestroyInstance(this->instance, this->allocator);
  LOG_RENDERER_INFO("Vulkan device destroyed.");
}

void Device::createInstance() {
#if VK_ENABLE_VALIDATION_LAYERS
  ASSERT(this->checkValidationLayerSupport(), "Validation layers requested, but not available!");
#endif

  VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.pApplicationName = this->appInfo.windowInfo.title.data();
  appInfo.applicationVersion = VK_MAKE_API_VERSION(0, this->appInfo.version.major, this->appInfo.version.minor, this->appInfo.version.patch);
  appInfo.pEngineName = "GameEngine";
  appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pApplicationInfo = &appInfo;

  const auto requiredExtensions = this->getRequiredExtensions();
  std::vector<const char*> extensions(requiredExtensions.size());
  for (size_t i = 0; i < requiredExtensions.size(); i++) {
    extensions[i] = requiredExtensions[i].data();
  }
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

#if VK_ENABLE_VALIDATION_LAYERS
  createInfo.enabledLayerCount = static_cast<uint32_t>(this->validationLayers.size());
  createInfo.ppEnabledLayerNames = this->validationLayers.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  PopulateDebugMessengerCreateInfo(debugCreateInfo);
  createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

  VK_CHECK(vkCreateInstance(&createInfo, this->allocator, &this->instance));
  this->checkHasWindowRequiredInstanceExtensions();
  LOG_RENDERER_INFO("Vulkan instance created.");
}

void Device::createSurface() {
  VK_CHECK(static_cast<VkResult>(this->window.createVulkanSurface(this)));
  LOG_RENDERER_INFO("Vulkan surface created.");
}

#if VK_ENABLE_DEBUG_MESSENGER
void Device::setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  PopulateDebugMessengerCreateInfo(createInfo);
  VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
  LOG_RENDERER_INFO("Vulkan debug messenger created.");
}
#endif

std::vector<std::string_view> Device::getRequiredExtensions() const {
  auto extensions = this->window.getVulkanRequiredExtensions();

#if VK_ENABLE_VALIDATION_LAYERS
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

  return extensions;
}

void Device::checkHasWindowRequiredInstanceExtensions() const {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  LOG_RENDERER_TRACE("available extensions:");
  std::unordered_set<std::string_view> available;
  for (const auto& extension : extensions) {
    LOG_RENDERER_TRACE("\t{}", extension.extensionName);
    available.insert(extension.extensionName);
  }

  LOG_RENDERER_TRACE("required extensions:");
  auto requiredExtensions = this->getRequiredExtensions();
  for (const auto required : requiredExtensions) {
    LOG_RENDERER_TRACE("\t{}", required);
    if (available.find(required) == available.end()) {
#ifdef _DEBUG
      ASSERT(false, "Missing required glfw extension above!");
#else
      throw std::runtime_error("Missing required glfw extension above!");
#endif
    }
  }
}

bool Device::checkValidationLayerSupport() const {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* _layerName : this->validationLayers) {
    bool layerFound = false;
    const std::string_view layerName(_layerName);
    for (const auto& layerProperties : availableLayers) {
      const std::string_view layerPropertyName(layerProperties.layerName);
      if (layerName == layerPropertyName) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      return false;
    }
  }

  return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
);

void Device::PopulateDebugMessengerCreateInfo(
  VkDebugUtilsMessengerCreateInfoEXT& createInfo
) {
  createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;  // Optional
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) {
  switch (messageSeverity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    LOG_RENDERER_TRACE("Validation Layer: {}", pCallbackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    LOG_RENDERER_INFO("Validation Layer: {}", pCallbackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    LOG_RENDERER_WARN("Validation Layer: {}", pCallbackData->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    LOG_RENDERER_ERROR("Validation Layer: {}", pCallbackData->pMessage);
    break;
  default:
    LOG_RENDERER_CRITICAL("Validation Layer: {}", pCallbackData->pMessage);
    break;
  }
  return VK_FALSE;
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) const {
  QueueFamilyIndices indices{};

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  uint8_t minTransferScore = 255;
  for (const auto& queueFamily : queueFamilies) {
    uint8_t currentScore = 0;
    if (queueFamily.queueCount == 0)
      continue;

    // Graphics Queue
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      currentScore++;
    }

    // Compute Queue
    if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      indices.computeFamily = i;
      currentScore++;
    }

    // Transfer Queue
    if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      if (currentScore <= minTransferScore) {
        indices.transferFamily = i;
        minTransferScore = currentScore;
      }
    }

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }
    i++;
  }
  return indices;
}

SwapChainSupportDetails Device::querySwapChainSupportForDevice(VkPhysicalDevice device) const {
  SwapChainSupportDetails details;
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surface, &details.capabilities));

  uint32_t formatCount;
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, nullptr));

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, details.formats.data()));
  }

  uint32_t presentModeCount;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, nullptr));

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
      device,
      this->surface,
      &presentModeCount,
      details.presentModes.data()
    ));
  }
  return details;
}

void Device::querySwapChainSupport() {
  this->physicalDeviceInfo.swapChainSupport = this->querySwapChainSupportForDevice(this->physicalDevice);
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device, const PhysicalDeviceRequirements& requirements) const {
  uint32_t availableExtensionsCount = 0;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionsCount, nullptr));
  std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
  VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionsCount, availableExtensions.data()));
  for (const auto& requiredExtension : requirements.extensions) {
    bool found = false;
    for (const auto& availableExtension : availableExtensions) {
      if (requiredExtension == availableExtension.extensionName) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

bool Device::isDeviceSuitable(VkPhysicalDevice device, const PhysicalDeviceRequirements& requirements, PhysicalDeviceInfo& info) const {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  vkGetPhysicalDeviceMemoryProperties(device, &deviceMemoryProperties);

  info.properties = deviceProperties;
  info.features = deviceFeatures;
  info.memory = deviceMemoryProperties;
  if (requirements.discreteGpu) {
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return false;
  }

  info.queueFamilyIndices = this->findQueueFamilies(device);
  LOG_RENDERER_TRACE("Queue Support:");
  LOG_RENDERER_TRACE("\tGraphics | Compute | Transfer | Present | Device");
  LOG_RENDERER_TRACE("\t{:8} | {:7} | {:8} | {:7} | {}",
    static_cast<bool>(info.queueFamilyIndices.graphicsFamily != -1),
    static_cast<bool>(info.queueFamilyIndices.computeFamily != -1),
    static_cast<bool>(info.queueFamilyIndices.transferFamily != -1),
    static_cast<bool>(info.queueFamilyIndices.presentFamily != -1),
    info.properties.deviceName
  );
  if (requirements.compute && info.queueFamilyIndices.computeFamily == -1)
    return false;
  if (requirements.graphics && info.queueFamilyIndices.graphicsFamily == -1)
    return false;
  if (requirements.present && info.queueFamilyIndices.presentFamily == -1)
    return false;
  if (requirements.transfer && info.queueFamilyIndices.transferFamily == -1)
    return false;
  if (!static_cast<bool>(info.queueFamilyIndices))
    return false;
  // checks for swapchain support also, so it needs to go before querying swapchain support
  if (!this->checkDeviceExtensionSupport(device, requirements))
    return false;
  info.swapChainSupport = this->querySwapChainSupportForDevice(device);
  if (info.swapChainSupport.formats.empty() || info.swapChainSupport.presentModes.empty())
    return false;
  if (requirements.sampleAnisotropy && !deviceFeatures.samplerAnisotropy)
    return false;
  return true;
}

void Device::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr));
  if (deviceCount == 0)
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  LOG_RENDERER_TRACE("Found {} GPUs with Vulkan support.", deviceCount);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &deviceCount, devices.data()));

  PhysicalDeviceRequirements requirements{};
  requirements.graphics = true;
  requirements.compute = true;
  requirements.present = true;
  requirements.transfer = true;
  requirements.sampleAnisotropy = true;
  requirements.discreteGpu = true;
  requirements.extensions = this->deviceExtensions;

  for (auto device : devices) {
    if (this->isDeviceSuitable(device, requirements, this->physicalDeviceInfo)) {
      this->physicalDevice = device;
      break;
    }
  }
  if (this->physicalDevice == VK_NULL_HANDLE)
    throw std::runtime_error("Failed to find a suitable GPU!");
  LOG_RENDERER_INFO("Selected GPU: {}", this->physicalDeviceInfo.properties.deviceName);
  LOG_RENDERER_INFO("  - Driver Version: {}.{}.{}",
    VK_VERSION_MAJOR(this->physicalDeviceInfo.properties.driverVersion),
    VK_VERSION_MINOR(this->physicalDeviceInfo.properties.driverVersion),
    VK_VERSION_PATCH(this->physicalDeviceInfo.properties.driverVersion)
  );
  LOG_RENDERER_INFO("  - API Version: {}.{}.{}",
    VK_VERSION_MAJOR(this->physicalDeviceInfo.properties.apiVersion),
    VK_VERSION_MINOR(this->physicalDeviceInfo.properties.apiVersion),
    VK_VERSION_PATCH(this->physicalDeviceInfo.properties.apiVersion)
  );
  switch (this->physicalDeviceInfo.properties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    LOG_RENDERER_INFO("  - Device Type: Other");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    LOG_RENDERER_INFO("  - Device Type: Integrated GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    LOG_RENDERER_INFO("  - Device Type: Discrete GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    LOG_RENDERER_INFO("  - Device Type: Virtual GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    LOG_RENDERER_INFO("  - Device Type: CPU");
    break;
  default:
    LOG_RENDERER_INFO("  - Device Type: Unknown");
    break;
  }

  for (uint32_t i = 0; i < this->physicalDeviceInfo.memory.memoryHeapCount; i++) {
    const auto& heap = this->physicalDeviceInfo.memory.memoryHeaps[i];
    float sizeGb = static_cast<float>(heap.size) / (1024 * 1024 * 1024);
    if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
      LOG_RENDERER_INFO("  - Memory Heap {}: Device Local, Size: {:.2f} GB", i, sizeGb);
    else
      LOG_RENDERER_INFO("  - Memory Heap {}: Shared System, Size: {:.2f} GB", i, sizeGb);
  }
  LOG_RENDERER_INFO("  - Graphics Queue Family: {}", this->physicalDeviceInfo.queueFamilyIndices.graphicsFamily);
  LOG_RENDERER_INFO("  - Compute Queue Family: {}", this->physicalDeviceInfo.queueFamilyIndices.computeFamily);
  LOG_RENDERER_INFO("  - Transfer Queue Family: {}", this->physicalDeviceInfo.queueFamilyIndices.transferFamily);
  LOG_RENDERER_INFO("  - Present Queue Family: {}", this->physicalDeviceInfo.queueFamilyIndices.presentFamily);
}

void Device::createLogicalDevice() {
  ASSERT(this->physicalDevice, "Physical device not selected!");
  auto& indicesInfo = this->physicalDeviceInfo.queueFamilyIndices;
  bool presentSameAsGraphics = indicesInfo.presentFamily == indicesInfo.graphicsFamily;
  bool transferSameAsGraphics = indicesInfo.transferFamily == indicesInfo.graphicsFamily;
  uint32_t uniqueQueueFamilies = 1;
  if (!presentSameAsGraphics)
    uniqueQueueFamilies++;
  if (!transferSameAsGraphics)
    uniqueQueueFamilies++;
  std::vector<uint32_t> indices(uniqueQueueFamilies);
  uint32_t index = 0;
  indices[index++] = indicesInfo.graphicsFamily;
  if (!presentSameAsGraphics)
    indices[index++] = indicesInfo.presentFamily;
  if (!transferSameAsGraphics)
    indices[index++] = indicesInfo.transferFamily;
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies);
  std::vector<float> queuePriorities(uniqueQueueFamilies, 1.0f);
  for (uint32_t i = 0; i < uniqueQueueFamilies; i++) {
    auto& createInfo = queueCreateInfos[i];
    createInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    createInfo.queueFamilyIndex = indices[i];
    createInfo.queueCount = 1;
    // TODO: required for MT rendering
    if (indices[i] == indicesInfo.graphicsFamily)
      createInfo.queueCount = 2;
    createInfo.pQueuePriorities = queuePriorities.data();
    LOG_TRACE("Queue Family: {}, Queue Count: {}", indices[i], createInfo.queueCount);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  std::vector<const char*> extensions(this->deviceExtensions.size());
  for (size_t i = 0; i < this->deviceExtensions.size(); i++) {
    extensions[i] = this->deviceExtensions[i].data();
  }
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VK_CHECK(vkCreateDevice(this->physicalDevice, &createInfo, this->allocator, &this->logicalDevice));
  LOG_RENDERER_INFO("Logical device created.");
  vkGetDeviceQueue(this->logicalDevice, indicesInfo.graphicsFamily, 0, &this->queues.graphics);
  vkGetDeviceQueue(this->logicalDevice, indicesInfo.presentFamily, 0, &this->queues.present);
  vkGetDeviceQueue(this->logicalDevice, indicesInfo.transferFamily, 0, &this->queues.transfer);
  LOG_RENDERER_INFO("Queues obtained.");
}

VkFormat Device::findSupportedFormat(
  const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features
) const {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
      return format;
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
      return format;
  }
  throw std::runtime_error("failed to find supported format!");
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
      (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  LOG_RENDERER_WARN("Failed to find suitable memory type!");
  return static_cast<uint32_t>(-1);
}
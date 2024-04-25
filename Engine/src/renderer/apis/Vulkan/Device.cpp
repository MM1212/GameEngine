#include <renderer/apis/Vulkan/Device.h>
#include <core/EngineInfo.h>
#include <utils/logger.h>

#include <unordered_set>

using namespace Engine::Renderers::Vulkan;

Device::Device(ApplicationInfo& appInfo, Window& window)
  : appInfo(appInfo), window(window) {
  this->createInstance();
#ifdef VK_ENABLE_DEBUG_MESSENGER
  this->setupDebugMessenger();
#endif
  this->createSurface();
  LOG_INFO("Vulkan device created.");
}

Device::~Device() {
  vkDestroySurfaceKHR(this->instance, this->surface, this->allocator);
#ifdef VK_ENABLE_DEBUG_MESSENGER
  DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, this->allocator);
#endif
  vkDestroyInstance(this->instance, this->allocator);
  LOG_INFO("Vulkan device destroyed.");
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
  LOG_INFO("Vulkan instance created.");
}

void Device::createSurface() {
  VK_CHECK(static_cast<VkResult>(this->window.createVulkanSurface(this)));
  LOG_INFO("Vulkan surface created.");
}

#if VK_ENABLE_DEBUG_MESSENGER
void Device::setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  PopulateDebugMessengerCreateInfo(createInfo);
  VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
  LOG_INFO("Vulkan debug messenger created.");
}
#endif

std::vector<std::string_view> Device::getRequiredExtensions() {
  auto extensions = this->window.getVulkanRequiredExtensions();

#if VK_ENABLE_VALIDATION_LAYERS
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

  return extensions;
}

void Device::checkHasWindowRequiredInstanceExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  LOG_TRACE("available extensions:");
  std::unordered_set<std::string_view> available;
  for (const auto& extension : extensions) {
    LOG_TRACE("\t{}", extension.extensionName);
    available.insert(extension.extensionName);
  }

  LOG_TRACE("required extensions:");
  auto requiredExtensions = this->getRequiredExtensions();
  for (const auto required : requiredExtensions) {
    LOG_TRACE("\t{}", required);
    ASSERT(available.find(required) != available.end(), "Missing required glfw extension above!");
  }
}

bool Device::checkValidationLayerSupport() {
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
  createInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
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
      LOG_TRACE("Validation Layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      LOG_INFO("Validation Layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      LOG_WARN("Validation Layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      LOG_ERROR("Validation Layer: {}", pCallbackData->pMessage);
      break;
    default:
      LOG_CRITICAL("Validation Layer: {}", pCallbackData->pMessage);
      break;
  }
  return VK_FALSE;
}
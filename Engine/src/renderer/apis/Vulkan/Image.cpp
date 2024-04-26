#include "renderer/apis/Vulkan/Image.h"

#include <utils/asserts.h>

using namespace Engine::Renderers::Vulkan;

Image::Image(Device& device, const ImageCreateInfo& createInfo)
  : device(device), type(createInfo.type),
  size({ createInfo.extent.width, createInfo.extent.height }),
  format(createInfo.format), tiling(createInfo.tiling),
  usage(createInfo.usage), memoryProperties(createInfo.memoryProperties) {
  this->init(createInfo);
}

Image::Image(Image&& other) : device(other.device) {
  *this = std::move(other);
}

Image& Image::operator=(Image&& other) {
  if (this == &other)
    return *this;
  this->handle = other.handle;
  this->view = other.view;
  this->memory = other.memory;
  this->type = other.type;
  this->size = other.size;
  this->format = other.format;
  this->tiling = other.tiling;
  this->usage = other.usage;
  this->memoryProperties = other.memoryProperties;
  this->viewAspectFlags = other.viewAspectFlags;
  other.handle = VK_NULL_HANDLE;
  other.view = VK_NULL_HANDLE;
  other.memory = VK_NULL_HANDLE;
  return *this;
}

Image::~Image() {
  if (this->view != VK_NULL_HANDLE)
    vkDestroyImageView(this->device, this->view, this->device.getAllocator());
  if (this->memory != VK_NULL_HANDLE)
    vkFreeMemory(this->device, this->memory, this->device.getAllocator());
  if (this->handle != VK_NULL_HANDLE)
    vkDestroyImage(this->device, this->handle, this->device.getAllocator());
}

void Image::init(const ImageCreateInfo& createInfo) {
  VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
  imageInfo.imageType = this->type;
  imageInfo.extent = createInfo.extent;
  imageInfo.mipLevels = createInfo.mipLevels;
  imageInfo.arrayLayers = createInfo.arrayLayers;
  imageInfo.format = this->format;
  imageInfo.tiling = this->tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = this->usage;
  imageInfo.samples = createInfo.samples;
  imageInfo.sharingMode = createInfo.sharingMode;

  VK_CHECK(vkCreateImage(this->device, &imageInfo, this->device.getAllocator(), &this->handle));

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(this->device, this->handle, &memoryRequirements);

  VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  allocateInfo.allocationSize = memoryRequirements.size;
  allocateInfo.memoryTypeIndex = this->device.findMemoryType(memoryRequirements.memoryTypeBits, this->memoryProperties);
  ASSERT(allocateInfo.memoryTypeIndex != -1, "Failed to find suitable memory type");

  VK_CHECK(vkAllocateMemory(this->device, &allocateInfo, this->device.getAllocator(), &this->memory));
  VK_CHECK(vkBindImageMemory(this->device, this->handle, this->memory, 0));

  if (createInfo.createView)
    this->createView(createInfo.viewCreateInfo);
}

void Image::createView(const ImageViewCreateInfo& createViewInfo) {
  ASSERT(this->view == VK_NULL_HANDLE, "Image view already exists");
  this->viewAspectFlags = viewAspectFlags;
  VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
  viewInfo.image = this->handle;
  switch (this->type) {
  case VK_IMAGE_TYPE_1D:
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
    break;
  case VK_IMAGE_TYPE_2D:
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    break;
  case VK_IMAGE_TYPE_3D:
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
    break;
  default:
    ASSERT(false, "Invalid image type");
  };
  viewInfo.format = createViewInfo.format != VK_FORMAT_UNDEFINED ? createViewInfo.format : this->format;
  viewInfo.subresourceRange = createViewInfo.subresourceRange;

  VK_CHECK(vkCreateImageView(this->device, &viewInfo, this->device.getAllocator(), &this->view));
}
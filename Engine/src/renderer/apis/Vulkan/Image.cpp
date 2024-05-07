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

void Image::transitionLayout(
  CommandBuffer& cmdBuffer,
  uint32_t queueFamilyIndex,
  VkImageLayout oldLayout,
  VkImageLayout newLayout
) {
  VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = queueFamilyIndex;
  barrier.dstQueueFamilyIndex = queueFamilyIndex;
  barrier.image = this->handle;
  barrier.subresourceRange.aspectMask = this->viewAspectFlags;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else
    ASSERT(false, "Unsupported layout transition");

  vkCmdPipelineBarrier(
    cmdBuffer,
    sourceStage,
    destinationStage,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );
}

void Image::copyFromBuffer(
  CommandBuffer& cmdBuffer,
  VkBuffer buffer,
  VkDeviceSize bufferOffset
) {
  VkBufferImageCopy region = {};
  region.bufferOffset = bufferOffset;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = this->viewAspectFlags;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageExtent.width = this->width;
  region.imageExtent.height = this->height;
  region.imageExtent.depth = 1;

  region.imageOffset = { 0, 0, 0 };

  vkCmdCopyBufferToImage(
    cmdBuffer,
    buffer,
    this->handle,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    1,
    &region
  );
}
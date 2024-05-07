#include "renderer/apis/Vulkan/Texture2D.h"
#include "renderer/apis/Vulkan/MemBuffer.h"

using namespace Engine::Renderers::Vulkan;

Texture2D::Texture2D(Device& device, const TextureSpecification& spec)
  : device(device), spec(spec), Size2D<uint32_t>{spec.size}, path("") {
  this->init();
}

Texture2D::~Texture2D() {
  this->device.waitIdle();
  if (this->sampler != VK_NULL_HANDLE)
    vkDestroySampler(this->device, this->sampler, this->device.getAllocator());
}

VkFormat Texture2D::TexChannelsToVkFormat(TextureChannels channels) {
  switch (channels) {
    case TextureChannels::R8: return VK_FORMAT_R8_UNORM;
    case TextureChannels::RG8: return VK_FORMAT_R8G8_UNORM;
    case TextureChannels::RGB8: return VK_FORMAT_R8G8B8_UNORM;
    case TextureChannels::RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
    default: return VK_FORMAT_UNDEFINED;
  }
}

static VkFilter TextureFilterToVkFilter(Engine::TextureSamplerSpecification::Filter filter) {
  using Sampler = Engine::TextureSamplerSpecification;
  switch (filter) {
    case Sampler::Filter::Nearest: return VK_FILTER_NEAREST;
    case Sampler::Filter::Linear: return VK_FILTER_LINEAR;
    default: return VK_FILTER_NEAREST;
  }
}

static VkSamplerMipmapMode TextureMipmapModeToVkMipmapMode(Engine::TextureSamplerSpecification::MipmapMode mode) {
  using Sampler = Engine::TextureSamplerSpecification;
  switch (mode) {
    case Sampler::MipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case Sampler::MipmapMode::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
  }
}

static VkSamplerAddressMode TextureWrapToVkWrap(Engine::TextureSamplerSpecification::Wrap wrap) {
  using Sampler = Engine::TextureSamplerSpecification;
  switch (wrap) {
    case Sampler::Wrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case Sampler::Wrap::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case Sampler::Wrap::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case Sampler::Wrap::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  }
}

static VkBorderColor TextureBorderColorToVkBorderColor(Engine::TextureSamplerSpecification::BorderColor color) {
  using Sampler = Engine::TextureSamplerSpecification;
  switch (color) {
    case Sampler::BorderColor::FloatTransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case Sampler::BorderColor::IntTransparentBlack: return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    case Sampler::BorderColor::FloatOpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case Sampler::BorderColor::IntOpaqueBlack: return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    case Sampler::BorderColor::FloatOpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    case Sampler::BorderColor::IntOpaqueWhite: return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
    default: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
  }
}

VkSamplerCreateInfo Texture2D::CreateSamplerInfo(const TextureSamplerSpecification& spec) {
  using Sampler = TextureSamplerSpecification;
  VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
  samplerInfo.magFilter = TextureFilterToVkFilter(spec.magFilter);
  samplerInfo.minFilter = TextureFilterToVkFilter(spec.minFilter);
  samplerInfo.addressModeU = TextureWrapToVkWrap(spec.wrap.s);
  samplerInfo.addressModeV = TextureWrapToVkWrap(spec.wrap.t);
  samplerInfo.addressModeW = TextureWrapToVkWrap(spec.wrap.r);
  samplerInfo.anisotropyEnable = spec.anisotropy;
  samplerInfo.maxAnisotropy = spec.anisotropy ? 16.0f : 1.0f;
  samplerInfo.borderColor = TextureBorderColorToVkBorderColor(spec.borderColor);
  samplerInfo.unnormalizedCoordinates = spec.unnormalizedCoordinates;
  samplerInfo.compareEnable = spec.compareEnable;
  samplerInfo.compareOp = static_cast<VkCompareOp>(spec.compareOp);
  samplerInfo.mipmapMode = TextureMipmapModeToVkMipmapMode(spec.mipmapMode);
  samplerInfo.minLod = spec.minLod;
  samplerInfo.maxLod = spec.maxLod;
  return samplerInfo;
}

void Texture2D::init() {
  VkFormat format = TexChannelsToVkFormat(this->spec.channelCount);

  ImageCreateInfo createInfo = {};
  createInfo.type = VK_IMAGE_TYPE_2D;
  createInfo.extent = { this->width, this->height, 1 };
  createInfo.format = format;
  createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  createInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  createInfo.viewCreateInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.createView = true;

  this->image = MakeScope<Image>(this->device, createInfo);

  VkSamplerCreateInfo samplerInfo = CreateSamplerInfo(this->spec.sampler);

  VK_CHECK(vkCreateSampler(this->device, &samplerInfo, this->device.getAllocator(), &this->sampler));
  this->generation++;
}

void Texture2D::bind(uint32_t slot) const {}
void Texture2D::loadData(const void* data, uint32_t size) {
  VkDeviceSize imageSize = this->width * this->height * static_cast<uint32_t>(this->spec.channelCount);
  MemBuffer stagingBuffer(
    this->device, imageSize, 1,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  stagingBuffer.map();
  stagingBuffer.writeTo(data, imageSize);
  stagingBuffer.unmap();

  {
    auto cmdBuffer = this->device.createGraphicsSingleTimeCmds();
    this->image->transitionLayout(cmdBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    this->image->copyFromBuffer(stagingBuffer);
    this->image->transitionLayout(cmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }
  this->generation++;
}
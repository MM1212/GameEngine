#include "renderer/Texture.h"
#include <engine/renderer/apis/Vulkan/VulkanRenderer.h>

using Engine::Texture2D;

Engine::Ref<Texture2D> Texture2D::CreateFromFile(std::string_view path) {
  switch (Renderer::GetAPI()) {
    case Renderer::API::Vulkan:
      return Renderers::Vulkan::Renderer::Get()->createTexture2D(path);
    default:
      LOG_ERROR("Renderer API {} not supported", Renderer::GetApiName(Renderer::GetAPI()));
      return nullptr;
  }
}

Engine::Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec) {
  switch (Renderer::GetAPI()) {
    case Renderer::API::Vulkan:
      return Renderers::Vulkan::Renderer::Get()->createTexture2D(spec);
    default:
      LOG_ERROR("Renderer API {} not supported", Renderer::GetApiName(Renderer::GetAPI()));
      return nullptr;
  }
}
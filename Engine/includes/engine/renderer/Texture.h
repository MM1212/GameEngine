#pragma once

#include <engine/utils/glm.h>
#include <engine/utils/memory.h>

#include <string>
#include <string_view>

namespace Engine {
  enum class TextureChannels {
    R8 = 1,
    RG8 = 2,
		RGB8 = 3,
		RGBA8 = 4
  };

  struct TextureSamplerSpecification {
    enum class Filter {
      Nearest = 0,
      Linear = 1
    };
    enum class Wrap {
      Repeat = 0,
      MirroredRepeat = 1,
      ClampToEdge = 2,
      ClampToBorder = 3
    };
    enum class Compare {
      Never = 0,
      Less = 1,
      Equal = 2,
      LessOrEqual = 3,
      Greater = 4,
      NotEqual = 5,
      GreaterOrEqual = 6,
      Always = 7
    };
    enum class MipmapMode {
      Nearest = 0,
      Linear = 1
    };
    enum class BorderColor {
      FloatTransparentBlack = 0,
      IntTransparentBlack = 1,
      FloatOpaqueBlack = 2,
      IntOpaqueBlack = 3,
      FloatOpaqueWhite = 4,
      IntOpaqueWhite = 5
    };

    Filter minFilter = Filter::Linear;
    Filter magFilter = Filter::Linear;
    glm::tvec3<Wrap> wrap = { Wrap::Repeat, Wrap::Repeat, Wrap::Repeat };

    bool anisotropy = true;
    float maxAnisotropy = 16.0f;
    bool compareEnable = false;
    Compare compareOp = Compare::Never;
    float minLod = 0.0f;
    float maxLod = 0.0f;
    MipmapMode mipmapMode = MipmapMode::Linear;
    BorderColor borderColor = BorderColor::IntOpaqueBlack;
    bool unnormalizedCoordinates = false;
  };

  struct TextureSpecification : Size2D<uint32_t> {
    TextureChannels channelCount = TextureChannels::RGBA8;
    bool autoRelease = false;
    bool transparent = false;
    uint32_t generation;
    TextureSamplerSpecification sampler{};
  };

  class Texture {
  public:
    virtual ~Texture() = default;

    virtual const TextureSpecification& getSpecification() const = 0;

    virtual const glm::uvec2& getSize() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual std::string_view getPath() const = 0;
    virtual uint64_t getId() const = 0;
    virtual bool isLoaded() const = 0;

    virtual void bind(uint32_t slot = 0) const = 0;
    virtual void loadData(const void* data, uint32_t size) = 0;

    virtual bool operator==(const Texture& other) const = 0;
  protected:
    Texture() = default;

    uint64_t id;
    bool loaded;
  };

  class Texture2D : public Texture {
  public:
    virtual ~Texture2D() = default;

    static Ref<Texture2D> CreateFromFile(std::string_view path);
    static Ref<Texture2D> Create(const TextureSpecification& spec);
  };
}
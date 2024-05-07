#pragma once

#include <engine/utils/hash.h>

#include <string_view>
#include <unordered_map>

namespace Engine {
  enum class Pools : uint64_t {
    RendererVertices = Hash("RENDERER_VERTICES"),
    RendererIndices = Hash("RENDERER_INDICES"),
  };
  class PoolManager {
  public:
    class Pool {
    public:
      Pool() = delete;
#ifdef _DEBUG
      Pool(std::string_view name, uint64_t size);
#endif
      Pool(uint16_t id, uint64_t size);
      ~Pool() = default;

      Pool(const Pool& other) = delete;
      Pool& operator=(const Pool& other) = delete;
      Pool(Pool&& other) noexcept = default;
      Pool& operator=(Pool&& other) noexcept = default;

      // will abort in debug mode if the pool is full
      bool isEmpty() const { return this->used == 0; }
      bool isFull() const { return this->used == this->size; }
      bool fill(uint64_t amount = 1);
      bool empty(uint64_t amount = 1);
      bool operator+=(uint64_t amount) { return this->fill(amount); }
      bool operator-=(uint64_t amount) { return this->empty(amount); }
      float getUsage() const { return static_cast<float>(this->used) / this->size; }

      uint64_t getSize() const { return this->used; }
      uint64_t getMaxSize() const { return this->size; }
#ifdef _DEBUG
      std::string_view getName() const { return this->name; }
#endif
      uint64_t getId() const { return this->id; }
    private:
      friend class PoolManager;
#ifdef _DEBUG
      std::string name;
#endif
      uint64_t id;
      uint64_t size;
      uint64_t used = 0;
    };
    using iterator = std::unordered_map<uint64_t, Pool>::iterator;
    static bool Init();
    static iterator Begin() { return instance.pools.begin(); }
    static iterator End() { return instance.pools.end(); }
    static Pool* Get(Pools pool) {
      auto it = instance.pools.find(static_cast<uint64_t>(pool));
      if (it == instance.pools.end())
        return nullptr;
      return &it->second;
    }
    template<Pools P>
    static constexpr Pool* Get() { return Get(P); }
    static uint64_t Count() { return instance.pools.size(); }
  private:
    static constexpr std::string_view PoolConfigFilePath = "assets/configs/pool_sizes.yaml";

    PoolManager() = default;
    ~PoolManager() = default;
  private:
    std::unordered_map<uint64_t, Pool> pools;
    bool initialized = false;
    static PoolManager instance;
  };
}
#include "core/PoolManager.h"
#include <utils/asserts.h>
#include <yaml-cpp/yaml.h>

using namespace Engine;

#ifdef _DEBUG
PoolManager::Pool::Pool(std::string_view name, uint64_t size) : name(name), size(size) {
  this->id = Hash(name);
}
#endif
PoolManager::Pool::Pool(uint16_t id, uint64_t size) : id(id), size(size) {}

bool PoolManager::Pool::fill(uint64_t amount) {
  if (this->used + amount > this->size) {
    ASSERT_MSG(false, "Pool {} is full", this->name);
    return false;
  }
  this->used += amount;
  return true;
}

bool PoolManager::Pool::empty(uint64_t amount) {
  if (this->used < amount) {
    ASSERT_MSG(false, "Pool {} is empty", this->name);
    return false;
  }
  this->used -= amount;
  return true;
}

PoolManager PoolManager::instance;

bool PoolManager::Init() {
  if (instance.initialized)
    return false;
  try {
    auto config = YAML::LoadFile(std::string{ PoolConfigFilePath });
    for (const auto& pool : config["pools"]) {
      auto name = pool["name"].as<std::string>();
      auto id = Hash(name);
      auto size = pool["size"].as<uint64_t>();
#ifdef _DEBUG
      instance.pools.emplace(id, Pool{ name, size });
      LOG_INFO("Pool {} initialized with size {}", name, size);
#else
      instance.pools.emplace(id, Pool{ id, size });
#endif
    }
  }
  catch (const std::exception& e) {
    ASSERT_MSG(false, "Failed to load pool configuration file: {}", e.what());
    return false;
  }
  instance.initialized = true;
  return true;
}
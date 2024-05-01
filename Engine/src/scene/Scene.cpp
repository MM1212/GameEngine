#include "engine/scene/Scene.h"
#include <engine/scene/Entity.h>

using Engine::Scene;
using Engine::Entity;

Entity Scene::createEntity(const std::string_view tag) {
  Entity entity{ this->registry.create(), this };
  Entity::UUID uuid = Components::ID::GenerateId();
  if (tag.empty())
    entity.addComponent<Components::ID>("Entity " + std::to_string(uuid), uuid);
  else
    entity.addComponent<Components::ID>(tag, uuid);
  entity.addComponent<Components::Transform>();
  this->entityMap[uuid] = entity.getHandle();
  return entity;
}
void Scene::destroyEntity(Entity entity) {
  this->registry.destroy(entity);
  this->entityMap.erase(entity.getUniqId());
}

bool Scene::entityExists(entt::entity id) const {
  return this->registry.valid(id);
}

bool Scene::entityExists(uint64_t uuid) const {
  return this->entityMap.find(uuid) != this->entityMap.end();
}

Entity Scene::getEntity(entt::entity id) {
  if (!this->entityExists(id))
    return Entity{};
  return Entity{ id, this };
}

std::vector<Entity> Scene::getEntitiesByTag(const std::string_view tag) {
  auto view = this->viewEntitiesWith<Components::ID>();
  std::vector<Entity> entities;
  entities.reserve(view.size());
  for (auto handle : view) {
    auto& id = this->registry.get<Components::ID>(handle);
    if (id.tag == tag)
      entities.emplace_back(handle, this);
  }
  return entities;
}
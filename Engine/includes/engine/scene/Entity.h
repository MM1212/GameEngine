#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <engine/scene/components/ID.h>
#include <engine/scene/components/Transform.h>
#include <engine/scene/Scene.h>

namespace Engine {
  class Entity {
  public:
    using ID = entt::entity;
    using UUID = uint64_t;
    Entity() = default;
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(ID handle, Scene* scene);
    Entity(ID handle, Scene& scene);
    ~Entity() = default;

    operator bool() const { return this->handle != entt::null; }
    operator ID() const { return this->handle; }
    bool operator==(const Entity& other) const { return this->handle == other.handle && this->scene == other.scene; }
    bool operator!=(const Entity& other) const { return !this->operator==(other); }

    void setHandle(ID handle) { this->handle = handle; }

    template<typename Component, typename... Args>
    Component& addComponent(Args&&... args) {
      assert(this->scene && "Entity doesn't belong to a scene");
      assert(!this->hasComponent<Component>() && "Entity already has component");
      return this->scene->registry.emplace<Component>(this->handle, std::forward<Args>(args)...);
    }

    template<typename Component>
    const Component& getComponent() const {
      assert(this->scene && "Entity doesn't belong to a scene");
      assert(this->hasComponent<Component>() && "Entity does not have component");
      return this->scene->registry.get<Component>(this->handle);
    }

    template<typename Component>
    Component& getComponent() {
      assert(this->scene && "Entity doesn't belong to a scene");
      assert(this->hasComponent<Component>() && "Entity does not have component");
      return this->scene->registry.get<Component>(this->handle);
    }

    template<typename ...Components>
    bool hasComponent() const {
      assert(this->scene && "Entity doesn't belong to a scene");
      return this->scene->registry.any_of<Components...>(this->handle);
    }

    template<typename Component, typename ...OtherComponents>
    bool removeComponent() {
      assert(this->scene && "Entity doesn't belong to a scene");
      assert(!this->hasComponent<Component>() && "Entity does not have component");
      return this->scene->registry.remove<Component, OtherComponents...>(this->handle) != 0;
    }

    // shortcuts
    entt::entity getHandle() const { return this->handle; }
    UUID getUniqId() const { return this->getComponent<Components::ID>().id; }
    const std::string_view getTag() const { return this->getComponent<Components::ID>().tag; }
    const auto& translation() const { return this->getComponent<Components::Transform>().translation; }
    auto& translation() { return this->getComponent<Components::Transform>().translation; }
    const auto& scale() const { return this->getComponent<Components::Transform>().scale; }
    auto& scale() { return this->getComponent<Components::Transform>().scale; }
    // TODO: when we go to 3d, make this return a ref
    auto rotation() const { return this->getComponent<Components::Transform>().rotation; }
    const Components::Transform& transform() const { return this->getComponent<Components::Transform>(); }
    Components::Transform& transform() { return this->getComponent<Components::Transform>(); }

  private:
    ID handle = entt::null;
    Scene* scene = nullptr;
  };
}
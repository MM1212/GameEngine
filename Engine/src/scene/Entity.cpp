#include "engine/scene/Entity.h"

using Engine::Entity;

Entity::Entity(ID handle, Scene* scene) : handle(handle), scene(scene) {}
Entity::Entity(ID handle, Scene& scene) : handle(handle), scene(&scene) {}
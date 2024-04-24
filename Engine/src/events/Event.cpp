#include "events/Event.h"
#include <utils/hash.h>

using Engine::EventTag;

constexpr EventTag::EventTag(Tag name) : name(name), id(Hash(name)) {}
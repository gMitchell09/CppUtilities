#include "ComponentExample.h"

#include "entt/entt.hpp"
#include "ImguiUtils/RAIID.h"

namespace Components {
void ComponentExample::drawEditor(entt::registry& registry, entt::entity entity) {
    RAIID _a(entt::to_integral(entity));
    RAIID _b(name().c_str());
}
}

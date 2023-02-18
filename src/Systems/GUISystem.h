#pragma once

#include <entt/fwd.hpp>

class GUISystem {
    GUISystem() = default;
    GUISystem(GUISystem& other) = default;
    GUISystem(GUISystem&& other) = default;
    GUISystem& operator=(GUISystem& other) = default;
    GUISystem& operator=(GUISystem&& other) = default;

public:
    static bool Update(entt::registry& registry);
};
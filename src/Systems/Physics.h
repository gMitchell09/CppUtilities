#pragma once

#include <entt/entt.hpp>

#include <Components/Position.h>
#include <Components/Velocity.h>

namespace Systems {
    using TimestepT = float;
    using DeltaTimestepT = float;

    void PhysicsUpdate(entt::registry& registry, TimestepT currentTime, DeltaTimestepT deltaTime) {
        // Let's do something trivial...
        // TODO: Add position, velocity components
        auto posVelView = registry.view<Components::Position, Components::Velocity>();
        for (const auto ent : posVelView) {
            auto& posComp = posVelView.get<Components::Position>(ent);
            const auto& velComp = posVelView.get<Components::Velocity>(ent);

            posComp.Value[0] += deltaTime * velComp.Value[0];
            posComp.Value[1] += deltaTime * velComp.Value[1];
            posComp.Value[2] += deltaTime * velComp.Value[2];
        }
        
    }
}
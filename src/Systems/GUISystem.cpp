#include "GUISystem.h"

#include <entt/entt.hpp>

#include "Widgets/EntityEditor.h"
#include "imgui.h"

bool GUISystem::Update(entt::registry& registry) {
    static entt::entity s_selectedEntity;
    
    ImGui::Begin("Entity Widget");
    DrawEntityEditor(registry, s_selectedEntity);
    ImGui::End();

    return true;
}
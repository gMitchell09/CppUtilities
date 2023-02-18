#include "Velocity.h"

#include "entt/entt.hpp"
#include "ImguiUtils/RAIID.h"
#include "ImguiUtils/UIState.h"

#include <misc/cpp/imgui_stdlib.h>
#include <array>

static UIStateStorage<ImGuiID, std::array<float, 3>> s_StateStorage;

namespace Components {
void Velocity::drawEditor(entt::registry& registry, entt::entity entity) {
    RAIID _a(entt::to_integral(entity));
    RAIID _b(name().c_str());

    auto [velLabel, velPtr] = s_StateStorage.getDataPtr<std::array<float, 3>>("##Value", Value);
    ImGui::InputFloat3(velLabel, velPtr->data());
    ImGui::SameLine();
    if (ImGui::Button("Set")) Value = *velPtr;
}
}

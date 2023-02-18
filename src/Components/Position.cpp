#include "Position.h"

#include "entt/entt.hpp"
#include "ImguiUtils/RAIID.h"
#include "ImguiUtils/UIState.h"

#include <array>
static UIStateStorage<ImGuiID, std::array<float, 3>> s_StateStorage;

namespace Components {
void Position::drawEditor(entt::registry& registry, entt::entity entity) {
    RAIID _a(entt::to_integral(entity));
    RAIID _b(name().c_str());

    auto [label, dataPtr] = s_StateStorage.getDataPtr<std::array<float, 3>>("##Value", Value);
    static bool update = true;
    ImGui::Checkbox("Update", &update);
    ImGui::InputFloat3(label, dataPtr->data());
    ImGui::SameLine();
    if (ImGui::Button("Set")) Value = *dataPtr;
    if (update) *dataPtr = Value;
}
}

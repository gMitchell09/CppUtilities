#include "Name.h"

#include "ImguiUtils/UIState.h"
#include "ImguiUtils/RAIID.h"

#include <misc/cpp/imgui_stdlib.h>

using StoredType = std::string;
static UIStateStorage<ImGuiID, StoredType> s_StateStorage;

namespace Components {
void Name::drawEditor(entt::registry& registry, entt::entity entity) {
    RAIID _a(entt::to_integral(entity));
    RAIID _b(name().c_str());

    auto [label, nameSzPtr] = s_StateStorage.getDataPtr<std::string>("##Name", Value);
    ImGui::InputText(label, nameSzPtr);
    ImGui::SameLine();
    if (ImGui::Button("Set")) Value = *nameSzPtr;
}
}
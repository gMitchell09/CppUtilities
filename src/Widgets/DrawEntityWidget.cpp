#include "DrawEntityWidget.h"
#include "Components/Name.h"
#include "EntityEditor.h"

#include <imgui.h>

static std::string getName(entt::registry& registry, entt::entity entity) {
    std::string name;

    if (entity == entt::null || !registry.valid(entity)) return name;
    if (registry.all_of<Components::Name>(entity)) name = registry.get<Components::Name>(entity).Value;
    else name = std::to_string(entt::to_integral(entity));

    return name;
}

entt::entity DrawEntityWidget(entt::registry& registry, entt::entity selectedEntity) {
    if (ImGui::Button("New Entity")) {
        selectedEntity = registry.create();
        auto& nameComp = registry.emplace<Components::Name>(selectedEntity);
        nameComp.Value = "Entity " + std::to_string(entt::to_integral(selectedEntity));
    }

    const char* textLabel = "Entity";
    if (ImGui::BeginCombo(textLabel, getName(registry, selectedEntity).c_str())) {
        registry.each([&selectedEntity, &registry](auto entity) {
            std::string name = getName(registry, entity);
            if (ImGui::Selectable(name.c_str(), entity == selectedEntity)) selectedEntity = entity;
        });
        ImGui::EndCombo();
    }

    ImGui::Separator();

    DrawEntityEditor(registry, selectedEntity);

    return selectedEntity;
}
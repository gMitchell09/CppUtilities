#include "EntityEditor.h"
#include "ComponentRegistry.h"
#include "Components/Name.h"

#include <entt/entt.hpp>
#include <imgui.h>
#include "ImguiUtils\ImGuiHelpers.h"

#include <string>

static std::string getName(entt::registry& registry, entt::entity entity) {
    std::string name;

    if (entity == entt::null || !registry.valid(entity)) return name;
    if (registry.all_of<Components::Name>(entity)) name = registry.get<Components::Name>(entity).Value;
    else name = std::to_string(entt::to_integral(entity));

    return name;
}

void DrawEntityEditor(entt::registry& registry, entt::entity entity) {
    static bool s_initializedComponentList = false;
    static std::vector<std::pair<ComponentRegistry::ComponentTypeID, ComponentRegistry::ComponentInfo>> s_componentPairs;
    static ComponentRegistry::ComponentTypeID s_selectedComponent;

    if (!s_initializedComponentList) {
        bool first = true;
        for (auto& [compID, compInfo] : ComponentRegistry::Get().Components) {
            if (first) {
                s_selectedComponent = compID;
                first = false;
            }

            s_componentPairs.emplace_back(compID, compInfo);            
        }        
        s_initializedComponentList = true;
    }

    auto selectedEntity = entity;

    if (ImGui::Button("New Entity")) {
        selectedEntity = registry.create();
        auto& nameComp = registry.emplace<Components::Name>(selectedEntity);
        nameComp.Value = "Entity " + std::to_string(entt::to_integral(selectedEntity));
    }

    
    registry.each([&selectedEntity, &registry](auto entity) {
        std::string name = getName(registry, entity);
        
        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_None;
        if (entity == selectedEntity) treeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        if (ImGui::TreeNodeEx(getName(registry, entity).c_str(), treeFlags)) {
            for (const auto& pair : s_componentPairs) {
                bool groupNotClosed = true;
                if (!ComponentRegistry::Get().entityHasComponent(registry, entity, pair.first)) continue;

                if (ImGui::CollapsingHeader(pair.second.name.c_str(), &groupNotClosed)) {
                    pair.second.draw(registry, entity);
                }

                if (!groupNotClosed) {
                    pair.second.destroy(registry, entity);
                }
            }

            std::string compName;
            if (s_selectedComponent != ComponentRegistry::Null) {
                compName = ComponentRegistry::Get().Components.at(s_selectedComponent).name;
            }

            if (ButtonHCentered("Add Component")) ImGui::OpenPopup("ComponentSelectPopup");
            if (ImGui::BeginPopup("ComponentSelectPopup")) {
                for (const auto& pair : s_componentPairs) {
                    if (ImGui::Selectable(pair.second.name.c_str(), pair.first)) {
                        auto componentInfo = ComponentRegistry::Get().Components[pair.first];
                        // entt has a hard requirement for max one component of a given type per entity
                        bool hasComp = ComponentRegistry::Get().entityHasComponent(registry, entity, pair.first);
                        if (!hasComp) componentInfo.create(registry, entity);
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
    });
}
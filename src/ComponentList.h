#pragma once

#include <map>

#include "imgui-docking/imgui.h"
#include <entt/entt.hpp>

#include "ComponentRegistry.h"

class ComponentList {
public:
    using EntityType = entt::entity;
    using Registry = entt::basic_registry<EntityType>;

    explicit ComponentList(Registry& registry) :
        m_registry(registry)
    {
        bool first = true;
        for (auto& [compID, compInfo] : ComponentRegistry::Get().Components) {
            if (first) {
                m_selectedComponent = compID;
                first = false;
            }
            m_componentPairs.emplace_back(compID, compInfo);
        }

        void draw(EntityType entity) {
            std::string imId = "ComponentList " + std::to_string(entt::to_integral(entity));
            ImGui::PushID(imId.c_str());

            ImGui::Text("%s: %d", "Entity Name", entt::to_integral(entity));
            std::string compName;
            if (m_selectedComponent != ComponentRegistry::Null) {
                compName = ComponentRegistry::Get().Components.at(m_selectedComponent).name;
            }

            if (ImGui::BeginCombo("Component Type", compName.c_str(), 0)) {
                for (auto& pair : m_componentPairs) {
                    if (ImGui::Selectable(pair.second.name.c_str(), pair.first)) m_selectedComponent = pair.first;
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Add Component")) {
                auto componentInfo = ComponentRegistry::Get().Components[m_selectedComponent];
                // entt has a hard requirement (in our current configuration) for component uniqueness per entity
                bool hasComp = ComponentRegistry::Get().entityHasComponent(m_registry, entity, m_selectedComponent);
                if (!hasComp)
                    componentInfo.create(m_registry, entity);
            }

            for (const auto& pair : m_componentPairs) {
                // Fucking naming is hard.  What the hell am I supposed to call this thing?
                bool groupNotClosed = true;
                if (!ComponentRegistry::Get().entityHasComponent(m_registry, entity, pair.first)) continue;

                if (ImGui::CollapsingHeader(pair.second.name.c_str(), &groupNotClosed)) {
                    pair.second.draw(m_registry, entity);
                }

                if (!groupNotClosed) {
                    pair.second.destroy(m_registry, entity);
                }
            }

            ImGui::PopID();
        }

private:
    Registry& m_registry;
    ComponentRegistry::ComponentTypeID m_selectedComponent = ComponentRegistry::Null;
    std::vector<std::pair<ComponentRegistry::ComponentTypeID, ComponentRegistry::ComponentInfo>> m_componentPairs;
};

#ifdef PYBIND_PASS
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind(py::module& m) {
    py::class_<EntityEditor>(m, "EntityEditor")
        .def(py::init<EntityEditor::Registry&>())
        .def("draw", &EntityEditor::draw)
        ;
}
#endif
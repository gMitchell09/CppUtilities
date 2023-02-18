#pragma once

#include <optional>
#include <map>
#include <any>
#include <entt/entt.hpp>

#ifdef USE_BOOST_JSON
#include <boost/json.hpp>
#endif

namespace Components {
template <class Component, class EntityType>
void ComponentEditorDraw(entt::registry& registry, EntityType entity) {
    // Note for future developers: If this fails to compile with a "reference to void" error
    // that means that ENTT is excitedly applying its empty type optimization.
    // make sure the correct -DENTT_DONT_OPTIMIZE_EMPTY_TYPES flag is set in the CXX compiler options
    auto&& compInst = registry.get<Component>(entity);
    compInst.drawEditor(registry, entity);
}

template <class Component, class EntityType>
void ComponentAddAction(entt::basic_registry<EntityType>& registry, EntityType entity) {
    registry.template emplace<Component>(entity);
}

template <class Component, class EntityType>
void ComponentRemoveAction(entt::basic_registry<EntityType>& registry, EntityType entity) {
    registry.template remove<Component>(entity);
}

#ifdef USE_BOOST_JSON
template <typename ResultT>
bool getValueFromJson(boost::json::object& obj, std::string name, ResultT& outResult) {
    outResult = boost::json::value_to<ResultT>(obj[name]);
    return true;
}
#endif

}

/* 
ComponentRegistry 
 - Contains a mapping of all components (ComponentID) -> Name and lifecycle callbacks
 - Singleton since each of our CU's need to be able to find it during the static global initialization phase
*/
class ComponentRegistry
{
    ComponentRegistry() = default;

public:
    using EntityType = entt::entity;
    using Registry = entt::basic_registry<entt::entity>;
    using ComponentTypeID = ENTT_ID_TYPE;
    
    ComponentRegistry(const ComponentRegistry& other) = delete;
    ComponentRegistry(ComponentRegistry&& other) = delete;

    static ComponentRegistry& Get() {
      static ComponentRegistry reg;
      return reg;
    }

    struct ComponentInfo {
        using Callback = std::function<void(Registry&, EntityType)>;
        using ConstCallback = std::function<void(Registry&, EntityType) const>;

        std::string name;
        Callback draw, create, destroy;
    };

    static const ComponentTypeID Null = (ComponentTypeID)0;

    std::map<ComponentTypeID, ComponentInfo> Components;
    std::map<std::string, ComponentTypeID> ComponentNames;

    template <class Component>
    ComponentInfo& registerComponent(const ComponentInfo& compInfo) {
        auto index = entt::type_id<Component>().hash();
        auto [it, insertResult] = Components.insert_or_assign(index, compInfo);
        assert(insertResult);

        auto [cnIt, nameInsertResult] = ComponentNames.insert_or_assign(Component::name(), index);
        assert(nameInsertResult);

        return std::get<ComponentInfo>(*it);
    }

    template <class Component>
    ComponentInfo& registerComponent(const std::string& name, typename ComponentInfo::Callback widget) {
        return registerComponent<Component>(ComponentInfo { name,
            widget,
            Components::ComponentAddAction<Component, EntityType>,
            Components::ComponentRemoveAction<Component, EntityType>,
            });
    }

    template <class Component>
    ComponentInfo& registerComponent(const std::string& name) {
        return registerComponent<Component>(name, Components::ComponentEditorDraw<Component, EntityType>);
    }

    bool entityHasComponent(Registry& registry, EntityType& entity, ComponentTypeID typeID) {
        ComponentTypeID type[] = { typeID };
        entt::runtime_view view;
        const auto& x = registry.storage();
        auto begin = registry.storage(typeID);
        
        if (begin == x.end()) return false;

        auto [name, pool] = *begin;
        return view.iterate(pool).contains(entity);
    }
};

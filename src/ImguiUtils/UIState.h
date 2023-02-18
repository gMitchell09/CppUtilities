#ifndef UISTATE_H
#define UISTATE_H

#include <imgui.h>
#include <imgui_internal.h>
#include <typeinfo>
#include <type_traits>
#include <functional>
#include <unordered_map>

#include <variant>
#include <iostream>
#include <typeinfo>
#include <optional>

/** 
 * @brief A mapped variant container for extended storage of transient ImGUI state (e.g.: unsaved text in input text fields)
 * @tparam HashType Data type of unique hashes in map.  Currently used with ImGui ID stack hashes
 * @tparam Storables The set of types that can be stored in this container
*/
template <typename HashType, typename... Storables>
struct UIStateStorage {
    using VariantType = std::variant<Storables...>;
    std::unordered_map<HashType, VariantType> Storage;

    template <typename Storable>
    std::pair<const char*, Storable*> getDataPtr(const char* label, std::optional<Storable> defaultValue = std::nullopt) {
        auto id = ImGui::GetID(label);

        auto storableItr = Storage.find(id);
        //if constexpr(std::is_default_constructible_v<Storable>) {
        if (storableItr == Storage.end()) {
            Storage.insert(std::make_pair(id, defaultValue.value_or(Storable())));
        }

        Storable *dataPtr = std::get_if<Storable>(&Storage[id]);
        if (dataPtr == nullptr) {
            Storage[id] = defaultValue.value_or(Storable());
            dataPtr = std::get_if<Storable>(&Storage[id]);
        }

        return { label, dataPtr };
    }

    template <typename Storable>
    void setData(const char* label, Storable data) {
        auto id = ImGui::GetID(label);
        Storage.insert(std::make_pair(id, data));
    }
};

#endif // UISTATE_H

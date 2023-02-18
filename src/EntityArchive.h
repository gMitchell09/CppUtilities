#include <entt/entt.hpp>
#include <boost/json.hpp>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "Components.h"

class EntityArchive {
public:
    EntityArchive() { m_jsonRoot["entities"] = boost::json::object(); }

    void operator()(std::underlying_type_t<entt::entity> size) {
        std::cout << "Size: " << size << std::endl;
    }

    void operator()(entt::entity entity) {
        std::string entityId = std::to_string(entt::to_integral(entity));
        auto& rootEnt = m_jsonRoot.at("entities").as_object();
        if (rootEnt.find(entityId) == rootEnt.end()) rootEnt[entityId] = boost::json::object();
        std::cout << "Entity: " << entt::to_integral(entity) << std::endl;
    }

    template <typename ComponentT>
    void operator()(entt::entity entity, const ComponentT& component) {
        std::cout << "Entity: " << entt::to_integral(entity) << " Component: " << ComponentT::name() << std::endl;

        auto& entities = m_jsonRoot["entities"].as_object();

        std::string entityId = std::to_string(entt::to_integral(entity));
        if (entities.find(entityId) == entities.end()) entities[entityId] = boost::json::object();

        auto& currentEntity = entities[entityId].as_object();
        std::string componentId = ComponentT::name();
        currentEntity[componentId] = component.toJSON();
    }

    boost::json::object dump() {
        return m_jsonRoot;
    }

    void addElement(std::string key, boost::json::value value) {
        m_jsonRoot[key] = value;
    }

private:
    boost::json::object m_jsonRoot;
};

////////////////////////////////////////////
class EntityInputArchive {
public:
    EntityInputArchive(std::string filePath) { 
        m_jsonRoot["entities"] = boost::json::object();
        boost::json::parse_options opt;
        opt.allow_comments = true;
        opt.allow_trailing_commas = true;
        boost::json::parser p(boost::json::storage_ptr(), opt);

        std::ifstream ifile(filePath);
        if (!ifile.good()) return;

        ifile.seekg(0, std::ios::end);
        auto end = ifile.tellg();
        ifile.seekg(0, std::ios::beg);
        auto size = end - ifile.tellg();
        std::vector<char> buf;
        buf.resize(size);
        ifile.read(buf.data(), buf.size());
        
        std::string jsonSz (buf.data(), buf.size());
        std::cout << "Sz: " << jsonSz << std::endl;
        boost::json::error_code ec;
        p.write_some(jsonSz, ec);

        if (ec) std::cout << "Error: " << ec.message() << std::endl;
        m_jsonRoot = p.release().as_object();
        m_success = true;

        // Do the parsing and stripping in here?
        // array of entities
        // array for each type of component with reference to entity id
    }

    bool isValid() { return m_success; }

    // order of calls:
    // 1. # of entities
    // 2. for first type of component, how many exist total?
    void operator()(std::underlying_type_t<entt::entity>& size) {
        boost::json::object entities = m_jsonRoot["entities"].as_object();
        
        // To explain: every time this function is called we can safely reset the 
        // current entity idx because first it just wants the number of entities
        // and then it just cares about components.  Resetting it here allows
        // us to simplify the ()(entity, component) function a slight bit
        m_currentEntitiyIdx = 0;

        // Case #1
        if (!m_hasSetNumEntities) {
            size = entities.size();
            m_hasSetNumEntities = true;
            std::cout << "Size: " << size << std::endl;
            return;
        }

        // Case #2
        size_t count = 0;
        std::string currentComponentName = Components::SerializeableComponentNames[m_currentComponentIdx];

        for (const auto& entityKvp : entities) {
            auto curEnt = entityKvp.value().as_object();
            count += curEnt.count(currentComponentName);
        }

        std::cout << "Size: (" << currentComponentName << "): " << size << std::endl;
        
        m_currentComponentIdx++;
        size = count;
    }

    void operator()(entt::entity& entity) {
        boost::json::object entities = m_jsonRoot["entities"].as_object();
        
        auto kvp = entities.begin() + m_currentEntitiyIdx;
        std::cout << "Deserialized E: " << kvp->key() << std::endl;
        m_currentEntitiyIdx++;
        entity = static_cast<entt::entity>(std::stoul(kvp->key()));
    }

    template <typename CompT>
    void operator()(entt::entity& entity, CompT& component) {
        boost::json::object entities = m_jsonRoot["entities"].as_object();

        // Get index of component in possible set of components:
        std::string compName = CompT::name();
        auto current = entities.begin();
        do {
            current = entities.begin() + m_currentEntitiyIdx++;
            assert(current < entities.end());
        } while (current->value().as_object().count(compName) == 0);

        std::cout << "Deserialized EC: " << std::stoul(current->key()) << std::endl;
        entity = static_cast<entt::entity>(std::stoul(current->key()));
        auto compOpt = CompT::fromJSON(entity, current->value().as_object()[compName].as_object());
        if (!compOpt) throw std::runtime_error(std::string("Failed to deserialize ") + std::string(current->key()) + ": " + compName);
        component = std::move(*compOpt);
    }

    boost::json::object dump() {
        return m_jsonRoot;
    }

    void addElement(std::string key, boost::json::value value) {
        m_jsonRoot[key] = value;
    }

private:
    boost::json::object m_jsonRoot;
    size_t m_currentComponentIdx = 0;
    size_t m_currentEntitiyIdx = 0;
    bool m_hasSetNumEntities = false;
    bool m_success = false;
};
#ifndef SCRIPT_H
#define SCRIPT_H

#include "ComponentAutoFactory.h"
#include <optional>

//#include <pybind11/embed.h>

namespace Components {
COMPONENT(Script) {
    NAME("Script");
    DESC("Python scripting interface for entities");
    std::string Contents;

    void run(entt::registry& registry, entt::entity entity) {
        //pybind11::scoped_interpreter guard{};
        //pybind11::exec(Contents);
    }
    void drawEditor(entt::registry& registry, entt::entity entity);
    //static void bind(pybind11::module& m);

#ifdef USE_BOOST_JSON
    boost::json::object toJSON() const {
      boost::json::object result;
      result["Contents"] = boost::json::value_from(Contents);

      return result;
    }

    static std::optional<This_t> fromJSON(entt::entity entity, boost::json::object& obj) {
      This_t result;
      //if (!obj.has("Vector")) return {};

      result.Contents = boost::json::value_to<std::string>(obj["Contents"]);
      
      return result;
    }
#endif

};
#ifdef ADD_TO_LIST
ADD_TO_LIST(SerComponents, Script, Script);
ADD_TO_LIST(AllComponents, Script, Script);
#endif
}

#ifdef PYBIND_PASS
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind(py::module& m) {
    py::class_<Components::Script>(m, "Script")
        .def("name", &Components::Script::name)
        .def_readwrite("Contents", &Components::Script::Contents)
        ;
}
#endif

#endif // SCRIPT_H

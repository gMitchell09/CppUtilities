#pragma once

#include "ComponentAutoFactory.h"
#include <boost/optional.hpp>

namespace pybind11 { class module; }

namespace Components {
COMPONENT(ComponentExample) {
    NAME("ComponentExample");
    DESC("Example Component");

    static std::optional<ComponentExample> construct() {
        return {};
    }

    void drawEditor(entt::registry& registry, entt::entity entity);
    static void bind(pybind11::module& m);

#ifdef USE_BOOST_JSON
    boost::json::object toJSON() const {
      boost::json::object result;
      return result;
    }

    static std::optional<This_t> fromJSON(entt::entity entity, boost::json::object& obj) {
      This_t result;
      return result;
    }
  #endif
};
ADD_TO_LIST(SerComponents, ComponentExample, ComponentExample)
ADD_TO_LIST(AllComponents, ComponentExample, ComponentExample)

}

#ifdef PYBIND_PASS
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind(py::module& m) {
    py::class_<Components::ScatterModel>(m, "ComponentExample")
        .def("name", &Components::ComponentExample::name)
        ;
}
#endif

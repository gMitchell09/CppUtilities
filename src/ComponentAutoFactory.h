#pragma once

#include <unordered_map>
#include <string>

#include "ComponentRegistry.h"

// Base-case definition for ADD_TO_LIST since we only want one compilation unit generating the type lists
#ifndef ADD_TO_LIST
#define ADD_TO_LIST(x, y, z)
#endif

#define COMPONENT(x) struct x : AutoRegistration<x>
#define SUBCLASSCOMPONENT(x, parents) struct x : AutoRegistration<x>, parents
#define NAME(x) static bool dummy() { return s_registered; } static std::string name() { return x; }
#define DESC(x) static std::string description() { return x; }

/* AutoRegistration - Static runtime initialization support class
  - Inheriting from this class (with CRTP template arg) will cause the registerT() method to be called at application start.
  - This can be used (like in this case) for populating registries and factories automagically.  This is as close to a compile-time 
  -   factory as we can easily get without many downsides.
*/
template <class ComponentT>
struct AutoRegistration {
    friend ComponentT;
public:
    using This_t = ComponentT;

    static bool registerT() {
        const auto name = ComponentT::name();

        ComponentRegistry::Get().registerComponent<ComponentT>(name);
        return true;
    }

protected:
    static bool s_registered;
};

// This statement is required to ensure that s_registered is emitted in a compilation unit.  
// The other magical portion is the NAME(x) macro whose dummy() function forces the compiler 
//    to emit the initialization code
template <class ComponentT>
bool AutoRegistration<ComponentT>::s_registered =
    AutoRegistration<ComponentT>::registerT();
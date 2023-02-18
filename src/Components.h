#pragma once

#include <tuple>

/* BEGIN_LIST, ADD_TO_LIST, END_LIST macro setup:
 - This section of code defines a way to create lists of types that can be appended to.
 - This is necessary for the entt serializer/deserializer since it requires template arguments 
     that contain the set of component types that need to be serialized/deserialized.
  - The useful side-effect of this requirement is that we can cordon off chunks of component types
      so that we can seperate our state into different categories. E.g.:
        - Serializeable Components - Components that we want to save off during bulk serialization
        - UpdateComponents - Components that we want to bundle together when communicating state remotely for syncing with UIs and other hardware, e.g.
        - All Components - the full set of components, serializeable and non-serializeable
*/

template <typename T, typename... Args> struct TupleConcat;

template <typename... Args0, typename... Args1>
struct TupleConcat<std::tuple<Args0...>, Args1...> {
    using Type = std::tuple<Args0..., Args1...>;
};

template <int N>
struct Counter : public Counter<(N - 1)> {
    static int const Num = N;
};

template <>
struct Counter<0> {
    static int const Num = 0;
};

#define BEGIN_LIST(ListName) \
Counter<0> ListName##_Size (Counter<0>); \
template <int Index> struct ListName##_Tuple; \
template <> struct ListName##_Tuple<0> { using Types = std::tuple<>; };

#define ADD_TO_LIST(ListName, Name, Value) \
static int const ListName##_##Name##_PrevLen = decltype(ListName##_Size(Counter<128>()))::Num; \
static int const ListName##_##Name##_Len = ListName##_##Name##_PrevLen + 1; \
Counter<ListName##_##Name##_Len> ListName##_Size (Counter<ListName##_##Name##_Len>); \
template <> struct ListName##_Tuple<ListName##_##Name##_Len> { \
   using Types = typename TupleConcat<ListName##_Tuple<ListName##_##Name##_PrevLen>::Types, Value>::Type; \
};

#define END_LIST(ListName) \
static int const ListName##_Len = decltype(ListName##_Size(Counter<128>()))::Num; \
using ListName##Tuple = typename ListName##_Tuple<ListName##_Len>::Types;

/* Example usage:
BEGIN_LIST(TestList)
ADD_TO_LIST(TestList, Int, int);
ADD_TO_LIST(TestList, Float, float);
ADD_TO_LIST(TestList, Double, double);
ADD_TO_LIST(TestList, String, std::string);
END_LIST(TestList)
static_assert(std::is_same_v<TestListTuple, std::tuple<int, float, double, std::string>>, "List tuple does not match test case");
*/

namespace Components {
    BEGIN_LIST(SerComponents)
    BEGIN_LIST(AllComponents)
};

#include "Components/ComponentExample.h"
#include "Components/Name.h"
#include "Components/Position.h"
#include "Components/Script.h"
#include "Components/Velocity.h"

namespace Components {
    END_LIST(SerComponents)
    END_LIST(AllComponents)
};

#include <entt/entt.hpp>

template<typename T, typename Tuple>
struct tuple_element_index_helper;

template<typename T>
struct tuple_element_index_helper<T, std::tuple<>>
{
  static constexpr std::size_t value = 0;
};

template<typename T, typename... Rest>
struct tuple_element_index_helper<T, std::tuple<T, Rest...>>
{
  static constexpr std::size_t value = 0;
  using RestTuple = std::tuple<Rest...>;
  static_assert(
    tuple_element_index_helper<T, RestTuple>::value == 
    std::tuple_size_v<RestTuple>,
    "type appears more than once in tuple");
};

template<typename T, typename First, typename... Rest>
struct tuple_element_index_helper<T, std::tuple<First, Rest...>>
{
  using RestTuple = std::tuple<Rest...>;
  static constexpr std::size_t value = 1 +
       tuple_element_index_helper<T, RestTuple>::value;
};

template<typename T, typename Tuple>
struct tuple_element_index
{
  static constexpr std::size_t value =
    tuple_element_index_helper<T, Tuple>::value;
  static_assert(value < std::tuple_size_v<Tuple>,
                "type does not appear in tuple");
};

template<typename T, typename Tuple>
inline constexpr std::size_t tuple_element_index_v
 = tuple_element_index<T, Tuple>::value;

namespace Components {
  using SerializeableComponents = SerComponentsTuple;

  template <typename>
  struct GetComponentNames;

  template <typename... Args>
  struct GetComponentNames<std::tuple<Args...>> {
      static const std::vector<std::string> Get() {
          return { std::string(Args::name())... };
      }
  };

  inline const std::vector<std::string> SerializeableComponentNames = GetComponentNames<SerComponentsTuple>::Get();

  template <typename>
  struct SnapshotMagic;

  template<typename... Args>
  struct SnapshotMagic<std::tuple<Args...>> {
    template <typename SnapshotType, typename ArchiveType>
    static const SnapshotType& EnttSnapshot(const SnapshotType& snapshot, ArchiveType& at) {
      return snapshot.template component<Args...>(at);
    }
  };
}

template <typename T>
void TestComponent() {
    T t;
    //auto sz = t.toJSON();
    //auto comp = T::fromJSON(static_cast<entt::entity>(99), sz);
}

//#define TESTCOMP(X) X x ## X; auto sz ## X = x ## X.toJSON(); auto comp ## X = X::fromJSON(static_cast<entt::entity>(99), sz ## X);
#define TESTCOMP(X) X x ## X;
static bool TestComponents() {
  using namespace Components;
  TESTCOMP(Position) 
  TESTCOMP(Velocity)
  TESTCOMP(ComponentExample)
  TESTCOMP(Script)
  TESTCOMP(Name)

  return true;
}

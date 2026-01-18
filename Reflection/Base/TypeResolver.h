#pragma once
#include <type_traits>

namespace reflection
{
  template<typename T, typename = void>
  struct TTypeResolver;

  template<typename T>
  struct TTypeResolver<T, std::void_t<decltype(T::GetStaticType())>>
  {
    static constexpr auto GetStaticType() { return T::GetStaticType(); }
  };
}
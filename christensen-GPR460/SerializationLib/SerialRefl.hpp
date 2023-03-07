#pragma once

#include <type_traits>

/*

 Serialization static reflection utilities
 Like type_traits

*/

template<typename... Ts>
struct has_all_helper {}; //Utility struct, pass parameter names in SFINAE

template<typename T, typename... TMembers>
struct has_all : std::conditional_t<
    false,
    has_all_helper<TMembers...>,
    void
> {};

template<typename T>
using is_container = has_all<T,
    typename T::value_type,
    typename T::size_type,
    typename T::allocator_type,
    typename T::iterator,
    typename T::const_iterator,
    decltype(std::declval<T>().size()),
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    decltype(std::declval<T>().cbegin()),
    decltype(std::declval<T>().cend())
>;
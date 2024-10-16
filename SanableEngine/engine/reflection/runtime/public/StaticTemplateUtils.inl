#pragma once

#include <optional>

namespace stix::detail
{
	template<typename T, T... vs>
	struct check_all_eq;

	template<typename T, T v1, T v2, T... vs>
	struct check_all_eq<T, v1, v2, vs...> { constexpr static bool value = v1 == v2 && check_all_eq<T, v2, vs...>::value; };

	template<typename T, T v1>
	struct check_all_eq<T, v1> { constexpr static bool value = true; };



	template<typename T, typename _ = void>
	struct is_complete : std::false_type {};

	template<typename T>
	struct is_complete<T, std::enable_if_t<sizeof(T) != 0>> : std::true_type {};

	template<typename T>
	constexpr bool is_complete_v = is_complete<T>::value;

	template<typename... Ts>
	constexpr bool is_complete_many_v = check_all_eq<bool, true, is_complete_v<Ts>...>::value;



	template<bool condition>
	struct ternary;
	template<>
	struct ternary<true>
	{
		//template<typename T, T _true, T _false>
		//constexpr static decltype(auto) value = _true;

		template<typename _true, typename _false>
		using type = _true;
	};
	template<>
	struct ternary<false>
	{
		template<typename T, T _true, T _false>
		constexpr static decltype(auto) value = _false;

		template<typename _true, typename _false>
		using type = _false;
	};

	template<bool condition, typename T, T _true, T _false>
	constexpr static T ternary_v = condition ? _true : _false;
	//constexpr static T ternary_v = ternary<condition>::template value<T, _true, _false>;

	template<bool condition, typename _true, typename _false>
	using ternary_t = typename ternary<condition>::template type<_true, _false>;
}

#pragma once

#include <type_traits>
#include <limits>

template<typename T>
struct Color3
{
	union
	{
		struct { T r, g, b; };
		T v[3];
	};

	using internal_cvt_t = float;

	template<typename U>
	explicit operator Color3<U>() const
	{
		Color3<U> out;
		ColorTraits<T>::cvt_impl(out.v, v);
		return out;
	}

	template<typename U>
	static Color3<U> mix(Color3<U> a, Color3<U> b, float t)
	{
		Color3<U> out;
		ColorTraits<T>::mix_impl(out.v, a.v, b.v, t);
		return out;
	}
};

template<typename T>
struct Color4
{
	union
	{
		struct { T r, g, b, a; };
		T v[4];
	};

	using internal_cvt_t = float;

	template<typename U>
	explicit operator Color4<U>() const
	{
		Color4<U> out;
		ColorTraits<T>::cvt_impl(out.v, v);
		return out;
	}

	template<typename U>
	static Color4<U> mix(Color4<U> a, Color4<U> b, float t)
	{
		Color4<U> out;
		ColorTraits<T>::mix_impl(out.v, a.v, b.v, t);
		return out;
	}
};


template<typename T>
struct ColorTraits
{
	static constexpr T max = std::is_floating_point_v<T> ? 1 : std::numeric_limits<T>::max();

	template<typename To, size_t N>
	static void cvt_impl(To(&out)[N], const T(&in)[N])
	{
		using cvt_t = std::conditional_t <
			std::is_floating_point_v<U> || std::is_floating_point_v<T> || sizeof(T) < sizeof(U),
			float,
			uint64_t
			> ;

		for (size_t i = 0; i < N; ++i)
		{
			out[i] = (cvt_t(in[i]) * ColorTraits<To>::max) / ColorTraits<T>::max;
		}
	}

	template<size_t N>
	static void mix_impl(T(&out)[N], const T(&a)[N], const T(&b)[N], float t)
	{
		float inv_t = 1 - t;
		for (size_t i = 0; i < N; ++i)
		{
			out[i] = a*inv_t + b*t;
		}
	}
};

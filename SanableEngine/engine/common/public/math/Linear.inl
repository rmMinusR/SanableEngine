#pragma once

//Lerp already declared in <cmath>

template<typename T>
inline constexpr T invlerp(T v, T min, T max)
{
	return (v - min) / (max - min);
}

//Clamp already declared in <algorithm>

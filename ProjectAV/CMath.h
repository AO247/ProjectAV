#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq( const T& x ) noexcept
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta) noexcept
{
	constexpr T twoPi = static_cast<T>(2.0 * PI);
	constexpr T pi = static_cast<T>(PI);

	T mod = (T)std::fmod(theta, twoPi);

	if (mod > pi)
	{
		mod -= twoPi;
	}
	else if (mod < -pi)
	{
		mod += twoPi;
	}
	return mod;
}

template<typename T>
constexpr T interpolate( const T& src,const T& dst,float alpha ) noexcept
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr float to_rad(T deg) noexcept
{
    return static_cast<float>(deg) * static_cast<float>(PI_D) / 180.0f;
}

template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}
#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq( const T& x )
{
	return x * x;
}

template <typename T>
T wrap_angle(T theta)
{
    // Use static_cast for constants to match template type T
    constexpr T twoPi = static_cast<T>(2.0 * PI);
    constexpr T pi = static_cast<T>(PI);

    // Reduce angle to basic range (-2PI, 2PI) using fmod
    T mod = std::fmod(theta, twoPi);

    // Adjust range to be [-PI, PI]
    if (mod > pi)
    {
        // Angle is > 180 degrees, wrap to negative equivalent
        mod -= twoPi;
    }
    else if (mod < -pi)
    {
        // Angle is < -180 degrees, wrap to positive equivalent
        mod += twoPi;
    }

    // Handle potential precision issues near -PI (optional but safer)
    // If mod is extremely close to -PI, prefer +PI if that's desired behavior
    // constexpr T epsilon = std::numeric_limits<T>::epsilon() * 10; 
    // if (std::abs(mod + pi) < epsilon) {
    //     mod = pi;
    // }

    return mod;
}

template<typename T>
constexpr T interpolate( const T& src,const T& dst,float alpha )
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad( T deg )
{
	return deg * PI / (T)180.0;
}

#include <cstddef>
#include <algorithm>
#include "math_ns_define_macros.hpp"
MATH_NS_BEGIN

namespace constants
{
	constexpr auto e = 2.7182818284590452353602874713526624977572470936999595749669676;
	constexpr auto tau = 6.283185307179586;
	constexpr auto pi = 3.14159265358979323846264338327950;
}

// TODO: Consider about overflows, will use auto return type help?
template <typename UnsignedT>
requires ::std::is_unsigned_v<UnsignedT>
UnsignedT sum_up_to_n(UnsignedT n)
{
	return n * (n + 1) / 2;
}

template <typename FloatingPointT>
requires ::std::is_floating_point_v<FloatingPointT>
FloatingPointT smoothstep(FloatingPointT first_edge, FloatingPointT second_edge, FloatingPointT value)
{
	value = ::std::clamp((value - first_edge) / (second_edge - first_edge), 0.0, 1.0);
	return value * value * (3 - 2 * value);
}


MATH_NS_END
#include "math_ns_undefine_macros.hpp"

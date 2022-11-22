#pragma once

#include "math_ns_define_macros.hpp"

#include <vector>
#include <array>



MATH_NS_BEGIN
namespace primes
{
	using prime_type = unsigned long long;

	namespace detail
	{

	}

	constexpr bool is_prime(const prime_type number)
	{
		prime_type half = number / 2;
		for (prime_type current = 2; current <= half; current++)
		{
			if (number % current == 0)
			{
				return false;
			}
		}
		return true;
	}

	constexpr std::vector<prime_type> get_primes(std::size_t last)
	{
		std::vector<prime_type> result;
		result.resize(last);

		prime_type current = 2;
		for (std::size_t i = 0; i < last; i++)
		{
			while (!is_prime(current))
			{
				current++;
			}
			result[i] = current++;
		}

		return result;
	}

	template <std::size_t N>
	constexpr auto get_primes()->std::array<prime_type, N>
	{
		std::array<prime_type, N> result{};

		prime_type current = 2;
		for (std::size_t i = 0; i < N; i++)
		{
			while (!is_prime(current))
			{
				current++;
			}
			result[i] = current++;
		}

		return result;
	}

}
MATH_NS_END

#include "math_ns_undefine_macros.hpp"

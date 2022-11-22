
#include "math_ns_define_macros.hpp"

#include <cstddef>
#include <vector>
#include <array>

MATH_NS_BEGIN
	inline
	constexpr std::size_t factorial(const std::size_t n)
	{
		std::size_t current = 1;
		for (std::size_t i = n; i > 0; i--)
		{
			current *= i;
		}

		return current;
	}

	namespace detail
	{
		
	}

	template <std::size_t _Max>
	constexpr std::size_t factorial(std::size_t n)
	{
		//static_assert(n < _Max, "Index value must be equal to or smaller than max value");
		static constexpr auto factorial_table
		{
			[]() constexpr {
				std::array<std::size_t, _Max + 1> result;
				for (std::size_t i{}; i < _Max + 1; i++)
				{
					result[i] = factorial(i);
				}
				return result;
		}()
		};
		

		return factorial_table[n];
	}

	std::size_t combinations_count(std::size_t n, std::size_t r)
	{
		return factorial(n) / (factorial(n - r) * factorial(r));
	}

	// Copy operations only, needs improvement for performance
	std::vector<std::vector<std::size_t>> combinations(const std::vector<std::size_t>& items, const std::size_t r)
	{
		const auto length = items.size();
		const auto num_combinations = combinations_count(length, r);
		std::vector<std::vector<std::size_t>> result(num_combinations, std::vector<size_t>(r));

		std::vector<size_t> choosens(r);
		for (std::size_t i = 0; i < r; i++)
		{
			choosens[i] = i;
		}
		const auto combinations_last_index = length - 1;
		const auto choosens_last_index = r - 1;
		choosens[choosens_last_index]--;
		for (std::size_t combinationIndex{}; combinationIndex < num_combinations; combinationIndex++)
		{
			std::size_t offset{};
			while (choosens[choosens_last_index - offset] == combinations_last_index - offset)
			{
				offset++;
			}
			choosens[choosens_last_index - offset]++;

			while (offset)
			{
				choosens[choosens_last_index - offset + 1] = choosens[choosens_last_index - offset] + 1;
				offset--;
			}

			for (std::size_t i{}; i < r; i++)
			{
				result[combinationIndex][i] = items[choosens[i]];
			}
		}

		return result;
	}
MATH_NS_END

#include "math_ns_undefine_macros.hpp"

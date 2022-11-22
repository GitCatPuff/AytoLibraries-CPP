
#include "../math_ns_define_macros.hpp"

//#include "../../utility.hpp"
#include <cstddef>
#include <vector>
#include <cctype>
#include <stack>

MATH_NS_BEGIN namespace expressions {

class polynomial
{
public:
	struct term
	{
		std::size_t id{};
		double exponent{};
		double multiplier{};

		friend class polynomial;
	};

private:
	::std::vector<term> terms;

private:

	double for_value(term variable, double value) const
	{
		return ::std::pow(value, variable.exponent) * variable.multiplier;
	}

public:

	polynomial() = default;

	constexpr
	polynomial(::std::initializer_list<term> variables)
		:
		terms(variables)
	{}
	
	/*
	TODO: Maybe we can use a class named "valid_string<Tag>"
	'Tag' is the dummy class we only use it to tag 
	what this string is valid for.

	valid_string<expressions::polynomial> valid string for polynomial class,
	polynomial class uses this parameter to force strong typing.

	if user supplies 'const char *' instead of strongly typed argument,
	then overloaded function does check the input first,
	
	if input is valid; calls the actual strongly typed overload,

	if input is not valid; probably throw an exception

	if we do not want to throw exceptions, then we can use a factory pattern
	this factory will return a 'std::optional<polynomial>'

	*/

	
	polynomial(const char* polynomial_string)
	{
		const auto string_length = ::std::strlen(polynomial_string);

		enum class operations
		{
			parse_double,
			parse_multiplier,
			parse_variable,
			parse_exponent
		};
		::std::stack<operations> operation_stack;

		struct operation_data
		{
			union
			{
				double value;
				char variable;
			};
		};
		::std::stack<operation_data> operation_data_stack;


		for (::std::size_t i{}; i < string_length; i++)
		{
			const auto current_character = polynomial_string[i];

			if (::std::isspace(current_character))
				continue;

			if (::std::isalpha(current_character) && ::std::islower(current_character))
			{
				if (operation_stack.empty())
				{
					operation_stack.push(operations::parse_variable);
					operation_data_stack.push({ .variable = current_character });
					continue;
				}

				if (operation_stack.top() == operations::parse_variable)
				{

				}
			}
		}
	}

	template <typename ...Values>
	constexpr
	double operator() (Values... values) const
	{
		double result{};
		for (const auto &term : terms)
		{
			result += (for_value(term, values) + ...);
		}

		return result;
	}

};




MATH_NS_END }
#include "../math_ns_undefine_macros.hpp"

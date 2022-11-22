#ifndef AYTO_PERFORMANCE_TIMER_HPP
#define AYTO_PERFORMANCE_TIMER_HPP

#include <chrono>

#include "../common/ayto_detail_define_macros.hpp"
#define NS_BEGIN namespace AYTO_ROOT_NAMESPACE { AYTO_INLINE_NAMESPACE AYTO_CURRENT_ABI { namespace performance {
#define NS_END } } }
NS_BEGIN

namespace detail
{
	template <class> 
	inline constexpr bool is_duration_v = false;

	template <class Rep, class Period>
	inline constexpr bool is_duration_v
		<std::chrono::duration<Rep, Period> > = true;
}

class timer
{
public:
	using clock_type = std::chrono::high_resolution_clock;
	using time_point = clock_type::time_point;
public:
	void capture()
	{
		last_capture = clock_type::now();
	}
	
	template <class DurationT>
	requires detail::is_duration_v<DurationT>
	auto elapsed()
	{
		const auto now = clock_type::now();

		return std::chrono::duration_cast<DurationT>(now - last_capture);
	}

private:
	time_point last_capture;
};


NS_END
#include "../common/ayto_detail_undefine_macros.hpp"
#endif
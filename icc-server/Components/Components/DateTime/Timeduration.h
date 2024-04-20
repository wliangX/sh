#pragma once

// boost
#include <boost/static_assert.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace ICC {

	// 解决boost::posix_time::microseconds计算溢出
	// （其实是boost::date_time::subsecond_duration计算溢出）
	template<class base_duration, boost::int64_t frac_of_second>
	class subsecond_duration : public base_duration
	{
	public:
		typedef typename base_duration::traits_type traits_type;

		// 构造函数中的计算要求traits_type::ticks_per_second和frac_of_second中的一方为另一方的整数倍
		BOOST_STATIC_ASSERT(
			traits_type::ticks_per_second % frac_of_second == 0 ||
			frac_of_second % traits_type::ticks_per_second == 0);

		explicit subsecond_duration(boost::int64_t ss) :
			base_duration(0, 0, 0,
			traits_type::ticks_per_second > frac_of_second
			? ss * (traits_type::ticks_per_second / frac_of_second)
			: ss / (frac_of_second / traits_type::ticks_per_second))
		{}

	private:
	};

	//! Allows expression of durations as milli seconds
	/*! \ingroup time_basics
	*/
	typedef subsecond_duration<boost::posix_time::time_duration, 1000> millisec;
	typedef subsecond_duration<boost::posix_time::time_duration, 1000> milliseconds;

	//! Allows expression of durations as micro seconds
	/*! \ingroup time_basics
	*/
	typedef subsecond_duration<boost::posix_time::time_duration, 1000000> microsec;
	typedef subsecond_duration<boost::posix_time::time_duration, 1000000> microseconds;
}


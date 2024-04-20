/*!
 * \file Base.hpp
 *
 * \author p27687
 * \date 十二月 2017
 *
 * 
 */
#pragma once
#include <string>
#include <stdio.h>
#include <ctime>
#include <time.h>
#include <stdlib.h>

//#include <boost/timer.hpp>
//#include <boost/locale.hpp>


#ifdef WIN32
#	define OS_WINDOWS WIN32
#	define SLEEP Sleep
#elif defined linux
#	define OS_LINUX linux
#	define SLEEP usleep
#else
//
#endif


#define MAX_BUFFER 64

//CLOCKS_PER_SEC
//在linux和windows不一样
//windows上是CLOCKS_PER_SEC，是一秒有多少毫秒
//而linux上得到的是一秒有多少微秒
namespace ICC
{
	class CClockTimer
	{
	public:
		CClockTimer(){ m_tStartTime = std::clock(); }

		void Restart(){ m_tStartTime = std::clock(); }

		double ElapsedSec() const	
		{
			return (double(std::clock() - m_tStartTime) / CLOCKS_PER_SEC);
		}

		double ElapsedMillionSec() const	//方法存在问题,windows可以使用，linux不可使用,linux得到的是微秒，并非毫秒
		{
			return (double(std::clock() - m_tStartTime) / CLOCKS_PER_SEC) * CLOCKS_PER_SEC;
		}

	private:
		std::clock_t m_tStartTime;
	};
}
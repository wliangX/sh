/*!
 * \file Base.hpp
 *
 * \author p27687
 * \date ʮ���� 2017
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
//��linux��windows��һ��
//windows����CLOCKS_PER_SEC����һ���ж��ٺ���
//��linux�ϵõ�����һ���ж���΢��
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

		double ElapsedMillionSec() const	//������������,windows����ʹ�ã�linux����ʹ��,linux�õ�����΢�룬���Ǻ���
		{
			return (double(std::clock() - m_tStartTime) / CLOCKS_PER_SEC) * CLOCKS_PER_SEC;
		}

	private:
		std::clock_t m_tStartTime;
	};
}
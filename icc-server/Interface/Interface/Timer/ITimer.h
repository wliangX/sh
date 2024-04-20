#pragma once

#include <IObject.h>
#include <Thread/IThread.h>

#define TIMER_TIMEOUT "timer_timeout"

namespace ICC
{
	namespace Timer
	{
		/*
		* class   定时器管理类
		* author  w16314
		* purpose
		* note
		*/
		class ITimerManager : public IObject
		{
		public:
			//************************************
			// Method:    AddTimer
			// FullName:  ACS::Timer::ITimerManager::AddTimer
			// Access:    virtual public 
			// Returns:   std::string 定时器名称
			// Qualifier: 添加定时器
			// Parameter: std::string p_strCmdName 定时器命令名
			// Parameter: unsigned int p_iInterval 间隔时间，单位秒， 0代表仅运行一次
			// Parameter: unsigned int p_iDelayStartSec 延迟启动时间，单位秒，0代表立即启动
			//************************************
			virtual std::string AddTimer(std::string p_strCmdName, unsigned int p_iInterval, unsigned int p_iDelayStartSec) = 0;
		};

		typedef boost::shared_ptr<ITimerManager> ITimerManagerPtr;
	}
}
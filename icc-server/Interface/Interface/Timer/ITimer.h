#pragma once

#include <IObject.h>
#include <Thread/IThread.h>

#define TIMER_TIMEOUT "timer_timeout"

namespace ICC
{
	namespace Timer
	{
		/*
		* class   ��ʱ��������
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
			// Returns:   std::string ��ʱ������
			// Qualifier: ��Ӷ�ʱ��
			// Parameter: std::string p_strCmdName ��ʱ��������
			// Parameter: unsigned int p_iInterval ���ʱ�䣬��λ�룬 0���������һ��
			// Parameter: unsigned int p_iDelayStartSec �ӳ�����ʱ�䣬��λ�룬0������������
			//************************************
			virtual std::string AddTimer(std::string p_strCmdName, unsigned int p_iInterval, unsigned int p_iDelayStartSec) = 0;
		};

		typedef boost::shared_ptr<ITimerManager> ITimerManagerPtr;
	}
}
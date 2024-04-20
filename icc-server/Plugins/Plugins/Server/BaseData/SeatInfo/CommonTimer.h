#ifndef __CommonTimer_H__
#define __CommonTimer_H__

#include "CommonTimerCore.h"
#include <mutex>
#include <boost/thread/thread.hpp>
#include <map>
#include <atomic>

struct CommonTimerData
{
	CCommonTimerCore* pCoreTimer;	       //回调指针
	unsigned long long     u64FirstTime;	   //开始设置的时间
	unsigned int	   uInterval;		   //定时触发事件事件单位毫秒
	unsigned int     u64FirstStartTime;
	bool                   startFlag;
};

enum TimerMode
{
	TimerMode_Relative,
	TimerMode_Absolute,
};

class CCommonTimer
{
public:
	void AddTimer(CCommonTimerCore* pTimerCore, std::string uMsgId, unsigned int uInterval);//uInterval,单位毫秒	  
	void AddTimer(CCommonTimerCore* pTimerCore, std::string uMsgId, unsigned int uInterval, unsigned int uStartTime);//uInterval,单位毫秒	  
	void RemoveTimer(std::string uMsgId);
	void SetTimerMode(TimerMode mode);//只调用一次
	void SetTimerName(const std::string& strTimerName);//只调用一次
	void SetTimerInterval(std::string uMsgId, unsigned long long uInterval);

public:
	CCommonTimer();
	virtual ~CCommonTimer();

protected:
	virtual void run();
	bool         _StartThread();
	void         _StopThread();

	void         _AddTimer(std::string uMsgId, CommonTimerData data);
	void         _RemoveTimer(std::string uMsgId);
	void         _ModifyFirstTime(std::string uMsgId, unsigned long long uTime);
	bool         _IsTimerMapEmpty();

	void		 _ModifyStartFlag(std::string uMsgId, bool uTime);

protected:
	//boost::future<void>                      m_threadExec;
	boost::shared_ptr<boost::thread> m_threadExec;

	std::atomic_bool                       m_bIsRunning;
	std::atomic_bool                       m_bStopThread;
	boost::condition_variable_any	       m_envetNotifyExec;
	boost::mutex                             m_mutexNotifyExec;

	boost::mutex					           m_mutexTimerInfo;
	std::map<std::string, CommonTimerData> m_mapTimerInfo;


	boost::mutex                             m_mutexStartThread;
	TimerMode                              m_timerMode;
	std::string                            m_strTimerName;
};

#endif
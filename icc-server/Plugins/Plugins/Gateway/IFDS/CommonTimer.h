/**
 @Copyright Copyright (C), 2014
 @file		CommonTimer.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonTimer_H__
#define __CommonTimer_H__

#include "CommonTimerCore.h"
#include <mutex>
#include <boost/thread/thread.hpp>
#include <map>
#include <atomic>

struct CommonTimerData
{
	CCommonTimerCore	   *pCoreTimer;	       //回调指针
	unsigned long long     u64FirstTime;	   //开始设置的时间
	unsigned long long	   uInterval;		   //定时触发事件事件单位毫秒
};

enum TimerMode
{
	TimerMode_Relative,
	TimerMode_Absolute,
};

class CCommonTimer
{
public:	
	void AddTimer(CCommonTimerCore *pTimerCore, unsigned long uMsgId, unsigned long long uInterval);//uInterval,单位毫秒	  
	void RemoveTimer(unsigned long uMsgId);		
	void SetTimerMode(TimerMode mode);//只调用一次
	void SetTimerName(const std::string& strTimerName);//只调用一次

public:
	CCommonTimer();
	virtual ~CCommonTimer();

protected:
	virtual void run();
	bool         _StartThread();
	void         _StopThread();
	
	void         _AddTimer(unsigned long uMsgId, CommonTimerData data);
	void         _RemoveTimer(unsigned long uMsgId);
	void         _ModifyFirstTime(unsigned uMsgId, unsigned long long uTime);
	bool         _IsTimerMapEmpty();

protected:
	//boost::future<void>                      m_threadExec;
	boost::shared_ptr<boost::thread> m_threadExec;
	
	std::atomic_bool                       m_bIsRunning;
	std::atomic_bool                       m_bStopThread;
	boost::condition_variable_any	       m_envetNotifyExec;
	std::mutex                             m_mutexNotifyExec;

	std::mutex					           m_mutexTimerInfo;
	std::map<unsigned long, CommonTimerData> m_mapTimerInfo;

	
	std::mutex                             m_mutexStartThread;
	TimerMode                              m_timerMode;
	std::string                            m_strTimerName;
};

#endif
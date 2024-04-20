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
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <atomic>
#include <memory>

struct CommonTimerData
{
	CommonTimerData():ulTimerID(0), pCoreTimer(NULL), u64FirstTime(0), bIsRunnning(false), uInterval(0), timerThread(0)
	{}
	void                   StopThread();       //停止定时器执行线程
	unsigned long          ulTimerID;          //定时器ID
	CCommonTimerCore*      pCoreTimer;	       //回调指针
	std::atomic_ullong     u64FirstTime;	   //开始设置的时间
	std::atomic_bool       bIsRunnning;        //线程是否在运行
	unsigned long long	   uInterval;		   //定时触发事件事件单位毫秒
	boost::thread*         timerThread;        //定时器执行线程
};
typedef std::shared_ptr<CommonTimerData> CommonTimerDataPtr;

#define INVALID_TIMER_ID 0

//通用定时器类
class CCommonTimer
{
public:
	//增加一个定时器，定时器间隔，返回定时器ID，>0 是正常添加， = 0是错误
	unsigned long AddTimer(CCommonTimerCore *pTimerCore, unsigned long long uInterval);//uInterval,单位毫秒	  
	//删除一个定时器，指定MsgId
	void RemoveTimer(unsigned long uMsgId);		

public:
	CCommonTimer();
	virtual ~CCommonTimer();

protected:
	//virtual void run();
	virtual void DoTimer(CommonTimerDataPtr pTimerData);
	bool         _StartThreadPool();
	void         _StopThreadPool();
	
	unsigned long _AddTimer(CommonTimerDataPtr data);
	unsigned long GetCurrentTimerID();
	void          _RemoveTimer(unsigned long uMsgId);
	void GetCommonTimerData(unsigned long& uMsgId, CommonTimerDataPtr& pTimerData);
protected:
	boost::thread_group                      m_threadpool; //线程池
	std::atomic_bool                         m_bIsRunning; //线程池是否运行
	std::atomic_bool                         m_bStopThread; //停止标识

	std::mutex					              m_mutexTimerInfo;
	std::map<unsigned long, CommonTimerDataPtr> m_mapTimerInfo;

	
	std::mutex                                m_mutexStartThreadPool;
	unsigned long                             m_ulCurrentTimerID; //TimerID从1开始，返回0是错误
};
typedef std::shared_ptr<CCommonTimer> CCommonTimerPtr;

class CCommonTimerManager
{
public:
	static CCommonTimerManager& GetInstance();
	//Interval为毫秒数
	unsigned long AddTimer(CCommonTimerCore* pTimerCore, unsigned long long uInterval);
	void RemoveTimer(unsigned long uTimerId);
private:
	CCommonTimerManager();

	static CCommonTimerManager m_instance;
	CCommonTimerPtr m_pTimerMgr;
};

#endif
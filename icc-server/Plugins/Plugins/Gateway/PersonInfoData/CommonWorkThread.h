/**
 @Copyright Copyright (C), 2014
 @file		CommonWorkThread.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonWorkThread_H__
#define __CommonWorkThread_H__

#include "CommonThreadCore.h"
#include <boost/thread/thread.hpp>
#include <mutex>
#include <vector>
#include <atomic>
#include <list>


struct CommonThread_Object
{
	CommonThread_data		thread_data;				///< 相关的线程数据
	CCommonThreadCore	   *threadcore;				///< 使用的类型
};

typedef std::list<CommonThread_Object>	CommonThread_Object_vector;	   ///< 线程要执行的消息队列

typedef void(*PFunc_LogOutput)(const std::string& strLog);

class CCommonWorkThread
{
public:
	CCommonWorkThread(void);
	virtual ~CCommonWorkThread(void);

public:	
	void         SetDeleteData(bool bDel = false);
	void         SetThreadName(const std::string& strThreadName);//只调用一次
	std::string  GetThreadName();
	int          GetCWorkThreadListCount();	
	int          AddMessage(CCommonThreadCore *pThreadCore = NULL, __culong_ uMsgId = 0, __culong_ ulParam = 0, __culong_ uwParam = 0, __clong_ lData = 0,
		std::string strmsg = "", std::string strExt = "", ICC::ObserverPattern::INotificationPtr pTask = nullptr, ICC::PROTOCOL::IRequestPtr pRequest = nullptr,
	ICC::PROTOCOL::IRespondPtr pRespond = nullptr);
	bool         Start();
	void         Stop(long lMilliseconds);
	void         SetLogOutput(PFunc_LogOutput pFuncLogOutput);
	bool         SetProcessCountBeforeNeedSleep(int iProcessCount);

private:
	bool         _AddMsgToVector(CommonThread_Object  msgData);
	bool         _IsAddMsgVectorEmpty();
	bool         _IsExecMsgVectorEmpty();

private:
	boost::mutex					    m_mutexAddMsgVector;
	boost::mutex					    m_mutexExecMsgVector;
	CommonThread_Object_vector		m_vectorExecMsg;
	CommonThread_Object_vector		m_vectorAddMsg;
	boost::shared_ptr<boost::thread>   			m_threadExecMsg;
	boost::condition_variable			m_eventExecWait;
	
	std::atomic_bool			    m_bStopRunning;
	std::atomic_bool                m_bDeleteData;
	std::atomic_bool                m_bExitExec;
	std::atomic_bool                m_bIsRunning;

	PFunc_LogOutput                 m_pFuncLogOutput;

	std::string                     m_strThreadName;

	std::atomic<int32_t>             m_iProcessCountBeforeNeedSleep;

	std::atomic<int32_t>             m_iAddMsgCount;
	std::atomic<int32_t>             m_iExecMsgCount;

	boost::mutex                      m_mutexWait;

private:
	void run();
};

typedef boost::shared_ptr<CCommonWorkThread> CommonWorkThreadPtr;

#endif

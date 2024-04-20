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
	CommonThread_data		thread_data;				///< ��ص��߳�����
	CCommonThreadCore	   *threadcore;				///< ʹ�õ�����
};

typedef std::list<CommonThread_Object>	CommonThread_Object_vector;	   ///< �߳�Ҫִ�е���Ϣ����

typedef void(*PFunc_LogOutput)(const std::string& strLog);

class CCommonWorkThread
{
public:
	CCommonWorkThread(void);
	virtual ~CCommonWorkThread(void);

public:	
	void         SetDeleteData(bool bDel = false);
	void         SetThreadName(const std::string& strThreadName);//ֻ����һ��
	std::string  GetThreadName();
	int          GetCWorkThreadListCount();	
	void         AddMessage(CCommonThreadCore *pThreadCore = NULL, __culong_ uMsgId = 0, __culong_ ulParam = 0, __culong_ uwParam = 0, __clong_ lData = 0, const std::string& strmsg = "", const std::string& strExt = "");
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
	boost::condition_variable_any			m_eventExecWait;
	
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


#endif


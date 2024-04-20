#include "Boost.h"
#include "CommonWorkThread.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
void CCommonWorkThread::SetDeleteData(bool bDel /* = false */)
{	
	m_bDeleteData = bDel;
}

void CCommonWorkThread::SetThreadName(const std::string& strThreadName)
{
	m_strThreadName = strThreadName;
}

std::string CCommonWorkThread::GetThreadName()
{
	return m_strThreadName;
}

int CCommonWorkThread::GetCWorkThreadListCount()
{
	return m_iAddMsgCount + m_iExecMsgCount;
}

void CCommonWorkThread::AddMessage(CCommonThreadCore *pThreadCore /* = NULL */, __culong_ uMsgId /* = 0 */, __culong_ ulParam /* = 0 */, __culong_ uwParam /* = 0 */, __clong_ lData /* = 0 */, std::string strmsg /* = "" */)
{
	try
	{		
		if (NULL != pThreadCore)
		{
			if (m_bStopRunning)
			{
				if (m_bDeleteData && pThreadCore)
				{
					pThreadCore->DeleteData(ulParam, uwParam, lData);
				}
				return ;
			}

			CommonThread_Object  msg_data;				

			msg_data.threadcore				= pThreadCore;
			msg_data.thread_data.msg_id		= uMsgId;
			msg_data.thread_data.lParam		= ulParam;
			msg_data.thread_data.wParam		= uwParam;
			msg_data.thread_data.data		= lData;			
			msg_data.thread_data.str_msg    = strmsg;

			_AddMsgToVector(msg_data);
			
			m_eventExecWait.notify_all();
			

			/*if (!_IsAddMsgVectorEmpty())
			{
				m_bStopRunning = false;
				if (m_threadExecMsg.isRunning())
				{
					m_eventExecWait.set();
					return ;
				}
			}

			if (!m_threadExecMsg.isRunning())
			{
				m_threadExecMsg.start(*this);
			}*/
		}
	}	
	catch (...)
	{
		std::string strTmp = m_strThreadName + "--Unknown HSAddMessage Exception";
		if (m_pFuncLogOutput)
		{
			m_pFuncLogOutput(strTmp);
		}
	}
}

void CCommonWorkThread::SetLogOutput(PFunc_LogOutput pFuncLogOutput)
{
	m_pFuncLogOutput = pFuncLogOutput;
}

#define MAX_PROCESSCOUNT 65535

bool CCommonWorkThread::SetProcessCountBeforeNeedSleep(int iProcessCount)
{
	if (iProcessCount <= 0 || iProcessCount > MAX_PROCESSCOUNT)
	{
		return false;
	}
	m_iProcessCountBeforeNeedSleep = iProcessCount;
	return true;
}

bool CCommonWorkThread::Start()
{
	m_bStopRunning = false;
	try
	{
		if (m_bIsRunning)
		{
			return true;
		}

        m_threadExecMsg.reset(new boost::thread(boost::bind(&CCommonWorkThread::run, this)));
		//m_threadExecMsg = std::async(&CCommonWorkThread::run, this);
		m_bIsRunning = true;
	}
	catch (...)
	{
		std::string strTmp = m_strThreadName + "--Unknown Start Exception";
		m_pFuncLogOutput(strTmp);
		return false;
	}
	return true;
}

void CCommonWorkThread::Stop(long lMilliseconds)
{
    m_bStopRunning = true;
	m_eventExecWait.notify_all();

	try
	{		/*
		if (lMilliseconds == 0)
		{
			m_threadExecMsg.wait();
		}
		else
		{
			m_threadExecMsg.wait_for(std::chrono::milliseconds(lMilliseconds));
		}*/
		m_threadExecMsg->join();
	}	
	catch (...)
	{
		std::string strTmp = m_strThreadName + "--Unknown Stop Exception";		
		if (m_pFuncLogOutput)
		{
			m_pFuncLogOutput(strTmp);
		}
	}

	{
		boost::lock_guard<boost::mutex> lock(m_mutexAddMsgVector);
		CommonThread_Object_vector::iterator iter;
		for (iter = m_vectorAddMsg.begin(); iter != m_vectorAddMsg.end(); ++iter)
		{
			if (m_bDeleteData && iter->threadcore)
			{
				iter->threadcore->DeleteData(iter->thread_data.lParam, iter->thread_data.wParam, iter->thread_data.data);
			}
		}
		m_vectorAddMsg.clear();
	}

	{		
		CommonThread_Object_vector::iterator iter;
		for (iter = m_vectorExecMsg.begin(); iter != m_vectorExecMsg.end(); ++iter)
		{
			if (m_bDeleteData && iter->threadcore)
			{
				iter->threadcore->DeleteData(iter->thread_data.lParam, iter->thread_data.wParam, iter->thread_data.data);
			}
		}
		m_vectorExecMsg.clear();
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CCommonWorkThread::CCommonWorkThread(void)
{
	m_iProcessCountBeforeNeedSleep = 200;
	m_bExitExec = false;
	m_pFuncLogOutput = NULL;
	m_bDeleteData = false;
	m_bStopRunning = false;
	m_bIsRunning = false;

	m_iAddMsgCount = 0;
	m_iExecMsgCount = 0;

	try
	{		
		//m_threadExecMsg = std::async(&CCommonWorkThread::run, this);
		 m_threadExecMsg.reset(new boost::thread(boost::bind(&CCommonWorkThread::run, this)));
		m_bIsRunning = true;
	}
	catch (...)
	{
		
	}
}

CCommonWorkThread::~CCommonWorkThread(void)
{
	m_pFuncLogOutput = NULL;
	Stop(0);	
}

#define EXETIME_ONESECOND 1000000

void CCommonWorkThread::run()
{
	int iExeCount = 0;
	while(!m_bStopRunning)
	{
		try
		{			

			if (!_IsExecMsgVectorEmpty())
			{				
				CommonThread_Object& tmp = m_vectorExecMsg.front();
					

				if (NULL != tmp.threadcore)
				{
					try
					{
						tmp.threadcore->ProcMessage(tmp.thread_data);
					}					
					catch (...)
					{
						if (m_pFuncLogOutput)
						{
							std::string strTmp = m_strThreadName + "--run Exception : ";
							m_pFuncLogOutput(strTmp);
						}
					}					
				}

				if (m_bDeleteData && tmp.threadcore)
                {
					tmp.threadcore->DeleteData(tmp.thread_data.lParam, tmp.thread_data.wParam, tmp.thread_data.data);
                }

				m_vectorExecMsg.pop_front();
				m_iExecMsgCount--;
                
				iExeCount++;
			}
			else
			{
				if (_IsAddMsgVectorEmpty())
				{					
					iExeCount = 0;
					boost::unique_lock<boost::mutex> lock(m_mutexWait);
					m_eventExecWait.wait(lock);
				}
				else
				{	
					boost::lock_guard<boost::mutex> lock(m_mutexAddMsgVector);
					m_vectorExecMsg.splice(m_vectorExecMsg.end(), m_vectorAddMsg, m_vectorAddMsg.begin(), m_vectorAddMsg.end());
					m_iExecMsgCount.store(m_iAddMsgCount.load());
					m_iAddMsgCount = 0;                    
				}
			}			

			if (iExeCount >= m_iProcessCountBeforeNeedSleep.load())
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				boost::this_thread::sleep(boost::posix_time::millisec(20));
				iExeCount = 0;
			}
						
		}
		catch (...)
		{
			int k = 0;
		}		
	}
}


bool CCommonWorkThread::_AddMsgToVector(CommonThread_Object msgData)
{
	boost::lock_guard<boost::mutex> lock(m_mutexAddMsgVector);
	m_vectorAddMsg.push_back(msgData);
	m_iAddMsgCount++;
	return true;
}

bool CCommonWorkThread::_IsAddMsgVectorEmpty()
{
	boost::lock_guard<boost::mutex> lock(m_mutexAddMsgVector);
	return m_vectorAddMsg.empty();
}

bool CCommonWorkThread::_IsExecMsgVectorEmpty()
{	
	return m_vectorExecMsg.empty();
}


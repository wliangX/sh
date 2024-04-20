#include "CommonTimer.h"
#include <iostream>
#include <thread> 
/************************************************************************/
/*                                                                      */
/************************************************************************/

CCommonTimerManager CCommonTimerManager::m_instance;
CCommonTimerManager& CCommonTimerManager::GetInstance()
{
	return m_instance;
}

CCommonTimerManager::CCommonTimerManager() :m_pTimerMgr(CCommonTimerPtr((CCommonTimer*)NULL))
{
}

unsigned long CCommonTimerManager::AddTimer(CCommonTimerCore* pTimerCore, unsigned long long uInterval)
{
	if (NULL == m_pTimerMgr)
	{
		m_pTimerMgr = std::make_shared<CCommonTimer>();
	}
	if (NULL != m_pTimerMgr)
	{
		return m_pTimerMgr->AddTimer(pTimerCore, uInterval);
	}
	return INVALID_TIMER_ID;
}
void CCommonTimerManager::RemoveTimer(unsigned long uTimerId)
{
	if (NULL == m_pTimerMgr)
	{
		m_pTimerMgr = std::make_shared<CCommonTimer>();
	}
	if (NULL != m_pTimerMgr)
	{
		m_pTimerMgr->RemoveTimer(uTimerId);
	};
}

unsigned long CCommonTimer::AddTimer(CCommonTimerCore *pTimerCore, unsigned long long uInterval)
{	
	CommonTimerDataPtr pCommonTimerData = std::make_shared<CommonTimerData>();
	pCommonTimerData->pCoreTimer = pTimerCore;
	std::chrono::system_clock::time_point clock = std::chrono::system_clock::now();
	pCommonTimerData->u64FirstTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.time_since_epoch()).count();
	pCommonTimerData->uInterval = uInterval;

	unsigned long ulMsgId = _AddTimer(pCommonTimerData);

	return ulMsgId;
}

void CCommonTimer::RemoveTimer(unsigned long uMsgId)
{
	_RemoveTimer(uMsgId);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CCommonTimer::CCommonTimer():m_bStopThread(false), m_bIsRunning(false), m_ulCurrentTimerID(1)
{
	_StartThreadPool();
}

CCommonTimer::~CCommonTimer()
{
	_StopThreadPool();
}

bool CCommonTimer::_StartThreadPool()
{	
	m_bStopThread = false;
	return true;
}

void CCommonTimer::_StopThreadPool()
{	
	try
	{
		m_bStopThread = true;
		m_threadpool.join_all();
	}	
	catch (...)
	{
		
	}
}

unsigned long CCommonTimer::_AddTimer(CommonTimerDataPtr pTimerData)
{
	unsigned long ulMsgId = GetCurrentTimerID();

	pTimerData->ulTimerID = ulMsgId;
	pTimerData->bIsRunnning.store(true);
	//std::cout << " m_threadpool size() " << m_threadpool.size() << std::endl;
	pTimerData->timerThread = m_threadpool.create_thread(boost::bind(&CCommonTimer::DoTimer, this, pTimerData));
	{
		std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
		m_mapTimerInfo.insert(std::make_pair(ulMsgId, pTimerData));
	}
	return ulMsgId;
}

unsigned long CCommonTimer::GetCurrentTimerID()
{
	if (m_ulCurrentTimerID == INVALID_TIMER_ID)
	{
		m_ulCurrentTimerID = 1;
	}
	return m_ulCurrentTimerID++;
}

void CCommonTimer::_RemoveTimer(unsigned long uMsgId)
{
	CommonTimerDataPtr pTimerData = NULL;
	GetCommonTimerData(uMsgId, pTimerData);
	if (NULL != pTimerData)
	{
		pTimerData->StopThread();
		if (NULL != pTimerData->timerThread)
		{
			m_threadpool.remove_thread(pTimerData->timerThread);
			delete pTimerData->timerThread;
			pTimerData->timerThread = NULL;
		}
	}
}
void CCommonTimer::GetCommonTimerData(unsigned long& uMsgId, CommonTimerDataPtr& pTimerData)
{
	std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
	std::map<unsigned long, CommonTimerDataPtr>::iterator itr;
	itr = m_mapTimerInfo.find(uMsgId);
	if (itr != m_mapTimerInfo.end())
	{
		pTimerData = itr->second;
		m_mapTimerInfo.erase(itr);
	}
}
void CCommonTimer::DoTimer(CommonTimerDataPtr pTimerData)
{
	while (!m_bStopThread)
	{
		std::chrono::system_clock::time_point clock = std::chrono::system_clock::now();
		unsigned long long u64CurTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.time_since_epoch()).count();
		long long llTimeSpan = u64CurTime - pTimerData->u64FirstTime;
		long long llTimeRemain = pTimerData->uInterval - llTimeSpan;
		//std::cout << " llTimeRemain " << llTimeRemain << " TimerID "<< pTimerData->ulTimerID << std::endl;
		if (llTimeRemain > 0)
		{
			//boost::this_thread::sleep(boost::posix_time::millisec(llTimeRemain));//当没有interrupt请求时，这条语句会让当前线程sleep llTimeRemain毫秒，若有interrupt requirement线程结束。
			std::this_thread::sleep_for(std::chrono::milliseconds(llTimeRemain));//强制等待一定时间
		}
		//std::cout << " pTimerData->bIsRunnning " << pTimerData->bIsRunnning << " TimerID " << pTimerData->ulTimerID << std::endl;
		if (!pTimerData->bIsRunnning)
		{
			break;
		}
		//取执行的时间为定时器当前执行的时间
		clock = std::chrono::system_clock::now();
		pTimerData->u64FirstTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.time_since_epoch()).count();
		pTimerData->pCoreTimer->OnTimer(pTimerData->ulTimerID);
	}
	//std::cout << " DoTimer over TimerID " << pTimerData->ulTimerID << std::endl;
	//定义interruption_point接收定时器线程的interrupt消息
	boost::this_thread::interruption_point();
}

void CommonTimerData::StopThread()
{
	if (bIsRunnning.load())
	{
		bIsRunnning.store(false);
		if (NULL != timerThread)
		{
			timerThread->interrupt();
			if (timerThread->joinable())
			{
				timerThread->join();
			}
		}
	}

}

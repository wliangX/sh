
#include "StatisticsTimer.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
void CStatisticsTimer::AddTimer(CStatisticsTimerCore *pTimerCore, unsigned long uMsgId, unsigned long long uInterval)
{	
	CommonTimerData data;
	data.pCoreTimer = pTimerCore;
	
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();	
	data.u64FirstTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	data.uInterval = uInterval;

	_AddTimer(uMsgId, data);

	if (m_bIsRunning)
	{
		m_envetNotifyExec.notify_all();
		return ;
	}

	_StartThread();	
}

void CStatisticsTimer::RemoveTimer(unsigned long uMsgId)
{
	_RemoveTimer(uMsgId);
}

void CStatisticsTimer::SetTimerMode(TimerMode mode)
{
	m_timerMode = mode;
}

void CStatisticsTimer::SetTimerName(const std::string& strTimerName)
{
	m_strTimerName = strTimerName;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CStatisticsTimer::CStatisticsTimer()
{
	m_bStopThread = false;
	m_timerMode = TimerMode_Absolute;
	m_bIsRunning = false;
}

CStatisticsTimer::~CStatisticsTimer()
{
	_StopThread();
}

bool CStatisticsTimer::_StartThread()
{	
	m_bStopThread = false;
	try
	{	
		if (!m_bIsRunning)
		{
			std::lock_guard<std::mutex> lock(m_mutexStartThread);
			if (!m_bIsRunning)
			{
				//m_threadExec = std::async(&CCommonTimer::run, this);
				m_threadExec.reset(new boost::thread(boost::bind(&CStatisticsTimer::run, this)));
				m_bIsRunning = true;				
			}			
		}		
	}		
	catch (...)
	{
		return false;
	}
	
	return true;
}

void CStatisticsTimer::_StopThread()
{	
	try
	{
		m_bStopThread = true;
		m_envetNotifyExec.notify_all();
		//m_threadExec.wait_for(std::chrono::milliseconds(100));
		m_threadExec->join();
		m_bIsRunning = false;
	}	
	catch (...)
	{
		
	}
}


void CStatisticsTimer::_AddTimer(unsigned long uMsgId, CommonTimerData data)
{
	std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
	m_mapTimerInfo.insert(std::make_pair(uMsgId, data));
}

void CStatisticsTimer::_RemoveTimer(unsigned long uMsgId)
{
	std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
	std::map<unsigned long, CommonTimerData>::iterator itr;
	itr = m_mapTimerInfo.find(uMsgId);
	if (itr != m_mapTimerInfo.end())
	{
		m_mapTimerInfo.erase(itr);
	}
}

void CStatisticsTimer::_ModifyFirstTime(unsigned uMsgId, unsigned long long uTime)
{
	std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
	std::map<unsigned long, CommonTimerData>::iterator itr;
	itr = m_mapTimerInfo.find(uMsgId);
	if (itr != m_mapTimerInfo.end())
	{
		itr->second.u64FirstTime = uTime;
	}
}

bool CStatisticsTimer::_IsTimerMapEmpty()
{
	std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
	return m_mapTimerInfo.empty();
}

void CStatisticsTimer::run()
{	
	while(!m_bStopThread)
	{
		if (_IsTimerMapEmpty())
		{
			std::unique_lock<std::mutex> lock(m_mutexNotifyExec);			
			m_envetNotifyExec.wait(lock);
		}
		else
		{
			std::map<unsigned long, CommonTimerData> mapTmps;
			{
				std::lock_guard<std::mutex> lock(m_mutexTimerInfo);
				mapTmps = m_mapTimerInfo;
			}		

			std::map<unsigned long, CommonTimerData>::const_iterator itr;
			for (itr = mapTmps.begin(); itr != mapTmps.end(); ++itr)
			{
				std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();				
				unsigned long long u64Time = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
				long long n64Interval = (long long)(u64Time - itr->second.u64FirstTime);
				if (n64Interval < 0)
				{
					_ModifyFirstTime(itr->first, u64Time);
					continue;
				}

				unsigned long long n64Tmp = itr->second.uInterval;
				n64Tmp *= (1000);					
				
				if ((unsigned long long)n64Interval > n64Tmp)
				{
					if (n64Interval > (long long)(n64Tmp * 2))
					{
						_ModifyFirstTime(itr->first, u64Time);
					}
					else
					{
						if (m_timerMode == TimerMode_Absolute)
						{
							_ModifyFirstTime(itr->first, itr->second.u64FirstTime + n64Tmp);
						}
						else
						{
							_ModifyFirstTime(itr->first, u64Time);
						}						
					}
					
					if (m_bStopThread)
					{
						break;
					}

					try
					{
						itr->second.pCoreTimer->OnStatisticsTimer(itr->first);
					}					
					catch (...)
					{
					}
									
				}
			}
			
			boost::this_thread::sleep(boost::posix_time::millisec(1));
		}		
	}	
	
}
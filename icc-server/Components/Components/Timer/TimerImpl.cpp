#include "Boost.h"
//Project
#include "TimerImpl.h"


//////////////////////////////////////////////////////////////////////////
/*TimerController实现*/
//////////////////////////////////////////////////////////////////////////

TimerController::TimerController(std::string p_strCmdName,
	std::string p_strTimerName,
	AsioEngine& p_IoService,
	unsigned int p_iInterval, 
	unsigned int p_iDelayStartSec, IResourceManagerPtr p_ResourceManager): m_strCmdName(p_strCmdName),
	 m_strTimerName(p_strTimerName),
	 m_Timer(p_IoService, boost::posix_time::seconds(p_iDelayStartSec)),
	 m_pResourceManager(p_ResourceManager)
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);
	m_pObserverCenterList = ICCGetIObserverFactory()->GetObserverCenterList();
	m_pString = ICCGetIStringFactory()->CreateString();

	m_iInterval = p_iInterval;
	m_iDelayStartSec = p_iDelayStartSec;

	m_Timer.async_wait([this, p_strTimerName](const boost::system::error_code &){
		OnTimer(p_strTimerName);
	});
}

TimerController::~TimerController()
{
	m_Timer.cancel();
}

//定时器响应
void TimerController::OnTimer(std::string p_strTimerName)
{	
	PROTOCOL::CTimer l_timer;
	l_timer.m_oHeader.m_strCmd = m_strCmdName;
	l_timer.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_timer.m_oBody.m_strTimerName = m_strTimerName;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_timer.ToString(l_pIJson);
	NotifyObserverList(boost::make_shared<CNotifiReceive>(l_strMsg, l_timer.m_oHeader.m_strMsgid), ObserverPattern::ENotifyType::Post);

	if (m_iInterval != 0)
	{
		m_Timer.expires_at(m_Timer.expires_at() + boost::posix_time::seconds(m_iInterval));

		m_Timer.async_wait([this, p_strTimerName](const boost::system::error_code &){
			OnTimer(p_strTimerName);
		});
	}
	else
	{
		//清除执行一次的定时器
		PROTOCOL::CTimer l_timer;
		l_timer.m_oHeader.m_strCmd = TIMER_TIMEOUT;		
		l_timer.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_timer.m_oBody.m_strTimerName = p_strTimerName;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_timer.ToString(l_pIJson);
		NotifyObserverList(boost::make_shared<CNotifiReceive>(l_strMsg, l_timer.m_oHeader.m_strMsgid), ObserverPattern::ENotifyType::Post);
	}
}

//获取定时器名称
std::string TimerController::GetName()
{
	return m_strTimerName;
}

void TimerController::NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, ObserverPattern::ENotifyType NotifyType /* = ObserverPattern::ENotifyType::Post */)
{
	for (auto l_observer : m_pObserverCenterList)
	{
		l_observer->Notify(p_pNotifiRequest, NotifyType);
	}
}

IResourceManagerPtr TimerController::GetResourceManager()
{
	return m_pResourceManager;
}

//////////////////////////////////////////////////////////////////////////
/*CTimerManagerImpl实现*/
//////////////////////////////////////////////////////////////////////////

CTimerManagerImpl::CTimerManagerImpl(IResourceManagerPtr p_ResourceManagerPtr) 
	: m_pThread(nullptr), 
	m_Permanence(m_IoService), 
	m_bIsRunning(false),
	m_IsStarted(false),
	m_pResourceManager(p_ResourceManagerPtr)
{	
	Start();
}

CTimerManagerImpl::~CTimerManagerImpl()
{	
	Stop();
}

//启动定时器控制块
bool CTimerManagerImpl::Start()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);

	MANUL_ADDOBSERVER(m_pObserverCenter, CTimerManagerImpl, TIMER_TIMEOUT, TimeOut);

	if (m_bIsRunning)
	{
		return false;
	}

	try
	{		
		m_pThread = boost::make_shared<boost::thread>([this](){
			{
				boost::lock_guard<boost::mutex> guard(m_Mutex);
				m_IsStarted = true;
				m_Conditon.notify_all();
			}
			m_IoService.run();
		});
	}	
	catch (...)
	{

	}	

	{
		boost::lock_guard<boost::mutex> guard(m_Mutex);
		while (!m_IsStarted)
		{
			boost::posix_time::seconds const delay(5);
			boost::system_time const start = boost::get_system_time();
			boost::system_time const timeout = start + delay;
			if (!m_Conditon.timed_wait(m_Mutex, timeout))
			{
				return false;
			}			
		}
	}
	m_bIsRunning = true;

	return true;
}

//停止定时器控制块
void CTimerManagerImpl::Stop()
{
	m_IoService.stop();
	if (m_pThread)
	{
		m_pThread->join();
	}
	m_TimerMap.clear();

	MANUL_REMOVEOBSERVER(m_pObserverCenter, CTimerManagerImpl, TIMER_TIMEOUT, TimeOut);
}

//添加定时器
std::string CTimerManagerImpl::AddTimer(std::string p_strCmdName, unsigned int p_iInterval, unsigned int p_iDelayStartSec)
{
	boost::uuids::random_generator rgen;//随机生成器  
	boost::uuids::uuid u = rgen();//生成一个随机的UUID
	std::string p_strTimerName = boost::uuids::to_string(u);
	TimerControllerPtr tmp_pTimerController;
	try
	{
		tmp_pTimerController = boost::make_shared<TimerController>(p_strCmdName, p_strTimerName, m_IoService, p_iInterval, p_iDelayStartSec, m_pResourceManager);
	}	
	catch (...)
	{
	}

	if (tmp_pTimerController)
	{
		boost::lock_guard<boost::mutex> guard(m_TimerMapLock);
		std::map<std::string, TimerControllerPtr>::iterator it = m_TimerMap.find(tmp_pTimerController->GetName());
		if (it == m_TimerMap.end())
		{
			m_TimerMap[tmp_pTimerController->GetName()] = tmp_pTimerController;
		}
	}	

	return p_strTimerName;
}

void CTimerManagerImpl::TimeOut(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	boost::lock_guard<boost::mutex> guard(m_TimerMapLock);

 	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
 	PROTOCOL::CTimer l_TimerReq;
	if (!l_TimerReq.ParseString(p_pNotifiRequest->GetMessages(), l_pJson))
	{
		return;
	}
	std::string l_strTimerName = l_TimerReq.m_oBody.m_strTimerName;
	std::map<std::string, TimerControllerPtr>::iterator it = m_TimerMap.find(l_strTimerName);
	if (it != m_TimerMap.end())
	{
		m_TimerMap.erase(it);
	}
}

IResourceManagerPtr CTimerManagerImpl::GetResourceManager()
{
	return m_pResourceManager;
}
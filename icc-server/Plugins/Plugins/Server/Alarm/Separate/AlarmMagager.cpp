#include "Boost.h"
#include "AlarmMagager.h"


CAlarmQueue* CAlarmQueue::sm_instance = nullptr;
boost::mutex CAlarmQueue::sm_mutexInstance;

CAlarmQueue* CAlarmQueue::Instance()
{
	if (nullptr == sm_instance)
	{
		boost::lock_guard<boost::mutex> lock(sm_mutexInstance);
		if (nullptr == sm_instance)
		{
			sm_instance = new CAlarmQueue();
		}
	}
	return sm_instance;

}

bool CAlarmQueue::Init(IResourceManagerPtr p_pResourceManager)
{
	if (m_bIsInit)
	{
		return true;
	}

	m_pResourceManager = p_pResourceManager;
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();

	MANUL_ADDOBSERVER(m_pObserverCenter, CAlarmQueue, ALARM_QUEUE_TIMER_CMD_NAME, OnTimer);
	m_pTimerMgr->AddTimer(ALARM_QUEUE_TIMER_CMD_NAME, 2, 1);

	m_bIsInit = true;

	return true;
}

CAlarmQueue::CAlarmQueue() :m_bIsInit(false)
{

}

CAlarmQueue::~CAlarmQueue()
{

}

IResourceManagerPtr CAlarmQueue::GetResourceManager()
{
	return m_pResourceManager;
}

void CAlarmQueue::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{

}

void CAlarmQueue::AddAlarm(Alarm& p_alarm)
{
	std::string l_val = p_alarm.ToString(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet(ALARM_QUEUE_KEY, p_alarm.m_oBody.m_oAlarm.m_strID, l_val);
}

bool CAlarmQueue::GetAlarm(std::string p_strAlarmID, Alarm& p_alarm)
{
	std::string l_val;
	if (!m_pRedisClient->HGet(ALARM_QUEUE_KEY, p_strAlarmID, l_val))
	{
		return false;
	}
	
	return p_alarm.ParseString(l_val, m_pJsonFty->CreateJson());
}

//É¾³ý½Ó¾¯µ¥
void CAlarmQueue::DelAlarm(std::string p_strAlarmID)
{
	std::string l_val;
	if (m_pRedisClient->HGet(ALARM_QUEUE_KEY, p_strAlarmID, l_val))
	{
		Alarm l_alarm;
		l_alarm.ParseString(l_val, m_pJsonFty->CreateJson());

		PROTOCOL::CDelAlarmCacheNotify l_notify;
		l_notify.m_oHeader.m_strSystemID = "ICC";
		l_notify.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_notify.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_notify.m_oHeader.m_strCmd = "del_alarm_cache";
		l_notify.m_oHeader.m_strRequest = "queue_alarm_separate";
		l_notify.m_oHeader.m_strRequestType = "0";

		l_notify.m_oBody.m_strAlarmID = p_strAlarmID;
		l_notify.m_oBody.m_strSeatNo = l_alarm.m_oBody.m_oAlarm.m_strSeatNo;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_notify.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}

	m_pRedisClient->HDel(ALARM_QUEUE_KEY, p_strAlarmID);
}


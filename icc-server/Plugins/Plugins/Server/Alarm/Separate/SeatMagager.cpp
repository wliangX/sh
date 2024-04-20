#include "Boost.h"
#include "SeatMagager.h"


CSeatManager* CSeatManager::sm_instance = nullptr;
boost::mutex CSeatManager::sm_mutexInstance;

CSeatManager* CSeatManager::Instance()
{
	if (nullptr == sm_instance)
	{
		boost::lock_guard<boost::mutex> lock(sm_mutexInstance);
		if (nullptr == sm_instance)
		{
			sm_instance = new CSeatManager();
		}
	}
	return sm_instance;

}

bool CSeatManager::Init(IResourceManagerPtr p_pResourceManager)
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
	m_pLockFty = ICCGetILockFactory();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();

	MANUL_ADDOBSERVER(m_pObserverCenter, CSeatManager, SEAT_MANAGER_TIMER_CMD_NAME, OnTimer);
	m_pTimerMgr->AddTimer(SEAT_MANAGER_TIMER_CMD_NAME, 2, 1);

	m_bIsInit = true;
	return true;
}

CSeatManager::CSeatManager() :m_bIsInit(false)
{

}

CSeatManager::~CSeatManager()
{

}

IResourceManagerPtr CSeatManager::GetResourceManager()
{
	return m_pResourceManager;
}

void CSeatManager::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{


}

//获取在线席位的信息
bool CSeatManager::GetOnlineSeat(std::string p_strSeatNo, Separate::SeatInfo & p_SeatInfo)
{
	std::string l_strVal;
	if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO_KEY, p_strSeatNo, l_strVal))
	{
		return false;
	}

	return p_SeatInfo.Parse(l_strVal, m_pJsonFty->CreateJson());
}

//获取一个和当前接警席同一部门空闲的处警席
std::string CSeatManager::GetFreeProcessSeat(std::string p_strDeptCode)
{
	static std::string l_strLastHandleSeatNo;

	std::map<std::string, std::string> l_mapItem;
	if (!m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO_KEY, l_mapItem))
	{
		return "";
	}

	std::string l_strFreeSeat;
	auto var = l_mapItem.begin();
	for (; var != l_mapItem.end(); ++var)
	{
		Separate::SeatInfo l_seatInfo;
		if (l_seatInfo.Parse(var->second, m_pJsonFty->CreateJson()))
		{
			int l_iProcessSize = GetCacheCount(l_seatInfo.m_strSeatNo); //处警席缓存，暂时没用到，后续可根据缓存大小调整策略
			
			//找出是本单位的处警席或者接处警席
			if ((l_seatInfo.m_nSeatType == SEAT_PROCESS || l_seatInfo.m_nSeatType == SEAT_SYNTHETICAL) && p_strDeptCode == l_seatInfo.m_strDeptCode)
			{
				//上一次的已经处理过的就跳过
				if (l_strLastHandleSeatNo != l_seatInfo.m_strSeatNo)
				{
					//记录查找位置，下次从该位置继续往下查找
					l_strLastHandleSeatNo = l_seatInfo.m_strSeatNo;
					return l_seatInfo.m_strSeatNo;
				}
			}			
		}
	}

	//但如果没有可用的，还是继续分给该坐席
	if (l_strFreeSeat.empty() && !l_strLastHandleSeatNo.empty())
	{
		l_strFreeSeat = l_strLastHandleSeatNo;
	}

	return l_strFreeSeat;
}

//席位添加一条暂存单
bool CSeatManager::AddCache(std::string p_strSeatNo, std::string p_strAlarmID)
{
	std::string l_strVal;
	m_pRedisClient->HGet(ALARM_CASH_KEY, p_strSeatNo, l_strVal);

	Separate::AlarmCacheList l_alarmCacheList;
	if (!l_strVal.empty())
	{
		if (!l_alarmCacheList.Parse(l_strVal, m_pJsonFty->CreateJson()))
		{
			return false;
		}
	}

	Separate::AlarmCache l_alarmCache;
	l_alarmCache.m_strAlarmID = p_strAlarmID;
	l_alarmCache.m_strSeatNo = p_strSeatNo;
	l_alarmCache.m_isSend = true;

	l_alarmCacheList.m_alarmCaches.push_back(l_alarmCache);
	
	std::string l_strNewVal = l_alarmCacheList.ToString(m_pJsonFty->CreateJson());
	return m_pRedisClient->HSet(ALARM_CASH_KEY, p_strSeatNo, l_strNewVal);
}

//席位删除一条暂存单
bool CSeatManager::DeleteCache(std::string p_strSeatNo, std::string p_strAlarmID)
{
	std::string l_strVal;
	m_pRedisClient->HGet(ALARM_CASH_KEY, p_strSeatNo, l_strVal);

	Separate::AlarmCacheList l_alarmCacheList;
	if (!l_alarmCacheList.Parse(l_strVal, m_pJsonFty->CreateJson()))
	{
		return false;
	}

	auto it = l_alarmCacheList.m_alarmCaches.begin();
	for (; it != l_alarmCacheList.m_alarmCaches.end();)
	{
		if (it->m_strAlarmID == p_strAlarmID)
		{
			it = l_alarmCacheList.m_alarmCaches.erase(it);
			break;
		}

		++it;
	}

	std::string l_strNewVal = l_alarmCacheList.ToString(m_pJsonFty->CreateJson());
	if (!m_pRedisClient->HSet(ALARM_CASH_KEY, p_strSeatNo, l_strNewVal))
	{
		return false;
	}
	
	PROTOCOL::CDelProccessCacheNotify l_notify;
	l_notify.m_oHeader.m_strSystemID = "ICC";
	l_notify.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_notify.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_notify.m_oHeader.m_strCmd = "del_alarm_cache";
	l_notify.m_oHeader.m_strRequest = "queue_alarm_separate";
	l_notify.m_oHeader.m_strRequestType = "0";

	l_notify.m_oBody.m_strAlarmID = p_strAlarmID;
	l_notify.m_oBody.m_strSeatNo = p_strSeatNo;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_notify.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	
	return true;
}

//获取席位暂存单
bool CSeatManager::GetCacheList(std::string p_strSeatNo, std::list<std::string>& p_listCache)
{
	std::string l_strVal;
	m_pRedisClient->HGet(ALARM_CASH_KEY, p_strSeatNo, l_strVal);

	Separate::AlarmCacheList l_alarmCacheList;
	if (!l_alarmCacheList.Parse(l_strVal, m_pJsonFty->CreateJson()))
	{
		return false;
	}

	for (auto var : l_alarmCacheList.m_alarmCaches)
	{
		if (p_strSeatNo.empty())
		{
			p_listCache.push_back(var.m_strAlarmID);
		}
		else
		{
			if (var.m_strSeatNo == p_strSeatNo)
			{
				p_listCache.push_back(var.m_strAlarmID);
			}
		}		
	}

	return true;
}

//获取席位暂存单数量
unsigned int CSeatManager::GetCacheCount(std::string p_strSeatNo)
{
	std::list<std::string> l_listCache;
	GetCacheList(p_strSeatNo, l_listCache);

	return l_listCache.size();
}


//添加公共警单
void CSeatManager::AddPubCache(std::string p_strDeptCode, std::string p_strAlarmID)
{
	std::string l_strVal;
	m_pRedisClient->HGet(PUB_ALARM_CASH_KEY, p_strDeptCode, l_strVal);

	Separate::AlarmCacheList l_alarmCacheList;
	if (!l_strVal.empty())
	{		
		if (!l_alarmCacheList.Parse(l_strVal, m_pJsonFty->CreateJson()))
		{
			return;
		}
	}

	Separate::AlarmCache l_alarmCache;
	l_alarmCache.m_strAlarmID = p_strAlarmID;
	l_alarmCache.m_strDeptCode = p_strDeptCode;
	l_alarmCache.m_isSend = true;

	l_alarmCacheList.m_alarmCaches.push_back(l_alarmCache);

	std::string l_strNewVal = l_alarmCacheList.ToString(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet(PUB_ALARM_CASH_KEY, p_strDeptCode, l_strNewVal);
}

//删除公共警单
void CSeatManager::DelPubCache(std::string p_strDeptCode, std::string p_strAlarmID)
{
	std::string l_strVal;
	m_pRedisClient->HGet(PUB_ALARM_CASH_KEY, p_strDeptCode, l_strVal);

	Separate::AlarmCacheList l_alarmCacheList;
	if (!l_alarmCacheList.Parse(l_strVal, m_pJsonFty->CreateJson()))
	{
		return ;
	}

	auto it = l_alarmCacheList.m_alarmCaches.begin();
	for (; it != l_alarmCacheList.m_alarmCaches.end();)
	{
		if (it->m_strAlarmID == p_strAlarmID)
		{
			it = l_alarmCacheList.m_alarmCaches.erase(it);
			break;
		}

		++it;
	}

	std::string l_strNewVal = l_alarmCacheList.ToString(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet(ALARM_CASH_KEY, p_strDeptCode, l_strNewVal);
}

//获取指定单位下的公共警单
bool CSeatManager::GetPubCache(std::string p_strDeptCode, PubAlarmList & p_pubList)
{
	std::map<std::string, std::string> tmp_List;
	if (!m_pRedisClient->HGetAll(PUB_ALARM_CASH_KEY, tmp_List))
	{
		return false;
	}

	for (auto var : tmp_List)
	{
		if (p_strDeptCode == var.first)
		{
			Separate::AlarmCacheList l_alarmCacheList;
			if (!l_alarmCacheList.Parse(var.second, m_pJsonFty->CreateJson()))
			{
				return false;
			}

			for (auto varCache : l_alarmCacheList.m_alarmCaches)
			{
				p_pubList.push_back(varCache.m_strAlarmID);
				//从公共队列中删除
				DelPubCache(p_strDeptCode, varCache.m_strAlarmID);
			}					
		}
	}
	return true;
}
//获取所有的公共警单
bool CSeatManager::GetAllPubCache(PubAlarmList & p_pubList)
{
	std::map<std::string, std::string> tmp_List;
	if (!m_pRedisClient->HGetAll(PUB_ALARM_CASH_KEY, tmp_List))
	{
		return false;
	}

	for (auto var : tmp_List)
	{
		Separate::AlarmCacheList l_alarmCacheList;
		if (!l_alarmCacheList.Parse(var.second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		for (auto varCache : l_alarmCacheList.m_alarmCaches)
		{
			p_pubList.push_back(varCache.m_strAlarmID);
		}
		
		//从公共队列中删除
		m_pRedisClient->HDel(PUB_ALARM_CASH_KEY, var.first);
	}
	return true;
}

//获取处警单位代码
std::string CSeatManager::GetPrcoessDeptCode(std::string p_strAlarmID, std::string p_strProcessID)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_pjdb";
	l_oSeleteAlarmSQLReq.param["id"] = p_strProcessID;
	l_oSeleteAlarmSQLReq.param["alarm_id"] = p_strAlarmID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}
	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
		return "";
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get process failed, [%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	std::string l_strDeptCode;
	if (l_pResult->Next())
	{
		l_strDeptCode = l_pResult->GetValue("process_dept_code");
	}

	ICC_LOG_DEBUG(m_pLog, "get process deptcode[%s], alarm_id", p_strAlarmID.c_str());

	return l_strDeptCode;
}
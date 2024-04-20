#include "Boost.h"

#include "CallManager.h"
#include "Task.h"
#include "TaskManager.h"
#include "DeviceManager.h"
#include "SysConfig.h"

#define CHECK_TIMESPAN			5		//	话务检测时间间隔，单位：秒
#define SLEEP_TIMESPAN			100		//	单位：毫秒
#define CALL_OVER_TIMESPAN		2		//	单位：分钟，2 分钟后定时删除已结束通话的话务

boost::shared_ptr<CCallManager> CCallManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CCallManager> CCallManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CCallManager>();
	}

	return m_pManagerInstance;
}
void CCallManager::ExitInstance()
{
	//
}

CCallManager::CCallManager(void)
{
	m_pLog = nullptr;
	m_pStrUtil = nullptr;
	m_pDateTime = nullptr;
}

CCallManager::~CCallManager(void)
{
	//
}
//////////////////////////////////////////////////////////////////////////

std::string CCallManager::CreateCTICallRefId(long p_lCSTACallRefId)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));
	//strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", localtime(&l_tCurrentTime));

	char l_szCallRefId[MAX_BUFFER] = { 0 };

#ifdef _DEBUG
	sprintf_s(l_szCallRefId, MAX_BUFFER - 1, "110%u", p_lCSTACallRefId);
#else
	sprintf_s(l_szCallRefId, MAX_BUFFER - 1, "%s%06u", l_szDayTime, p_lCSTACallRefId);
#endif // _DEBUG

	return std::string(l_szCallRefId);
}

std::string CCallManager::CreateNewCall(long p_lCSTACallRefId)
{
	std::string l_strCTICallRefId = this->CreateCTICallRefId(p_lCSTACallRefId);

	boost::shared_ptr<CCall> l_pCall = boost::make_shared<CCall>(p_lCSTACallRefId);
	l_pCall->SetCTICallRefId(l_strCTICallRefId);
	l_pCall->SetCallDirection(CallDirectionString[CALL_DIRECTION_OUT]);
	l_pCall->SetStateTime(m_pDateTime->CurrentDateTimeStr());
	this->AddCall(l_pCall);

	ICC_LOG_DEBUG(m_pLog, "Create New Call, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
		p_lCSTACallRefId, l_strCTICallRefId.c_str(), this->GetCallQueueSize());

	return l_strCTICallRefId;
}
bool CCallManager::AddCall(boost::shared_ptr<CCall> p_pCall)
{
	RemoveCallQueueHead();

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	m_mapCallQueue[p_pCall->GetCSTACallRefId()] = p_pCall;

	return true;
}
bool CCallManager::RemoveCallQueueHead()
{
	if (GetCallQueueSize() < MAX_CALLQUEUE_COUNT)
	{
		return false;
	}

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.begin();
	if (iter != m_mapCallQueue.end())
	{
		if (iter->first && iter->second /*&& it->second->GetCallState().compare(CallStateString[STATE_FREE]) == 0*/)
		{
			long l_lCSTACallRefId = iter->first;
			std::string l_strCTICallRefId = iter->second->GetCTICallRefId();

			m_mapCallQueue.erase(iter);

			ICC_LOG_DEBUG(m_pLog, "Remove Call Queue Head, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				l_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());
		}
	}

	return true;
}

bool CCallManager::DeleteCall(long p_lCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = true;
			std::string l_strCTICallRefId = iter->second->GetCTICallRefId();
			
			m_mapCallQueue.erase(iter);

			ICC_LOG_DEBUG(m_pLog, "Delete Call, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				p_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());
		}
	}

	return l_bReturn;
}

void CCallManager::ClearCallList()
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	m_mapCallQueue.clear();

	ICC_LOG_DEBUG(m_pLog, "Clear Call Queue !!!");
}

int CCallManager::GetCallQueueSize()
{
	int l_nCount = 0;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	l_nCount = m_mapCallQueue.size();

	return l_nCount;
}
//////////////////////////////////////////////////////////////////////////
void CCallManager::SetOriginalCallerId(long p_lCSTACallRefId, const std::string& p_strOriginalCallerId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetOriginalCallerId(p_strOriginalCallerId);
		}
	}
}
std::string CCallManager::GetOriginalCallerId(long p_lCSTACallRefId)
{
	std::string l_strOriginalCallerId = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strOriginalCallerId = iter->second->GetOriginalCallerId();
		}
	}

	return l_strOriginalCallerId;
}

//	原始被叫
void CCallManager::SetOriginalCalledId(long p_lCSTACallRefId, const std::string& p_strOriginalCalledId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetOriginalCalledId(p_strOriginalCalledId);
		}
	}
}
std::string CCallManager::GetOriginalCalledId(long p_lCSTACallRefId)
{
	std::string l_strOriginalCalledId = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strOriginalCalledId = iter->second->GetOriginalCalledId();
		}
	}

	return l_strOriginalCalledId;
}

void CCallManager::SetIsTransferCall(long p_lCSTACallRefId, bool p_bIsTransferCall)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetIsTransferCall(p_bIsTransferCall);
		}
	}
}
bool CCallManager::GetIsTransferCall(long p_lCSTACallRefId)
{
	bool l_bIsTransferCall = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bIsTransferCall = iter->second->GetIsTransferCall();
		}
	}

	return l_bIsTransferCall;
}

void CCallManager::SetRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetRelatedCSTACallRefId(p_lRelatedCSTACallRefId);
		}
	}
}

long CCallManager::GetRelatedCSTACallRefId(long p_lCSTACallRefId)
{
	long p_lRelatedCSTACallRefId = 0;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			p_lRelatedCSTACallRefId = iter->second->GetRelatedCSTACallRefId();
		}
	}

	return p_lRelatedCSTACallRefId;
}

void CCallManager::SetCalledByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCalled)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetCalledParty(p_strCalled);
		}
	}
}

bool CCallManager::GetCalledByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCalled)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);

	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = true;
			p_strCalled = iter->second->GetCalledParty();
		}
	}

	return l_bReturn;
}

void CCallManager::SetConfCompere(long p_lCSTACallRefId, const std::string& p_strCompere)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetConfCompere(p_strCompere);
		}
	}
}
std::string CCallManager::GetConfCompere(long p_lCSTACallRefId)
{
	std::string l_strCompere = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strCompere = iter->second->GetConfCompere();
		}
	}

	return l_strCompere;
}

/*
void CCallManager::SetRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelateCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetRelatedCSTACallRefId(p_lRelateCSTACallRefId);
		}
	}
}*/
bool CCallManager::GetRelatedCSTACallRefId(long p_lInCSTACallRefId, long& p_lOutRelateCSTACallRefId)
{
	bool l_bFind = false;

	/*std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;
			p_lRelateCSTACallRefId = iter->second->GetRelatedCSTACallRefId();
		}
	}*/

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.second->FindRelatedCSTACallRefId(p_lInCSTACallRefId))
		{
			l_bFind = true;
			p_lOutRelateCSTACallRefId = l_pCallObj.first;

			break;
		}
	}

	return l_bFind;
}
/*void CCallManager::SetRelateCTICallRefId(long p_lCSTACallRefId, const std::string& p_strRelateCTICallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetRelateCTICallRefId(p_strRelateCTICallRefId);
		}
	}
}
bool CCallManager::GetRelateCTICallRefId(long p_lCSTACallRefId, std::string& p_strRelateCTICallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second && !(iter->second->GetRelateCTICallRefId().empty()))
		{
			l_bReturn = true;

			p_strRelateCTICallRefId = iter->second->GetRelateCTICallRefId();
		}
	}

	return l_bReturn;
}*/

std::string CCallManager::GetCallState(long p_lCSTACallRefId)
{
	std::string l_strCallState = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strCallState = iter->second->GetCallState();
		}
	}

	return l_strCallState;
}
std::string CCallManager::GetIncomingTime(long p_lCSTACallRefId)
{
	std::string l_strIncomingTime = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strIncomingTime = iter->second->GetIncomingTime();
		}
	}

	return l_strIncomingTime;
}
std::string CCallManager::GetDialTime(long p_lCSTACallRefId)
{
	std::string l_strDialTime = "";

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strDialTime = iter->second->GetDialTime();
		}
	}

	return l_strDialTime;
}

//////////////////////////////////////////////////////////////////////////
bool CCallManager::IsAlarmCall(long p_lCSTACallRefId)
{
	bool l_IsAlarmCall = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_IsAlarmCall = iter->second->GetIsAlarmCall();
		}
	}

	return l_IsAlarmCall;
}

bool CCallManager::IsBlackCall(long p_lCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = iter->second->GetIsBlackCall();
		}
	}

	return l_bReturn;
}

bool CCallManager::IsReleaseCall(long p_lCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			if (iter->second->GetHangupType().compare(HangupTypeString[HANGUP_TYPE_RELEASE]) == 0)
			{
				l_bReturn = true;
			}
		}
	}

	return l_bReturn;
}

std::string CCallManager::GetCallMode(long p_lCSTACallRefId)
{
	std::string l_strCallMode = CallModeString[CALL_MODE_UNKNOWN];

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_strCallMode = iter->second->GetCallMode();
		}
	}

	return l_strCallMode;
}

bool CCallManager::FindCallByCSTACallRefId(long p_lCSTACallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		l_bFind = true;
	}

	return l_bFind;
}
bool CCallManager::FindCallByCTICallRefId(const std::string& p_strCTICallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;

			break;
		}
	}

	return l_bFind;
}

bool CCallManager::GetCTICallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;

			p_strCTICallRefId = iter->second->GetCTICallRefId();
		}
	}

	return l_bFind;
}

bool CCallManager::GetDeviceNum(long p_lCSTACallRefId, std::string& p_strDeviceNum)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;

			p_strDeviceNum = iter->second->GetCalledParty();
		}
	}

	return l_bFind;
}

bool CCallManager::GetCSTACallRefId(const std::string& p_strCTICallRefId, long& p_lCSTACallRefId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0 || l_callObj.second->GetCTIOldCallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;
			p_lCSTACallRefId = l_callObj.first;

			break;
		}
	}

	if (!l_bFind)
	{
		ICC_LOG_DEBUG(m_pLog, "CCallManager::GetCSTACallRefId failed: [%s],size=%d", p_strCTICallRefId.c_str(), m_mapCallQueue.size());
		for each (auto l_pCallObj in m_mapCallQueue)
		{
			int i = 0;
			ICC_LOG_DEBUG(m_pLog, "GetCSTACallRefId:[%s],index =%d", l_pCallObj.second->GetCTICallRefId().c_str(), i);
			i++;

		}
	}
	return l_bFind;
}

bool CCallManager::GetCallMembers(long p_lCSTACallRefId, std::vector<std::string>& p_vCallMembers)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.first == p_lCSTACallRefId/* || l_pCallObj.second->GetRelatedCSTACallRefId() == p_lCSTACallRefId*/)
		{
			l_bFind = true;

			std::string l_strCallerId = l_pCallObj.second->GetCallerId();
			std::string l_strCalledId = l_pCallObj.second->GetCalledParty();
			p_vCallMembers.push_back(l_strCallerId);
			p_vCallMembers.push_back(l_strCalledId);

			if (l_pCallObj.second->GetConfMembersCount() > 0)
			{
				std::vector<std::string> l_vConfParty = l_pCallObj.second->GetConfMembersQueue();
				for each (auto l_strParty in l_vConfParty)
				{
					if (l_strParty.compare(l_strCallerId) != 0 && l_strParty.compare(l_strCalledId) != 0)
					{
						// 去掉重复成员
						p_vCallMembers.push_back(l_strParty);
					}
				}
			}
		}
	}

	return l_bFind;
}

bool CCallManager::GetOriginalCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledParty)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;

			p_strOriginalCallerId = l_pCallObj.second->GetOriginalCallerId();
			p_strOriginalCalledParty = l_pCallObj.second->GetOriginalCalledId();
			if (p_strOriginalCallerId.empty() && p_strOriginalCalledParty.empty())
			{
				p_strOriginalCallerId = l_pCallObj.second->GetCallerId();
				p_strOriginalCalledParty = l_pCallObj.second->GetCalledParty();
			}

			break;
		}
	}

	return l_bFind;
}
bool CCallManager::GetCallByCTICallRefId(const std::string& p_strInCTICallRefId, std::string& p_strOutCallerId, std::string& p_strOutCalledParty)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.second->GetCTICallRefId().compare(p_strInCTICallRefId) == 0)
		{
			l_bFind = true;

			p_strOutCallerId = l_pCallObj.second->GetCallerId();
			p_strOutCalledParty = l_pCallObj.second->GetCalledParty();

			break;
		}
	}

	return l_bFind;
}
bool CCallManager::GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_stCTICallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	/*for each (auto l_pCallObj in m_mapCallQueue)
	{
		if (l_pCallObj.first == p_lCSTACallRefId || l_pCallObj.second->GetRelatedCSTACallRefId() == p_lCSTACallRefId)
		{
			l_bReturn = true;
			p_stCTICallRefId = l_pCallObj.second->GetCTICallRefId();

			break;
		}
	}*/
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = true;
			p_stCTICallRefId = iter->second->GetCTICallRefId();
		}
	}

	return l_bReturn;
}
bool  CCallManager::GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty,
	std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;

			p_strCTICallRefId = iter->second->GetCTICallRefId();
			p_strCallerId = iter->second->GetCallerId();
			p_strCalledParty = iter->second->GetCalledParty();
			//ICC_LOG_DEBUG(m_pLog, "GetCalled, p_lCSTACallRefId: [%u], CalledId: [%s]", p_lCSTACallRefId, p_strCalledParty.c_str());
			p_strOriginalCallerId = iter->second->GetOriginalCallerId();
			p_strOriginalCalledId = iter->second->GetOriginalCalledId();
			p_strCallDirection = iter->second->GetCallDirection();
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetCallByCSTACallRefId == NULL,p_lCSTACallRefId=%d", p_lCSTACallRefId);
		}
	}
	if (!l_bFind)
	{
		for each (auto l_pCallObj in m_mapCallQueue)
		{
			ICC_LOG_ERROR(m_pLog, "GetCallByCSTACallRefId failed,key=%d,CTICallRefId=%s", l_pCallObj.first, l_pCallObj.second->GetCTICallRefId().c_str());
		}
	}

	return l_bFind;
}

void  CCallManager::GetCallList(ISwitchNotifPtr l_pSwitchNotif)
{
	IGetDeviceListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetDeviceListResultNotif>(l_pSwitchNotif);
	if (NULL == l_pResultNotif)
	{
		ICC_LOG_ERROR(m_pLog, "GetCallList l_pResultNotif == NULL");
		return;
	}

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pDeviceStateObj in l_pResultNotif->m_DeviceList)
	{
		if (l_pDeviceStateObj)
		{
			auto iter = m_mapCallQueue.find(l_pDeviceStateObj->GetCSTACallRefId());
			if (iter != m_mapCallQueue.end())
			{
				if (iter->second)
				{
					l_pDeviceStateObj->SetCTICallRefId(iter->second->GetCTICallRefId());
					l_pDeviceStateObj->SetCallerId(iter->second->GetCallerId());
					l_pDeviceStateObj->SetCalledId(iter->second->GetCalledParty());
					l_pDeviceStateObj->SetCallDirection(iter->second->GetCallDirection());
				}
			}
		}
	}

	return;
}

void CCallManager::UpdateCall(long p_lCSTACallRefId, std::vector<CProperty>& p_vProperties)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			for (std::size_t i = 0; i < p_vProperties.size(); ++i)
			{
				std::string l_strKey = p_vProperties[i].m_strKey;
				std::string l_strValue = p_vProperties[i].m_strValue;

				if (l_strKey.compare(Pr_CallState) == 0)
				{
					iter->second->SetCallState(l_strValue, m_pDateTime->CurrentDateTimeStr());
				}
				else if (l_strKey.compare(Pr_StateTime) == 0)
				{
					iter->second->SetStateTime(l_strValue);
				}
				else if (l_strKey.compare(Pr_CTICallRefId) == 0)
				{
					iter->second->SetCTICallRefId(l_strValue);
				}
				else if (l_strKey.compare(Pr_CallerId) == 0)
				{
					iter->second->SetCallerId(l_strValue);
				}
				else if (l_strKey.compare(Pr_CalledId) == 0)
				{
					iter->second->SetCalledParty(l_strValue);
//					ICC_LOG_DEBUG(m_pLog, "UpdateCall, p_lCSTACallRefId: [%u], CalledId: [%s]", p_lCSTACallRefId, l_strValue.c_str());
				}
				else if (l_strKey.compare(Pr_ACDGrp) == 0)
				{
					iter->second->SetCalledId(l_strValue);
				}
				else if (l_strKey.compare(Pr_CallDirection) == 0)
				{
					iter->second->SetCallDirection(l_strValue);
				}
				else if (l_strKey.compare(Pr_IsBlackCall) == 0)
				{
					iter->second->SetIsBlackCall(l_strValue.compare(Result_True) == 0 ? true : false);
				}
				else if (l_strKey.compare(Pr_IsAlarmCall) == 0)
				{
					iter->second->SetIsAlarmCall(l_strValue.compare(Result_True) == 0 ? true : false);
				}
				else if (l_strKey.compare(Pr_RelatedCSTACallRefId) == 0)
				{
					iter->second->AddRelatedCSTACallRefId(atol(l_strValue.c_str()));
				}
				else
				{
					//
				}
			}
		}
	}
}

bool CCallManager::CopyCall(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId)
{
	bool l_bFindSrc = false;

	
	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lSrcCSTACallRefId);
	int oldSize = m_mapCallQueue.size();
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFindSrc = true;
			std::string l_strTmpID;
			std::string l_strTmpID1;
			if (m_mapCallQueue[p_lDestCSTACallRefId])
			{
				l_strTmpID = m_mapCallQueue[p_lDestCSTACallRefId]->GetCTICallRefId();
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Not Find p_lDestCSTACallRefId:[new_%d],old_lSrcCSTACallRefId:[old_%d]", p_lDestCSTACallRefId, p_lSrcCSTACallRefId);
				if (l_strTmpID.empty())
				{
					l_strTmpID = iter->second->GetCTIOldCallRefId();
				}
			}
			l_strTmpID1 = iter->second->GetCTICallRefId();
			m_mapCallQueue[p_lDestCSTACallRefId] = iter->second;
			m_mapCallQueue[p_lDestCSTACallRefId]->SetCTIOldCallRefId(l_strTmpID);
			
			//DestCSTACallRefId: [175] , l_strTmpID : 20230314075747000174, new : 20230314075737000173, size : [1 - 2]
			//Copy Call, SrcCSTACallRefId: [150] , DestCSTACallRefId : [151] , l_strTmpID : 20230314053644000151, new : 20230314053634000150
			//Copy Call, SrcCSTACallRefId : [151] , DestCSTACallRefId : [152] , l_strTmpID : , new : 20230314053634000150

			m_mapCallQueue[p_lDestCSTACallRefId]->SetCSTACallRefId(p_lDestCSTACallRefId);
			//Copy Call, SrcCSTACallRefId: [148] , DestCSTACallRefId : [149] , l_strTmpID : , new : 20230314025624000147
			int newSize = m_mapCallQueue.size();
			ICC_LOG_DEBUG(m_pLog, "Copy Call, SrcCSTACallRefId: [%u], DestCSTACallRefId: [%u],l_strTmpID:%s,new:%s,size:[%d-%d]", p_lSrcCSTACallRefId, p_lDestCSTACallRefId,
				l_strTmpID.c_str(), l_strTmpID1.c_str(), oldSize, newSize);
		}
	}

	return l_bFindSrc;
}

void CCallManager::ResetHangupAttribute(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->ResetHangupAttribute();
		}
	}
}
void CCallManager::SetHangupState(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strHangupType)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			if (iter->second->GetHangupDevice().empty())
			{
				if (p_strHangupType.compare(CallStateString[STATE_RELEASE]) == 0)
				{
					//	早释
					iter->second->SetCallState(CallStateString[STATE_RELEASE], m_pDateTime->CurrentDateTimeStr());
				}

				iter->second->SetHangupType(p_strHangupType);
				iter->second->SetHangupDevice(p_strHangupDevice);
				iter->second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTime->CurrentDateTimeStr());
			}
		}
	}
}
void CCallManager::ProcessDeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			if (iter->second->GetHangupDevice().empty())
			{
				bool l_bUpdateHangupType = true;

				if (iter->second->GetIsBlackCall())
				{
					//	黑名单拒接
					iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_BLACKCALL]);
				}
				else if ((iter->second->GetCallState().compare(CallStateString[STATE_INCOMING]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_QUEUE]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_ASSIGN]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_RING]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_SIGNALBACK]) == 0) &&
					iter->second->GetCallDirection().compare(CallDirectionString[CALL_DIRECTION_IN]) == 0)
				{
					//	早释
					iter->second->SetCallState(CallStateString[STATE_RELEASE], m_pDateTime->CurrentDateTimeStr());
					iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_RELEASE]);
				}
				else
				{
					if (p_strHangupDevice.compare(iter->second->GetCallerId()) == 0)
					{
						//	主叫挂机
						iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLER]);
					}
					else if (p_strHangupDevice.compare(iter->second->GetCalledParty()) == 0)
					{
						//	被叫挂机
						iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLED]);
					}
					else
					{
						//	会议成员挂机
						l_bUpdateHangupType = false;
					}
				}

				if (l_bUpdateHangupType)
				{
					iter->second->SetHangupDevice(p_strHangupDevice);
					iter->second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTime->CurrentDateTimeStr());
				}
			}
			else
			{
				iter->second->SetHangupTime(m_pDateTime->CurrentDateTimeStr());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool CCallManager::AddRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = iter->second->AddRelatedCSTACallRefId(p_lRelatedCSTACallRefId);

			ICC_LOG_DEBUG(m_pLog, "Add Related CSTACallRefId, CSTACallRefId: [%u], RelatedCSTACallRefId: [%u]",
				p_lCSTACallRefId, p_lRelatedCSTACallRefId);
		}
	}

	return l_bReturn;
}
bool CCallManager::DeleteRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = iter->second->DeleteRelatedCSTACallRefId(p_lRelatedCSTACallRefId);

			ICC_LOG_DEBUG(m_pLog, "Delete Related CSTACallRefId, CSTACallRefId: [%u], RelatedCSTACallRefId: [%u]",
				p_lCSTACallRefId, p_lRelatedCSTACallRefId);
		}
	}

	return l_bReturn;
}
bool CCallManager::FindRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		l_bReturn = true;
	}

	return l_bReturn;
}

bool CCallManager::AddConferenceParty(long p_lCSTACallRefId, const std::string& p_strJoinPhone)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetCallMode(CallModeString[CALL_MODE_CONFERENCE]);
			l_bReturn = iter->second->AddConfTalkParty(p_strJoinPhone);

			ICC_LOG_DEBUG(m_pLog, "Add Conference Member, CSTACallRefId: [%u], Party: [%s]",
				p_lCSTACallRefId, p_strJoinPhone.c_str());
		}
	}

	return l_bReturn;
}
bool CCallManager::DeleteConferenceParty(long p_lCSTACallRefId, const std::string& p_strHangupPhone)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = iter->second->DeleteConfTalkParty(p_strHangupPhone);

			ICC_LOG_DEBUG(m_pLog, "Delete Conference Member, CSTACallRefId: [%u], Party: [%s]",
				p_lCSTACallRefId, p_strHangupPhone.c_str());
		}
	}

	return l_bReturn;
}
int	CCallManager::GetConfMemCount(long p_lCSTACallRefId)
{
	int l_nCount = 0;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_nCount = iter->second->GetConfMembersCount();
		}
	}

	return l_nCount;
}

//////////////////////////////////////////////////////////////////////////
void CCallManager::PostRefuseCallNotif(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			IRefuseCallNotifPtr l_pRefuseCallRequestNotif = boost::make_shared<CRefuseCallNotif>();
			if (l_pRefuseCallRequestNotif)
			{
				l_pRefuseCallRequestNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pRefuseCallRequestNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pRefuseCallRequestNotif->SetCallerId(iter->second->GetCallerId());
				l_pRefuseCallRequestNotif->SetCalledId(iter->second->GetCalledId());
				l_pRefuseCallRequestNotif->SetRefuseReason("BlackList");
				l_pRefuseCallRequestNotif->SetStateTime(m_pDateTime->CurrentDateTimeStr()/*std::string(l_szCurrentTime)*/);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_RefuseCallEvent, l_pRefuseCallRequestNotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask RefuseCallEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create CRefuseCallNotif Object Failed !!!");
			}
		}
	}
}
void CCallManager::PostReleaseCall(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			ICallStateNotifPtr l_pCallStateNotif = boost::make_shared<CCallStateNotif>();
			if (l_pCallStateNotif)
			{
				l_pCallStateNotif->SetAgentId(iter->second->GetCalledParty());
				l_pCallStateNotif->SetACDGrp(iter->second->GetCalledId());
				l_pCallStateNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pCallStateNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pCallStateNotif->SetCallerId(iter->second->GetCallerId());
				l_pCallStateNotif->SetCalledId(iter->second->GetCalledParty());
				l_pCallStateNotif->SetCallState(CallStateString[STATE_RELEASE]);
				l_pCallStateNotif->SetStateTime(iter->second->GetStateTime());

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallStateEvent, l_pCallStateNotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask CallStateEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create ICallStateNotif Object Failed !!!");
			}
		}
	}
}
void CCallManager::PostCallOverNotif(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second && !iter->second->GetIsPostCallOver())
		{
			iter->second->SetIsPostCallOver(true);

			ICallOverNotifPtr l_pCallOverNotif = boost::make_shared<CCallOverNotif>();
			if (l_pCallOverNotif)
			{
				std::string l_strCallerId = iter->second->GetCallerId();
				std::string l_strCalledId = iter->second->GetCalledParty();
				std::string l_strOriginalCallerId = iter->second->GetOriginalCallerId();
				std::string l_strOriginalCalledId = iter->second->GetOriginalCalledId();
				if (!l_strOriginalCallerId.empty() && !l_strOriginalCalledId.empty())
				{
					if (iter->second->GetCallMode().compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
					{
						//	会议，修改原始主被叫
						l_strCallerId = l_strOriginalCallerId;
						l_strCalledId = l_strOriginalCalledId;
					}
					else if (iter->second->GetIsTransferCall())
					{
						//	转移，修改原始主叫
						l_strCallerId = l_strOriginalCallerId;
					}
					else
					{
						//
					}
				}

				l_pCallOverNotif->SetAgentId(l_strCalledId);
				l_pCallOverNotif->SetACDGrp(iter->second->GetCalledId());
				l_pCallOverNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pCallOverNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pCallOverNotif->SetIsBlackCall(iter->second->GetIsBlackCall());
				l_pCallOverNotif->SetCallerId(l_strCallerId);
				l_pCallOverNotif->SetCalledId(l_strCalledId);
				l_pCallOverNotif->SetCallState(iter->second->GetCallState());
				l_pCallOverNotif->SetCallDirection(iter->second->GetCallDirection());
				l_pCallOverNotif->SetDialTime(iter->second->GetDialTime());
				l_pCallOverNotif->SetIncomingTime(iter->second->GetIncomingTime());
				l_pCallOverNotif->SetAssignTime(iter->second->GetAssignTime());
				l_pCallOverNotif->SetRingTime(iter->second->GetRingTime());
				l_pCallOverNotif->SetSignalBackTime(iter->second->GetSignalBackTime());
				l_pCallOverNotif->SetReleaseTime(iter->second->GetReleaseTime());
				l_pCallOverNotif->SetTalkTime(iter->second->GetTalkTime());
				l_pCallOverNotif->SetHangupTime(iter->second->GetHangupTime());
				l_pCallOverNotif->SetHangupType(iter->second->GetHangupType());
				l_pCallOverNotif->SetSwitchType(SWITCH_TYPE_TAPI);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallOverEvent, l_pCallOverNotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask CallOverEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create CallOverNotif Object Failed !!!");
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Create CallOverNotif Object Failed !!!,flag=%d", iter->second->GetIsPostCallOver());
		}
	}
}

void CCallManager::PostCallOverNotif(long p_lCSTACallRefId, const std::string& p_strDeviceNum)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if ((p_strDeviceNum != iter->second->GetCalledParty())&&(p_strDeviceNum != iter->second->GetOriginalCalledId()))
		{
			ICC_LOG_ERROR(m_pLog, "PostCallOverNotif no match CallRefId=[%u], DeviceNum=[%s], Called=[%s]", p_lCSTACallRefId, p_strDeviceNum.c_str(), iter->second->GetCalledParty().c_str());

			std::string strCaller = iter->second->GetCallerId();
			if (strCaller.size() > p_strDeviceNum.size())
			{
				if (0 != strCaller.compare(strCaller.size() - p_strDeviceNum.size(), p_strDeviceNum.size(), p_strDeviceNum))
				{
					ICC_LOG_ERROR(m_pLog, "PostCallOverNotif no match CallRefId=[%u], DeviceNum=[%s], Caller=[%s]", p_lCSTACallRefId, p_strDeviceNum.c_str(), iter->second->GetCallerId().c_str());
					return;
				}
			}
			else
			{
				if (0 != strCaller.compare(p_strDeviceNum))
				{
					ICC_LOG_ERROR(m_pLog, "PostCallOverNotif no match CallRefId=[%u], DeviceNum=[%s], Caller=[%s]", p_lCSTACallRefId, p_strDeviceNum.c_str(), iter->second->GetCallerId().c_str());
					return;
				}
			}
		}

		if (iter->second && !iter->second->GetIsPostCallOver())
		{
			iter->second->SetIsPostCallOver(true);

			ICallOverNotifPtr l_pCallOverNotif = boost::make_shared<CCallOverNotif>();
			if (l_pCallOverNotif)
			{
				std::string l_strCallerId = iter->second->GetCallerId();
				std::string l_strCalledId = iter->second->GetCalledParty();
				std::string l_strOriginalCallerId = iter->second->GetOriginalCallerId();
				std::string l_strOriginalCalledId = iter->second->GetOriginalCalledId();
				if (!l_strOriginalCallerId.empty() && !l_strOriginalCalledId.empty())
				{
					if (iter->second->GetCallMode().compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
					{
						//	会议，修改原始主被叫
						l_strCallerId = l_strOriginalCallerId;
						l_strCalledId = l_strOriginalCalledId;
					}
					else if (iter->second->GetIsTransferCall())
					{
						//	转移，修改原始主叫
						l_strCallerId = l_strOriginalCallerId;
					}
					else
					{
						//
					}
				}

				l_pCallOverNotif->SetAgentId(l_strCalledId);
				l_pCallOverNotif->SetACDGrp(iter->second->GetCalledId());
				l_pCallOverNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pCallOverNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pCallOverNotif->SetIsBlackCall(iter->second->GetIsBlackCall());
				l_pCallOverNotif->SetCallerId(l_strCallerId);
				l_pCallOverNotif->SetCalledId(l_strCalledId);
				l_pCallOverNotif->SetCallState(iter->second->GetCallState());
				l_pCallOverNotif->SetCallDirection(iter->second->GetCallDirection());
				l_pCallOverNotif->SetDialTime(iter->second->GetDialTime());
				l_pCallOverNotif->SetIncomingTime(iter->second->GetIncomingTime());
				l_pCallOverNotif->SetAssignTime(iter->second->GetAssignTime());
				l_pCallOverNotif->SetRingTime(iter->second->GetRingTime());
				l_pCallOverNotif->SetSignalBackTime(iter->second->GetSignalBackTime());
				l_pCallOverNotif->SetReleaseTime(iter->second->GetReleaseTime());
				l_pCallOverNotif->SetTalkTime(iter->second->GetTalkTime());
				l_pCallOverNotif->SetHangupTime(iter->second->GetHangupTime());
				l_pCallOverNotif->SetHangupType(iter->second->GetHangupType());
				l_pCallOverNotif->SetSwitchType(SWITCH_TYPE_TAPI);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallOverEvent, l_pCallOverNotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask CallOverEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create CallOverNotif Object Failed !!!");
			}
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "PostCallOverNotif m_mapCallQueue Cannot Find CallRefId=[%u]", p_lCSTACallRefId);
	}
}

void CCallManager::GetCallOverNotif(long p_lCSTACallRefId, ICallOverNotifPtr& p_pCallOverNotif)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second && !iter->second->GetIsPostCallOver())
		{
			iter->second->SetIsPostCallOver(true);

			if (p_pCallOverNotif)
			{
				std::string l_strCallerId = iter->second->GetCallerId();
				std::string l_strCalledId = iter->second->GetCalledParty();
				std::string l_strOriginalCallerId = iter->second->GetOriginalCallerId();
				std::string l_strOriginalCalledId = iter->second->GetOriginalCalledId();
				if (!l_strOriginalCallerId.empty() && !l_strOriginalCalledId.empty())
				{
					if (iter->second->GetCallMode().compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
					{
						//	会议，修改原始主被叫
						l_strCallerId = l_strOriginalCallerId;
						l_strCalledId = l_strOriginalCalledId;
					}
					else if (iter->second->GetIsTransferCall())
					{
						//	转移，修改原始主叫
						l_strCallerId = l_strOriginalCallerId;
					}
					else
					{
						//
					}
				}

				p_pCallOverNotif->SetAgentId(l_strCalledId);
				p_pCallOverNotif->SetACDGrp(iter->second->GetCalledId());
				p_pCallOverNotif->SetCSTACallRefId(p_lCSTACallRefId);
				p_pCallOverNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				p_pCallOverNotif->SetIsBlackCall(iter->second->GetIsBlackCall());
				p_pCallOverNotif->SetCallerId(l_strCallerId);
				p_pCallOverNotif->SetCalledId(l_strCalledId);
				p_pCallOverNotif->SetCallState(iter->second->GetCallState());
				p_pCallOverNotif->SetCallDirection(iter->second->GetCallDirection());
				p_pCallOverNotif->SetDialTime(iter->second->GetDialTime());
				p_pCallOverNotif->SetIncomingTime(iter->second->GetIncomingTime());
				p_pCallOverNotif->SetAssignTime(iter->second->GetAssignTime());
				p_pCallOverNotif->SetRingTime(iter->second->GetRingTime());
				p_pCallOverNotif->SetSignalBackTime(iter->second->GetSignalBackTime());
				p_pCallOverNotif->SetReleaseTime(iter->second->GetReleaseTime());
				p_pCallOverNotif->SetTalkTime(iter->second->GetTalkTime());
				p_pCallOverNotif->SetHangupTime(iter->second->GetHangupTime());
				p_pCallOverNotif->SetHangupType(iter->second->GetHangupType());
			}
		}
	}
}

void CCallManager::GetCallStateNotif(long p_lCSTACallRefId, ICallStateNotifPtr& p_pCallStateNotif)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			if (p_pCallStateNotif)
			{
				p_pCallStateNotif->SetAgentId(iter->second->GetCalledParty());
				p_pCallStateNotif->SetACDGrp(iter->second->GetCalledId());
				p_pCallStateNotif->SetCSTACallRefId(p_lCSTACallRefId);
				p_pCallStateNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				p_pCallStateNotif->SetCallerId(iter->second->GetCallerId());
				p_pCallStateNotif->SetCalledId(iter->second->GetCalledParty());
				p_pCallStateNotif->SetCallState(iter->second->GetCallState());
				p_pCallStateNotif->SetStateTime(iter->second->GetStateTime());			
			}
		}
	}
}

void CCallManager::PostCallState(long p_lCSTACallRefId)
{
	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			ICallStateNotifPtr l_pCallStateNotif = boost::make_shared<CCallStateNotif>();
			if (l_pCallStateNotif)
			{
				l_pCallStateNotif->SetAgentId(iter->second->GetCalledParty());
				l_pCallStateNotif->SetACDGrp(iter->second->GetCalledId());
				l_pCallStateNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pCallStateNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pCallStateNotif->SetCallerId(iter->second->GetCallerId());
				l_pCallStateNotif->SetCalledId(iter->second->GetCalledParty());
				l_pCallStateNotif->SetCallState(iter->second->GetCallState());
				l_pCallStateNotif->SetStateTime(iter->second->GetStateTime());

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallStateEvent, l_pCallStateNotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask CallStateEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create ICallStateNotif Object Failed !!!");
			}
		}
	}
}

void CCallManager::GetCallList(long p_lRequestId)
{
	IGetCallListResultNotifPtr l_pRespondNotif = boost::make_shared<CGetCallListResultNotif>();
	if (l_pRespondNotif)
	{
		l_pRespondNotif->SetRequestId(p_lRequestId);

		{
			std::lock_guard<std::mutex> guard(m_callQueueMutex);
			for each (auto l_pCallObj in m_mapCallQueue)
			{
				if (l_pCallObj.second->GetCallState().compare(CallStateString[STATE_HANGUP]) != 0)
				{
					CGetCallListResultNotif::CCallObj callObj;
					callObj.m_lCSTACallRefId = l_pCallObj.second->GetCSTACallRefId();
					callObj.m_strCTICallRefId = l_pCallObj.second->GetCTICallRefId();
					callObj.m_strCallerId = l_pCallObj.second->GetCallerId();
					callObj.m_strCalledId = l_pCallObj.second->GetCalledId();
					callObj.m_strCalledParty = l_pCallObj.second->GetCalledParty();
					callObj.m_strCallDierection = l_pCallObj.second->GetCallDirection();
					callObj.m_strCallState = l_pCallObj.second->GetCallState();
					callObj.m_strStateTime = l_pCallObj.second->GetStateTime();

					l_pRespondNotif->m_CallList.push_back(callObj);
				}
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetCallListRet, l_pRespondNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetCallListRet, RequestId: [%u], TaskId: [%u]",
			p_lRequestId, l_lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create GetCallListResultNotif Object Failed !!!");
	}
}

//////////////////////////////////////////////////////////////////////////
bool CCallManager::FindTimeoutCall(long &p_lCSTACallRefId)
{
	bool l_bFind = false;

	std::lock_guard<std::mutex> guard(m_callQueueMutex);
	for each (auto l_pCallObj in m_mapCallQueue)
	{
		std::string l_strCallState = l_pCallObj.second->GetCallState();
		std::string l_strCallTime = l_pCallObj.second->GetStateTime();
		int l_nCallInvalidTime = CSysConfig::Instance()->GetCallInvalidTime();

		DateTime::CDateTime l_oCurrentTime = m_pDateTime->CurrentDateTime();
		DateTime::CDateTime l_oCallOverEndTime = m_pDateTime->AddMinutes(m_pDateTime->FromString(l_strCallTime), CALL_OVER_TIMESPAN);
		DateTime::CDateTime l_oTimeoutEndTime = m_pDateTime->AddHours(m_pDateTime->FromString(l_strCallTime), l_nCallInvalidTime);

		if (l_strCallState.compare(CallStateString[STATE_HANGUP]) == 0 && l_oCurrentTime > l_oCallOverEndTime)
		{
			//	话务结束 2 分钟后，删除内存中的话务
			l_bFind = true;

			p_lCSTACallRefId = l_pCallObj.first;
			std::string l_strCTICallRefId = l_pCallObj.second->GetCTICallRefId();

			ICC_LOG_DEBUG(m_pLog, "Call Over , CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				p_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());

			break;
		}
		else if (l_oCurrentTime > l_oTimeoutEndTime)
		{
			//	话务超过 24 小时，删除内存中的话务
			l_bFind = true;

			l_pCallObj.second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLED]);
			l_pCallObj.second->SetHangupDevice(l_pCallObj.second->GetCalledParty());
			l_pCallObj.second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTime->CurrentDateTimeStr());

			p_lCSTACallRefId = l_pCallObj.first;
			std::string l_strCTICallRefId = l_pCallObj.second->GetCTICallRefId();

			ICC_LOG_WARNING(m_pLog, "Call Timeout , CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				p_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());

			break;
		}
		else
		{
			//
		}
	}

	return l_bFind;
}
void CCallManager::ProcessTimeoutCall(long p_lCSTACallRefId)
{
	CDeviceManager::Instance()->DeleteAllLogicalCallState(p_lCSTACallRefId);

	//this->PostCallOverNotif(p_lCSTACallRefId);
	this->DeleteCall(p_lCSTACallRefId);
}
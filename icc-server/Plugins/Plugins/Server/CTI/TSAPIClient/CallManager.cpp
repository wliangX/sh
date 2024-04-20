#include "Boost.h"

#include "CallManager.h"
#include "Task.h"
#include "TaskManager.h"
#include "DeviceManager.h"
#include "SysConfig.h"

//#define MAX_CALLQUEUE_COUNT		2048
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
	this->ClearAll();

	if (m_pManagerInstance)
	{
		m_pManagerInstance.reset();
	}
}

CCallManager::CCallManager(void)
{
	m_pLogPtr = nullptr;
	m_pStrUtil = nullptr;
	m_pDateTimePtr = nullptr;
	m_pCheckCallThread = nullptr;

	m_bCheckThreadAlive = false;
	m_oLastCheckTime = 0;
}

CCallManager::~CCallManager(void)
{
	//ClearAll();
}

void CCallManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLogPtr = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();

	m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();
}
void CCallManager::OnStart()
{
	this->StartCheckThread();
}
void CCallManager::OnStop()
{
	this->StopCheckThread();

	ClearAll();
}
//////////////////////////////////////////////////////////////////////////
std::string CCallManager::CreateCTICallRefId(long p_lCSTACallRefId)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));

#if defined(WIN32) || defined(WIN64)
#ifdef _DEBUG
	std::string l_strCallRefId = m_pStrUtil->Format("110%u", p_lCSTACallRefId);
#else
	std::string l_strCallRefId = m_pStrUtil->Format("%s%06u", l_szDayTime, p_lCSTACallRefId);
#endif // _DEBUG
#else
	std::string l_strCallRefId = m_pStrUtil->Format("%s%06u", l_szDayTime, p_lCSTACallRefId);
#endif

	return l_strCallRefId;
}

std::string CCallManager::CreateNewCall(long p_lCSTACallRefId)
{
	std::string l_strCTICallRefId = this->CreateCTICallRefId(p_lCSTACallRefId);

	boost::shared_ptr<CCall> l_pCall = boost::make_shared<CCall>(p_lCSTACallRefId);
	l_pCall->SetCTICallRefId(l_strCTICallRefId);
	l_pCall->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());
	this->AddCall(l_pCall);

	ICC_LOG_DEBUG(m_pLogPtr, "Create New Call, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
		p_lCSTACallRefId, l_strCTICallRefId.c_str(), this->GetCallQueueSize());

	return l_strCTICallRefId;
}
bool CCallManager::AddCall(boost::shared_ptr<CCall> p_pCall)
{
	RemoveCallQueueHead();

	SAFE_LOCK(m_callQueueMutex);
	m_mapCallQueue[p_pCall->GetCSTACallRefId()] = p_pCall;

	return true;
}
bool CCallManager::RemoveCallQueueHead()
{
	if (GetCallQueueSize() < MAX_CALLQUEUE_COUNT)
	{
		return false;
	}

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.begin();
	if (iter != m_mapCallQueue.end())
	{
		if (iter->first && iter->second /*&& it->second->GetCallState().compare(CallStateString[STATE_FREE]) == 0*/)
		{
			long l_lCSTACallRefId = iter->first;
			std::string l_strCTICallRefId = iter->second->GetCTICallRefId();

			m_mapCallQueue.erase(iter);

			ICC_LOG_DEBUG(m_pLogPtr, "Remove Call Queue Head, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				l_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());
		}
	}

	return true;
}

bool CCallManager::DeleteCall(long p_lCSTACallRefId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = true;
			std::string l_strCTICallRefId = iter->second->GetCTICallRefId();
			
			m_mapCallQueue.erase(iter);

			ICC_LOG_DEBUG(m_pLogPtr, "Delete Call, CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				p_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());
		}
	}

	return l_bReturn;
}

void CCallManager::ClearAll()
{
	SAFE_LOCK(m_callQueueMutex);
	m_mapCallQueue.clear();

	ICC_LOG_DEBUG(m_pLogPtr, "Clear Call Queue !!!");
}

int CCallManager::GetCallQueueSize()
{
	int l_nCount = 0;

	SAFE_LOCK(m_callQueueMutex);
	l_nCount = m_mapCallQueue.size();

	return l_nCount;
}

void CCallManager::AddMemberDevice(long lcstaCallId, const std::string& strDevice)
{
	ICC_LOG_DEBUG(m_pLogPtr, "add member device enter !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(lcstaCallId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->AddMemberDevice(strDevice);
			ICC_LOG_DEBUG(m_pLogPtr, "add member device success !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());
		}
	}

	ICC_LOG_DEBUG(m_pLogPtr, "add member device not find call !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());
}

void CCallManager::DeleteMemberDevice(long lcstaCallId, const std::string& strDevice)
{
	ICC_LOG_DEBUG(m_pLogPtr, "delete member device enter !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(lcstaCallId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->DeleteDevice(strDevice);
			ICC_LOG_DEBUG(m_pLogPtr, "delete member device success !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());
			return;
		}
	}

	ICC_LOG_DEBUG(m_pLogPtr, "delete member device not find call !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());
}

bool CCallManager::ExistMemberDevice(long lcstaCallId, const std::string& strDevice)
{
	ICC_LOG_DEBUG(m_pLogPtr, "exist member device enter !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(lcstaCallId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			return iter->second->ExistDevice(strDevice);
		}
	}

	ICC_LOG_DEBUG(m_pLogPtr, "exist member device complete, not find call !!!, call:[%ld], device:[%s]", lcstaCallId, strDevice.c_str());

	return false;
}

int CCallManager::MemberDeviceCount(long lcstaCallId)
{
	ICC_LOG_DEBUG(m_pLogPtr, "member device count enter !!!, call:[%ld]", lcstaCallId);

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(lcstaCallId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			int iCount = iter->second->MemberDeviceCount();
			ICC_LOG_DEBUG(m_pLogPtr, "member device count success !!!, call:[%ld], count:[%d]", lcstaCallId,iCount);
			return iCount;
		}
	}

	ICC_LOG_DEBUG(m_pLogPtr, "member device count not find call !!!, call:[%ld]", lcstaCallId);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CCallManager::SetOriginalCallerId(long p_lCSTACallRefId, const std::string& p_strOriginalCallerId)
{
	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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
	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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
	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

void CCallManager::SetConfCompere(long p_lCSTACallRefId, const std::string& p_strCompere)
{
	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

void CCallManager::SetRelateCSTACallRefId(const std::string& p_strCTICallRefId, long p_lRelateCSTACallRefId)
{
	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_callObj.second->SetRelateCSTACallRefId(p_lRelateCSTACallRefId);

			break;
		}
	}
}
long CCallManager::GetRelateCSTACallRefId(const std::string& p_strCTICallRefId)
{
	long l_lCSTACallRefId = DEFAULT_CALLREFID;

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_lCSTACallRefId = l_callObj.second->GetRelateCSTACallRefId();

			break;
		}
	}

	return l_lCSTACallRefId;
}
void CCallManager::SetRelateCTICallRefId(long p_lCSTACallRefId, const std::string& p_strRelateCTICallRefId)
{
	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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
}

//////////////////////////////////////////////////////////////////////////
bool CCallManager::IsAlarmCall(long p_lCSTACallRefId)
{
	bool l_IsAlarmCall = false;

	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
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

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
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

	SAFE_LOCK(m_callQueueMutex);
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
		ICC_LOG_ERROR(m_pLogPtr, "GetCSTACallRefId error, CTICallRefId:%s,CSTACallRefId:%d", p_strCTICallRefId.c_str(), p_lCSTACallRefId);
	}
	return l_bFind;
}


bool CCallManager::GetOriginalCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledParty)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;

			p_strOriginalCallerId = l_callObj.second->GetOriginalCallerId();
			p_strOriginalCalledParty = l_callObj.second->GetOriginalCalledId();
			if (p_strOriginalCallerId.empty() && p_strOriginalCalledParty.empty())
			{
				p_strOriginalCallerId = l_callObj.second->GetCallerId();
				p_strOriginalCalledParty = l_callObj.second->GetCalledParty();
			}

			break;
		}
	}

	return l_bFind;
}
bool CCallManager::GetCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;

			p_strCallerId = l_callObj.second->GetCallerId();
			p_strCalledParty = l_callObj.second->GetCalledParty();

			break;
		}
	}

	return l_bFind;
}

bool CCallManager::GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime)
{
	bool l_bFind = false;
	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		if (l_callObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bFind = true;
			p_strRingTime = l_callObj.second->GetRingTime();
			p_strTalkTime = l_callObj.second->GetTalkTime();

			break;
		}
	}
	return l_bFind;
}

bool CCallManager::GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_stCTICallRefId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_callQueueMutex);
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
	std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection, std::string& p_strTalkTime)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;

			p_strCTICallRefId = iter->second->GetCTICallRefId();
			p_strCallerId = iter->second->GetCallerId();
			p_strCalledParty = iter->second->GetCalledParty();
			p_strOriginalCallerId = iter->second->GetOriginalCallerId();
			p_strOriginalCalledId = iter->second->GetOriginalCalledId();
			p_strCallDirection = iter->second->GetCallDirection();
			p_strTalkTime = iter->second->GetTalkTime();
		}
	}

	return l_bFind;
}

bool  CCallManager::GetCallByCSTACallRefIdEx(long p_lCSTACallRefId, std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty,
	std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection, std::string& p_strAcdGrp,std::string &p_strTalkTime)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bFind = true;

			p_strCTICallRefId = iter->second->GetCTICallRefId();
			p_strCallerId = iter->second->GetCallerId();
			p_strCalledParty = iter->second->GetCalledParty();
			p_strOriginalCallerId = iter->second->GetOriginalCallerId();
			p_strOriginalCalledId = iter->second->GetOriginalCalledId();
			p_strCallDirection = iter->second->GetCallDirection();
			p_strTalkTime = iter->second->GetTalkTime();
			p_strAcdGrp = iter->second->GetCalledId();
		}
	}

	return l_bFind;
}

void CCallManager::UpdateCall(long p_lCSTACallRefId, std::vector<CProperty>& p_vProperties)
{
	SAFE_LOCK(m_callQueueMutex);
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
					iter->second->SetCallState(l_strValue, m_pDateTimePtr->CurrentDateTimeStr());
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
				}
				else if (l_strKey.compare(Pr_OriginalCalledId) == 0)
				{
					iter->second->SetOriginalCalledId(l_strValue);
				}
				else if (l_strKey.compare(Pr_OriginalCallerId) == 0)
				{
					iter->second->SetOriginalCallerId(l_strValue);
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
				else
				{
					//
				}
			}
		}
	}
}
bool CCallManager::CopyCall(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId,const std::string &p_strCallState)
{
	bool l_bFindSrc = false;

	std::map<std::string, std::string> l_mapRelayIndexNumber;
	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lSrcCSTACallRefId);
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
				l_mapRelayIndexNumber = m_mapCallQueue[p_lDestCSTACallRefId]->m_mapRelayIndexNumber;
			}
			l_strTmpID1 = iter->second->GetCTICallRefId();
			m_mapCallQueue[p_lDestCSTACallRefId] = iter->second;
			m_mapCallQueue[p_lDestCSTACallRefId]->SetCTIOldCallRefId(l_strTmpID);

			m_mapCallQueue[p_lDestCSTACallRefId]->SetCSTACallRefId(p_lDestCSTACallRefId);

			m_mapCallQueue[p_lDestCSTACallRefId]->SetCallState(p_strCallState, m_pDateTimePtr->CurrentDateTimeStr());
			m_mapCallQueue[p_lDestCSTACallRefId]->AddE1Relay(l_mapRelayIndexNumber);
			//m_mapCallQueue[p_lDestCSTACallRefId]->m_mapRelayIndexNumber = l_mapRelayIndexNumber;
			//m_mapCallQueue[p_lDestCSTACallRefId]->SetCallState();
			ICC_LOG_DEBUG(m_pLogPtr, "Copy Call, SrcCSTACallRefId: [%u], DestCSTACallRefId: [%u],l_strTmpID:%s,new:%s", p_lSrcCSTACallRefId, p_lDestCSTACallRefId,
				l_strTmpID.c_str(), l_strTmpID1.c_str());
		}
	}

	return l_bFindSrc;
}

void CCallManager::SetHangupState(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strHangupType)
{
	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			if (iter->second->GetHangupDevice().empty())
			{
				iter->second->SetHangupType(p_strHangupType);
				iter->second->SetHangupDevice(p_strHangupDevice);
				iter->second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTimePtr->CurrentDateTimeStr());
			}
		}
	}
}
void CCallManager::ProcessDeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice)
{
	SAFE_LOCK(m_callQueueMutex);
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
				else if (iter->second->GetIsAlarmCall() &&
					(iter->second->GetCallState().compare(CallStateString[STATE_INCOMING]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_QUEUE]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_ASSIGN]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_RING]) == 0 ||
					iter->second->GetCallState().compare(CallStateString[STATE_SIGNALBACK]) == 0))
				{
					//	早释
					iter->second->SetCallState(CallStateString[STATE_RELEASE], m_pDateTimePtr->CurrentDateTimeStr());
					iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_RELEASE]);
				}
				else
				{
					if (p_strHangupDevice.compare(iter->second->GetCallerId()) == 0)
					{
						//	主叫挂机
						iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLER]);
						ICC_LOG_DEBUG(m_pLogPtr, "SetHangupType: HANGUP_TYPE_CALLER");
					}
					else if (p_strHangupDevice.compare(iter->second->GetCalledParty()) == 0)
					{
						//	被叫挂机
						iter->second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLED]);
						ICC_LOG_DEBUG(m_pLogPtr, "SetHangupType: HANGUP_TYPE_CALLED");
					}
					else
					{
						//	会议成员挂机
						l_bUpdateHangupType = false;
					}
				}

				if (l_bUpdateHangupType)
				{
					ICC_LOG_DEBUG(m_pLogPtr, "SetHangupDevice: [%s]", p_strHangupDevice.c_str());
					iter->second->SetHangupDevice(p_strHangupDevice);
					iter->second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTimePtr->CurrentDateTimeStr());
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool CCallManager::AddConferenceParty(long p_lCSTACallRefId, const std::string& p_strJoinPhone)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			iter->second->SetCallMode(CallModeString[CALL_MODE_CONFERENCE]);
			l_bReturn = iter->second->AddConfTalkParty(p_strJoinPhone);
			ICC_LOG_DEBUG(m_pLogPtr, "Add Conference Member, CSTACallRefId: [%u], Party: [%s]",
				p_lCSTACallRefId, p_strJoinPhone.c_str());
		}
	}

	return l_bReturn;
}
bool CCallManager::DeleteConferenceParty(long p_lCSTACallRefId, const std::string& p_strHangupPhone)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			l_bReturn = iter->second->DeleteConfTalkParty(p_strHangupPhone);
			ICC_LOG_DEBUG(m_pLogPtr, "Delete Conference Member, CSTACallRefId: [%u], Party: [%s],DeleteConfTalkPartyReturn=%d",
				p_lCSTACallRefId, p_strHangupPhone.c_str(), l_bReturn);

			l_bReturn = true;
		}
	}

	return l_bReturn;
}
int	CCallManager::GetConfMemCount(long p_lCSTACallRefId)
{
	int l_nCount = 0;

	SAFE_LOCK(m_callQueueMutex);
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
	SAFE_LOCK(m_callQueueMutex);
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
				l_pRefuseCallRequestNotif->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_RefuseCallEvent, l_pRefuseCallRequestNotif);
				ICC_LOG_DEBUG(m_pLogPtr, "AddSwitchEventTask RefuseCallEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLogPtr, "Create CRefuseCallNotif Object Failed !!!");
			}
		}
	}
}
void CCallManager::PostReleaseCall(long p_lCSTACallRefId)
{
	SAFE_LOCK(m_callQueueMutex);
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
				ICC_LOG_DEBUG(m_pLogPtr, "AddSwitchEventTask CallStateEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLogPtr, "Create ICallStateNotif Object Failed !!!");
			}
		}
	}
}
void CCallManager::PostCallOverNotif(long p_lCSTACallRefId)
{
	SAFE_LOCK(m_callQueueMutex);
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
						//l_strCalledId = l_strOriginalCalledId;
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
				l_pCallOverNotif->SetOriginalCalledId(l_strOriginalCalledId);
				l_pCallOverNotif->SetCallState(iter->second->GetCallState());
				l_pCallOverNotif->SetCallDirection(iter->second->GetCallDirection());
				l_pCallOverNotif->SetDialTime(iter->second->GetDialTime());
				l_pCallOverNotif->SetIncomingTime(iter->second->GetIncomingTime());
				l_pCallOverNotif->SetAssignTime(iter->second->GetAssignTime());
				l_pCallOverNotif->SetRingTime(iter->second->GetRingTime());
				l_pCallOverNotif->SetSignalBackTime(iter->second->GetSignalBackTime());
				l_pCallOverNotif->SetReleaseTime(iter->second->GetReleaseTime());
				l_pCallOverNotif->SetTalkTime(iter->second->GetTalkTime());
				l_pCallOverNotif->SetHangupTime(m_pDateTimePtr->CurrentDateTimeStr());
				l_pCallOverNotif->SetHangupType(iter->second->GetHangupType());
				l_pCallOverNotif->SetSwitchType(SWITCH_TYPE_TSAPI);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallOverEvent, l_pCallOverNotif);
				ICC_LOG_DEBUG(m_pLogPtr, "AddSwitchEventTask CallOverEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLogPtr, "Create CallOverNotif Object Failed !!!");
			}
		}
	}
}

void CCallManager::PostCallState(long p_lCSTACallRefId)
{
	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			ICallStateNotifPtr l_pCallStateNotif = boost::make_shared<CCallStateNotif>();
			if (l_pCallStateNotif)
			{
				l_pCallStateNotif->SetAgentId(iter->second->GetCalledParty());
				std::string l_strACDGrp = iter->second->GetCalledId();
				l_pCallStateNotif->SetACDGrp(l_strACDGrp);
				l_pCallStateNotif->SetCSTACallRefId(p_lCSTACallRefId);
				l_pCallStateNotif->SetCTICallRefId(iter->second->GetCTICallRefId());
				l_pCallStateNotif->SetCallerId(iter->second->GetCallerId());
				l_pCallStateNotif->SetCalledId(iter->second->GetCalledParty());
				l_pCallStateNotif->SetCallState(iter->second->GetCallState());
				l_pCallStateNotif->SetStateTime(iter->second->GetStateTime());

				l_pCallStateNotif->SetOriginalCalledId(iter->second->GetOriginalCalledId());

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_CallStateEvent, l_pCallStateNotif);
				ICC_LOG_DEBUG(m_pLogPtr, "AddSwitchEventTask CallStateEvent, TaskId: [%u]", l_lTaskId);
			}
			else
			{
				ICC_LOG_FATAL(m_pLogPtr, "Create ICallStateNotif Object Failed !!!");
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

		SAFE_LOCK(m_callQueueMutex);
		for (auto l_callObj : m_mapCallQueue)
		{
			if (l_callObj.second->GetCallState().compare(CallStateString[STATE_HANGUP]) != 0)
			{
				CGetCallListResultNotif::CCallObj callObj;
				callObj.m_lCSTACallRefId = l_callObj.second->GetCSTACallRefId();
				callObj.m_strCTICallRefId = l_callObj.second->GetCTICallRefId();
				callObj.m_strCallerId = l_callObj.second->GetCallerId();
				callObj.m_strCalledId = l_callObj.second->GetCalledId();
				callObj.m_strCalledParty = l_callObj.second->GetCalledParty();
				callObj.m_strCallDierection = l_callObj.second->GetCallDirection();
				callObj.m_strCallState = l_callObj.second->GetCallState();
				callObj.m_strStateTime = l_callObj.second->GetStateTime();

				l_pRespondNotif->m_CallList.push_back(callObj);
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetCallListRet, l_pRespondNotif);
		ICC_LOG_DEBUG(m_pLogPtr, "AddSwitchEventTask GetCallListRet, RequestId: [%u], TaskId: [%u]",
			p_lRequestId, l_lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "Create GetCallListResultNotif Object Failed !!!");
	}
}

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

void CCallManager::SetE1RelayIndexNumber(long p_lCSTACallRefId, const std::string& p_strE1RelayIndex, std::string& p_strNumber)
{
	SAFE_LOCK(m_callQueueMutex);
	auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
	if (iter != m_mapCallQueue.end())
	{
		if (iter->second)
		{
			int nSize = 0;
			iter->second->SetE1RelayIndexNumber(p_strE1RelayIndex, p_strNumber, nSize);

			ICC_LOG_DEBUG(m_pLogPtr, "save E1_relay number info,CSTACallRefId:[%d], E1RelayIndex:[%s], number:[%s],size:%d",
				p_lCSTACallRefId,p_strE1RelayIndex.c_str(), p_strNumber.c_str(), nSize);
		}
	}
}


std::string CCallManager::GetE1RelayIndexNumber(long p_lCSTACallRefId, const std::string& p_strE1RelayIndex,bool p_bDeleteFlag)
{
	bool l_bFind = false;
	int nSize = 0;
	std::string l_strNumber = "";
	{
		SAFE_LOCK(m_callQueueMutex);
		auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
		if (iter != m_mapCallQueue.end())
		{
			if (iter->second)
			{
				
				l_bFind = true;
				l_strNumber = iter->second->GetE1RelayIndexNumber(p_strE1RelayIndex, nSize);
				if (p_bDeleteFlag)
				{
					iter->second->DeleteE1RelayIndexNumber(p_strE1RelayIndex);
				}
			}
		}
	}

	if (!l_strNumber.empty())
	{
		ICC_LOG_DEBUG(m_pLogPtr, "get E1_relay number info,CSTACallRefId:[%d], E1RelayIndex:[%s], number:[%s],bFind:%d,size:%d",
			p_lCSTACallRefId, p_strE1RelayIndex.c_str(), l_strNumber.c_str(), l_bFind, nSize);
	}
	return l_strNumber;
	
}

std::string CCallManager::GetE1RelayIndexByNumber(long p_lCSTACallRefId, const std::string& p_strNumber)
{
	bool l_bFind = false;
	int nSize = 0;
	std::string l_strE1RelayIndex = "";
	{
		SAFE_LOCK(m_callQueueMutex);
		auto iter = m_mapCallQueue.find(p_lCSTACallRefId);
		if (iter != m_mapCallQueue.end())
		{
			if (iter->second)
			{
				l_bFind = true;
				l_strE1RelayIndex = iter->second->GetE1RelayIndexByNumber(p_strNumber, nSize);
			}
		}
	}

	if (!l_strE1RelayIndex.empty())
	{
		ICC_LOG_DEBUG(m_pLogPtr, "get E1_relay info by number,CSTACallRefId:[%d], E1RelayIndex:[%s], number:[%s],l_bFind:%d,nSize:%d",
			p_lCSTACallRefId, l_strE1RelayIndex.c_str(), p_strNumber.c_str(), l_bFind, nSize);
	}

	return l_strE1RelayIndex;
}


//////////////////////////////////////////////////////////////////////////
void CCallManager::ResetCheckTime()
{
	m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();
}
bool CCallManager::CheckIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastCheckTime, CHECK_TIMESPAN);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}
bool CCallManager::FindTimeoutCall(long &p_lCSTACallRefId)
{
	bool l_bFind = false;

	SAFE_LOCK(m_callQueueMutex);
	for (auto l_callObj : m_mapCallQueue)
	{
		p_lCSTACallRefId = l_callObj.first;
		std::string l_strCTICallRefId = l_callObj.second->GetCTICallRefId();
		std::string l_strCallState = l_callObj.second->GetCallState();
		std::string l_strCallTime = l_callObj.second->GetStateTime();
		int l_nCallInvalidTime = CSysConfig::Instance()->GetCallInvalidTime();

		DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
		DateTime::CDateTime l_oCallOverEndTime = m_pDateTimePtr->AddMinutes(m_pDateTimePtr->FromString(l_strCallTime), CALL_OVER_TIMESPAN);
		DateTime::CDateTime l_oTimeoutEndTime = m_pDateTimePtr->AddHours(m_pDateTimePtr->FromString(l_strCallTime), l_nCallInvalidTime);

		if (l_callObj.second->GetIsPostCallOver()/*l_strCallState.compare(CallStateString[STATE_HANGUP]) == 0*/ && l_oCurrentTime > l_oCallOverEndTime)
		{
			//	话务结束 2 分钟后，删除内存中的话务
			l_bFind = true;

			//	p_lCSTACallRefId = iter->first;
			//	std::string l_strCTICallRefId = iter->second->GetCTICallRefId();

			ICC_LOG_DEBUG(m_pLogPtr, "Call Over , CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
				p_lCSTACallRefId, l_strCTICallRefId.c_str(), m_mapCallQueue.size());

			break;
		}
		else if (l_oCurrentTime > l_oTimeoutEndTime)
		{
			//	话务超过 24 小时，删除内存中的话务
			l_bFind = true;

			l_callObj.second->SetHangupType(HangupTypeString[HANGUP_TYPE_CALLED]);
			l_callObj.second->SetHangupDevice(l_callObj.second->GetCalledParty());
			l_callObj.second->SetCallState(CallStateString[STATE_HANGUP], m_pDateTimePtr->CurrentDateTimeStr());

			//	p_lCSTACallRefId = iter->first;
			//	std::string l_strCTICallRefId = iter->second->GetCTICallRefId();

			ICC_LOG_WARNING(m_pLogPtr, "Call Timeout , CSTACallRefId: [%u], CTICallRefId: [%s], QueueSize: [%d]",
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

	this->PostCallOverNotif(p_lCSTACallRefId);
	this->DeleteCall(p_lCSTACallRefId);
}

void CCallManager::StartCheckThread()
{
	if (!m_bCheckThreadAlive)
	{
		m_bCheckThreadAlive = true;
		m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();

		m_pCheckCallThread = boost::make_shared<boost::thread>(boost::bind(&CCallManager::DoCheckCall, this));
	}
}
void CCallManager::StopCheckThread()
{
	if (m_bCheckThreadAlive)
	{
		m_bCheckThreadAlive = false;

		if (m_pCheckCallThread)
		{
			m_pCheckCallThread->join();
		}
	}
}

void CCallManager::DoCheckCall()
{
	ICC_LOG_DEBUG(m_pLogPtr, "=============== CheckCall Thread Start!! ===============");

	while (CCallManager::Instance()->m_bCheckThreadAlive)
	{
		if (CCallManager::Instance()->CheckIsTimeout())
		{
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			while (CCallManager::Instance()->FindTimeoutCall(l_lCSTACallRefId))
			{
				CCallManager::Instance()->ProcessTimeoutCall(l_lCSTACallRefId);
			}

			CCallManager::Instance()->ResetCheckTime();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLogPtr, "------------------ CheckCall Thread Exit!! ------------------");
}
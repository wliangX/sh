#include "Boost.h"

#include "ConferenceManager.h"
#include "HytProtocol.h"

#define MAX_BUFFER					64
#define CHECK_TIMESPAN				1		//单位：秒
#define SLEEP_TIME					100		//单位：毫秒
#define CONFERENCE_RING_TIMEOUT		20		//单位：秒

CConference::CConference()
{
	m_bBargein = false;

	m_strCompere = "";
	m_strCTICallRefId = "";
	//m_strConferenceId = "";
	m_strRingTime = "";

	m_nIndex = 0;
}
CConference::~CConference()
{
	//
}

CConference::CConference(bool p_bBargein,
	const std::string& p_strConferenceId,
	const std::string& p_strCTICallRefId,
	const std::string& p_strSrcCaller,
	const std::string& p_strSrcCallerType,
	const std::string& p_strTmpState,
	const std::string& p_strCompere,
	const std::string& p_strParty,
	const std::string& p_strCreateTime,
	ISwitchClientPtr p_SwitchClientPtr)
		:m_bBargein(p_bBargein),
		m_strCTICallRefId(p_strCTICallRefId),
		m_strCompere(p_strCompere),
		m_strBargeinSponsor(p_strParty),
		m_strConferenceId(p_strConferenceId),
		m_nIndex(0)
{
	//强插创建会议
	m_strCreateTime = p_strCreateTime;

	CConferenceParty l_party;
	l_party.m_strAddTime = p_strCreateTime;
	l_party.m_strJoinTime = p_strCreateTime;
	l_party.m_strDisEnable = "0";
	l_party.m_strCallState = p_strTmpState;
	l_party.m_strCallId = p_strCTICallRefId;
	l_party.m_lConferenceCallTaskId = 0;
	l_party.m_strActiveCallRefId = "";

	std::string l_strRingTime;
	std::string l_strTalkTime;
	if (p_SwitchClientPtr->GetCallTime(p_strCTICallRefId, l_strRingTime, l_strTalkTime))
	{
		l_party.m_strRingTime = l_strRingTime;
		l_party.m_strTalkTime = l_strTalkTime;
	}

	//加入成员列表
	l_party.m_strTarget = p_strSrcCaller;
	l_party.m_strTargetDeviceType = p_strSrcCallerType;
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strSrcCaller] = l_party;

	l_party.m_strTarget = p_strCompere;
	l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strCompere] = l_party;

	l_party.m_strTarget = p_strParty;
	l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strParty] = l_party;
}

CConference::CConference(std::string& p_strConferenceId,
	const std::string &p_strCTICallRefId, 
	const std::string& p_strActiveCallRefId,
	const std::string& p_strCompere, 
	const std::string& p_strTarget, 
	const std::string& p_strCreateTime,
	ISwitchClientPtr p_SwitchClientPtr)
	:m_bBargein(false),
	m_strConferenceId(p_strConferenceId)
{
	std::string l_strState;
	m_strCompere = p_strCompere;
	m_strCTICallRefId = p_strCTICallRefId;

	m_nIndex = 0;
	//将呼叫三方加入成员列表
	CConferenceParty l_party;
	l_party.m_strAddTime = p_strCreateTime;
	l_party.m_strJoinTime = p_strCreateTime;
	l_party.m_lConferenceCallTaskId = 0;
	l_party.m_strDisEnable = "0";
	l_party.m_strCallState = CallStateString[STATE_TALK];
	l_party.m_strTarget = p_strCompere;
	l_party.m_strCallId = p_strCTICallRefId;
	l_party.m_strActiveCallRefId = "";

	std::string l_strRingTime;
	std::string l_strTalkTime;
	if (p_SwitchClientPtr->GetCallTime(p_strCTICallRefId, l_strRingTime, l_strTalkTime))
	{
		l_party.m_strRingTime = l_strRingTime;
		l_party.m_strTalkTime = l_strTalkTime;
	}

	l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strCompere] = l_party; 

	std::string l_strSrcCalled;
	std::string l_strSrcCaller = p_SwitchClientPtr->GetCallerID(p_strCTICallRefId, l_strSrcCalled);
	std::string l_strPeerNum;
	if (l_strSrcCaller == p_strCompere)
	{
		l_strPeerNum = l_strSrcCalled;
	}
	else
	{
		l_strPeerNum = l_strSrcCaller;
	}

	l_strState = p_SwitchClientPtr->GetDeviceState(l_strPeerNum);
	if (l_strState.empty())
	{
		l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_OUTSIDE;
	}
	else
	{
		l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
	}
	l_party.m_strCallId = p_strCTICallRefId;
	l_party.m_strActiveCallRefId = "";
	l_party.m_strTarget = l_strPeerNum;
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[l_strPeerNum] = l_party;

	l_party.m_strTarget = p_strTarget;
	l_party.m_strCallId = p_strActiveCallRefId;
	l_party.m_strActiveCallRefId = p_strActiveCallRefId;
	l_party.m_strJoinTime = "";
	if (p_SwitchClientPtr->GetCallTime(p_strActiveCallRefId, l_strRingTime, l_strTalkTime))
	{
		l_party.m_strRingTime = l_strRingTime;
		l_party.m_strTalkTime = l_strTalkTime;
	}
	else
	{
		l_party.m_strRingTime = "";
		l_party.m_strTalkTime = "";
	}

	l_strState = p_SwitchClientPtr->GetDeviceState(p_strTarget);
	if (l_strState.empty())
	{
		l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_OUTSIDE;
	}
	else
	{
		l_party.m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
	}
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strTarget] = l_party;

}

void CConference::AddConferencePartyEx(const std::string &p_strCallId,
	const std::string& p_strConsultCallId, 
	const std::string& p_strTarget, 
	const std::string &p_strTime,
	ISwitchClientPtr p_SwitchClientPtr)
{
	CConferenceParty l_party;
	//l_party.m_strCallState = p_strCurrState;
	l_party.m_strDisEnable = "0";
	l_party.m_strTarget = p_strTarget;
	l_party.m_strAddTime = p_strTime;
	l_party.m_strCallId = p_strCallId;

	std::string l_strRingTime;
	std::string l_strTalkTime;
	if (p_strConsultCallId.empty())
	{
		l_party.m_strCallState = CallStateString[STATE_TALK];
		if (p_SwitchClientPtr->GetCallTime(p_strCallId, l_strRingTime, l_strTalkTime))
		{
			l_party.m_strRingTime = l_strRingTime;
			l_party.m_strTalkTime = l_strTalkTime;
		}
	}
	else
	{
		l_party.m_strCallState = CallStateString[STATE_DIAL];
		l_party.m_strActiveCallRefId = p_strConsultCallId;
		if (p_SwitchClientPtr->GetCallTime(p_strConsultCallId, l_strRingTime, l_strTalkTime))
		{
			l_party.m_strRingTime = l_strRingTime;
			l_party.m_strTalkTime = l_strTalkTime;
		}
	}
	
	l_party.m_nIndex = m_nIndex++;
	m_mapPartys[p_strTarget] = l_party;
}

bool CConference::UpdateConferencePartyState(const std::string& p_strDeviceNum, const std::string p_DeviceState,const std::string &p_strCurrTime, Log::ILogPtr m_log)
{
	std::string l_strLastState;
	auto it = m_mapPartys.find(p_strDeviceNum);
	if (it != m_mapPartys.end())
	{
		l_strLastState = it->second.m_strCallState;
		it->second.m_strCallState = p_DeviceState;
		bool l_bSynCallOver = false;
		
		if (p_DeviceState == CallStateString[STATE_HANGUP])
		{
			it->second.m_strDisEnable = "1";
			if (it->second.m_strHangupTime.empty())
			{
				it->second.m_strHangupTime = p_strCurrTime;
				l_bSynCallOver = true;
				CHytProtocol::Instance()->Sync_MakeConference(m_strConferenceId, m_strCTICallRefId, m_strCompere, p_strDeviceNum, p_DeviceState, Result_Success);
			}
		}
		else if (p_DeviceState == CallStateString[STATE_RING])
		{
			if (it->second.m_strRingTime.empty())
			{
				it->second.m_strRingTime = p_strCurrTime;
			}
		}
		else if (p_DeviceState == CallStateString[STATE_TALK])
		{
			if (it->second.m_strTalkTime.empty())
			{
				it->second.m_strTalkTime = p_strCurrTime;
			}
		}
		else if (p_DeviceState == CallStateString[STATE_CONFERENCE])
		{
			it->second.m_strCallState = CallStateString[STATE_TALK];
			if (it->second.m_strJoinTime.empty())
			{
				it->second.m_strJoinTime = p_strCurrTime;
			}
		}

		if (!l_bSynCallOver)
		{
			CHytProtocol::Instance()->Sync_MakeConference(m_strConferenceId, m_strCTICallRefId, m_strCompere, p_strDeviceNum, it->second.m_strCallState, Result_Success);
		}
		else
		{
			if (!it->second.m_strTalkTime.empty())
			{
				//呼叫成功才发子话务结束通知
				CHytProtocol::Instance()->Sync_ConferenceDetailCallOver(p_strDeviceNum,
					it->second.m_strTargetDeviceType,
					m_strCompere,
					it->second.m_strCallId,
					it->second.m_strActiveCallRefId,
					it->second.m_strRingTime,
					it->second.m_strTalkTime,
					it->second.m_strJoinTime,
					it->second.m_strHangupTime);
			}
		}
	}
	else
	{
		ICC_LOG_ERROR(m_log, "not find party,deviceNum:[%s],DeviceState:[%s]", p_strDeviceNum.c_str(), p_DeviceState.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_log, "update party state,deviceNum:[%s],DeviceState:[%s],LastState:[%s]", p_strDeviceNum.c_str(), p_DeviceState.c_str(), l_strLastState.c_str());

	return true;
}

bool CConference::GetPartyListEx(std::map<std::string, CConferenceParty>& p_ConferencePartyList, std::string& p_strLastTarget)
{
	int l_nIndex = 0;
	auto it = m_mapPartys.begin();
	while (it != m_mapPartys.end())
	{
		p_ConferencePartyList[it->first] = it->second;
		if (l_nIndex < it->second.m_nIndex)
		{
			//查最后一个加入的
			p_strLastTarget = it->first;
			l_nIndex = it->second.m_nIndex;
		}
		it++;
	}

	return true;
}


bool CConference::SetDisenableParty(const std::string& p_strTarget,bool p_bDisEnable)
{
	bool bRes = false;
	auto it = m_mapPartys.find(p_strTarget);
	if (it != m_mapPartys.end())
	{
		std::string l_strDisEnable;
		if (p_bDisEnable)
		{
			it->second.m_strDisEnable = "1";
		}
		else
		{
			it->second.m_strDisEnable = "0";
		}
		return true;
	}
	return bRes;
}


//没有实现删除会议成员的逻辑，只有挂断会议成员呼叫，fk
void CConference::DeleteConferenceParty(const std::string& p_strDeviceNum)
{
	m_mapPartys.erase(p_strDeviceNum);
}

void CConference::ClearConferenceParty()
{
	m_mapPartys.clear();
}

bool CConference::FindConferenceParty(const std::string& p_strDeviceNum)
{
	bool l_bRet = false;
	auto it = m_mapPartys.find(p_strDeviceNum);
	if (it != m_mapPartys.end())
	{
		return true;
	}
	return l_bRet;
}

std::string CConference::GetPartyByActiveCTICallRefId(const std::string& p_strActiveCTICallRefId)
{
	std::string l_strTarget = "";
	auto it = m_mapPartys.begin();
	while (it != m_mapPartys.end())
	{
		if (it->second.m_strActiveCallRefId == p_strActiveCTICallRefId)
		{
			l_strTarget = it->first;
			break;
		}
		it++;
	}

	return l_strTarget;
}


std::string CConference::GetPartyState(const std::string& p_strParty)
{
	std::string l_strCallState = "";
	auto it = m_mapPartys.find(p_strParty);
	if (it != m_mapPartys.end())
	{
		l_strCallState = it->second.m_strCallState;
	}
	return l_strCallState;
}


std::string CConference::GetActiveCTICallRefIdByParty(const std::string& p_strParty)
{
	std::string l_strActiveCallRefId = "";
	auto it = m_mapPartys.find(p_strParty);
	if (it != m_mapPartys.end())
	{
		l_strActiveCallRefId = it->second.m_strActiveCallRefId;
	}
	return l_strActiveCallRefId;
}

bool CConference::SetConferenceTarskId(const std::string& p_strParty, const std::string& p_strActiveCallRefId, long p_lTaskId)
{
	auto it = m_mapPartys.find(p_strParty);
	if (it != m_mapPartys.end())
	{
		it->second.m_lConferenceCallTaskId = p_lTaskId;
		return true;
	}

	return false;
}

bool CConference::FindConferenceCallTaskId(long p_lTaskId, std::string& p_strParty, std::string& p_strActiveCallRefId)
{
	auto it = m_mapPartys.begin();
	while (it != m_mapPartys.end())
	{
		if (it->second.m_lConferenceCallTaskId == p_lTaskId)
		{
			p_strParty = it->first;
			p_strActiveCallRefId = it->second.m_strActiveCallRefId;
			return true;
		}
		it++;
	}
	return false;
}

void CConference::AddConsultationCallTaskId(long p_lConsultationCallTaskId, const std::string& p_strTarget)
{
	m_mapConsultationTaskIdTB[p_lConsultationCallTaskId] = p_strTarget;
}

bool CConference::FindConsultationCallTaskId(long p_lConsultationCallTaskId)
{
	auto it = m_mapConsultationTaskIdTB.find(p_lConsultationCallTaskId);
	if (it != m_mapConsultationTaskIdTB.end())
	{
		return true;
	}

	return false;
}

void CConference::DeleteConsultationCallTaskId(long p_lConsultationCallTaskId)
{
	m_mapConsultationTaskIdTB.erase(p_lConsultationCallTaskId);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CConferenceManager> CConferenceManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CConferenceManager> CConferenceManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CConferenceManager>();
	}

	return m_pManagerInstance;
}
void CConferenceManager::ExitInstance()
{
	this->ClearConferenceList();
}

CConferenceManager::CConferenceManager(void)
{
	m_bThreadAlive = false;

	m_LogPtr = nullptr;
	m_DateTimePtr = nullptr;
	m_StringUtilPtr = nullptr;
	m_SwitchClientPtr = nullptr;
	m_pCheckRingTimeoutThread = nullptr;

	m_nRingTimeout = CONFERENCE_RING_TIMEOUT;
	m_oLastCheckTime = 0;
}

CConferenceManager::~CConferenceManager(void)
{
	//
}

std::string CConferenceManager::_CreateBargeinConference(const std::string& p_strCompere, //强插的Target作为主持人
	const std::string& l_strParty, //强插的发起者是成员
	const std::string& p_strCTICallRefId)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));//localtime(&l_tCurrentTime));
	std::string l_strConferenceId = std::string(l_szDayTime);

	std::string l_strSrcCalled;
	std::string l_strSrcCaller;
	l_strSrcCaller = m_SwitchClientPtr->GetCallerID(p_strCTICallRefId, l_strSrcCalled);
	std::string l_strTmpState = m_SwitchClientPtr->GetDeviceState(l_strSrcCalled);
	if (l_strTmpState.empty())
	{
		l_strTmpState = m_SwitchClientPtr->GetDeviceState(l_strSrcCaller);
	}

	std::string l_strCallerType = TARGET_DEVICE_TYPE_OUTSIDE;
	if (!m_SwitchClientPtr->GetDeviceState(l_strSrcCaller).empty())
	{
		l_strCallerType = TARGET_DEVICE_TYPE_INSIDE;
	}

	boost::shared_ptr<CConference> l_pConference = boost::make_shared<CConference>(true, 
		l_strConferenceId,
		p_strCTICallRefId,
		l_strSrcCaller,
		l_strCallerType,
		l_strTmpState, 
		p_strCompere, 
		l_strParty, m_DateTimePtr->CurrentDateTimeStr(),m_SwitchClientPtr);
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		m_mConferenceList[l_strConferenceId] = l_pConference;
	}
	
	ICC_LOG_DEBUG(m_LogPtr, "Create New Conference(Bargein), CTICallRefId:[%s] ConferenceId:[%s] Compere:[%s],SrcCaller:[%s],srcCalled:[%s],Party:[%s]",
		p_strCTICallRefId.c_str(), 
		l_strConferenceId.c_str(),
		p_strCompere.c_str(), 
		l_strSrcCaller.c_str(), 
		l_strSrcCalled.c_str(), l_strParty.c_str());

	return l_strConferenceId;
}

std::string CConferenceManager::CreateConferenceEx(const std::string& p_strActiveCallId, const std::string& p_strHoldCallId, const std::string& p_strTarget, const std::string& p_strCompere)
{
	std::string l_strConferenceId;
	std::string l_strCompere;
	std::string l_strOriginalCTICallRefId;
	l_strConferenceId = FindConferenceIdByCallId(p_strActiveCallId, l_strCompere, l_strOriginalCTICallRefId);
	if (!l_strConferenceId.empty())
	{
		ICC_LOG_WARNING(m_LogPtr, "Conference exists,conferenceId:[%s],callId:[%s]", l_strConferenceId.c_str(), p_strActiveCallId.c_str());
		return l_strConferenceId;
	}

	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));
	l_strConferenceId = std::string(l_szDayTime);

	boost::shared_ptr<CConference> l_oConferenceObj = boost::make_shared<CConference>(l_strConferenceId, 
		p_strHoldCallId, 
		p_strActiveCallId,
		p_strCompere,
		p_strTarget,
		m_DateTimePtr->CurrentDateTimeStr(),
		m_SwitchClientPtr);
	if (l_oConferenceObj != nullptr)
	{
		long l_lConferenceTaskId = _ConferenceCall(p_strCompere, p_strHoldCallId, p_strActiveCallId);
		l_oConferenceObj->SetConferenceTarskId(p_strTarget,p_strActiveCallId, l_lConferenceTaskId);
		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			m_mConferenceList[l_strConferenceId] = l_oConferenceObj;
		}
		ICC_LOG_DEBUG(m_LogPtr, "Create New Conference,conferenceId:[%s],callId:[%s],compere:[%s],taskId:[%d]", l_strConferenceId.c_str(), p_strHoldCallId.c_str(), p_strCompere.c_str(), l_lConferenceTaskId);
	}

	return l_strConferenceId;
}

std::string CConferenceManager::AddConferencePartyNew(const std::string& p_strCTICallRefId,
	const std::string& p_strCompere,
	const std::string& l_strTarget,
	const std::string& l_strTargetType, bool p_bBargein)
{
	//	在已有会议上新增会议成员
	std::string l_strConferenceId = "";
	if (!_GetConferenceByCTICallRefId(p_strCTICallRefId, l_strConferenceId))
	{
		if (p_bBargein)
		{
			l_strConferenceId = _CreateBargeinConference(p_strCompere, l_strTarget, p_strCTICallRefId);
			return l_strConferenceId;
		}
		//	会议未存在
		ICC_LOG_ERROR(m_LogPtr, "not find conferenceid,CTICallRefId:[%s],Compere:[%s],Target:[%s]", p_strCTICallRefId.c_str(), p_strCompere.c_str(), l_strTarget.c_str());
		return "";
	}
	else
	{
		boost::shared_ptr<CConference> p_Conference = _GetConferenceObject(l_strConferenceId);
		if (p_Conference)
		{
			long l_lConsultationTaskId = 0;
			if (!p_bBargein)
			{
				l_lConsultationTaskId = _ConsultationCall(p_strCTICallRefId, p_strCompere, l_strTarget, l_strTargetType);
				p_Conference->AddConsultationCallTaskId(l_lConsultationTaskId, l_strTarget);
			}
			else
			{
				p_Conference->AddConferencePartyEx(p_strCTICallRefId,"", l_strTarget, m_DateTimePtr->CurrentDateTimeStr(), m_SwitchClientPtr);
			}
			ICC_LOG_DEBUG(m_LogPtr, "AddConferenceParty,ConferenceId:[%s],target:[%s],bBargein:[%d],taskId:[%d]", l_strConferenceId.c_str(), l_strTarget.c_str(), p_bBargein, l_lConsultationTaskId);
		}
	}

	return l_strConferenceId;
}


//会议合并结果返回
void CConferenceManager::ProcessConferenceCallResult(long p_lTaskId, bool p_bResult)
{
	bool l_bAddNextParty = false;
	std::string l_strConferenceId = "";
	std::string l_strParty = "";
	std::string l_strActiveCallRefId = "";
	boost::shared_ptr<CConference> l_pConferenceObj = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for (auto l_oConferenceObj : m_mConferenceList)
		{
			if (l_oConferenceObj.second->FindConferenceCallTaskId(p_lTaskId, l_strParty, l_strActiveCallRefId))
			{
				l_strConferenceId = l_oConferenceObj.first;
				l_pConferenceObj = l_oConferenceObj.second;
				break;
			}
		}
	}

	if (p_bResult && l_pConferenceObj)
	{
		std::string l_strPartyState = l_pConferenceObj->GetPartyState(l_strParty);
		// 当前成员加入会议成功
		ICC_LOG_DEBUG(m_LogPtr, "process state conference result,conferenceId:[%s],taskId:[%d],CurrentParty:[%s],CurrentPartyState:[%s],ActiveCallRefId:[%s],srcCTICallRefId:[%s]",
			l_strConferenceId.c_str(), p_lTaskId, l_strParty.c_str(), l_strPartyState.c_str(), l_strActiveCallRefId.c_str(), l_pConferenceObj->GetCTICallRefId().c_str());

		bool bRes = l_pConferenceObj->UpdateConferencePartyState(l_strParty, CallStateString[STATE_CONFERENCE], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);

	}
	else
	{
		// 当前成员加入会议失败，继续加下一个成员
		ICC_LOG_ERROR(m_LogPtr, "result:[%d],conferenceId:[%s],taskId:[%d],ActiveCallRefId:[%s]", p_bResult, l_strConferenceId.c_str(), p_lTaskId, l_strActiveCallRefId.c_str());
		_ProcessAddConferenceFailedEvent(l_strConferenceId, l_strActiveCallRefId);
	}
}

//会议咨询呼结果
void CConferenceManager::ProcessConsultationCallResult(long p_lTaskId, 
	const std::string& p_strHeldCTICallRefId, 
	const std::string& p_strActiveCTICallRefId,
	const std::string & p_strTargetDevice, 
	bool p_bResult)
{
	std::string l_strConferenceId = "";
	boost::shared_ptr<CConference> l_pConferenceObj = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for (auto l_oConferenceObj : m_mConferenceList)
		{
			if (l_oConferenceObj.second->FindConsultationCallTaskId(p_lTaskId))
			{
				l_strConferenceId = l_oConferenceObj.first;
				l_pConferenceObj = l_oConferenceObj.second;
				break;
			}
		}
	}

	std::string l_strCTICallRefId;
	std::string l_strCompere;
	std::string l_strTarget;
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "Consultation Call Result,not find conference,res:[%d],TaskId:[%d],HeldCTICallRefId:[%s],ActiveCTICallRefId:[%s]",
			p_bResult, p_lTaskId, p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str());
		return;
	}
	else
	{
		l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();
		l_strCompere = l_pConferenceObj->GetCompere();
		l_strTarget = p_strTargetDevice;

		l_pConferenceObj->DeleteConsultationCallTaskId(p_lTaskId);
	}

	ICC_LOG_DEBUG(m_LogPtr, "process state Consultation Call Result,res:[%d],TaskId:[%d],HeldCTICallRefId:[%s],ActiveCTICallRefId:[%s],conferenceId:[%s],currTarget:%s,targetDevice:%s",
		p_bResult, p_lTaskId, p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), l_strConferenceId.c_str(), l_strTarget.c_str(), p_strTargetDevice.c_str());

	if (p_bResult && l_pConferenceObj != nullptr)
	{
		if (p_strTargetDevice != l_strTarget && !p_strTargetDevice.empty())
		{
			l_strTarget = p_strTargetDevice;
		}

		//咨询返回成功,将成员加入会议列表
		l_pConferenceObj->AddConferencePartyEx(l_strCTICallRefId, p_strActiveCTICallRefId, l_strTarget, m_DateTimePtr->CurrentDateTimeStr(), m_SwitchClientPtr);

		//更新成员状态为STATE_DIAL
		l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_DIAL], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);

	}
	else
	{
		//	通知客户端，开始呼叫会议成员
		CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
			l_strCompere, l_strTarget, CallStateString[STATE_HANGUP], "1");
	}
	
}


void CConferenceManager::ProcessDeviceStateNotif(const std::string& p_strDeviceNum,const std::string &p_strCalledId, const std::string& p_strDeviceState, const std::string& p_strCTICallRefId,const std::string &p_strConferenceId)
{
	if (p_strDeviceState.compare(CallStateString[STATE_SIGNALBACK]) == 0)
	{
		_ProcessRingState(p_strDeviceNum, p_strCalledId, p_strCTICallRefId, p_strConferenceId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
	{
		_ProcessTalkState(p_strDeviceNum, p_strCalledId, p_strCTICallRefId, p_strConferenceId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_HANGUP]) == 0)
	{
		// 加入会议前挂机，失败
		_ProcessHangupState(p_strDeviceNum, p_strCTICallRefId, p_strConferenceId);
	}
	else if(p_strDeviceState.compare(CallStateString[STATE_HOLD]) == 0)
	{
		//_ProcessHoldorUnholdState(true,p_strDeviceNum, p_strCTICallRefId, p_strConferenceId);
	}
	else if(p_strDeviceState.compare(CallStateString[STATE_CANCELHOLD]) == 0)
	{
		//_ProcessHoldorUnholdState(false,p_strDeviceNum, p_strCTICallRefId, p_strConferenceId);
	}
}
//////////////////////////////////////////////////////////////////////////
bool CConferenceManager::GetConferenceParty(const std::string& p_strConferenceId, PROTOCOL::CGetConferencePartyRespond& p_oOutRespond)
{
	boost::shared_ptr<CConference> l_pConferenceObj;
	int nSize;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(p_strConferenceId);
		nSize = m_mConferenceList.size();
		if (it != m_mConferenceList.end())
		{
			l_pConferenceObj = it->second;
		}
	}
	if (!l_pConferenceObj)
	{
		ICC_LOG_DEBUG(m_LogPtr, "GetConferenceParty Not find, ConferenceId:[%s],nSize=%d", p_strConferenceId.c_str(), nSize);
		return false;
	}

	std::string l_strLastTarget;
	std::map<std::string, CConferenceParty> tmp_PartyTable;
	p_oOutRespond.m_oBody.m_strCount = "0";
	p_oOutRespond.m_oBody.m_strCompere = l_pConferenceObj->GetCompere();

	if (!l_pConferenceObj->GetPartyListEx(tmp_PartyTable, l_strLastTarget))
	{
		return false;
	}

	for (auto l_oPartyObj : tmp_PartyTable)
	{
		PROTOCOL::CGetConferencePartyRespond::CBody::CData l_CData;
		l_CData.m_strTarget = l_oPartyObj.first;
		l_CData.m_strTargetDeviceType = l_oPartyObj.second.m_strTargetDeviceType;
		l_CData.m_strAddTimer = l_oPartyObj.second.m_strAddTime;
		l_CData.m_strState = l_oPartyObj.second.m_strCallState;
		l_CData.m_strDisEnable = l_oPartyObj.second.m_strDisEnable;
		if (l_strLastTarget == l_CData.m_strTarget)
		{
			//最后一个加入会议的  Finally add
			l_CData.m_strFinallyAdd = "1";
		}
		p_oOutRespond.m_oBody.m_vecData.push_back(l_CData);
	}


	p_oOutRespond.m_oBody.m_strCount = std::to_string(p_oOutRespond.m_oBody.m_vecData.size());
	ICC_LOG_DEBUG(m_LogPtr, "GetConferenceParty find OK, ConferenceId:[%s],CallRefId:[%s],nSize=%d,Compere:[%s]", 
		p_strConferenceId.c_str(),
		l_pConferenceObj->GetCTICallRefId().c_str(),
		nSize, 
		l_pConferenceObj->GetCompere().c_str());

	return true;
}


bool CConferenceManager::DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty)
{
	std::string l_strCompere = "";
	std::string l_strCallRefId = "";

	bool l_bRes = false;
	boost::shared_ptr<CConference> p_Conference = _GetConferenceObject(p_strConferenceId);
	if (p_Conference)
	{
		l_bRes = p_Conference->SetDisenableParty(p_strParty, true);
		ICC_LOG_DEBUG(m_LogPtr, "set party disenable, ConferenceId: [%s],party:[%s],res:%d ", p_strConferenceId.c_str(), p_strParty.c_str(), l_bRes);
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "Not find Conference, ConferenceId: [%s] ", p_strConferenceId.c_str());
	}
	return l_bRes;
}

void CConferenceManager::DeleteConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty)
{
	boost::shared_ptr<CConference> p_Conference = _GetConferenceObject(p_strConferenceId);
	if (p_Conference)
	{
		std::string l_strCompere = p_Conference->GetCompere();
		std::string l_strCallRefId = p_Conference->GetCTICallRefId();
		std::string l_strPartyState = p_Conference->GetPartyState(p_strParty);

		if (!l_strPartyState.empty() && l_strPartyState != CallStateString[STATE_TALK] &&
			l_strPartyState != CallStateString[STATE_HANGUP])
		{
			std::string l_strActiveCTICallRefId = p_Conference->GetActiveCTICallRefIdByParty(p_strParty);
			//this->_ReconnectCall(l_strActiveCTICallRefId, l_strCallRefId, l_strCompere, p_strParty);
			if (l_strActiveCTICallRefId.empty())
			{
				_ClearCall(l_strActiveCTICallRefId, l_strCompere, p_strParty);
			}
			else
			{
				this->_HangupCall(l_strCallRefId, l_strCompere, p_strParty);
			}

			ICC_LOG_DEBUG(m_LogPtr, "ClearCall party, ConferenceId:[%s],srcCallRefId:[%s],avtiveCTICallRefId:[%s],Party:[%s] ", 
				p_strConferenceId.c_str(), l_strCallRefId.c_str(), l_strActiveCTICallRefId.c_str(), p_strParty.c_str());
		}
		else
		{
			this->_HangupCall(l_strCallRefId, l_strCompere, p_strParty);

			ICC_LOG_DEBUG(m_LogPtr, "Hanup party, ConferenceId:[%s],srcCTICallRefId:[%s],Party:[%s] ",
				p_strConferenceId.c_str(), l_strCallRefId.c_str(), p_strParty.c_str());
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "Not find Conference, ConferenceId: [%s] ", p_strConferenceId.c_str());
	}
}

//	会议结束时，删除会议
void CConferenceManager::DeleteConference(const std::string& p_strConferenceId, const std::string& p_strCTICallRefId)
{
	bool l_bFlag = false;
	std::string l_strOriginalCTICallRefId;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(p_strConferenceId);
		if (it != m_mConferenceList.end())
		{
			l_strOriginalCTICallRefId = it->second->GetCTICallRefId();
			l_bFlag = true;
			m_mConferenceList.erase(it);
		}
	}
	
	if (l_bFlag)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Delete Conference,conferenceId:[%s], CTICallRefId:[%s],OriginalCTICallRefId:[%s] ", 
			p_strConferenceId.c_str(), p_strCTICallRefId.c_str(), l_strOriginalCTICallRefId.c_str());
	}
}

void CConferenceManager::ClearConferenceList()
{
	size_t l_size = m_mConferenceList.size();
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		m_mConferenceList.clear();
	}
	ICC_LOG_DEBUG(m_LogPtr, "ClearConferenceList,size:%d", l_size);
}

bool CConferenceManager::FindConferenceByConsultationCallTaskId(const long p_lTaskId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		if (l_oConferenceObj.second->FindConsultationCallTaskId(p_lTaskId))
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

bool CConferenceManager::FindConferenceByConferenceCallTaskId(const long p_lTaskId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		std::string l_strParty;
		std::string l_strActiveCTICallRefId;
		if (l_oConferenceObj.second->FindConferenceCallTaskId(p_lTaskId, l_strParty, l_strActiveCTICallRefId))
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

std::string CConferenceManager::FindConferenceIdByCallId(const std::string& p_strCTICallRefId,std::string &p_strCompere,std::string &p_strOriginalCTICallRefId)
{
	std::string l_strConferenceId = "";
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		std::string l_strCTICallRefId = l_oConferenceObj.second->GetCTICallRefId();
		std::string l_strParty = l_oConferenceObj.second->GetPartyByActiveCTICallRefId(p_strCTICallRefId);
		if (l_strCTICallRefId.compare(p_strCTICallRefId) == 0 || !l_strParty.empty())
		{
			l_strConferenceId = l_oConferenceObj.first;
			p_strOriginalCTICallRefId = l_strCTICallRefId;
			p_strCompere = l_oConferenceObj.second->GetCompere();
			break;
		}
	}
	return l_strConferenceId;
}

void CConferenceManager::ProcessConferenceHangup(const std::string& p_strHangupDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId)
{
	std::string l_strConferenceId = p_strConferenceId;
	std::string l_strCTICallRefId = p_strCTICallRefId;
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "process state hangup Not find conference,conferenceId:[%s],hangupDeviceNum:[%s],CTICallRefId:[%s]", p_strConferenceId.c_str(), p_strHangupDeviceNum.c_str(), p_strCTICallRefId.c_str());
		return;
	}

	std::string l_strCompere = l_pConferenceObj->GetCompere();

	if (l_pConferenceObj->FindConferenceParty(p_strHangupDeviceNum))
	{
		ICC_LOG_DEBUG(m_LogPtr, "process state hangup,conferenceId:[%s],CTICallRefId:[%s],party:[%s]", p_strConferenceId.c_str(), p_strCTICallRefId.c_str(), p_strHangupDeviceNum.c_str());

		l_pConferenceObj->UpdateConferencePartyState(p_strHangupDeviceNum, CallStateString[STATE_HANGUP], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "process state hangup Not find party,strHangupDeviceNum:%s,p_strCTICallRefId:%s", p_strHangupDeviceNum.c_str(), p_strCTICallRefId.c_str());
	}
}

void CConferenceManager::ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strActiveCallRefId)
{
	std::string l_strConferenceId = "";
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for (auto l_oConferenceObj : m_mConferenceList)
		{
			std::string l_strCompere = l_oConferenceObj.second->GetCompere();
			std::string l_strParty = l_oConferenceObj.second->GetPartyByActiveCTICallRefId(p_strActiveCallRefId);
			if (l_strCompere.compare(p_strDeviceNum) == 0 && !l_strParty.empty())
			{
				l_strConferenceId = l_oConferenceObj.first;
				break;
			}
		}
	}
	if (!l_strConferenceId.empty())
	{
		ICC_LOG_DEBUG(m_LogPtr, "process state failed,DeviceNum:[%s],ActiveCallRefId:[%s],conferenceId:[%s],calledId:[%s]", p_strDeviceNum.c_str(), p_strActiveCallRefId.c_str(), l_strConferenceId.c_str(), p_strCalledId.c_str());

		_ProcessAddConferenceFailedEvent(l_strConferenceId, p_strActiveCallRefId);
	}
}

bool CConferenceManager::GetBargeinCreateSponsor(const std::string& p_strConferenceId, std::string p_strSponsor, std::string& p_strTarget)
{
	bool l_bRet = false;
	std::string l_strCTICallRefId;

	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj && l_pConferenceObj->isBargein())
	{
		l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();

		p_strTarget = l_pConferenceObj->GetCompere();
		p_strSponsor = l_pConferenceObj->GetBargeinSponsor();
		l_bRet = true;
	}
	ICC_LOG_DEBUG(m_LogPtr, "conferenceID=%s CTICallRefId=%s,l_bRet=%d", p_strConferenceId.c_str(), l_strCTICallRefId.c_str(), l_bRet);
	return l_bRet;
	
}

bool CConferenceManager::IsBargeinCreate(const std::string& p_strConferenceId,std::string &p_strBargeinSponsor)
{
	bool l_bRet = false;
	std::string l_strCTICallRefId;

	boost::shared_ptr<CConference> p_Conference = _GetConferenceObject(p_strConferenceId);
	if (p_Conference)
	{
		if (p_Conference->isBargein())
		{
			l_strCTICallRefId = p_Conference->GetCTICallRefId();
			p_strBargeinSponsor = p_Conference->GetBargeinSponsor();
			l_bRet = true;
		}

	}

	ICC_LOG_DEBUG(m_LogPtr, "conferenceID=%s CTICallRefId=%s,l_bRet=%d", p_strConferenceId.c_str(), l_strCTICallRefId.c_str(), l_bRet);
	return l_bRet;
}

//////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr<CConference> CConferenceManager::_GetConferenceObject(const std::string& p_strConferenceId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(p_strConferenceId);
		if (it != m_mConferenceList.end())
		{
			l_pConferenceObj = it->second;
		}
	}

	return l_pConferenceObj;
}

//咨询成功，合并会议
long CConferenceManager::_ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	long l_lTaskId = DEFAULT_TASKID;

	if (m_SwitchClientPtr)
	{
		l_lTaskId = m_SwitchClientPtr->ConferenceCall(p_strTarget, p_strHeldCTICallRefId, p_strActiveCTICallRefId);

		ICC_LOG_DEBUG(m_LogPtr, "Conference Execute ConferenceCall, HeldCallRefId: %s, ActiveCallRefId: %s Target: %s,TaskId:%d",
			p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), p_strTarget.c_str(), l_lTaskId);
	}

	CHytProtocol::Instance()->DeleteGeneralConsultationInfo(p_strActiveCTICallRefId);
	//	发送话务给MRCC
	CHytProtocol::Instance()->SyncCallRefIdToMrcc(p_strHeldCTICallRefId, p_strActiveCTICallRefId);
	
	return l_lTaskId;
}

long CConferenceManager::_ConsultationCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType)
{
	long l_lTaskId = DEFAULT_TASKID;

	if (m_SwitchClientPtr)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Conference Execute ConsultationCall, HeldCallRefId: %s, Sponsor: %s Target: %s",
			p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		l_lTaskId = m_SwitchClientPtr->ConsultationCall(p_strHeldCTICallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType);
	}

	return l_lTaskId;
}

bool CConferenceManager::_GetConferenceByCTICallRefId(const std::string& p_strCTICallRefId, std::string& p_strConferenceId)
{
	bool l_bRet = false;
	if (p_strCTICallRefId.empty())
	{
		return false;
	}
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	int nSize = m_mConferenceList.size();
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		if (l_oConferenceObj.second->GetCTICallRefId().compare(p_strCTICallRefId) == 0)
		{
			l_bRet = true;
			p_strConferenceId = l_oConferenceObj.first;
			ICC_LOG_DEBUG(m_LogPtr, "GetConferenceByCTICallRefId, ConferenceId:[%s],nSize=%d", p_strConferenceId.c_str(), nSize);
			break;
		}
	}

	return l_bRet;
}

void CConferenceManager::_ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strCalledId,const std::string& p_strCTICallRefId, const std::string& p_strConferenceId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "Not find conference,conferenceId:[%s],DeviceNum:[%s],CTICallRefId:[%s]", p_strConferenceId.c_str(), p_strDeviceNum.c_str(), p_strCTICallRefId.c_str());
		return;
	}

	std::string l_strConferenceId = p_strConferenceId;
	std::string l_strCompere = l_pConferenceObj->GetCompere();
	std::string l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();
	bool l_bActiveCTICall = false;
	std::string l_strTarget = l_pConferenceObj->GetPartyByActiveCTICallRefId(p_strCTICallRefId);
	if (!l_strTarget.empty())
	{
		//为咨询呼叫对应的callid
		l_bActiveCTICall = true;
	}
	else
	{
		l_strTarget = p_strCalledId;
	}

	int l_nResult = 0;
	if (l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
	{
		l_nResult = 1;
		// 同步 m_mapPartys
		bool bRes = l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_RING], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);

	}
	else if (l_bActiveCTICall && l_strCompere.compare(p_strDeviceNum) == 0)
	{
		l_nResult = 2;
		bool bRes = l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_RING], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
	}
	else
	{
		l_nResult = 3;
		if (l_pConferenceObj->FindConferenceParty(p_strDeviceNum))
		{
			l_pConferenceObj->UpdateConferencePartyState(p_strDeviceNum, CallStateString[STATE_RING], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
			l_nResult = 4;
		}
		
	}

	ICC_LOG_DEBUG(m_LogPtr, "process state ring,res:[%d],DeviceNum:[%s],srcCTICallRefId:[%s],CurrCTICallRefId:[%s],conferenceId:[%s],compere:[%s],Target:[%s] ", 
		l_nResult,
		p_strDeviceNum.c_str(),
		l_strCTICallRefId.c_str(),
		p_strCTICallRefId.c_str(),
		l_strConferenceId.c_str(), 
		l_strCompere.c_str(), l_strTarget.c_str());
}

void CConferenceManager::_ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strCalledId,  const std::string& p_strCTICallRefId, const std::string& p_strConferenceId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "Not find conference,conferenceId:[%s],DeviceNum:[%s],CTICallRefId:[%s]", p_strConferenceId.c_str(), p_strDeviceNum.c_str(), p_strCTICallRefId.c_str());
		return;
	}
	long l_nResult = 0;
	std::string l_strConferenceId = p_strConferenceId;
	std::string l_strCompere = l_pConferenceObj->GetCompere();
	std::string l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();
	
	bool l_bActiveCTICall = false;
	std::string l_strTarget = l_pConferenceObj->GetPartyByActiveCTICallRefId(p_strCTICallRefId);
	if (!l_strTarget.empty())
	{
		//为咨询呼叫对应的callid
		l_bActiveCTICall = true;
	}
	else
	{
		l_strTarget = p_strCalledId;
	}

	if (l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
	{
		//单呼话务接通，继续添加下一会议成员
		l_nResult = 1;
		std::string l_strTmpState;
		l_strTmpState = CallStateString[STATE_TALK];
		// 挂断同步 m_mapPartys
		bool bRes = l_pConferenceObj->UpdateConferencePartyState(l_strTarget, l_strTmpState, m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);

	}
	else if (l_bActiveCTICall && l_strCompere.compare(p_strDeviceNum) == 0)
	{
		l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_TALK], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
		// 呼叫手台，无振铃状态,需在通话时合并,会议合并成功后再添加下一会议成员。。。
		long l_lConferenceTaskId = _ConferenceCall(l_strCompere, l_strCTICallRefId, p_strCTICallRefId);

		l_pConferenceObj->SetConferenceTarskId(l_strTarget,p_strCTICallRefId, l_lConferenceTaskId);
		l_nResult = -2;
		if (l_lConferenceTaskId != DEFAULT_TASKID)
		{
			l_nResult = l_lConferenceTaskId;
		}
	}
	else
	{
		l_nResult = -1;
		if (l_pConferenceObj->FindConferenceParty(p_strDeviceNum))
		{
			l_pConferenceObj->UpdateConferencePartyState(p_strDeviceNum, CallStateString[STATE_TALK], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
			l_nResult = 3;
		}
		
	}

	ICC_LOG_DEBUG(m_LogPtr, "process state talk,res:[%d],DeviceNum:[%s],srcCTICallRefId:[%s],currCTICallRefId:[%s],conferenceId:[%s],compere:[%s],Target:[%s]",
		l_nResult,
		p_strDeviceNum.c_str(),
		l_strCTICallRefId.c_str(),
		p_strCTICallRefId.c_str(),
		l_strConferenceId.c_str(),
		l_strCompere.c_str(), l_strTarget.c_str());
}

void CConferenceManager::_ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "Not find conference,conferenceId:[%s],DeviceNum:[%s],CTICallRefId:[%s]", p_strConferenceId.c_str(), p_strDeviceNum.c_str(), p_strCTICallRefId.c_str());
		return;
	}

	std::string l_strConferenceId = p_strConferenceId;
	std::string l_strCompere = l_pConferenceObj->GetCompere();
	std::string l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();
	std::string l_strTarget = l_pConferenceObj->GetPartyByActiveCTICallRefId(p_strCTICallRefId);
	if (!l_strTarget.empty())
	{
		if (m_SwitchClientPtr)
		{
			m_SwitchClientPtr->RetrieveCall(l_strCTICallRefId, l_strCompere, l_strCompere);
		}
		// 同步 m_mapPartys
		bool bRes = l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_HANGUP], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
	}
	else
	{
		// 同步 m_mapPartys
		bool bRes = l_pConferenceObj->UpdateConferencePartyState(p_strDeviceNum, CallStateString[STATE_HANGUP], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
	}
	
	ICC_LOG_DEBUG(m_LogPtr, "process state hangup,DeviceNum:[%s],srcCTICallRefId:[%s],currCTICallRefId:[%s],conferenceId:[%s],compere:[%s],Target:[%s]",
		p_strDeviceNum.c_str(),
		l_strCTICallRefId.c_str(),
		p_strCTICallRefId.c_str(),
		l_strConferenceId.c_str(),
		l_strCompere.c_str(), l_strTarget.c_str());
}

void CConferenceManager::_ProcessHoldorUnholdState(bool p_bIsHold, const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if (l_pConferenceObj == nullptr)
	{
		ICC_LOG_ERROR(m_LogPtr, "Not find conference,conferenceId:[%s],DeviceNum:[%s],CTICallRefId:[%s]", p_strConferenceId.c_str(), p_strDeviceNum.c_str(), p_strCTICallRefId.c_str());
		return;
	}
	int l_nResult = 0;

	std::string l_strConferenceId = p_strConferenceId;
	std::string l_strCompere = l_pConferenceObj->GetCompere();
	std::string l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();

	bool bRes = false;
	std::string l_strState = CallStateString[STATE_HOLD];
	if (!p_bIsHold)
	{
		l_strState = CallStateString[STATE_CANCELHOLD];
	}

	if (p_strCTICallRefId == l_strCTICallRefId)
	{
		l_pConferenceObj->UpdateConferencePartyState(p_strDeviceNum, l_strState, m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);
		bRes = true;
	}
	

	ICC_LOG_DEBUG(m_LogPtr, "process state %s,res:[%d],DeviceNum:[%s],CTICallRefId:[%s],conferenceId:[%s],compere:[%s]",
		l_strState.c_str(),
		l_nResult,
		p_strDeviceNum.c_str(),
		p_strCTICallRefId.c_str(),
		l_strConferenceId.c_str(),
		l_strCompere.c_str());
}


void CConferenceManager::_ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget)
{
	if (m_SwitchClientPtr)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Conference ReconnectCall, AvtiveCallRefId: %s HeldCallRefId: %s Sponsor: %s Target: %s",
			p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		//m_SwitchClientPtr->ReconnectCall(p_strActiveCTICallRefId, p_strHeldCTICallRefId, p_strSponsor, p_strSponsor);
		m_SwitchClientPtr->ReconnectCall(p_strActiveCTICallRefId, p_strHeldCTICallRefId, p_strSponsor, p_strTarget);
	}
}

void CConferenceManager::_ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	if (m_SwitchClientPtr)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Conference ClearCall, CTICallRefId: %s  Sponsor: %s Target: %s",
			p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		m_SwitchClientPtr->ClearCall(p_strCTICallRefId, p_strSponsor, p_strSponsor);
	}
}

void CConferenceManager::_HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	if (m_SwitchClientPtr)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Conference HangupCall, CTICallRefId: %s  Sponsor: %s Target: %s",
			p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		m_SwitchClientPtr->Hangup(p_strCTICallRefId, p_strSponsor, p_strTarget);
	}
}

void CConferenceManager::_ProcessAddConferenceFailedEvent(const std::string& p_strConferenceId, const std::string& p_strActiveCallRefId)
{
	boost::shared_ptr<CConference> l_pConferenceObj = _GetConferenceObject(p_strConferenceId);
	if(l_pConferenceObj)
	{
		std::string l_strCompere = l_pConferenceObj->GetCompere();
		std::string l_strTarget = l_pConferenceObj->GetPartyByActiveCTICallRefId(p_strActiveCallRefId);
		if (!l_strTarget.empty())
		{
			std::string l_strCTICallRefId = l_pConferenceObj->GetCTICallRefId();
			// 同步 m_mapPartys
			l_pConferenceObj->UpdateConferencePartyState(l_strTarget, CallStateString[STATE_HANGUP], m_DateTimePtr->CurrentDateTimeStr(), m_LogPtr);

			//	取回原话务
			_ReconnectCall(p_strActiveCallRefId, l_strCTICallRefId, l_strCompere, l_strCompere);
		}
		
	}

}

//////////////////////////////////////////////////////////////////////////
void CConferenceManager::ResetCheckTime()
{
	m_oLastCheckTime = m_DateTimePtr->CurrentDateTime();
}

bool CConferenceManager::CheckIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_DateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_DateTimePtr->AddSeconds(m_oLastCheckTime, CHECK_TIMESPAN);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}

void CConferenceManager::ProcessRingTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall)
{
	//	振铃超时，应答该话务再挂断，该操作只对内部话机有效
	//_AnswerCall(p_strConferenceId);
}

bool CConferenceManager::FindRingTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall)
{
	bool b_lRingTimeOut = false;
	/*
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		std::string l_strConferenceId = l_oConferenceObj.first;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();
		std::string l_strTargetState = l_oConferenceObj.second->GetCurrentPartyState();
		std::string l_strRingTime = l_oConferenceObj.second->m_strRingTime;

		if (l_strTargetState.compare(CallStateString[STATE_RING]) == 0)
		{
			DateTime::CDateTime l_CurrentTime = m_DateTimePtr->CurrentDateTime();
			DateTime::CDateTime l_RingTime = m_DateTimePtr->FromString(l_strRingTime);
			DateTime::CDateTime l_EndTime = m_DateTimePtr->AddSeconds(l_RingTime, m_nRingTimeout);

			if (l_CurrentTime > l_EndTime)
			{
				b_lRingTimeOut = true;
				p_strConferenceId = l_strConferenceId;
				p_bIsMakeCall = false;

				//	重置振铃事件，避免重复检测
				l_oConferenceObj.second->m_strRingTime = m_DateTimePtr->CurrentDateTimeStr();

				ICC_LOG_DEBUG(m_LogPtr, "Conference Party: [%s] Ring Timeout !", l_strTarget.c_str());
			}

			break;
		}
	}
	*/
	return b_lRingTimeOut;
}

void CConferenceManager::DoCheckRingTime()
{
	ICC_LOG_DEBUG(m_LogPtr, "================= Begin Conference CheckRingTime Thread ! =================");

	while (CConferenceManager::Instance()->m_bThreadAlive)
	{
		if (CConferenceManager::Instance()->CheckIsTimeout())
		{
			bool l_bIsMakeCall = false;
			std::string l_strConferenceId = "";

			while (CConferenceManager::Instance()->FindRingTimeout(l_strConferenceId, l_bIsMakeCall))
			{
				CConferenceManager::Instance()->ProcessRingTimeout(l_strConferenceId, l_bIsMakeCall);
			}

			CConferenceManager::Instance()->ResetCheckTime();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, "-------------------- End Conference CheckRingTime Thread! --------------------");
}

void CConferenceManager::StartThread()
{
	if (!m_bThreadAlive)
	{
		m_bThreadAlive = true;

		m_pCheckRingTimeoutThread = boost::make_shared<boost::thread>(boost::bind(&CConferenceManager::DoCheckRingTime, this));
	}
}

void CConferenceManager::StopThread()
{
	if (m_bThreadAlive)
	{
		m_bThreadAlive = false;

		if (m_pCheckRingTimeoutThread)
		{
			m_pCheckRingTimeoutThread->join();
		}
	}
}

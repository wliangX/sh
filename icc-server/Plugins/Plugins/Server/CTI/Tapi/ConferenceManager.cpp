#include "Boost.h"

#include "ConferenceManager.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include "HytProtocol.h"
#include "DeviceManager.h"
#include "CallManager.h"
#define CHECK_TIMESPAN				1		//单位：秒
#define SLEEP_TIME					100		//单位：毫秒
#define CONFERENCE_DIAL_TIMEOUT		10		//单位：秒

CConference::CConference()
{
	m_bIsMakeCall = false;
	m_bDoingConference = false;
	m_bAddResult = true;

	m_lMakeCallTaskId = DEFAULT_TASKID;
	m_lConsultationCallTaskId = DEFAULT_TASKID;
	m_lConferenceCallTaskId = DEFAULT_TASKID;
	m_lAnswerCallTaskId = DEFAULT_TASKID;
	m_lRetrieveCallTaskId = DEFAULT_TASKID;
	m_nCurrentPartyIndex = 0;

	m_strCompere = "";
	m_strCTICallRefId = "";
	m_strActiveCTICallRefId = "";
	m_strConferenceId = "";
	m_strDialTime = "";
	m_strRingTime = "";
	m_strTargetState = "";
}
CConference::~CConference()
{
	//
}

void CConference::AddConferenceParty(std::vector<CConferenceParty>& p_vPartyQueue)
{
	for (std::size_t i = 0; i < p_vPartyQueue.size(); ++i)
	{
		m_vPartyQueue.push_back(p_vPartyQueue[i]);
	}
}
void CConference::DeleteConferenceParty(const std::string& p_strDeviceNum)
{
	auto it = m_vPartyQueue.begin();
	while (it != m_vPartyQueue.end())
	{
		if (it->m_strTarget.compare(p_strDeviceNum) == 0)
		{
			it->m_strDisEnable = "1";
			it->m_strCallState =  CallStateString[STATE_HANGUP];
			//m_vPartyQueue.erase(it);

			break;
		}

		++it;
	}
}
void CConference::ClearConferenceParty()
{
	m_vPartyQueue.clear();
}
bool CConference::FindConferenceParty(const std::string& p_strDeviceNum)
{
	bool l_bRet = false;

	for each (auto l_oPartyObj in m_vPartyQueue)
	{
		if (l_oPartyObj.m_strTarget.compare(p_strDeviceNum) == 0)
		{
			l_bRet = true;
			break;
		}
	}

	return l_bRet;
}
std::size_t CConference::GetConferencePartyCount()
{
	return m_vPartyQueue.size();
}
bool CConference::AddConferencePartyIsOver()
{
	return (m_nCurrentPartyIndex >= m_vPartyQueue.size());
}

std::string CConference::GetCurrentParty()
{
	std::string l_strTarget = "";
	if (m_nCurrentPartyIndex < m_vPartyQueue.size())
	{
		l_strTarget = m_vPartyQueue[m_nCurrentPartyIndex].m_strTarget;
	}

	return l_strTarget;
}
bool CConference::GetCurrentParty(std::string& p_strTarget, std::string& p_strTargetType)
{
	if (m_nCurrentPartyIndex < m_vPartyQueue.size())
	{
		p_strTarget = m_vPartyQueue[m_nCurrentPartyIndex].m_strTarget;
		p_strTargetType = m_vPartyQueue[m_nCurrentPartyIndex].m_strTargetDeviceType;
	} 
	else
	{
		return false;
	}
	
	return true;
}


bool CConference::SetConferencePartyState(const std::string& p_strDeviceNum, const string& p_strState)
{
	bool bRes = false;
	auto it = m_vPartyQueue.begin();
	while (it != m_vPartyQueue.end())
	{
		if (it->m_strTarget.compare(p_strDeviceNum) == 0)
		{
			it->m_strCallState = p_strState;
			//break;
			bRes = true;
		}

		++it;
	}
	if (!bRes)
	{
		if (p_strDeviceNum == m_strSrcCaller)
		{
			m_strSrcCallerState = p_strState;
			bRes = true;
		}
		else if (p_strDeviceNum == m_strSrcCalled)
		{
			m_strSrcCalledState = p_strState;
			bRes = true;
		}
	}
	return bRes;
}

bool CConference::SetDisenableParty(const std::string& p_strTarget, bool p_bDisEnable)
{
	bool bRes = false;

	std::string l_strDisEnable;
	if (p_bDisEnable)
	{
		l_strDisEnable = "1";
	}
	else
	{
		l_strDisEnable = "0";
	}

	auto it = m_vPartyQueue.begin();
	while (it != m_vPartyQueue.end())
	{
		if (it->m_strTarget.compare(p_strTarget) == 0)
		{
			it->m_strDisEnable = l_strDisEnable;
			//break;
			bRes = true;
		}

		++it;
	}

	if (!bRes)
	{
		if (p_strTarget == m_strSrcCaller)
		{
			m_strSrcCallerDisEnable = l_strDisEnable;
			bRes = true;
		}
		else if (p_strTarget == m_strSrcCalled)
		{
			m_strSrcCalledDisEnable = l_strDisEnable;
			bRes = true;
		}
	}
	return bRes;
}

bool CConference::GetPartyList(std::map<std::string, CConferenceParty>& p_ConferencePartyList, std::string& p_strLastTarget, DateTime::IDateTimePtr p_DateTimePtr)
{
	std::string l_strLastTime;
	std::string l_strLastTarget = "";
	for (int i = 0; i < m_vPartyQueue.size(); i++)
	{
		p_ConferencePartyList[m_vPartyQueue[i].m_strTarget].m_strTargetDeviceType = m_vPartyQueue[i].m_strTargetDeviceType;
		p_ConferencePartyList[m_vPartyQueue[i].m_strTarget].m_strAddTime = m_vPartyQueue[i].m_strAddTime;
		p_ConferencePartyList[m_vPartyQueue[i].m_strTarget].m_strCallState = m_vPartyQueue[i].m_strCallState;
		p_ConferencePartyList[m_vPartyQueue[i].m_strTarget].m_strTarget = m_vPartyQueue[i].m_strTarget;
		p_ConferencePartyList[m_vPartyQueue[i].m_strTarget].m_strDisEnable = m_vPartyQueue[i].m_strDisEnable;
		if (l_strLastTime.empty())
		{
			l_strLastTime = m_vPartyQueue[i].m_strAddTime;
			l_strLastTarget = m_vPartyQueue[i].m_strTarget;
		}
		else
		{
			if (p_DateTimePtr->FromString(l_strLastTime) < p_DateTimePtr->FromString(m_vPartyQueue[i].m_strAddTime))
			{
				l_strLastTime = m_vPartyQueue[i].m_strAddTime;
				l_strLastTarget = m_vPartyQueue[i].m_strTarget;
			}
		}
	}
	p_strLastTarget = l_strLastTarget;

	std::string l_strState;
	PROTOCOL::CGetConferencePartyRespond::CBody::CData l_tmpCData;
	//主叫
	if (p_ConferencePartyList.find(m_strSrcCaller) == p_ConferencePartyList.end())
	{
		p_ConferencePartyList[m_strSrcCaller].m_strTarget = m_strSrcCaller;
		p_ConferencePartyList[m_strSrcCaller].m_strAddTime = m_strCreateTime;
		CDeviceManager::Instance()->GetDeviceState(m_strSrcCaller);
		if (l_strState.empty())
		{
			p_ConferencePartyList[m_strSrcCaller].m_strTargetDeviceType = TARGET_DEVICE_TYPE_OUTSIDE;
		}
		else
		{
			p_ConferencePartyList[m_strSrcCaller].m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
		}
		p_ConferencePartyList[m_strSrcCaller].m_strCallState = m_strSrcCallerState;
		p_ConferencePartyList[m_strSrcCaller].m_strDisEnable = m_strSrcCallerDisEnable;
	}

	//被叫
	if (p_ConferencePartyList.find(m_strSrcCalled) == p_ConferencePartyList.end())
	{
		p_ConferencePartyList[m_strSrcCalled].m_strTarget = m_strSrcCalled;
		p_ConferencePartyList[m_strSrcCalled].m_strAddTime = m_strCreateTime;

		l_strState = CDeviceManager::Instance()->GetDeviceState(m_strSrcCalled);
		if (l_strState.empty())
		{
			p_ConferencePartyList[m_strSrcCalled].m_strTargetDeviceType = TARGET_DEVICE_TYPE_OUTSIDE;
		}
		else
		{
			p_ConferencePartyList[m_strSrcCalled].m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
		}
		p_ConferencePartyList[m_strSrcCalled].m_strCallState = m_strSrcCalledState;
		p_ConferencePartyList[m_strSrcCalled].m_strDisEnable = m_strSrcCalledDisEnable;
	}

	//主持人
	if (p_ConferencePartyList.find(m_strCompere) == p_ConferencePartyList.end())
	{
		p_ConferencePartyList[m_strCompere].m_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
		p_ConferencePartyList[m_strCompere].m_strAddTime = m_strCreateTime;
		p_ConferencePartyList[m_strCompere].m_strTarget = m_strCompere;

		l_strState = CDeviceManager::Instance()->GetDeviceState(m_strCompere);
		p_ConferencePartyList[m_strCompere].m_strCallState = l_strState;
	}
	return true;
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
	m_pLog = nullptr;
	m_pDateTime = nullptr;
	m_pStrUtil = nullptr;

	m_nRingTimeout = DEFAULT_RING_TIMEOUT;
}

CConferenceManager::~CConferenceManager(void)
{
	//
}

//////////////////////////////////////////////////////////////////////////
std::string CConferenceManager::CreateNewConference(const std::string& p_strCompere, std::vector<CConferenceParty>& p_vPartyQueue, const std::string& p_strCTICallRefId)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));
	std::string l_strConferenceId = std::string(l_szDayTime);

	boost::shared_ptr<CConference> l_pConference = boost::make_shared<CConference>();
	/////////////////////////////////////////////////
	l_pConference->SetCreateTime(m_pDateTime->CurrentDateTimeStr());

	std::string l_strTmpCalled;
	{
		std::string l_strOutCallerId;
		CCallManager::Instance()->GetCallByCTICallRefId(p_strCTICallRefId, l_strOutCallerId, l_strTmpCalled);
		l_pConference->m_strSrcCaller = l_strOutCallerId;
	}

	//l_pConference->m_strSrcCaller = m_SwitchClientPtr->GetCallerID(p_strCTICallRefId, l_strTmpCalled);
	std::string l_strTmpState = CDeviceManager::Instance()->GetDeviceState(l_strTmpCalled);

	l_pConference->m_strSrcCalled = l_strTmpCalled;
	l_pConference->m_strSrcCallerState = l_strTmpState;
	l_pConference->m_strSrcCalledState = l_strTmpState;

	//l_pConference->m_bBargein = p_bBargein;
	l_pConference->m_bBargein = false;
	/*
	if (p_bBargein && !p_vPartyQueue.empty())
	{
		l_pConference->m_strBargeinSponsor = p_vPartyQueue[0].m_strTarget;
	}*/

	l_pConference->m_strCompere = p_strCompere;
	l_pConference->m_vPartyQueue = p_vPartyQueue;
	l_pConference->m_strCTICallRefId = p_strCTICallRefId;
	l_pConference->m_strConferenceId = l_strConferenceId;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	m_mConferenceList[l_strConferenceId] = l_pConference;
	//////////////////////////////////////////////
	/*
	l_pConference->m_strCompere = p_strCompere;
	l_pConference->m_vPartyQueue = p_vPartyQueue;
	l_pConference->m_strCTICallRefId = p_strCTICallRefId;
	l_pConference->m_strConferenceId = l_strConferenceId;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	m_mConferenceList[l_strConferenceId] = l_pConference;
	*/
	int nSize = m_mConferenceList.size();
	int nPartySize = p_vPartyQueue.size();
	ICC_LOG_DEBUG(m_pLog, "Create New Conference, CTICallRefId: [%s] ConferenceId: [%s] Compere: [%s],ConfSize=%d,PartySize=%d ,SrcCaller=%s.srcCalled=%s,bBargein=%d",
		p_strCTICallRefId.c_str(), l_strConferenceId.c_str(), p_strCompere.c_str(), nSize, nPartySize, l_pConference->m_strSrcCaller.c_str(), l_strTmpCalled.c_str(), l_pConference->m_bBargein);

	return l_strConferenceId;
}

std::string CConferenceManager::AddConferenceParty(const std::string& p_strCTICallRefId, const std::string& p_strCompere, std::vector<CConferenceParty>& p_vPartyQueue)
{
	//	在已有通话或会议上新增会议成员
	std::string l_strConferenceId;

	if (!GetConferenceByCTICallRefId(p_strCTICallRefId, l_strConferenceId))
	{
		//	会议未存在，先建立
		l_strConferenceId = CreateNewConference(p_strCompere, p_vPartyQueue, p_strCTICallRefId);
	}
	else
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(l_strConferenceId);
		if (it != m_mConferenceList.end())
		{
			if (it->second)
			{
				it->second->AddConferenceParty(p_vPartyQueue);
			}
		}
	}

	if (!IsDoingConference(l_strConferenceId))
	{
		//	未处于执行新增会议成员过程中，开始新增会议成员
		ConsultationCallEx(l_strConferenceId);
	}

	return l_strConferenceId;
}

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
		ICC_LOG_DEBUG(m_pLog, "GetConferenceParty Not find, ConferenceId:[%s],nSize=%d", p_strConferenceId.c_str(), nSize);
		return false;
	}

	p_oOutRespond.m_oBody.m_strCount = "0";
	p_oOutRespond.m_oBody.m_strCompere = l_pConferenceObj->m_strCompere;
	std::string l_strLastTarget;
	std::map<std::string, CConferenceParty> tmp_PartyTable;
	if (l_pConferenceObj->GetPartyList(tmp_PartyTable, l_strLastTarget, m_pDateTime))
	{
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
	}
	else
	{
		return false;
	}

	p_oOutRespond.m_oBody.m_strCount = std::to_string(p_oOutRespond.m_oBody.m_vecData.size());
	ICC_LOG_DEBUG(m_pLog, "GetConferenceParty find OK, ConferenceId:[%s],CallRefId:[%s],nSize=%d,Caller:[%s],strCompere:[%s],Called:[%s]", p_strConferenceId.c_str(),
		l_pConferenceObj->m_strCTICallRefId.c_str(), nSize,
		l_pConferenceObj->m_strSrcCaller.c_str(),
		l_pConferenceObj->m_strCompere.c_str(),
		l_pConferenceObj->m_strSrcCalled.c_str());

	return true;
}


bool CConferenceManager::DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty)
{
	std::string l_strCompere = "";
	std::string l_strCallRefId = "";

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			return it->second->SetDisenableParty(p_strParty, true);
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Not find Conference, ConferenceId: [%s] ", p_strConferenceId.c_str());
	}
	return false;
}


bool CConferenceManager::IsConferenceExist(const std::string& p_strCompere)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strCompere);
	if (it != m_mConferenceList.end())
	{
		return true;
	}

	return false;
}

bool CConferenceManager::IsConferenceOver(const std::string& p_strCompere)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strCompere);
	if (it != m_mConferenceList.end())
	{
		if (it->second->m_vPartyQueue.empty())
		{
			return true;
		}
	}

	return false;
}

void CConferenceManager::DeleteConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty)
{
	std::string l_strCompere = "";
	std::string l_strCallRefId = "";

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			l_strCompere = it->second->m_strCompere;
			l_strCallRefId = it->second->m_strCTICallRefId;

			this->HangupCall(l_strCallRefId, l_strCompere, p_strParty);
		}
	}
}


bool CConferenceManager::CR_DeleteConferenceParty(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strHangupDevice)
{
	bool l_bRet = false;
	string l_strID;
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCompere.compare(p_strHangupDevice) == 0 || l_oConferenceObj.second->FindConferenceParty(p_strHangupDevice))
		{
			l_bRet = true;
			l_oConferenceObj.second->DeleteConferenceParty(p_strHangupDevice);
			l_strID = l_oConferenceObj.second->m_strConferenceId;
			break;
		}
	}
	ICC_LOG_DEBUG(m_pLog, "Delete Conference Party: [%s],ConferenceID:[%s] ", p_strHangupDevice.c_str(), l_strID.c_str());

	return l_bRet;
}


//	会议结束时，删除会议
void CConferenceManager::DeleteConference(const std::string& p_strCTICallRefId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCTICallRefId.compare(p_strCTICallRefId) == 0)
		{
			m_mConferenceList.erase(l_oConferenceObj.first);

			ICC_LOG_DEBUG(m_pLog, "Delete Conference, CTICallRefId: [%s] ",
				p_strCTICallRefId.c_str());

			break;
		}
	}
}
void CConferenceManager::ClearConferenceList()
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	m_mConferenceList.clear();
}

bool CConferenceManager::FindConference(const std::string& p_strCTICallRefId, const std::string& p_strCompere)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCompere.compare(p_strCompere) == 0 &&
			(l_oConferenceObj.second->m_strCTICallRefId.compare(p_strCTICallRefId) == 0 ||
			l_oConferenceObj.second->m_strActiveCTICallRefId.compare(p_strCTICallRefId) == 0))
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}
bool CConferenceManager::FindConferenceByCTICallRefId(const std::string& p_strCTICallRefId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCTICallRefId.compare(p_strCTICallRefId) == 0)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

bool CConferenceManager::FindConferenceByMakeCallTaskId(const long p_lTaskId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_lMakeCallTaskId == p_lTaskId)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

bool CConferenceManager::FindConferenceByConsultationCallTaskId(const long p_lTaskId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_lConsultationCallTaskId == p_lTaskId)
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
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_lConferenceCallTaskId == p_lTaskId)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}
bool CConferenceManager::FindConferenceByAnswerCallTaskId(const long p_lTaskId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_lAnswerCallTaskId == p_lTaskId)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}

/*
bool CConferenceManager::FindConferenceByHeldCallRefId(const std::string& p_strHeldCTICallRefId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCTICallRefId.compare(p_strHeldCTICallRefId) == 0)
		{
			l_bRet = true;

			break;
		}
	}

	/ *auto it = m_mConferenceList.begin();
	while (it != m_mConferenceList.end())
	{
		if (it->second && it->second->m_strCTICallRefId.compare(p_strHeldCTICallRefId) == 0)
		{
			l_bRet = true;

			break;
		}

		++it;
	}* /

	return l_bRet;
}*/
bool CConferenceManager::FindConferenceByActiveCallRefId(const std::string& p_strActiveCTICallRefId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strActiveCTICallRefId.compare(p_strActiveCTICallRefId) == 0)
		{
			l_bRet = true;

			break;
		}
	}

	return l_bRet;
}
bool CConferenceManager::GetConferenceByCTICallRefId(const std::string& p_strCTICallRefId, std::string& p_strConferenceId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCTICallRefId.compare(p_strCTICallRefId) == 0)
		{
			l_bRet = true;
			p_strConferenceId = l_oConferenceObj.first;

			break;
		}
	}

	return l_bRet;
}

bool CConferenceManager::GetCTICallRefIdByActiveCallRefId(const std::string& p_strActiveCTICallRefId, std::string& p_strCTICallRefId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strActiveCTICallRefId.compare(p_strActiveCTICallRefId) == 0)
		{
			l_bRet = true;
			p_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;

			break;
		}
	}

	return l_bRet;
}
bool CConferenceManager::GetCTICallRefIdByParty(const std::string& p_strParty, std::string& p_strCTICallRefId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		if (l_oConferenceObj.second->m_strCompere.compare(p_strParty) == 0 || l_oConferenceObj.second->FindConferenceParty(p_strParty))
		{
			l_bRet = true;
			p_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;

			break;
		}
	}

	return l_bRet;
}


void CConferenceManager::SetActiveCallRefId(const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		std::string l_strConferenceId = l_oConferenceObj.first;
		std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();
		std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;

		if (l_strCTICallRefId.compare(p_strHeldCTICallRefId) == 0)
		{
			l_oConferenceObj.second->m_strActiveCTICallRefId = p_strActiveCTICallRefId;

			ICC_LOG_DEBUG(m_pLog, "Conference: %s Set ActiveCTICallRefId: %s, Compere: %s, Target: %s, CTICallRefId: %s",
				l_strConferenceId.c_str(), p_strActiveCTICallRefId.c_str(), l_strCompere.c_str(), l_strTarget.c_str(), l_strCTICallRefId.c_str());

			break;
		}
	}
}
bool CConferenceManager::GetAddCurrentPartyResult(const std::string& p_strCompere, const std::string& p_strActiveCTICallRefId)
{
	bool l_bResult = true;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
		std::string l_strActiveCTICallRefId = l_oConferenceObj.second->m_strActiveCTICallRefId;

		if (l_strCompere.compare(p_strCompere) == 0 && l_strActiveCTICallRefId.compare(p_strActiveCTICallRefId) == 0)
		{
			l_bResult = l_oConferenceObj.second->m_bAddResult;

			break;
		}
	}

	return l_bResult;
}

//////////////////////////////////////////////////////////////////////////
/*
void CConferenceManager::ProcessDialState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId)
{
	// TAPI 事件无法直接获取 MakeCall 产生的话务ID，因此通过主叫来做关联
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		bool l_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;
		std::string l_strConferenceId = l_oConferenceObj.first;
		std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();

		if (l_bIsMakeCall && l_strCompere.compare(p_strDeviceNum) == 0)
		{
			l_oConferenceObj.second->m_strCTICallRefId = p_strCTICallRefId;
			ICC_LOG_DEBUG(m_pLog, "Set Compere: %s, Target: %s, MakeCall CTICallRefId: %s", 
				l_strCompere.c_str(), l_strTarget.c_str(), p_strCTICallRefId.c_str());

			break;
		}
	}
}*/
void CConferenceManager::ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		std::string l_strConferenceId = l_oConferenceObj.first;
		bool l_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;
		std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();
		std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
		std::string l_strActiveCTICallRefId = l_oConferenceObj.second->m_strActiveCTICallRefId;

		if (l_bIsMakeCall && l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
		{
			// 单呼振铃，会议发起者处于回铃态
			l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_RING];
			l_oConferenceObj.second->m_strRingTime = m_pDateTime->CurrentDateTimeStr();
			ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_RING].c_str());

			//设置会员成员状态
			l_oConferenceObj.second->SetConferencePartyState(l_strTarget, CallStateString[STATE_RING]);
			//	通知客户端，会议成员振铃
			CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
				l_strCompere, l_strTarget, CallStateString[STATE_RING], Result_Success);

			break;
		}
		else if (!l_bIsMakeCall && l_strActiveCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
		{
			// 会议发起者处于回铃态，并且话务 ID 与 咨询产生的话务 ID 一致,合并会议
			long l_lConferenceTaskId = ConferenceCall(l_strCompere, l_strCTICallRefId, l_strActiveCTICallRefId);

			l_oConferenceObj.second->m_lConferenceCallTaskId = l_lConferenceTaskId;
			l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_RING];
			l_oConferenceObj.second->m_strRingTime = m_pDateTime->CurrentDateTimeStr();
			ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_RING].c_str());

			//设置会员成员状态
			l_oConferenceObj.second->SetConferencePartyState(l_strTarget, CallStateString[STATE_RING]);
			//	通知客户端，会议成员振铃
			CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,l_strCompere, l_strTarget, CallStateString[STATE_RING], Result_Success);

			break;
		}
		else
		{
			//
		}
	}
}
void CConferenceManager::ProcessTalkState(const std::string& p_strDeviceNum,/* const std::string& p_strRelateDeviceNum,*/ const std::string& p_strCTICallRefId)
{
	int l_nResult = 0;

	bool l_bIsMakeCall = false;
	std::string l_strConferenceId = "";
	std::string l_strCompere = "";
	std::string l_strTarget = "";
	std::string l_strCTICallRefId = "";
	std::string l_strHeldCTICallRefId = "";
	std::string l_strActiveCTICallRefId = "";

	boost::shared_ptr<CConference> l_pConferenceObj;
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			l_strConferenceId = l_oConferenceObj.first;
			l_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;
			l_strCompere = l_oConferenceObj.second->m_strCompere;
			l_strTarget = l_oConferenceObj.second->GetCurrentParty();
			l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
			l_strActiveCTICallRefId = l_oConferenceObj.second->m_strActiveCTICallRefId;

			if (l_bIsMakeCall && l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
			{
				//单呼话务接通，继续添加下一会议成员
				l_nResult = 1;

				// 下一轮呼叫
				l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_TALK];
				l_oConferenceObj.second->m_nCurrentPartyIndex++;
				ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_TALK].c_str());

				l_pConferenceObj = l_oConferenceObj.second;
				break;
			}
			else if (!l_bIsMakeCall && l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strTarget.compare(p_strDeviceNum) == 0)
			{
				l_nResult = 2;
				//	咨询话务接通，继续添加下一会议成员
				// 下一轮呼叫，取最新的话务 ID

				l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_CONFERENCE];
				l_oConferenceObj.second->m_nCurrentPartyIndex++;
				ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_CONFERENCE].c_str());

				l_pConferenceObj = l_oConferenceObj.second;
				break;
			}
			else if (!l_bIsMakeCall && l_strActiveCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
			{
				std::string l_strTargetState = l_oConferenceObj.second->m_strTargetState;
				if (l_strTargetState.compare(CallStateString[STATE_RING]) != 0)
				{
					l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_TALK];
					ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_TALK].c_str());

					// 呼叫手台，无振铃状态,需在通话时合并,会议合并成功后再添加下一会议成员。。。
					long l_lConferenceTaskId = ConferenceCall(l_strCompere, l_strCTICallRefId, l_strActiveCTICallRefId);
					l_oConferenceObj.second->m_lConferenceCallTaskId = l_lConferenceTaskId;
				}

				break;
			}
			else
			{
				//
			}
		}
	}

	if (l_nResult > 0)
	{
		std::string l_strTmpState;
		if (l_pConferenceObj)
		{
			if (l_pConferenceObj->AddConferencePartyIsOver())
			{
				l_strTmpState = CallStateString[STATE_TALK];
			}
			else
			{
				l_strTmpState = CallStateString[STATE_CONFERENCE];
			}
		}
		
		bool bRes = l_pConferenceObj->SetConferencePartyState(l_strTarget, l_strTmpState);
		ICC_LOG_DEBUG(m_pLog, "SetConferencePartyState res:[%d],state:[%s],Target:[%s],p_strDeviceNum:[%s],l_nResult:%d", bRes, l_strTmpState.c_str(),
			l_strTarget.c_str(), p_strDeviceNum.c_str(), l_nResult);

		//	通知客户端，会议成员添加成功
		CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
			l_strCompere, l_strTarget, CallStateString[STATE_CONFERENCE], Result_Success);

		ConsultationCallEx(l_strConferenceId);
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "ProcessTalkState,p_strDeviceNum:[%s],p_strCTICallRefId:[%s],l_nResult=%d", p_strDeviceNum.c_str(), p_strCTICallRefId.c_str(), l_nResult);
	}
}

void CConferenceManager::ProcessConferenceHangup(const std::string& p_strHangupDeviceNum, const std::string& p_strCTICallRefId)
{
	std::string l_strConferenceId = "";
	std::string l_strCompere = "";
	std::string l_strCTICallRefId = "";

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	int nFindFlag = -1;
	for (auto l_oConferenceObj : m_mConferenceList)
	{
		nFindFlag = 0;
		l_strConferenceId = l_oConferenceObj.first;
		l_strCompere = l_oConferenceObj.second->m_strCompere;
		l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
		if (l_strCTICallRefId.compare(p_strCTICallRefId) == 0)
		{
			if (l_oConferenceObj.second->FindConferenceParty(p_strHangupDeviceNum))
			{
				nFindFlag = 1;
			}
			else
			{
				if (l_oConferenceObj.second->m_strSrcCaller == p_strHangupDeviceNum || l_oConferenceObj.second->m_strSrcCalled == p_strHangupDeviceNum)
				{
					nFindFlag = 1;
				}
			}

			if (nFindFlag == 1)
			{
				l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_HANGUP];
				bool bRes = l_oConferenceObj.second->SetConferencePartyState(p_strHangupDeviceNum, CallStateString[STATE_HANGUP]);

				ICC_LOG_DEBUG(m_pLog, "SetConferencePartyState res:[%d],state:[%s],Target:[%s],strCompere:[%s]", bRes, CallStateString[STATE_HANGUP].c_str(),
					p_strHangupDeviceNum.c_str(), l_strCompere.c_str());
				//	通知客户端，会议成员挂机
				CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
					l_strCompere, p_strHangupDeviceNum, CallStateString[STATE_HANGUP], Result_Success);

				break;
			}
		}
	}
	if (nFindFlag != 1)
	{
		ICC_LOG_ERROR(m_pLog, "Not find,strHangupDeviceNum:%s,p_strCTICallRefId:%s,nFindFlag=%d", p_strHangupDeviceNum.c_str(), p_strCTICallRefId.c_str(), nFindFlag);
	}
}

void CConferenceManager::ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId)
{
	bool l_bCallNextParty = false;
	bool l_bForcePopCall = false;
	bool l_bConsultationNextParty = false;
	std::string l_strConferenceId = "";
	std::string l_strCompere = "";
	std::string l_strTarget = "";
	std::string l_strCTICallRefId = "";
	std::string l_strActiveCTICallRefId = "";
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			l_strConferenceId = l_oConferenceObj.first;

			bool l_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;
			bool l_bDoingConference = l_oConferenceObj.second->m_bDoingConference;
			l_strCompere = l_oConferenceObj.second->m_strCompere;
			l_strTarget = l_oConferenceObj.second->GetCurrentParty();
			l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
			l_strActiveCTICallRefId = l_oConferenceObj.second->m_strActiveCTICallRefId;

			ICC_LOG_DEBUG(m_pLog, "ProcessHangupState,ConferenceId:[%s],bIsMakeCall:[%d],CTICallRefId:[%s],strCompere:[%s]", l_strConferenceId.c_str(), 
				l_bIsMakeCall, l_strCTICallRefId.c_str(), l_strCompere.c_str());

			if (l_bIsMakeCall && l_strCTICallRefId.compare(p_strCTICallRefId) == 0 && l_strCompere.compare(p_strDeviceNum) == 0)
			{
				//	建立新话务失败，需挂机后才能继续呼叫下一成员
				l_bCallNextParty = true;
			}
			else if (!l_bIsMakeCall && l_strCTICallRefId.compare(p_strCTICallRefId) == 0)
			{
				if (p_strDeviceNum.compare(l_strCompere) == 0 && 
					(/*l_oConferenceObj.second->m_strTargetState.compare(CallStateString[STATE_RING]) == 0 ||*/
					l_oConferenceObj.second->m_strTargetState.compare(CallStateString[STATE_CONFERENCE]) == 0) )
				{
					l_bForcePopCall = true;
					l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_HANGUP];
				}

				bool bRes = l_oConferenceObj.second->SetConferencePartyState(p_strDeviceNum, CallStateString[STATE_HANGUP]);
				ICC_LOG_DEBUG(m_pLog, "ProcessHangupState,isForcePopCall:[%d] Target: %s State: %s,bRes:%d", l_bForcePopCall, l_strTarget.c_str(), CallStateString[STATE_HANGUP].c_str(), bRes);
			//	l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_HANGUP];
				//	通知客户端，会议成员挂机
				CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
					l_strCompere, p_strDeviceNum, CallStateString[STATE_HANGUP], Result_Success);

				break;
			}
			else if (!l_bIsMakeCall && l_strActiveCTICallRefId.compare(p_strCTICallRefId) == 0 && p_strDeviceNum.compare(l_strCompere) == 0)
			{
				// 下一轮呼叫
				l_bConsultationNextParty = true;
				l_oConferenceObj.second->m_nCurrentPartyIndex++;

				l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_HANGUP];
				bool bRes = l_oConferenceObj.second->SetConferencePartyState(p_strDeviceNum, CallStateString[STATE_HANGUP]);
				ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s,bRes:%d", l_strTarget.c_str(), CallStateString[STATE_HANGUP].c_str(), bRes);

				break;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "----------ProcessHangupState,device:[%s],p_strCTICallRefId:[%s],l_strConferenceId:[%s]", p_strDeviceNum.c_str(), p_strCTICallRefId.c_str(), l_strConferenceId.c_str());
			}
		}
	}
	
	if (l_bForcePopCall)
	{
		//	主持人挂机，强拆会议
		this->ClearCall(l_strConferenceId);
	}


	if (l_bConsultationNextParty)
	{
		this->ConsultationCallEx(l_strConferenceId);
	}
}

void CConferenceManager::DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strDeviceState, const std::string& p_strCTICallRefId)
{
	if (p_strDeviceState.compare(CallStateString[STATE_SIGNALBACK]) == 0)
	{
		this->ProcessRingState(p_strDeviceNum, p_strCTICallRefId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_CONFERENCE]) == 0 || p_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
	{
		this->ProcessTalkState(p_strDeviceNum, p_strCTICallRefId);
	}
	else if (p_strDeviceState.compare(CallStateString[STATE_HANGUP]) == 0)
	{
		// 加入会议前挂机，失败
		this->ProcessHangupState(p_strDeviceNum, p_strCTICallRefId);
	}
	else
	{
		//
	}
}

bool CConferenceManager::IsDoingConference(const std::string& p_strConferenceId)
{
	bool l_bRet = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second && it->second->m_bDoingConference)
		{
			l_bRet = true;
		}
	}

	return l_bRet;
}

void CConferenceManager::ProcessMakeCallResult(long p_lTaskId, const std::string& p_strCTICallRefId, bool p_bResult)
{
	if (p_bResult)
	{
		// 会议主持人发起单呼成功，等待被叫方振铃
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			if (l_oConferenceObj.second->m_lMakeCallTaskId == p_lTaskId)
			{
				l_oConferenceObj.second->m_strCTICallRefId = p_strCTICallRefId;
				ICC_LOG_DEBUG(m_pLog, "TaskId: %u, MakeCall Success, Set CTICallRefId: %s", p_lTaskId, p_strCTICallRefId.c_str());

				break;
			}
		}
	}
	else
	{
		//	单呼失败，继续呼叫下一会议成员
		std::string l_strConferenceId = "";

		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			for each (auto l_oConferenceObj in m_mConferenceList)
			{
				if (l_oConferenceObj.second->m_lMakeCallTaskId == p_lTaskId)
				{
					l_strConferenceId = l_oConferenceObj.second->m_strConferenceId;

					break;
				}
			}
		}

		if (!l_strConferenceId.empty())
		{
			ProcessMakeCallFailedEvent(l_strConferenceId);
		}
	}
}

void CConferenceManager::ConsultationCallEx(const std::string& p_strConferenceId)
{
	//	执行添加会议成员
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			if (!it->second->AddConferencePartyIsOver())
			{
				// 咨询新增的会议成员
				std::string l_strCTICallRefId = it->second->m_strCTICallRefId;
				std::string l_strCompere = it->second->m_strCompere;
	
				std::string l_strTarget;
				std::string l_strTargetDeviceType;
				it->second->GetCurrentParty(l_strTarget, l_strTargetDeviceType);

				long l_lConsultationTaskId = ConsultationCall(l_strCTICallRefId, l_strCompere, l_strTarget, l_strTargetDeviceType);

				it->second->m_lConsultationCallTaskId = l_lConsultationTaskId;
				it->second->m_bAddResult = true;
				it->second->m_bIsMakeCall = false;
				it->second->m_bDoingConference = true;
				it->second->m_strTargetState = CallStateString[STATE_DIAL];
				it->second->m_strDialTime = m_pDateTime->CurrentDateTimeStr();
				ICC_LOG_DEBUG(m_pLog, "ConsultationTaskId: %u Set Target: %s State: %s", l_lConsultationTaskId, l_strTarget.c_str(), CallStateString[STATE_DIAL].c_str());
			}
			else
			{
				//新增会议成员结束
				it->second->m_bDoingConference = false;
			}
		}
	}
}

long CConferenceManager::ConsultationCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType)
{
	ICC_LOG_DEBUG(m_pLog, "Conference Execute ConsultationCall, HeldCallRefId: %s, Sponsor: %s Target: %s",
		p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->ConsultationCall(p_strHeldCTICallRefId, p_strSponsor, p_strTarget, p_strTargetDeviceType);
}
void CConferenceManager::ProcessConsultationCallResult(long p_lTaskId, bool p_bResult)
{
	if (p_bResult)
	{
		// 会议主持人咨询成功，等待被叫方加入会议，等待被叫振铃
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			if (l_oConferenceObj.second->m_lConsultationCallTaskId == p_lTaskId)
			{
				std::string l_strTarget = "";
				std::string l_strTargetDeviceType = "";
				std::string l_strConferenceId = l_oConferenceObj.first;
				std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
				std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
			//	l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_DIAL];
				l_oConferenceObj.second->GetCurrentParty(l_strTarget, l_strTargetDeviceType);

				//	通知客户端，开始呼叫会议成员
				/*CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,
					l_strCompere, l_strTarget, CallStateString[STATE_DIAL], Result_Success);*/

			//	long l_lConferenceTaskId = ConferenceCall(l_strCompere, l_strCTICallRefId, "");

				break;
			}
		}
	}
	else
	{
		// 会议主持人咨询失败，新增的会议成员失败
		std::string l_strConferenceId = "";

		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			for each (auto l_oConferenceObj in m_mConferenceList)
			{
				if (l_oConferenceObj.second->m_lConsultationCallTaskId == p_lTaskId)
				{
					l_strConferenceId = l_oConferenceObj.first;

					break;
				}
			}
		}

		//继续咨询下一会议成员
		if (!l_strConferenceId.empty())
		{
			ProcessAddConferenceFailedEvent(l_strConferenceId);
		}
	}
}
void CConferenceManager::ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId, bool p_bResult)
{
	if (p_bResult)
	{
		// 会议主持人咨询成功，等待被叫方加入会议，等待被叫振铃
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			if (l_oConferenceObj.second->m_lConsultationCallTaskId == p_lTaskId)
			{
				l_oConferenceObj.second->m_strActiveCTICallRefId = p_strActiveCTICallRefId;

				std::string l_strTarget = "";
				std::string l_strTargetDeviceType = "";
				std::string l_strConferenceId = l_oConferenceObj.first;
				std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
				std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
				
				bool l_bSetPartState = false;
				if (l_oConferenceObj.second->GetCurrentParty(l_strTarget, l_strTargetDeviceType))
				{
					l_bSetPartState = l_oConferenceObj.second->SetConferencePartyState(l_strTarget, CallStateString[STATE_DIAL]);
				}

				ICC_LOG_DEBUG(m_pLog, "ConsultationCall Success, ConferenceId: %s HeldCallRefId: %s ActiveCTICallRefId: %s Sponsor: %s Target: %s,bSetPartState:%d",
					l_strConferenceId.c_str(), p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), l_strCompere.c_str(), l_strTarget.c_str(), l_bSetPartState);

				//	通知客户端，开始呼叫会议成员
				CHytProtocol::Instance()->Sync_MakeConference(l_strConferenceId, l_strCTICallRefId,l_strCompere, l_strTarget, CallStateString[STATE_DIAL], Result_Success);
				break;
			}
		}
	}
	else
	{
		// 会议主持人咨询失败，新增的会议成员失败
		std::string l_strConferenceId = "";
		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			for each (auto l_oConferenceObj in m_mConferenceList)
			{
				if (l_oConferenceObj.second->m_lConsultationCallTaskId == p_lTaskId)
				{
					std::string l_strTarget = "";
					std::string l_strTargetDeviceType = "";
					l_strConferenceId = l_oConferenceObj.first;
					std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
					std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
					l_oConferenceObj.second->GetCurrentParty(l_strTarget, l_strTargetDeviceType);

					ICC_LOG_DEBUG(m_pLog, "ConsultationCall Failed, ConferenceId: %s HeldCallRefId: %s Sponsor: %s Target: %s",
						l_strConferenceId.c_str(), p_strHeldCTICallRefId.c_str(), l_strCompere.c_str(), l_strTarget.c_str());

					break;
				}
			}
		}

		//继续咨询下一会议成员
		if (!l_strConferenceId.empty())
		{
			ProcessAddConferenceFailedEvent(l_strConferenceId);
		}
	}
}

long CConferenceManager::ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	ICC_LOG_DEBUG(m_pLog, "Conference Execute ConferenceCall, HeldCallRefId: %s, ActiveCallRefId: %s Target: %s",
		p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->ConferenceCall(p_strTarget, p_strHeldCTICallRefId, p_strActiveCTICallRefId);
}
void CConferenceManager::ProcessConferenceCallResult(long p_lTaskId, bool p_bResult)
{
	bool l_bAddNextParty = false;
	std::string l_strConferenceId = "";

	if (p_bResult)
	{
		// 当前成员加入会议成功
		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			for each (auto l_oConferenceObj in m_mConferenceList)
			{
				if (l_oConferenceObj.second->m_lConferenceCallTaskId == p_lTaskId)
				{
					l_strConferenceId = l_oConferenceObj.first;

					std::string l_strTarget;
					std::string l_strTargetType;
					l_oConferenceObj.second->GetCurrentParty(l_strTarget, l_strTargetType);
					if (l_strTargetType.compare(TARGET_DEVICE_TYPE_WIRELESS) == 0)
					{
						//	PDT 会议合并成功，继续添加下一会议成员
						// 下一轮呼叫，取最新的话务 ID
						l_bAddNextParty = true;

						l_oConferenceObj.second->m_strTargetState = CallStateString[STATE_CONFERENCE];
					//	l_oConferenceObj.second->m_nCurrentPartyIndex++;
						ICC_LOG_DEBUG(m_pLog, "Set Target: %s State: %s", l_strTarget.c_str(), CallStateString[STATE_CONFERENCE].c_str());
					}

					break;
				}
			}
		}

		//	继续加下一个会议成员
		if (l_bAddNextParty)
		{
			ConsultationCallEx(l_strConferenceId);
		}
	}
	else
	{
		// 当前成员加入会议失败，继续加下一个成员
		{
			std::lock_guard<std::mutex> guard(m_conferenceMutex);
			for each (auto l_oConferenceObj in m_mConferenceList)
			{
				if (l_oConferenceObj.second->m_lConferenceCallTaskId == p_lTaskId)
				{
					l_strConferenceId = l_oConferenceObj.first;

					break;
				}
			}
		}

		ProcessAddConferenceFailedEvent(l_strConferenceId);
	}
}

long CConferenceManager::RetrieveCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "Conference Execute RetrieveCall, HeldCallRefId: %s, Sponsor: %s Target: %s",
		p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->RetrieveCall(p_strHeldCTICallRefId, p_strSponsor, p_strTarget);
}
void CConferenceManager::ProcessRetrieveCallResult(long p_lTaskId, bool p_bResult)
{
	std::string l_strConferenceId = "";

	if (p_bResult)
	{
		// 重拾成功
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			if (l_oConferenceObj.second->m_lRetrieveCallTaskId == p_lTaskId)
			{
				l_strConferenceId = l_oConferenceObj.first;

				break;
			}
		}
	}

	// 添加下一个会议成员
	/*if (!l_strConferenceId.empty())
	{
		ConsultationCall(l_strConferenceId);
	}*/
}
void CConferenceManager::ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget)
{
	/*if (m_SwitchClientPtr)
	{
		ICC_LOG_DEBUG(m_pLog, "Conference ReconnectCall, AvtiveCallRefId: %s HeldCallRefId: %s Sponsor: %s Target: %s",
			p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

		m_SwitchClientPtr->ReconnectCall(p_strActiveCTICallRefId, p_strHeldCTICallRefId, p_strSponsor, p_strSponsor);
	}*/
}

/*
void CConferenceManager::ForcePopCall(const std::string& p_strConferenceId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			std::string l_strCompere = it->second->m_strCompere;
			std::string l_strCallRefId = it->second->m_strCTICallRefId;

			CTaskManager::Instance()->ForcePopCall(l_strCallRefId, l_strCompere, l_strCompere);
		}
	}
}*/
void CConferenceManager::ClearCall(const std::string& p_strConferenceId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			std::string l_strCompere = it->second->m_strCompere;
			std::string l_strCallRefId = it->second->m_strCTICallRefId;

			CTaskManager::Instance()->ClearCall(l_strCallRefId, l_strCompere);
		}
	}
}
void CConferenceManager::AnswerCall(const std::string& p_strConferenceId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			std::string l_strTarget = "";
			std::string l_strTargetType = "";
			std::string l_strCTICallRefId = it->second->m_strCTICallRefId;
			it->second->GetCurrentParty(l_strTarget, l_strTargetType);

			long l_lTaskId = this->AnswerCall(l_strCTICallRefId, l_strTarget);
			it->second->m_lAnswerCallTaskId = l_lTaskId;
		}
	}
}
long CConferenceManager::AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "Conference AnswerCall, CTICallRefId: %s Target: %s",
		p_strCTICallRefId.c_str(), p_strTarget.c_str());

	return CTaskManager::Instance()->AnswerCall(p_strCTICallRefId, p_strTarget);
}
void CConferenceManager::ProcessAnswerCallResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult)
{
	if (p_bResult)
	{
		// 应答成功，挂断该成员
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			if (l_oConferenceObj.second->m_lAnswerCallTaskId == p_lTaskId)
			{
				/*std::string l_strTarget = "";
				std::string l_strTargetType = "";*/
				std::string l_strCTICallRefId = l_oConferenceObj.second->m_strCTICallRefId;
				std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
				//l_oConferenceObj.second->GetCurrentParty(l_strTarget, l_strTargetType);
				this->HangupCall(l_strCTICallRefId, l_strCompere, p_strTarget);

				// 下一轮呼叫，取最新的会议成员
			//	l_oConferenceObj.second->m_nCurrentPartyIndex++;

				break;
			}
		}
	}
}
void CConferenceManager::HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "Conference HangupCall, CTICallRefId: %s  Sponsor: %s Target: %s",
		p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	CTaskManager::Instance()->Hangup(p_strCTICallRefId, p_strSponsor, p_strTarget);
}
void CConferenceManager::RemoveFromConference(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	ICC_LOG_DEBUG(m_pLog, "Conference Party RemoveFromConference, CTICallRefId: %s  Sponsor: %s Target: %s",
		p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());

	CTaskManager::Instance()->RemoveFromConference(p_strCTICallRefId, p_strSponsor, p_strTarget);
}

void CConferenceManager::ProcessMakeCallFailedEvent(const std::string& p_strConferenceId)
{
	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	auto it = m_mConferenceList.find(p_strConferenceId);
	if (it != m_mConferenceList.end())
	{
		if (it->second)
		{
			it->second->m_strTargetState = "";

			std::string l_strCTICallRefId = it->second->m_strCTICallRefId;
			std::string l_strCompere = it->second->m_strCompere;
			std::string l_strTarget = it->second->GetCurrentParty();
			//it->second->m_strCTICallRefId = "";

			this->HangupCall(l_strCTICallRefId, l_strCompere, l_strCompere);
			//	通知客户端新增会议成员失败
			CHytProtocol::Instance()->Sync_MakeConference(p_strConferenceId, "",  l_strCompere, l_strTarget, "", Result_Failed);

		//	this->ForcePopCall(l_strCTICallRefId, l_strCompere, l_strCompere);

			// 下一轮呼叫，取最新的会议成员
			it->second->m_nCurrentPartyIndex++;
		}
	}

	// 需收到挂机消息后才能添加下一个会议成员
//	MakeCall(p_strConferenceId);
}
void CConferenceManager::ProcessAddConferenceFailedEvent(const std::string& p_strConferenceId)
{
	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(p_strConferenceId);
		if (it != m_mConferenceList.end())
		{
			if (it->second)
			{
				it->second->m_strTargetState = "";

				std::string l_strCompere = it->second->m_strCompere;
				std::string l_strTarget = it->second->GetCurrentParty();
				std::string l_strCTICallRefId = it->second->m_strCTICallRefId;
				std::string l_strActiveCTICallRefId = it->second->m_strActiveCTICallRefId;

			//	this->HangupCall(l_strCTICallRefId, l_strCompere, l_strCompere);
				//	取回原话务
				it->second->m_lRetrieveCallTaskId = this->RetrieveCall(l_strCTICallRefId, l_strCompere, l_strCompere);
			//	ReconnectCall(l_strActiveCTICallRefId, l_strCTICallRefId, l_strCompere, l_strCompere);
				//	通知客户端新增会议成员失败
				CHytProtocol::Instance()->Sync_MakeConference(p_strConferenceId, l_strCTICallRefId, l_strCompere, l_strTarget, "", Result_Failed);

				it->second->m_bAddResult = false;
			//	it->second->m_strTargetState = CallStateString[STATE_CONFERENCE];
				// 下一轮呼叫
			//	it->second->m_nCurrentPartyIndex++;
			}
		}
	}

	// 添加下一个会议成员
//	ConsultationCall(p_strConferenceId);
}
void CConferenceManager::ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strActiveCallRefId)
{
	bool l_bIsMakeCall = false;
	bool l_bDoingConference = false;
	std::string l_strConferenceId = "";

	{
		std::lock_guard<std::mutex> guard(m_conferenceMutex);
		for each (auto l_oConferenceObj in m_mConferenceList)
		{
			std::string l_strCompere = l_oConferenceObj.second->m_strCompere;
			std::string l_strCurrentParty = l_oConferenceObj.second->GetCurrentParty();
			if (l_strCompere.compare(p_strDeviceNum) == 0 && l_strCurrentParty.find(p_strCalledId) != std::string::npos)
			{
				l_strConferenceId = l_oConferenceObj.first;
				l_bDoingConference = l_oConferenceObj.second->m_bDoingConference;
				l_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;
				l_oConferenceObj.second->m_strActiveCTICallRefId = p_strActiveCallRefId;

				break;
			}
		}
	}

	if (l_bDoingConference)
	{
		if (l_bIsMakeCall)
		{
			ProcessMakeCallFailedEvent(l_strConferenceId);
		}
		else
		{
			ProcessAddConferenceFailedEvent(l_strConferenceId);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CConferenceManager::ProcessDialTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall)
{
	if (p_bIsMakeCall)
	{
		this->ProcessMakeCallFailedEvent(p_strConferenceId);
	}
	else
	{
		//	拨号超时
		this->ProcessAddConferenceFailedEvent(p_strConferenceId);
	}
}
void CConferenceManager::ProcessRingTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall)
{
	if (p_bIsMakeCall)
	{
		ProcessMakeCallFailedEvent(p_strConferenceId);
	}
	else
	{
		//	振铃超时，应答该话务再挂断，该操作只对内部话机有效
		AnswerCall(p_strConferenceId);
		/*std::lock_guard<std::mutex> guard(m_conferenceMutex);
		auto it = m_mConferenceList.find(p_strConferenceId);
		if (it != m_mConferenceList.end())
		{
			if (it->second)
			{
				it->second->m_strTargetState = "";

				std::string l_strCompere = it->second->m_strCompere;
				std::string l_strTarget = it->second->GetCurrentParty();
				std::string l_strCTICallRefId = it->second->m_strCTICallRefId;

				this->RemoveFromConference(l_strCTICallRefId, l_strCompere, l_strTarget);
				//	取回原话务
				//it->second->m_lRetrieveCallTaskId = this->RetrieveCall(l_strCTICallRefId, l_strCompere, l_strCompere);
				//	ReconnectCall(l_strActiveCTICallRefId, l_strCTICallRefId, l_strCompere, l_strCompere);
				//	通知客户端新增会议成员失败
				CHytProtocol::Instance()->Sync_MakeConference(p_strConferenceId, l_strCTICallRefId, l_strCompere, l_strTarget, "", Result_Failed);

				// 下一轮呼叫
				it->second->m_nCurrentPartyIndex++;
			}
		}*/
	}
}
bool CConferenceManager::FindDialTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall)
{
	bool b_lDialTimeOut = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		std::string l_strConferenceId = l_oConferenceObj.first;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();
		std::string l_strTargetState = l_oConferenceObj.second->m_strTargetState;
		std::string l_strDialTime = l_oConferenceObj.second->m_strDialTime;

		if (l_strTargetState.compare(CallStateString[STATE_DIAL]) == 0)
		{
			DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
			DateTime::CDateTime l_DialTime = m_pDateTime->FromString(l_strDialTime);
			DateTime::CDateTime l_EndTime = m_pDateTime->AddSeconds(l_DialTime, CONFERENCE_DIAL_TIMEOUT);

			if (l_CurrentTime > l_EndTime)
			{
				b_lDialTimeOut = true;
				p_strConferenceId = l_strConferenceId;
				p_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;

				//	重置振铃事件，避免重复检测
				l_oConferenceObj.second->m_strDialTime = m_pDateTime->CurrentDateTimeStr();

				ICC_LOG_DEBUG(m_pLog, "Conference [%s] Party: [%s] Dial Timeout !", l_strConferenceId.c_str(), l_strTarget.c_str());

				break;
			}
		}
	}

	return b_lDialTimeOut;
}
bool CConferenceManager::FindRingTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall)
{
	bool b_lRingTimeOut = false;

	std::lock_guard<std::mutex> guard(m_conferenceMutex);
	for each (auto l_oConferenceObj in m_mConferenceList)
	{
		std::string l_strConferenceId = l_oConferenceObj.first;
		std::string l_strTarget = l_oConferenceObj.second->GetCurrentParty();
		std::string l_strTargetState = l_oConferenceObj.second->m_strTargetState;
		std::string l_strRingTime = l_oConferenceObj.second->m_strRingTime;

		if (l_strTargetState.compare(CallStateString[STATE_RING]) == 0)
		{
			m_nRingTimeout = CSysConfig::Instance()->GetConferenceRingTimeout();
			DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
			DateTime::CDateTime l_RingTime = m_pDateTime->FromString(l_strRingTime);
			DateTime::CDateTime l_EndTime = m_pDateTime->AddSeconds(l_RingTime, m_nRingTimeout);

			if (l_CurrentTime > l_EndTime)
			{
				b_lRingTimeOut = true;
				p_strConferenceId = l_strConferenceId;
				p_bIsMakeCall = l_oConferenceObj.second->m_bIsMakeCall;

				//	重置振铃事件，避免重复检测
				l_oConferenceObj.second->m_strRingTime = m_pDateTime->CurrentDateTimeStr();

				ICC_LOG_DEBUG(m_pLog, "Conference [%s] Party: [%s] Ring Timeout !", l_strConferenceId.c_str(), l_strTarget.c_str());

				break;
			}
		}
	}

	return b_lRingTimeOut;
}

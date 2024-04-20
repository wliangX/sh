#include "Boost.h"
#include "Call.h"

CCall::CCall(long p_lCSTACallRefId)
{
	m_bIsPostCallOver = false;
	m_bIsAlarmCall = false;
	m_bIsBlackCall = false;		
	m_bIsTransferCall = false;

	m_lCSTACallRefId = p_lCSTACallRefId;			// 交换机产生的话务ID
	m_lRelatedCATSCallRefId = 0;
	m_lConferenceCallRefId = 0;

	m_strCTICallRefId = "";
	m_strRelateCTICallRefId = "";

	m_strOriginalCallerId = "";			// 原始主叫号码
	m_strOriginalCalledId = "";			// 原始被叫号码

	m_strCallerId = "";					// 主叫号码
	m_strCalledId = "";					// 被叫号码(110, 119等)
	m_strCalledPaty = "";				// 被叫分机号(7001,8001)

	m_strCallState = "";				// 话务状态(incoming, waiting, assing, ring,connect,hangup...)
	m_strStateTime = "";

	m_strIncomingTime = "";				//	呼入时间
	m_strAssignTime = "";				//	分配时间
	m_strRingTime = "";					//	振铃时间
	m_strSignalBackTime = "";			//	回铃时间
	m_strDialTime = "";					//	拨号时间
	m_strTalkTime = "";					//	开始通话时间
	m_strHangupTime = "";				//	挂机时间

	m_strCallDierection = "";// CallDirectionString[CALL_DIRECTION_UNKNOWN];	// 呼出: Out  呼入: In
	m_strCallMode = CallModeString[CALL_MODE_SINGLE];					// 单一呼叫：single, 会议呼叫：Conference

	m_strHangupType = "";// HangupTypeString[HANGUP_TYPE_CALLER];			// 挂机类型
	m_strHangupDevice = "";// = m_strCallerId;								// 挂机设备

	m_nConfMembersCount = 0;
	m_strCompere = "";
}
CCall::~CCall(void)
{
	//
}

void CCall::SetStateTime(const std::string& p_strStateTime)
{
	m_strStateTime = p_strStateTime;

	if (m_strCallState.compare(CallStateString[STATE_DIAL]) == 0)
	{
		SetDialTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_INCOMING]) == 0)
	{
		SetIncomingTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_ASSIGN]) == 0)
	{
		SetAssignTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_RING]) == 0)
	{
		SetRingTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_SIGNALBACK]) == 0)
	{
		SetSignalBackTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_RELEASE]) == 0)
	{
		SetReleaseTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_TALK]) == 0)
	{
		SetTalkTime(p_strStateTime);
	}
	else if (m_strCallState.compare(CallStateString[STATE_HANGUP]) == 0)
	{
		SetHangupTime(p_strStateTime);
	}
	else
	{
		//
	}
}
void CCall::SetCallState(const std::string& p_strCallState, const std::string& p_strTime)
{ 
	m_strCallState = p_strCallState;

	SetStateTime(p_strTime);
}

void CCall::SetHangupType(const std::string& p_strHangupType)
{ 
	if (m_strHangupType.empty())
	{
		m_strHangupType = p_strHangupType;
	}
}
void CCall::SetHangupDevice(const std::string& p_strHangupDevice)
{
	if (m_strHangupDevice.empty())
	{
		m_strHangupDevice = p_strHangupDevice;
	}
}
void CCall::ResetHangupAttribute()
{
	m_strHangupDevice = "";
	m_strHangupType = "";
	m_strHangupTime = "";
	m_strReleaseTime = "";
}
//////////////////////////////////////////////////////////////////////////
bool CCall::AddRelatedCSTACallRefId(long l_lRelatedCSTACallRefId)
{
	bool l_bRet = false;

	auto iter = std::find(m_vRelatedCSTACallRefId.begin(), m_vRelatedCSTACallRefId.end(), l_lRelatedCSTACallRefId);
	if (iter == m_vRelatedCSTACallRefId.end())
	{
		l_bRet = true;

		m_vRelatedCSTACallRefId.push_back(l_lRelatedCSTACallRefId);
	}

	return l_bRet;
}
bool CCall::DeleteRelatedCSTACallRefId(long l_lRelatedCSTACallRefId)
{
	bool l_bRet = false;

	auto iter = std::find(m_vRelatedCSTACallRefId.begin(), m_vRelatedCSTACallRefId.end(), l_lRelatedCSTACallRefId);
	if (iter != m_vRelatedCSTACallRefId.end())
	{
		l_bRet = true;

		m_vRelatedCSTACallRefId.erase(iter);
	}

	return l_bRet;
}
bool CCall::FindRelatedCSTACallRefId(long l_lRelatedCSTACallRefId)
{
	bool l_bRet = false;

	auto iter = std::find(m_vRelatedCSTACallRefId.begin(), m_vRelatedCSTACallRefId.end(), l_lRelatedCSTACallRefId);
	if (iter != m_vRelatedCSTACallRefId.end())
	{
		l_bRet = true;
	}

	return l_bRet;
}

// conference
bool CCall::AddConfTalkParty(const std::string& p_strParty)
{
	bool l_bRet = false;

	if (!p_strParty.empty())
	{
		auto iter = std::find(m_vConfMembersQueue.begin(), m_vConfMembersQueue.end(), p_strParty);
		if (iter == m_vConfMembersQueue.end())
		{
			m_vConfMembersQueue.push_back(p_strParty);

			l_bRet = true;
		}
	}

	return l_bRet;
}
bool CCall::DeleteConfTalkParty(const std::string& p_strParty)
{
	bool l_bRet = false;

	if (!p_strParty.empty())
	{
		auto iter = std::find(m_vConfMembersQueue.begin(), m_vConfMembersQueue.end(), p_strParty);
		if (iter != m_vConfMembersQueue.end())
		{
			l_bRet = true;

			m_vConfMembersQueue.erase(iter);		
		}
	}

	return l_bRet;
}

int CCall::GetConfMembersCount()
{
	int l_nCount = 0;

	l_nCount = m_vConfMembersQueue.size();

	return l_nCount;
}
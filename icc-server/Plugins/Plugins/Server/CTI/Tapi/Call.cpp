#include "Boost.h"
#include "Call.h"

CCall::CCall(long p_lCSTACallRefId)
{
	m_bIsPostCallOver = false;
	m_bIsAlarmCall = false;
	m_bIsBlackCall = false;		
	m_bIsTransferCall = false;

	m_lCSTACallRefId = p_lCSTACallRefId;			// �����������Ļ���ID
	m_lRelatedCATSCallRefId = 0;
	m_lConferenceCallRefId = 0;

	m_strCTICallRefId = "";
	m_strRelateCTICallRefId = "";

	m_strOriginalCallerId = "";			// ԭʼ���к���
	m_strOriginalCalledId = "";			// ԭʼ���к���

	m_strCallerId = "";					// ���к���
	m_strCalledId = "";					// ���к���(110, 119��)
	m_strCalledPaty = "";				// ���зֻ���(7001,8001)

	m_strCallState = "";				// ����״̬(incoming, waiting, assing, ring,connect,hangup...)
	m_strStateTime = "";

	m_strIncomingTime = "";				//	����ʱ��
	m_strAssignTime = "";				//	����ʱ��
	m_strRingTime = "";					//	����ʱ��
	m_strSignalBackTime = "";			//	����ʱ��
	m_strDialTime = "";					//	����ʱ��
	m_strTalkTime = "";					//	��ʼͨ��ʱ��
	m_strHangupTime = "";				//	�һ�ʱ��

	m_strCallDierection = "";// CallDirectionString[CALL_DIRECTION_UNKNOWN];	// ����: Out  ����: In
	m_strCallMode = CallModeString[CALL_MODE_SINGLE];					// ��һ���У�single, ������У�Conference

	m_strHangupType = "";// HangupTypeString[HANGUP_TYPE_CALLER];			// �һ�����
	m_strHangupDevice = "";// = m_strCallerId;								// �һ��豸

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
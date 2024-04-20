#include "Boost.h"
#include "Call.h"

CCall::CCall(long p_lCSTACallRefId)
{
	m_bIsPostCallOver = false;
	m_bIsAlarmCall = false;
	m_bIsBlackCall = false;		
	m_bIsTransferCall = false;

	m_lCSTACallRefId = p_lCSTACallRefId;			// �����������Ļ���ID
	m_lRelateCATSCallRefId = p_lCSTACallRefId;
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
void CCall::SetCallState(const std::string& p_strCallState, const std::string& p_strStateTime)
{ 
	m_strCallState = p_strCallState;

	SetStateTime(p_strStateTime);
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
//////////////////////////////////////////////////////////////////////////
// conference
bool CCall::AddConfTalkParty(const std::string& p_strParty)
{
	bool l_bRet = false;

	if (!p_strParty.empty())
	{
		auto iter = find(m_vConfMembersQueue.begin(), m_vConfMembersQueue.end(), p_strParty);
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
		auto iter = find(m_vConfMembersQueue.begin(), m_vConfMembersQueue.end(), p_strParty);
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

void CCall::SetE1RelayIndexNumber(const std::string& p_strE1RelayIndex, const std::string& p_strNumber, int& p_nSize)
{
	m_mapRelayIndexNumber[p_strE1RelayIndex] = p_strNumber;

	p_nSize = m_mapRelayIndexNumber.size();
}

void CCall::AddE1Relay(const std::map<std::string, std::string> &p_mapRelayIndexNumber)
{
	for (auto l_E1RelayObj : p_mapRelayIndexNumber)
	{
		m_mapRelayIndexNumber[l_E1RelayObj.first] = l_E1RelayObj.second;
	}
}

std::string CCall::GetE1RelayIndexNumber(const std::string& p_strE1RelayIndex, int& p_nSize)
{
	std::string l_strNumber = "";

	p_nSize = m_mapRelayIndexNumber.size();
	auto iter = m_mapRelayIndexNumber.find(p_strE1RelayIndex);
	if (iter != m_mapRelayIndexNumber.end())
	{
		l_strNumber = iter->second;
	}
	return l_strNumber;
}

void CCall::DeleteE1RelayIndexNumber(const std::string& p_strE1RelayIndex)
{
	m_mapRelayIndexNumber.erase(p_strE1RelayIndex);
}

std::string CCall::GetE1RelayIndexByNumber(const std::string& p_strNumber,int &p_nSize)
{
	std::string l_strE1RelayIndex = "";
	p_nSize = m_mapRelayIndexNumber.size();
	if (m_mapRelayIndexNumber.size() == 0)
	{
		return l_strE1RelayIndex;
	}

	for (auto l_E1RelayObj : m_mapRelayIndexNumber)
	{
		if (l_E1RelayObj.second == p_strNumber && !p_strNumber.empty())
		{
			l_strE1RelayIndex = l_E1RelayObj.first;
			break;
		}
	}

	return l_strE1RelayIndex;
}

void CCall::AddMemberDevice(const std::string& strDevice)
{
	//std::lock_guard<std::mutex> lock(m_mutexMemberDevices);
	m_mapMemberDevices.insert(std::make_pair(strDevice, strDevice));
}

void CCall::DeleteDevice(const std::string& strDevice)
{
	//std::lock_guard<std::mutex> lock(m_mutexMemberDevices);
	m_mapMemberDevices.erase(strDevice);
}

int CCall::MemberDeviceCount()
{
	//std::lock_guard<std::mutex> lock(m_mutexMemberDevices);
	return m_mapMemberDevices.size();
}

bool CCall::ExistDevice(const std::string& strDevice)
{
	//std::lock_guard<std::mutex> lock(m_mutexMemberDevices);
	return m_mapMemberDevices.find(strDevice) != m_mapMemberDevices.end();
}
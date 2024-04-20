
#include "Boost.h"
#include "FSAesEventProtocol.h"
#include "CTIFSAesDefine.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
std::map<std::string, std::string> g_mapDeviceStateHelp
{
	{CTIEVT_CALL_INCOMMING,CallStateString[STATE_INCOMING]},
	{CTIEVT_CALL_WAITING,CallStateString[STATE_QUEUE]},
	{CTIEVT_CALL_ASSIGNED,CallStateString[STATE_ASSIGN]},

	{CTIEVT_CALL_RING,CallStateString[STATE_RING]},
	{CTIEVT_CALL_RINGBACK,CallStateString[STATE_SIGNALBACK]},
	{CTIEVT_CALL_CONNECTED,CallStateString[STATE_TALK]},
	{CTIEVT_CALL_HANGUP,CallStateString[STATE_HANGUP]},
	{CTIEVT_CALL_HOLD,CallStateString[STATE_HOLD]},
	{CTIEVT_CALL_UNHOLD,CallStateString[STATE_CANCELHOLD]},
	{CTIEVT_CALL_MONITOR,CallStateString[STATE_SILENTMONITOR]}
	
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
bool CHcpDeviceCallEvent::IsSkillCallState(const std::string& p_strEventName)
{
	if (p_strEventName == CTIEVT_CALL_INCOMMING ||
		p_strEventName == CTIEVT_CALL_WAITING ||
		p_strEventName == CTIEVT_CALL_ASSIGNED)
	{
		return true;
	}
	return false;
}

std::string CHcpDeviceCallEvent::InitCTICallState(const std::string& p_strLastState)
{
	if (m_strEventName == CTIEVT_CALL_HANGUP)
	{
		if (p_strLastState == CallStateString[STATE_INCOMING] ||
			p_strLastState == CallStateString[STATE_QUEUE] ||
			p_strLastState == CallStateString[STATE_ASSIGN])
		{
			m_strCurrState = CallStateString[STATE_RELEASE]; //早释
		}
		else
		{
			m_strCurrState = CallStateString[STATE_HANGUP];
		}

		return m_strCurrState;
	}

	if (g_mapDeviceStateHelp.find(m_strEventName) != g_mapDeviceStateHelp.end())
	{
		m_strCurrState = g_mapDeviceStateHelp[m_strEventName];
	}
	return m_strCurrState;
}


string  CHcpDeviceCallEvent::GetCTICallDirection() const
{
	std::string l_strCallDir = CallDirectionString[CALL_DIRECTION_IN];
	if (m_oBody.m_strCallDirection == "2") //呼出
	{
		l_strCallDir = CallDirectionString[CALL_DIRECTION_OUT];
	}
	return l_strCallDir;
}

std::string CHcpDeviceCallEvent::GetHangupType() const
{
	if (m_strCurrState == CallStateString[STATE_RELEASE])
	{
		return HangupTypeString[HANGUP_TYPE_RELEASE];
	}

	if (m_oBody.m_strHangupType == "1")
	{
		return HangupTypeString[HANGUP_TYPE_CALLER];
	}
	else if (m_oBody.m_strHangupType == "2")
	{
		return HangupTypeString[HANGUP_TYPE_CALLED];
	}
	else if (m_oBody.m_strHangupType == "3")
	{
		return HangupTypeString[HANGUP_TYPE_BLACKCALL];
	}
	else if (m_oBody.m_strHangupType == "6")
	{
		return HangupTypeString[HANGUP_TYPE_NORMAL];
	}
	
	return HangupTypeString[HANGUP_TYPE_UNKNOWN];
}
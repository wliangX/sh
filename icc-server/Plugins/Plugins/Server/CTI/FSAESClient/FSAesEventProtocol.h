#pragma once

#include <string>
#include <map>
#include <Json/IJsonFactory.h>

namespace ICC
{
///////////////////////////////////////////////////////////////////////////////////
//
class CHcpAgentStateEvent
{
public:
	virtual bool ParseString(const std::string& p_strReq, JsonParser::IJsonPtr p_pJson)
	{
		if (p_strReq.empty())
		{
			return false;
		}
		if (!p_pJson->LoadJson(p_strReq))
		{
			return false;
		}

		m_strMsgID = p_pJson->GetNodeValue("/body/msgid", "");
		m_strCmdName = p_pJson->GetNodeValue("/body/cmd", "");
		m_strAgentId = p_pJson->GetNodeValue("/body/agentid", "");
		m_strExtension = p_pJson->GetNodeValue("/body/extension", "");
		m_strStatus = p_pJson->GetNodeValue("/body/status", "");
		m_strLoginState = p_pJson->GetNodeValue("/body/login_state", "");
		m_strSkill = p_pJson->GetNodeValue("/body/skill", "");
		m_strTime = p_pJson->GetNodeValue("/body/time", "");
		return true;
	}
public:
	std::string m_strMsgID;  //消息唯一ID

	std::string m_strCmdName; //
	std::string m_strAgentId;
	std::string m_strExtension;
	std::string m_strStatus;
	std::string m_strLoginState;
	std::string m_strSkill;
	std::string m_strTime;
};
///////////////////////////////////////////////////////////////////////////////////
//
class CHcpDeviceCallEvent
{
public:
	virtual bool ParseString(const std::string& p_strReq, JsonParser::IJsonPtr p_pJson)
	{
		if (nullptr == p_pJson)
		{
			return false;
		}
		if (!p_pJson->LoadJson(p_strReq))
		{
			return false;
		}
		m_oBody.m_strDeviceNum= p_pJson->GetNodeValue("/body/device", "");
		m_oBody.m_strAgentExtension = p_pJson->GetNodeValue("/body/agentid", "");

		m_oBody.m_strHcpCallId = p_pJson->GetNodeValue("/body/call_id", "");
		m_oBody.m_strOrgHcpCallId = p_pJson->GetNodeValue("/body/org_callid", "");

		m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/body/call_direction", "");
		m_oBody.m_strCaller = p_pJson->GetNodeValue("/body/caller", "");
		m_oBody.m_strCalled = p_pJson->GetNodeValue("/body/called", "");
		m_oBody.m_strOrgCalled = p_pJson->GetNodeValue("/body/org_called", "");
		m_oBody.m_strSkill = p_pJson->GetNodeValue("/body/skill", "");

		m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
		m_oBody.m_strSwitchAddr = p_pJson->GetNodeValue("/body/switch_address", "");
		m_oBody.m_strRecordFile = p_pJson->GetNodeValue("/body/record_file", "");

		m_oBody.m_strConferenceName = p_pJson->GetNodeValue("/body/conference_name", "");
		m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
		return true;
	};

	virtual std::string ToString(const std::string& p_strEventName, JsonParser::IJsonPtr p_pJson)
	{
		if (nullptr == p_pJson)
		{
			return "";
		}

		p_pJson->SetNodeValue("/body/cmd", p_strEventName);
		p_pJson->SetNodeValue("/body/call_id", m_oBody.m_strHcpCallId);
		p_pJson->SetNodeValue("/body/device", m_oBody.m_strDeviceNum);
		if (!m_oBody.m_strTimeOutFlag.empty())
		{
			p_pJson->SetNodeValue("/body/call_timeout_flag", m_oBody.m_strTimeOutFlag);
		}

		return p_pJson->ToString();
	}

	void SetEventName(const std::string& p_strEventName) { m_strEventName = p_strEventName; };
	std::string GetEventName() const { return m_strEventName; };

	bool IsSkillCallState(const std::string& p_strEventName);
	std::string InitCTICallState(const std::string& p_strLastState);

	std::string GetCTICallState() const { return m_strCurrState; };
	std::string GetCTICallDirection() const;
	std::string GetHangupType() const;

public:
	std::string m_strEventName;
	std::string m_strCurrState;
	class CBody
	{
	public:
		std::string m_strDeviceNum;

		std::string m_strHcpCallId;
		std::string m_strOrgHcpCallId;
		std::string m_strAgentExtension;

		std::string m_strCallDirection;
		std::string m_strOrgCaller;
		std::string m_strCaller; //主叫
		std::string m_strCalled; //被叫
		std::string m_strOrgCalled; //原被叫
		std::string m_strTime;

		std::string m_strSkill;
		std::string m_strHangupType;//挂机类型
		std::string m_strSwitchAddr;
		std::string m_strRecordFile;

		std::string m_strConferenceName;

		std::string m_strTimeOutFlag;

		std::string m_strTransferType;

	};
	CBody m_oBody;
};

class CHcpConferenceEvent
{
public:
	virtual bool ParseString(const std::string& p_strReq, JsonParser::IJsonPtr p_pJson)
	{
		if (nullptr == p_pJson)
		{
			return false;
		}
		if (!p_pJson->LoadJson(p_strReq))
		{
			return false;
		}
		m_oBody.m_strConferenceName = p_pJson->GetNodeValue("/body/conference_name", "");
		m_oBody.m_strHcpCallId = p_pJson->GetNodeValue("/body/call_id", "");
		m_oBody.m_strOrgHcpCallId = p_pJson->GetNodeValue("/body/org_callid", "");
		m_oBody.m_strMember = p_pJson->GetNodeValue("/body/member", "");
		m_oBody.m_strMemberType = p_pJson->GetNodeValue("/body/member_type", "");
		m_oBody.m_strMemberState = p_pJson->GetNodeValue("/body/state", "");
		m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
		return true;
	};

public:
	class CBody
	{
	public:
		std::string m_strHcpCallId;
		std::string m_strOrgHcpCallId;
		std::string m_strConferenceName;
		std::string m_strMember; //会议成员
		std::string m_strMemberType;
		std::string m_strMemberState;
		std::string m_strTime;
		
	};
	CBody m_oBody;
};

}
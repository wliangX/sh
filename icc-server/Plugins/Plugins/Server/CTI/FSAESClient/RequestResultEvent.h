#pragma once

#include <string>
#include <map>
#include <Json/IJsonFactory.h>

namespace ICC
{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//同步返回
class CCTISyncResponse
{
public:
	CCTISyncResponse() {};
	virtual ~CCTISyncResponse() {};
	virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
	{
		p_pJson->SetNodeValue("/body/code", m_strCode);
		p_pJson->SetNodeValue("/body/msgid", m_strMsgID);
		if (!m_strClientID.empty())
		{
			p_pJson->SetNodeValue("/body/clientid", m_strClientID);
		}

		return p_pJson->ToString();
	}

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

		m_strCode = p_pJson->GetNodeValue("/body/code", "");
		m_strMsgID = p_pJson->GetNodeValue("/body/msgid", "");
		m_strClientID = p_pJson->GetNodeValue("/body/clientid", "");

		return true;
	}

public:
	std::string m_strCode;
	std::string m_strMsgID;
	std::string m_strClientID;
};

//
class CGetAgentListSyncResponse
{
public:
	virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
	{
		p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
		p_pJson->SetNodeValue("/body/msgid", m_oBody.m_strMsgID);
		if (!m_oBody.m_strClientID.empty())
		{
			p_pJson->SetNodeValue("/body/clientid", m_oBody.m_strClientID);
		}
		size_t l_Count = m_oBody.m_vecData.size();
		p_pJson->SetNodeValue("/body/count", std::to_string(l_Count));
		for (size_t i = 0; i < l_Count; i++)
		{
			std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
			p_pJson->SetNodeValue(l_strPrefixPath + "agent_id", m_oBody.m_vecData[i].m_strAgentId);
			p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_vecData[i].m_strState);
			p_pJson->SetNodeValue(l_strPrefixPath + "status", m_oBody.m_vecData[i].m_strStatus);
			p_pJson->SetNodeValue(l_strPrefixPath + "skills", m_oBody.m_vecData[i].m_strSkills);
			p_pJson->SetNodeValue(l_strPrefixPath + "extension", m_oBody.m_vecData[i].m_strExtension);
		}
		return p_pJson->ToString();
	}

	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
	{
		if (nullptr == p_pJson)
		{
			return false;
		}
		if (!p_pJson->LoadJson(p_strReq))
		{
			return false;
		}

		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		m_oBody.m_strMsgID = p_pJson->GetNodeValue("/body/msgid", "");
		int l_iCount = p_pJson->GetCount("/body/count");
		for (int i = 0; i < l_iCount; i++)
		{
			std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
			CBody::CData l_oData;

			l_oData.m_strAgentId = p_pJson->GetNodeValue(l_strPrefixPath + "agent_id", "");
			l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
			l_oData.m_strStatus = p_pJson->GetNodeValue(l_strPrefixPath + "status", "");
			l_oData.m_strSkills = p_pJson->GetNodeValue(l_strPrefixPath + "skills", "");
			l_oData.m_strExtension = p_pJson->GetNodeValue(l_strPrefixPath + "extension", "");

			m_oBody.m_vecData.push_back(l_oData);
		}
		return true;
	}
public:
	class CBody
	{
	public:
		std::string m_strCode;
		std::string m_strMsgID;
		std::string m_strClientID;
		class CData
		{
		public:
			std::string m_strAgentId;
			std::string m_strSkills;
			std::string m_strState;
			std::string m_strStatus;
			std::string m_strExtension;
		};
		std::vector<CData>m_vecData;
	};
	CBody m_oBody;
};

//异步返回结果
class CAsyncResultEvent
{
public:
	virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
	{
		if (nullptr == p_pJson)
		{
			return "";
		}
		/*
		p_pJson->SetNodeValue("/body/msgid", m_oBody.m_strMsgID);
		p_pJson->SetNodeValue("/body/requestid", m_oBody.m_strRequestID);
		p_pJson->SetNodeValue("/body/cmd", m_oBody.m_strCmdName);
		p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
		p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

		if (!m_oBody.m_strCallID.empty())
		{
			p_pJson->SetNodeValue("/body/call_id", m_oBody.m_strCallID);
		}
		*/
		return p_pJson->ToString();
	}

	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
	{
		if (!p_pJson || !p_pJson->LoadJson(p_strReq))
		{
			return false;
		}
		std::string l_strTmp;
		l_strTmp = p_pJson->GetNodeValue("/body/msgid", "");
		if (!l_strTmp.empty())
		{
			m_strMsgID = l_strTmp;
			m_tbResultParam["msgid"] = l_strTmp;
		}

		l_strTmp = p_pJson->GetNodeValue("/body/requestid", "");
		if (!l_strTmp.empty())
		{
			m_tbResultParam["requestid"] = l_strTmp;
		}

		l_strTmp = p_pJson->GetNodeValue("/body/cmd", "");
		if (!l_strTmp.empty())
		{
			m_tbResultParam["cmd"] = l_strTmp;
		}

		l_strTmp = p_pJson->GetNodeValue("/body/result", "");
		if (!l_strTmp.empty())
		{
			m_strResult = l_strTmp;
			m_tbResultParam["result"] = l_strTmp;
		}

		l_strTmp = p_pJson->GetNodeValue("/body/extend_info", "");
		if (!l_strTmp.empty())
		{
			m_tbResultParam["extend_info"] = l_strTmp;
		}

		l_strTmp = p_pJson->GetNodeValue("/body/time", "");
		if (!l_strTmp.empty())
		{
			m_tbResultParam["time"] = l_strTmp;
		}
		return true;
	}
public:
	std::string m_strMsgID;  //消息唯一ID
	std::string m_strResult;
	std::string m_strMessage;

	std::map<std::string, std::string> m_tbResultParam;

};

////////////////////////////////////////////////////////////////////////////////
//
class CResponseGetExtensions
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

		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		m_oBody.m_strMsg = p_pJson->GetNodeValue("/body/message", "");

		int l_nCount = p_pJson->GetCount("/body/data");
		for (int i = 0; i < l_nCount; ++i)
		{
			std::string l_strIndex = std::to_string(i);
			std::string l_strNum;
			l_strNum = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/extension", "");
			if (!l_strNum.empty())
			{
				m_oBody.m_vecData.push_back(l_strNum);
			}
		}
		return true;
	}
public:
	class CBody
	{
	public:
		std::string m_strMsgid;
		std::string m_strSendTime;
		std::string m_strCode;						//结果				
		std::string m_strMsg;						//结果描述

		std::vector<std::string>m_vecData;
	};
	CBody m_oBody;
};

class CResponseGetFreeAgents
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

		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		m_oBody.m_strMsg = p_pJson->GetNodeValue("/body/message", "");

		int l_nCount = p_pJson->GetCount("/body/data");
		for (int i = 0; i < l_nCount; ++i)
		{
			std::string l_strIndex = std::to_string(i);
			std::string l_strNum;
			l_strNum = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/agent_id", "");
			if (!l_strNum.empty())
			{
				m_oBody.m_vecData.push_back(l_strNum);
			}
		}
		return true;
	}
public:
	class CBody
	{
	public:
		std::string m_strMsgid;
		std::string m_strSendTime;
		std::string m_strCode;						//结果				
		std::string m_strMsg;						//结果描述

		std::vector<std::string>m_vecData;
	};
	CBody m_oBody;
};
////////////////////////////////////////////////////////////////////////////////
}
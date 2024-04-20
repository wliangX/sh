#include "Boost.h"
#include "ManagerTransportQueue.h"


ICC::CSeatInfo::CSeatInfo()
{

}

ICC::CSeatInfo::~CSeatInfo()
{

}

std::string ICC::CSeatInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (p_pJson)
	{
		p_pJson->SetNodeValue("/no", m_strNo);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
		p_pJson->SetNodeValue("/dept_name", m_strDeptName);
		return p_pJson->ToString();
	}
	return "";
}

bool ICC::CSeatInfo::Parse(std::string p_strStaffInfo, JsonParser::IJsonPtr p_pJson)
{
	if (p_pJson && p_pJson->LoadJson(p_strStaffInfo))
	{
		m_strNo = p_pJson->GetNodeValue("/no", "");
		m_strName = p_pJson->GetNodeValue("/name", "");
		m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
		m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");

		return true;
	}
	return false;
}



ICC::CUserDept::CUserDept()
{

}

ICC::CUserDept::~CUserDept()
{

}

std::string ICC::CUserDept::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strInfo("");
	if (p_pJson)
	{
		p_pJson->SetNodeValue("/dept_name", m_strDeptName);
		p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
		p_pJson->SetNodeValue("/user_name", m_strUserName);
		p_pJson->SetNodeValue("/seat_no", m_strSeatNo);
		p_pJson->SetNodeValue("/busy_idle", m_strBuyIdle);
		l_strInfo = p_pJson->ToString();
	}
	return l_strInfo;
}

bool ICC::CUserDept::Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strUserInfo))
	{
		return false;
	}

	m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
	m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
	m_strUserName = p_pJson->GetNodeValue("/user_name", "");
	m_strSeatNo = p_pJson->GetNodeValue("/seat_no", "");
	m_strBuyIdle = p_pJson->GetNodeValue("/busy_idle", "");
	return true;
}

ICC::CTransportQueue::CTransportQueue()
{

}

ICC::CTransportQueue::~CTransportQueue()
{

}

std::string ICC::CTransportQueue::ToJson(JsonParser::IJsonFactoryPtr p_pJsonF)
{
	std::string l_strInfo("");
	JsonParser::IJsonPtr l_pJson = p_pJsonF->CreateJson();
	if (l_pJson)
	{
		l_pJson->SetNodeValue("/alarmid", strAlarmID);
		l_pJson->SetNodeValue("/timername", strTimerName);
		l_pJson->SetNodeValue("/transport_type", strTransportType);
		
		auto iter = m_mapRelateID.begin();
		size_t l_iIndex = 0;
		for (; iter != m_mapRelateID.end();iter++)
		{
			l_pJson->SetNodeValue("/map_relateid/" + std::to_string(l_iIndex) + "/first", iter->first);
			l_pJson->SetNodeValue("/map_relateid/" + std::to_string(l_iIndex) + "/second", iter->second);
			l_iIndex++;
		}
		std::string str1 = oSourceInfo.ToJson(p_pJsonF->CreateJson());
		l_pJson->SetNodeValue("/sourceinfo", oSourceInfo.ToJson(p_pJsonF->CreateJson()));
		std::string str2 = oTransferCallInfo.ToJson(p_pJsonF->CreateJson());
		l_pJson->SetNodeValue("/transferinfo", oTransferCallInfo.ToJson(p_pJsonF->CreateJson()));

		l_strInfo = l_pJson->ToString();
	}
	return l_strInfo;
}

bool ICC::CTransportQueue::Parse(std::string p_strInfo, JsonParser::IJsonFactoryPtr p_pJsonF)
{
	JsonParser::IJsonPtr l_pJson = p_pJsonF->CreateJson();
	if (!l_pJson || !l_pJson->LoadJson(p_strInfo))
	{
		return false;
	}
	strAlarmID = l_pJson->GetNodeValue("/alarmid", "");
	strTimerName = l_pJson->GetNodeValue("/timername", strTimerName);
	strTransportType = l_pJson->GetNodeValue("/transport_type", "");

	oSourceInfo.ParseString(l_pJson->GetNodeValue("/sourceinfo", ""), p_pJsonF->CreateJson());
	oTransferCallInfo.ParseString(l_pJson->GetNodeValue("/transferinfo", ""), p_pJsonF->CreateJson());

	size_t l_iCount = 0;
	if (l_pJson->GetCount("/map_relateid")>= 0)
	{
		l_iCount = l_pJson->GetCount("/map_relateid");
	}
	for (size_t index = 0; index < l_iCount;index++)
	{
		std::string strfirst = l_pJson->GetNodeValue("/map_relateid/" + std::to_string(index) + "/first", "");
		std::string strsecond = l_pJson->GetNodeValue("/map_relateid/" + std::to_string(index) + "/second", "");
		m_mapRelateID[strfirst] = strsecond;
	}

	return true;
}

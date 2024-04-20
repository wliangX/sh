#include "CRegisterInfo.h"
#include "CMQClientInfo.h"
#include <boost/format.hpp>
using namespace ICC;

CRegisterInfo::CRegisterInfo()
{

}

CRegisterInfo::~CRegisterInfo()
{

}

std::string CRegisterInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strRegisterInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/client_id", m_strClientID);
		p_pJson->SetNodeValue("/client_name", m_strClientName);
		p_pJson->SetNodeValue("/client_type", m_strClientType);
		p_pJson->SetNodeValue("/client_ip", m_strClientIP);

		p_pJson->SetNodeValue("/user_name", m_strUserName);
		p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
		p_pJson->SetNodeValue("/dept_name", m_strDeptName);
		p_pJson->SetNodeValue("/seat_type", m_strSeatType);
		p_pJson->SetNodeValue("/staff_code", m_strStaffCode);
		p_pJson->SetNodeValue("/staff_name", m_strStaffName);


		p_pJson->SetNodeValue("/heartbeat_time", m_strHeartTime);

		p_pJson->SetNodeValue("/district_code", m_strDistrictCode);
		l_strRegisterInfo = p_pJson->ToString();
	}
	return l_strRegisterInfo;
}

bool CRegisterInfo::Parse(std::string p_strRegisterInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strRegisterInfo))
	{
		return false;
	}
	
	m_strClientID	= p_pJson->GetNodeValue("/client_id", "");
	m_strClientName = p_pJson->GetNodeValue("/client_name", "");
	m_strClientType = p_pJson->GetNodeValue("/client_type", "");
	m_strClientIP = p_pJson->GetNodeValue("/client_ip", "");

	m_strUserName = p_pJson->GetNodeValue("/user_name", "");
	m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
	m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
	m_strSeatType = p_pJson->GetNodeValue("/seat_type", "");
	m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
	m_strStaffName = p_pJson->GetNodeValue("/staff_name", "");
	
	m_strHeartTime = p_pJson->GetNodeValue("/heartbeat_time", "");

	m_strDistrictCode = p_pJson->GetNodeValue("/district_code", "");
	return true;
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
		p_pJson->SetNodeValue("/dept_district_code", m_strDeptDistrictCode);
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

	m_strDeptDistrictCode = p_pJson->GetNodeValue("/dept_district_code", "");
	m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
	m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
	m_strUserName = p_pJson->GetNodeValue("/user_name", "");
	m_strSeatNo = p_pJson->GetNodeValue("/seat_no", "");
	m_strBuyIdle = p_pJson->GetNodeValue("/busy_idle", "");

	return true;
}

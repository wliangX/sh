#include "SeatInfo.h"


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


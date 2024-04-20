#include "FuncInfo.h"
using namespace ICC;

CFuncInfo::CFuncInfo()
{

}

CFuncInfo::~CFuncInfo()
{

}

std::string CFuncInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strFuncInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);		
		p_pJson->SetNodeValue("/code", m_strCode);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/sort", m_strSort);

		l_strFuncInfo = p_pJson->ToString();
	}
	return l_strFuncInfo;
}

bool CFuncInfo::Parse(std::string p_strFuncInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strFuncInfo))
	{
		return false;
	}
	
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
	m_strCode = p_pJson->GetNodeValue("/code", "");
	m_strName = p_pJson->GetNodeValue("/name", "");		
	m_strSort = p_pJson->GetNodeValue("/sort", "");
	
	return true;
}

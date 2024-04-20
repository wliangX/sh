#include "CAlarmOnlineList.h"
#include <boost/format.hpp>
using namespace ICC;

CAlarmOnlineList::CAlarmOnlineList()
{

}

CAlarmOnlineList::~CAlarmOnlineList()
{

}

std::string CAlarmOnlineList::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strAlarmOnlineList("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/client_id", m_strClientID);

		l_strAlarmOnlineList = p_pJson->ToString();
	}
	return l_strAlarmOnlineList;
}

bool CAlarmOnlineList::Parse(std::string p_strAlarmOnlineList,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strAlarmOnlineList))
	{
		return false;
	}
	
	m_strClientID = p_pJson->GetNodeValue("/client_id", "");
	
	return true;
}

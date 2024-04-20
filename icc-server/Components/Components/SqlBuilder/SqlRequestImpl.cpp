#include "Boost.h"
#include "SqlRequestImpl.h"

 CSqlRequest::CSqlRequest( )
{

}

CSqlRequest::~CSqlRequest()
{

}
 
std::string CSqlRequest::GetSqlID()
{
	return m_strSqlID;
}

void  CSqlRequest::SetSqlID(std::string p_strRequestID)
{
	m_strSqlID = p_strRequestID;
}

std::map<std::string, std::string>& CSqlRequest::GetNewValParam()
{
	return m_mapNewValParam;
}

void CSqlRequest::SetParam(const std::map<std::string, std::string>& p_mapParam)
{
	auto it = m_vecParam.begin();
	if (it != m_vecParam.end())
	{
		*it = p_mapParam;
	}
	else
	{
		m_vecParam.push_back(p_mapParam);
	}
}

void CSqlRequest::SetParam(const std::vector<std::map<std::string, std::string>>& p_vecParams)
{
	m_vecParam = p_vecParams;
}

void CSqlRequest::SetParam(std::string p_strName, std::string p_strValue)
{
	map<string, string> l_mapParam;
	l_mapParam.insert(make_pair(p_strName, p_strValue));
	this->SetParam(l_mapParam);
}

void CSqlRequest::AddParam(const std::map<std::string, std::string>& p_mapParam)
{
	m_vecParam.push_back(p_mapParam);
}

std::vector<std::map<std::string, std::string>>& CSqlRequest::GetParam()
{
	return  m_vecParam;
}


void CSqlRequest::SetNewValParam(std::string p_strName, std::string p_strValue)
{
	m_mapNewValParam[p_strName] = p_strValue;
}

void CSqlRequest::SetNewValParam(std::map<std::string, std::string>& p_mapNewValParam)
{
	m_mapNewValParam = p_mapNewValParam;
}

CSqlRequestList::CSqlRequestList(JsonParser::IJsonPtr p_pJson, StringUtil::IStringUtilPtr p_pStrUtil)
	:m_pJson(p_pJson), m_pString(p_pStrUtil)
{

}

CSqlRequestList::~CSqlRequestList()
{

}


unsigned int CSqlRequestList::GetRequestsCount()
{
	return m_vecRequests.size();
}

void CSqlRequestList::AddRequest(ISqlRequestPtr &p_oParams)
{
	m_vecRequests.push_back(p_oParams);
}

void CSqlRequestList::AddRequest(vector<ISqlRequestPtr> & p_vecParams)
{
	for (auto it : p_vecParams)
	{
		m_vecRequests.push_back(it);
	}
}

void CSqlRequestList::SetRequests(vector<ISqlRequestPtr> & p_vecParams)
{
	m_vecRequests = p_vecParams;
}


vector<ISqlRequestPtr>& CSqlRequestList::GetAllRequest()
{
	return m_vecRequests;
}

/*
 * 方法: ICC::CSqlRequestList::Parse
 * 功能: 解析Json格式的SQL请求，延后修改
 * 参数: string p_strRequest，SQL请求
 * 返回: bool
 * 作者：[3/9/2018 t26150]
*/
bool CSqlRequestList::Parse(string p_strRequest)
{
	if (!m_pJson || !m_pJson->LoadJson(p_strRequest))
	{
		return false;
	}

	ISqlRequestPtr l_pRequest = boost::make_shared<CSqlRequest>();
	//get request id
	string l_strRequestID = m_pJson->GetNodeValue("/sql_id","");//兼容老版本协议
	if (l_strRequestID.empty())
	{
		return false;
	}
	l_pRequest->SetSqlID(l_strRequestID);

	//parse update set parameter from the request
	string l_strParam = m_pJson->GetNodeValue("/param", "");
	if (l_strParam.empty())
	{
		return false;
	}

	std::vector<std::string> l_vecParam;
	m_pString->Split(l_strParam, ",:", l_vecParam,true);
	for (auto it = l_vecParam.begin(); it != l_vecParam.end(); ++it)
	{
		l_pRequest->SetParam(*it, *(++it));
	}
	m_vecRequests.push_back(l_pRequest);
	return true;
}

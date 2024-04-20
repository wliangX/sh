#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define KeyWord ("KeyWord")

//同步状态
enum EnSyncType
{
	ADD = 1, //添加
	EDIT, //更新
	DEL,   //删除

};

CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}
void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pIDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pJsonFac = ICCGetIJsonFactory();
	//m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);	

	/*while (!_LoadKeyWordFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.keyword failed to load keyword info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_key_word_info_request", OnCNotifiSetKeyWordRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_key_word_info_request", OnCNotifiQueryKeyWordByPageRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_key_word_info_request", OnNotifiDeleteKeyWordRequest);	

	ICC_LOG_INFO(m_pLog, "plugin basedata.keyword start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "keyword stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiSetKeyWordRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CKeyWordSetRequest l_oSetRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oSetRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse set_key_word_info_request failed:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CKeyWordSetRespond l_oSetRespond;
	GenRespHeader("set_key_word_info_request", l_oSetRequest.m_oHeader, l_oSetRespond.m_oHeader);

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());	

	DataBase::SQLRequest l_oSQLReq;
	
	EnSyncType tmp_enSyncType = ADD;

	if (l_oSetRequest.m_oBody.m_oKeyWord.m_strGuid.empty())
	{
		std::string l_strID = m_pString->CreateGuid();
		l_oSQLReq.sql_id = "insert_icc_t_keyword";
		l_oSQLReq.param["guid"] = l_strID;
		l_oSetRespond.m_oBody.m_strGuid = l_strID;
		l_oSetRequest.m_oBody.m_oKeyWord.m_strGuid = l_strID;

		l_oSQLReq.param["key_word"] = l_oSetRequest.m_oBody.m_oKeyWord.m_strKeyWord;
		l_oSQLReq.param["content"] = l_oSetRequest.m_oBody.m_oKeyWord.m_strContent;
		l_oSQLReq.param["create_user"] = l_oSetRequest.m_oBody.m_strCreateUser;
		l_oSQLReq.param["create_time"] = l_strCurrentTime;
	}
	else
	{
		tmp_enSyncType = EDIT;

		l_oSQLReq.sql_id = "update_icc_t_keyword";
		l_oSQLReq.param["guid"] = l_oSetRequest.m_oBody.m_oKeyWord.m_strGuid;
		l_oSQLReq.set["key_word"] = l_oSetRequest.m_oBody.m_oKeyWord.m_strKeyWord;
		l_oSQLReq.set["content"] = l_oSetRequest.m_oBody.m_oKeyWord.m_strContent;
		l_oSQLReq.set["update_user"] = l_oSetRequest.m_oBody.m_strUpdateUser;
		l_oSQLReq.set["update_time"] = l_strCurrentTime;
	}		
	
	if (!ExecSql(l_oSQLReq))
	{
		l_oSetRespond.m_oHeader.m_strResult = "1";
		l_oSetRespond.m_oHeader.m_strMsg = "execute " + l_oSQLReq.sql_id + " failed";
	}
	
	std::string l_strMessage = l_oSetRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send:[%s]", l_strMessage.c_str());

	//成功才同步
	if ("0" == l_oSetRespond.m_oHeader.m_strResult)
	{
		_SendSyncKeyWord(l_oSetRequest.m_oBody.m_oKeyWord, (int)tmp_enSyncType);
	}
	
}

void CBusinessImpl::OnCNotifiQueryKeyWordByPageRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CKeyWordQueryRequest l_oRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse query keyword failed:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CKeyWordQueryRespond l_oRespond;
	GenRespHeader("get_all_key_dept_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	int l_iPageSize = m_pString->ToInt(l_oRequest.m_oBody.m_strPageSize);
	int l_iPageIndex = m_pString->ToInt(l_oRequest.m_oBody.m_strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	l_iPageSize = std::min(l_iPageSize, 1000);

	if (l_iPageSize <= 0)
	{//检查客户端传入的page_size参数
		l_iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oRequest.m_oBody.m_strPageSize.c_str(), l_iPageSize);
	}
	if (l_iPageIndex < 1)
	{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		l_iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_oRequest.m_oBody.m_strPageIndex.c_str(), l_iPageIndex);
	}

	//根据请求参数构造查询条件	
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始
	//std::string l_strLimitOffset = str(boost::format(" limit %1% offset %2%") % l_iPageSize % l_iRequestIdxBegin);

	int iTotalCount = 0;
	if (_QueryKeyWordCount(iTotalCount)>0)
	{
		_QueryKeyWordByPage(l_oRespond.m_oBody.m_vecData, l_iPageSize, l_iRequestIdxBegin);
		int iCurrentCount = l_oRespond.m_oBody.m_vecData.size();
		l_oRespond.m_oBody.m_strAllCount = m_pString->Format("%d", iTotalCount);
		l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iCurrentCount);
	}
	
	ICC_LOG_DEBUG(m_pLog, "[OnCNotifiGetAllKeyWordRequest] HGetAll complete! msgid = %s", l_oRequest.m_oHeader.m_strMsgid.c_str());	

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiDeleteKeyWordRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CKeyWordDeleteRequest l_oRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse delete keyword request failed:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CKeyWordDeleteRespond l_oRespond;
	GenRespHeader("delete_key_word_info_response", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "delete_icc_t_keyword";
	l_oKeyDeptSQLReq.param["guid"] = l_oRequest.m_oBody.m_strguid;	  

	if (!ExecSql(l_oKeyDeptSQLReq))
	{
		ICC_LOG_ERROR(m_pLog, "delete key word failed!!! guid[%s]", l_oRequest.m_oBody.m_strguid.c_str());
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute delete_icc_t_keyword failed";
	}	
	else
	{		
		l_oRespond.m_oHeader.m_strResult = "0";
	}
	
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	//成功才同步
	if ("0" == l_oRespond.m_oHeader.m_strResult)
	{
		ICC::PROTOCOL::CKeyWordInfo tmp_oKeyWord;
		tmp_oKeyWord.m_strGuid = l_oRequest.m_oBody.m_strguid;
		_SendSyncKeyWord(tmp_oKeyWord, (int)DEL);
	}
}

void CBusinessImpl::_SendSyncKeyWord(const ICC::PROTOCOL::CKeyWordInfo& in_oKeyWordInfo, int in_iSynType)
{
	ICC::PROTOCOL::CKeyWordSync tmp_oSync;
	
	
	tmp_oSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	tmp_oSync.m_oHeader.m_strCmd = "key_word_info_sync";
	tmp_oSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	tmp_oSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	tmp_oSync.m_oHeader.m_strRequestType = "1";//主题

	tmp_oSync.m_oBody.m_strSyncType = std::to_string(in_iSynType);
	tmp_oSync.m_oBody.m_oKeyWordInfo = in_oKeyWordInfo;

	//同步消息
	std::string l_strMessage(tmp_oSync.ToString(m_pJsonFac->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "Send Sync Info:[%s]", l_strMessage.c_str());
}	

bool CBusinessImpl::_QueryKeyWordCount(int& iCount)
{
	iCount = 0;
	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "query_icc_t_keyword_all_count";
	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oKeyDeptSQLReq);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	if (l_result->Next())
	{
		std::string strCount = l_result->GetValue("count");
		if (!strCount.empty())
		{
			iCount = std::stoi(strCount);
		}
	}

	return true;
}

bool CBusinessImpl::_QueryKeyWordByPage(std::vector<ICC::PROTOCOL::CKeyWordInfo>& vecAllKeyWords, int iPageSize, int iBeginIndex)
{
	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "query_icc_t_keyword_by_page";
	l_oKeyDeptSQLReq.param["page_size"] = std::to_string(iPageSize);
	l_oKeyDeptSQLReq.param["begin_index"] = std::to_string(iBeginIndex);

	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oKeyDeptSQLReq, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = 0;
	while (l_result->Next())
	{
		ICC::PROTOCOL::CKeyWordInfo keyword;

		keyword.m_strGuid = l_result->GetValue("guid");
		keyword.m_strKeyWord = l_result->GetValue("key_word");
		keyword.m_strContent = l_result->GetValue("content");
		//keyword.strcreate_user = l_result->GetValue("create_user");
		//keyword.strcreate_time = l_result->GetValue("create_time");

		vecAllKeyWords.push_back(keyword);

		++l_iSize;
	}
	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Success size[%d]", l_iSize);

	return true;
}

bool CBusinessImpl::_QueryAllKeyWord(std::vector<ICC::PROTOCOL::CKeyWordInfo>& vecAllKeyWords)
{
	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "query_icc_t_keyword_all";
	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oKeyDeptSQLReq);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = 0;
	while (l_result->Next())
	{
		ICC::PROTOCOL::CKeyWordInfo keyword;

		keyword.m_strGuid = l_result->GetValue("guid");
		keyword.m_strKeyWord = l_result->GetValue("key_word");
		keyword.m_strContent = l_result->GetValue("content");
		//keyword.strcreate_user = l_result->GetValue("create_user");
		//keyword.strcreate_time = l_result->GetValue("create_time");

		vecAllKeyWords.push_back(keyword);

		++l_iSize;
	}
	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Success size[%d]", l_iSize);

	return true;
}

//bool CBusinessImpl::_LoadKeyWordFromDB()
//{
//	std::vector<ICC::PROTOCOL::CKeyWord> vecAllKeyWords;
//	if (!_QueryAllKeyWord(vecAllKeyWords))
//	{
//		ICC_LOG_ERROR(m_pLog, "_QueryAllKeyWord failed!!!");
//		return false;
//	}
//
//	if (vecAllKeyWords.empty())
//	{
//		ICC_LOG_INFO(m_pLog, "keyword size is 0!!!");
//		return true;
//	}
//
//	std::map<std::string, std::string> mapAllKeyWords;
//	int iCount = vecAllKeyWords.size();
//	for (int i = 0; i < iCount; ++i)
//	{
//		mapAllKeyWords.insert(std::make_pair(vecAllKeyWords[i].strguid, vecAllKeyWords[i].strkey_word));
//	}
//
//	m_pRedisClient->Del(KeyWord);
//
//	return m_pRedisClient->HMSet(KeyWord, mapAllKeyWords);	
//}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader)
{
	p_pRespHeader.m_strMsgId = p_pRequestHeader.m_strMsgId;

	p_pRespHeader.m_strSystemID = "icc_server_basedata_KeyDept";
	p_pRespHeader.m_strSubsystemID = "icc_server_basedata_KeyDept";
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
}

bool CBusinessImpl::ExecSql(DataBase::SQLRequest p_oSQLReq)
{
	DataBase::IResultSetPtr l_pRSet = m_pIDBConn->Exec(p_oSQLReq);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet)
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error:[%s]", "l_pRSet is null");
		return false;
	}
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

//bool ICC::CBusinessImpl::_AddKeyWord(const std::string& strguid, std::string& strkeyword)
//{
//	if (strguid.empty())
//	{
//		ICC_LOG_ERROR(m_pLog, "_AddKeyWord guid is empty!!!");
//		return false;
//	}
//	
//	return m_pRedisClient->HSet(KeyWord, strguid, strkeyword);	
//}

/*bool ICC::CBusinessImpl::_DeleteKeyWord(const std::string& strguid)
{
	return m_pRedisClient->HDel(KeyWord, strguid);
}*/	


//bool ICC::CBusinessImpl::GetKeyDeptByPhone(std::string p_strPhone, CKeyDept& p_OKeyDept)
//{
//	std::string l_strKeyDept;
//	if (!m_pRedisClient->HGet(KeyDept, p_strPhone, l_strKeyDept))
//	{
//		return false;
//	}
//	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
//	return p_OKeyDept.Parse(l_strKeyDept, l_pJson);
//}
//
//std::string ICC::CBusinessImpl::GetIDByPhone(std::string p_strPhone)
//{
//	std::string l_strKeyDept;
//	if (!m_pRedisClient->HGet(KeyDept, p_strPhone, l_strKeyDept))
//	{
//		return "";
//	}
//	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
//	CKeyDept l_oKeyDept;
//	l_oKeyDept.Parse(l_strKeyDept, l_pJson);
//	return l_oKeyDept.strID;
//}
//
//bool ICC::CBusinessImpl::ExistInCache(std::string p_strPhone)
//{
//	return m_pRedisClient->HExists(KeyDept, p_strPhone);
//}


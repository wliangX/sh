#include "Boost.h"
#include <algorithm>
#include "DictInfo.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

bool Comp(const CValue lhs, const CValue rhs)
{
	return lhs.m_strGuid < rhs.m_strGuid;
}

CBusinessImpl::CBusinessImpl()
{
	//
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_LockFacPtr = ICCGetILockFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	//数据库DBConn在初始化时会自动连接
	/*while (!LoadDict())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.dict failed to load dict info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_dict_request", OnCNotifiSetDictRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dict_request", OnCNotifiGetDictRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_dict_request", OnCNotifiDeleteDictRequest);

	ICC_LOG_INFO(m_pLog, "plugin basedata.dict start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "dict stop success");
}

void CBusinessImpl::OnDestroy()
{

}
//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::CreateDictLevel(std::unordered_map<std::string, std::string>& p_mapOutParent,
	const std::string& p_strInLevel, const std::string& p_strParentGuid, const std::vector<PAIR>& p_vecDict)
{
	//待优化
	std::map<std::string, std::string> tmp_mapCache;

	for (auto l_objDict : p_vecDict)
	{
		if (l_objDict.second.m_strParentGuid.compare(p_strParentGuid) == 0)
		{
			std::string l_strDictGuid = l_objDict.first;
			std::string l_strDictCode = l_objDict.second.m_strCode;
			std::string l_strDictJson = l_objDict.second.ToJson(ICCGetIJsonFactory()->CreateJson());

			p_mapOutParent.insert(std::make_pair(l_strDictGuid, l_strDictCode));
			
			tmp_mapCache.insert(std::make_pair(l_strDictCode, l_strDictJson));
		}
	}

	// 批量存入Redis缓存
	if (!tmp_mapCache.empty())
	{
		bool bSetRedis = m_pRedisClient->HMSet(p_strInLevel, tmp_mapCache);

		ICC_LOG_DEBUG(m_pLog, "Redis HMSet key[%s] result[%s] map_size[%d]", p_strInLevel.c_str(), bSetRedis ? "success" : "fail", tmp_mapCache.size());
	}
}

void CBusinessImpl::AddDictCache(const std::map<std::string, CDictInfo>& p_mapDict)
{
	ICC_LOG_LOWDEBUG(m_pLog, "Begin sort");
	std::vector<PAIR> l_vecTemp(p_mapDict.begin(), p_mapDict.end());
	std::sort(l_vecTemp.begin(), l_vecTemp.end(), CCodeCompare());
	ICC_LOG_LOWDEBUG(m_pLog, "End sort");

	std::unordered_map<std::string, std::string> l_mapParentDictCache;

	do 
	{
		std::unordered_map<std::string, std::string> l_mapTempValue(std::move(l_mapParentDictCache));

		if (l_mapTempValue.size() > 0)
		{
			for (auto it : l_mapTempValue)
			{
				std::string l_strKey = DICT_LEVEL + it.second;
				//m_pRedisClient->Del(l_strKey);

				CreateDictLevel(l_mapParentDictCache, l_strKey, it.first, l_vecTemp );
			}
		} 
		else
		{
			//m_pRedisClient->Del(l_strKey);
			
			//根节点 "0" 后续可改为可配置
			CreateDictLevel(l_mapParentDictCache, DICT_LEVEL_ROOT, "0", l_vecTemp);
		}

	} while (l_mapParentDictCache.size() > 0);
}

bool CBusinessImpl::LoadDict()
{
	ICC_LOG_DEBUG(m_pLog, "LoadingDictInfo Begin");

	std::map<std::string, CDictInfo> tmp_mapDictInfo;
	
	{ // 获取所有字典Key信息
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_dict";
		l_SqlRequest.param["is_delete"] = "false";
		//l_SqlRequest.param["orderby"] = "code asc";

		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			return false;
		}

		int l_iCount = 0;
		while (l_result->Next())
		{
			CDictInfo l_oDict;
			l_oDict.m_strGuid = l_result->GetValue("guid");
			l_oDict.m_strCode = l_result->GetValue("code");
			l_oDict.m_strParentGuid = l_result->GetValue("parent_guid");
			l_oDict.m_strSort = l_result->GetValue("sort");
			l_oDict.m_strShortCut = l_result->GetValue("shortcut");

			tmp_mapDictInfo[l_oDict.m_strGuid] = l_oDict;
			l_iCount++;
		}
		ICC_LOG_DEBUG(m_pLog, "Load Dict Key map_number[%d] db_number[%d] sql[%s]", tmp_mapDictInfo.size(), l_iCount, l_result->GetSQL().c_str());
	}

	{ // 获取所有字典Value信息
		DataBase::SQLRequest l_SqlRequestValue;
		l_SqlRequestValue.sql_id = "select_icc_t_dict_value";
		l_SqlRequestValue.param["is_delete"] = "false";

		DataBase::IResultSetPtr l_resultValue = m_pDBConn->Exec(l_SqlRequestValue);
		if (!l_resultValue->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultValue->GetErrorMsg().c_str());
			return false;
		}

		int l_iCount = 0;
		int l_iCountValid = 0;
		while (l_resultValue->Next())
		{
			auto it = tmp_mapDictInfo.find(l_resultValue->GetValue("dict_guid"));
			if (it != tmp_mapDictInfo.end())
			{
				CValue l_oValue;
				l_oValue.m_strGuid = l_resultValue->GetValue("guid");
				l_oValue.m_strValue = l_resultValue->GetValue("value");
				l_oValue.m_strLangGuid = l_resultValue->GetValue("lang_guid");

				it->second.m_vecValue.push_back(l_oValue);
				l_iCountValid++;
			}
			l_iCount++;
		}
		ICC_LOG_DEBUG(m_pLog, "Load Dict Value valid_number[%d] db_number[%d] sql[%s]", l_iCountValid, l_iCount, l_resultValue->GetSQL().c_str());
	}

	m_pRedisClient->Del(DICT_KEY);
	bool bSetRedis = true;
	{ // 批量存入Redis缓存
		std::map<std::string, std::string> tmp_mapCache;
		for (auto var : tmp_mapDictInfo)
		{
			tmp_mapCache[var.first] = var.second.ToJson(ICCGetIJsonFactory()->CreateJson());
		}
		if (!tmp_mapCache.empty())
		{
			bSetRedis = m_pRedisClient->HMSet(DICT_KEY, tmp_mapCache);
		}
		ICC_LOG_DEBUG(m_pLog, "Redis HMSet result[%s] map_size[%d]", bSetRedis ? "success" : "fail", tmp_mapDictInfo.size());
	}

	ICC_LOG_LOWDEBUG(m_pLog, "Begin build dict level");
	AddDictCache(tmp_mapDictInfo);
	ICC_LOG_LOWDEBUG(m_pLog, "End build dict level");

	return bSetRedis;
}

void CBusinessImpl::OnCNotifiSetDictRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSetDictRequest l_CSetDictRequest;
	if (!l_CSetDictRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		return;
	}

	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	//ESyncType l_oSyncType = m_pRedisClient->HExists(DICT_KEY, l_CSetDictRequest.m_oBody.m_strGuid) ? ESyncType::SYNC_TYPE_UPDATE : ESyncType::SYNC_TYPE_ADD;;
	ESyncType l_oSyncType = (l_CSetDictRequest.m_oBody.m_strSyncType == "2" ? ESyncType::SYNC_TYPE_UPDATE : ESyncType::SYNC_TYPE_ADD);
	
	//发送同步
	CDictInfo l_oDictInfo;
	l_oDictInfo.m_strGuid = l_CSetDictRequest.m_oBody.m_strGuid;
	l_oDictInfo.m_strCode = l_CSetDictRequest.m_oBody.m_strCode;
	l_oDictInfo.m_strShortCut = l_CSetDictRequest.m_oBody.m_strShortCut;;
	l_oDictInfo.m_strSort = l_CSetDictRequest.m_oBody.m_strSort;
	l_oDictInfo.m_strParentGuid = l_CSetDictRequest.m_oBody.m_strParentGuid;
	for (size_t i = 0; i < l_CSetDictRequest.m_oBody.m_vecValue.size(); i++)
	{
		CValue l_CValue;
		l_CValue.m_strGuid = l_CSetDictRequest.m_oBody.m_vecValue.at(i).m_strGuid;
		l_CValue.m_strLangGuid = l_CSetDictRequest.m_oBody.m_vecValue.at(i).m_strLangGuid;
		l_CValue.m_strValue = l_CSetDictRequest.m_oBody.m_vecValue.at(i).m_strValue;
		l_oDictInfo.m_vecValue.push_back(l_CValue);
	}

	SyncDict(l_oDictInfo, l_oSyncType);
}

void CBusinessImpl::OnCNotifiGetDictRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetDictRequest l_CGetDictRequest;
	if (!l_CGetDictRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCNotifiGetDictRequest] parse json error!, msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strLangGuid;
	std::string l_strLangCode = l_CGetDictRequest.m_oBody.m_strLanguageCode;

	//可改为从redis读取
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_language";
	l_SqlRequest.param["code"] = l_strLangCode;

	PROTOCOL::CGetDictRespond l_CGetDictRespond;
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		l_CGetDictRespond.m_oBody.m_strCount = "0";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_CGetDictRespond.ToString(l_pIJson);

		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		return;
	}
	while (l_result->Next())
	{
		l_strLangGuid = l_result->GetValue("guid");
	}

	std::string l_strDictCode = l_CGetDictRequest.m_oBody.m_strDictCode;
	std::string l_strPageSize = l_CGetDictRequest.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_CGetDictRequest.m_oBody.m_strPageIndex;
	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	std::string l_strDictKey = DICT_LEVEL + l_strDictCode;
	if (l_strDictCode.empty()) l_strDictKey = DICT_KEY;

	std::map<std::string, std::string>l_mapDictValue;
	m_pRedisClient->HGetAll(l_strDictKey, l_mapDictValue);
	if (l_mapDictValue.size() == 0)
	{
		ICC_LOG_WARNING(m_pLog, "Get Dict Error ,Error Message :No Search Dict Data");

		l_CGetDictRespond.m_oBody.m_strAllCount = "0";
		l_CGetDictRespond.m_oBody.m_strCount = "0";
		l_CGetDictRespond.m_oBody.m_strPageIndex = l_strPageIndex;
		PROTOCOL::CGetDictRespond::CBody::CData l_CData;
		l_CData.m_strGuid = "";
		l_CData.m_strCode = "";
		l_CData.m_strShortCut = "";
		l_CData.m_strSort = "";
		l_CData.m_strParentGuid = "";
		l_CData.m_strValue = "";
		l_CGetDictRespond.m_oBody.m_vecData.push_back(l_CData);

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_CGetDictRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);

		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	} 
	else
	{
		int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
		int l_nEndIndex = l_nPageIndex * l_nPageSize;

		if (l_nBeginIndex > l_mapDictValue.size())
		{
			l_CGetDictRespond.m_oHeader.m_strResult = "1";
			l_CGetDictRespond.m_oHeader.m_strMsg = "Out of range";
			l_CGetDictRespond.m_oBody.m_strCount = "0";
		}
		else
		{
			//std::map<std::string, CDictInfo>l_mapDictFind;

			std::vector<CDictInfo>	l_vecDictFind;
			std::vector<std::pair<std::string, std::string>> l_vecDictValue(l_mapDictValue.begin(), l_mapDictValue.end());
			std::sort(l_vecDictValue.begin(), l_vecDictValue.end(),
				[](const std::pair<std::string, std::string>& p_infoFirst, const std::pair<std::string, std::string>& p_infoSecond)
				{
					return p_infoFirst.first < p_infoSecond.first;
				});

			int l_nIndex = 1;
			//for (auto it : l_mapDictValue)
			for (auto it = l_vecDictValue.begin() + l_nBeginIndex; 
				it < l_vecDictValue.begin() + l_nEndIndex && it != l_vecDictValue.end(); it++)
			{
				/*if (l_nIndex < l_nBeginIndex)
				{
					l_nIndex++;
					continue;
				}*/

				CDictInfo l_oDict;
				l_oDict.Parse(it->second, ICCGetIJsonFactory()->CreateJson());
				for (size_t i = 0; i < l_oDict.m_vecValue.size(); i++)
				{
					if (l_oDict.m_vecValue.at(i).m_strLangGuid == l_strLangGuid)
					{
						CDictInfo l_oDictValue;
						l_oDictValue.m_strGuid = l_oDict.m_strGuid;
						l_oDictValue.m_strCode = l_oDict.m_strCode;
						l_oDictValue.m_strParentGuid = l_oDict.m_strParentGuid;
						l_oDictValue.m_strShortCut = l_oDict.m_strShortCut;
						l_oDictValue.m_strSort = l_oDict.m_strSort;
						l_oDictValue.m_vecValue.push_back(l_oDict.m_vecValue.at(i));
						//l_mapDictFind[l_oDictValue.m_strGuid] = l_oDictValue;
						l_vecDictFind.emplace_back(l_oDictValue);
					}
				}

				//if (++l_nIndex > l_nEndIndex) break;
			}

			for (auto l_itFind : l_vecDictFind)
			{
				PROTOCOL::CGetDictRespond::CBody::CData l_CData;
				l_CData.m_strGuid = l_itFind.m_strGuid;
				l_CData.m_strCode = l_itFind.m_strCode;
				l_CData.m_strShortCut = l_itFind.m_strShortCut;
				l_CData.m_strSort = l_itFind.m_strSort;
				l_CData.m_strParentGuid = l_itFind.m_strParentGuid;
				
				for (size_t i = 0; i < l_itFind.m_vecValue.size(); i++)
				{
					l_CData.m_strValue = l_itFind.m_vecValue.at(i).m_strValue;
				}

				l_CGetDictRespond.m_oBody.m_vecData.push_back(l_CData);
			}

			/*std::sort(l_CGetDictRespond.m_oBody.m_vecData.begin(), l_CGetDictRespond.m_oBody.m_vecData.end(),
				[](const PROTOCOL::CGetDictRespond::CBody::CData& lp, const PROTOCOL::CGetDictRespond::CBody::CData& rp) 
				{
					return std::atoi(lp.m_strSort.c_str()) < std::atoi(rp.m_strSort.c_str());
				});*/
			l_CGetDictRespond.m_oBody.m_strCount = std::to_string(l_vecDictFind.size());
		}

		l_CGetDictRespond.m_oBody.m_strAllCount = std::to_string(l_mapDictValue.size());
		l_CGetDictRespond.m_oBody.m_strPageIndex = std::to_string(l_nPageIndex);
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_CGetDictRespond.ToString(l_pIJson);

		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::OnCNotifiDeleteDictRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CDeleteDictRequest l_CDeleteDictRequest;
	if (!l_CDeleteDictRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		return;
	}

	CDictInfo l_oDict;
	l_oDict.m_strGuid = l_CDeleteDictRequest.m_oBody.m_strGuid;
	SyncDict(l_oDict, ESyncType::SYNC_TYPE_DELETE);
}

void ICC::CBusinessImpl::SyncDict(const CDictInfo& p_oDict, const ESyncType& p_roSyncType)
{
	PROTOCOL::CDictSync l_CDictSync;
	l_CDictSync.m_oHeader.m_strSystemID = "ICC";
	l_CDictSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_CDictSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CDictSync.m_oHeader.m_strRelatedID = "";
	l_CDictSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CDictSync.m_oHeader.m_strCmd = DICT_SYNC;
	l_CDictSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_CDictSync.m_oHeader.m_strRequestType = "1";
	l_CDictSync.m_oHeader.m_strResponse = "";
	l_CDictSync.m_oHeader.m_strResponseType = "";

	l_CDictSync.m_oBody.m_strGuid = p_oDict.m_strGuid;
	l_CDictSync.m_oBody.m_strCode = p_oDict.m_strCode;
	l_CDictSync.m_oBody.m_strShortCut = p_oDict.m_strShortCut;
	l_CDictSync.m_oBody.m_strSort = p_oDict.m_strSort;
	l_CDictSync.m_oBody.m_strParentGuid = p_oDict.m_strParentGuid;
	l_CDictSync.m_oBody.m_strSyncType = std::to_string(p_roSyncType);

	PROTOCOL::CDictSync::CBody::CValue l_CValue;
	for (size_t i = 0; i < p_oDict.m_vecValue.size(); i++)
	{		
		l_CValue.m_strGuid = p_oDict.m_vecValue.at(i).m_strGuid;
		l_CValue.m_strLangGuid = p_oDict.m_vecValue.at(i).m_strLangGuid;
		l_CValue.m_strValue = p_oDict.m_vecValue.at(i).m_strValue;
		l_CDictSync.m_oBody.m_vecValue.push_back(l_CValue);
	}

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_CDictSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = "ICC";
	p_pRespHeader.m_strSubsystemID = "ICC-ApplicationServer";
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
}

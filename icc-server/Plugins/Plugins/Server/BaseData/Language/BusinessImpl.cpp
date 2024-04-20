#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

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

	/*while (!LoadLanguage())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.language failed to load language info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/
   // LoadLanguage();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_language_request", OnCNotifiSetLanguageRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_language_request", OnCNotifiGetLanguageRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_language_request", OnCNotifiDeleteLanguageRequest);

	ICC_LOG_INFO(m_pLog, "plugin basedata.language start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "language stop success");
}

void CBusinessImpl::OnDestroy()
{

}

bool CBusinessImpl::LoadLanguage()
{
	m_pRedisClient->Del(LANGUAGE_KEY);

    DataBase::SQLRequest l_SqlRequest;
    l_SqlRequest.sql_id = "select_icc_t_language";
    l_SqlRequest.param["is_delete"] = "false";

    ICC_LOG_DEBUG(m_pLog, "LodingLanguageInfo Begin");

    DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
    if (!l_result->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
        return false;
    }
    while (l_result->Next())
    {
        CLanguageInfo l_oLanguage;
        l_oLanguage.m_strGuid = l_result->GetValue("guid");
        l_oLanguage.m_strName = l_result->GetValue("name");
        l_oLanguage.m_strSort = l_result->GetValue("sort");
        l_oLanguage.m_strCode = l_result->GetValue("code");

        if (!SetRedisLanguageInfo(l_oLanguage))
        {
            ICC_LOG_ERROR(m_pLog, "Load DeptInfo to Redis Error");
            return false;
        }
    }
    ICC_LOG_DEBUG(m_pLog, "LodingLanguageInfo Success,Language Size[%d]", l_result->RecordSize());

	return true;
}

void CBusinessImpl::OnCNotifiSetLanguageRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CSetLanguageRequest l_CSetLanguageRequest;
	if (!l_CSetLanguageRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
        return;
    }

	//std::string l_strSyncType = m_pRedisClient->HExists(LANGUAGE_KEY, l_CSetLanguageRequest.m_oBody.m_strGuid) ? SYNC_TYPE_CHANGE : SYNC_TYPE_ADD;
	std::string l_strSyncType = l_CSetLanguageRequest.m_oBody.m_strSyncType;

	CLanguageInfo l_oLanguage;
	l_oLanguage.m_strGuid = l_CSetLanguageRequest.m_oBody.m_strGuid;
	l_oLanguage.m_strCode = l_CSetLanguageRequest.m_oBody.m_strCode;
	l_oLanguage.m_strName = l_CSetLanguageRequest.m_oBody.m_strName;
	l_oLanguage.m_strSort = l_CSetLanguageRequest.m_oBody.m_strSort;
	SyncLanguage(l_oLanguage, l_strSyncType);
}

void CBusinessImpl::OnCNotifiGetLanguageRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CGetLanguageRequest l_CGetLanguageRequest;
	if (!l_CGetLanguageRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
		ICC_LOG_DEBUG(m_pLog, "[OnCNotifiGetLanguageRequest] parse json error! msg = %s", p_pNotify->GetMessages().c_str());
        return;
    }

    std::map<std::string, std::string>l_mapLanguage;
	m_pRedisClient->HGetAll(LANGUAGE_KEY, l_mapLanguage);
	//ICC_LOG_DEBUG(m_pLog, "Language HGetAll complete! msgid = %s", l_CGetLanguageRequest.m_oHeader.m_strMsgid.c_str());

    //std::string l_strPageSize = l_CGetLanguageRequest.m_oBody.m_strPageSize;
    //std::string l_strPageIndex = l_CGetLanguageRequest.m_oBody.m_strPageIndex;
    PROTOCOL::CGetLanguageRespond l_CGetLanguageRespond;

    std::string l_strResult;
    if (l_mapLanguage.size() == 0)
    {
        ICC_LOG_WARNING(m_pLog, "Found No Language Data");
        PROTOCOL::CGetLanguageRespond::CBody::CData l_CData;
        l_CData.m_strGuid = "";
        l_CData.m_strCode = "";
        l_CData.m_strName = "";
        l_CData.m_strSort = "";
        l_CGetLanguageRespond.m_oBody.m_strAllCount = "0";
        l_CGetLanguageRespond.m_oBody.m_strCount = "0";
        l_CGetLanguageRespond.m_oBody.m_vecData.push_back(l_CData);

        JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
        std::string l_strMessage = l_CGetLanguageRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
    }
    else
    {
        JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
        for (auto it = l_mapLanguage.begin(); it != l_mapLanguage.end(); it++)
        {
            CLanguageInfo l_oLanguage;
            l_oLanguage.Parse(it->second, l_pJson);
            PROTOCOL::CGetLanguageRespond::CBody::CData l_CData;
            
            l_CData.m_strGuid = l_oLanguage.m_strGuid;
            l_CData.m_strCode = l_oLanguage.m_strCode;
            l_CData.m_strName = l_oLanguage.m_strName;
            l_CData.m_strSort = l_oLanguage.m_strSort;
            l_CGetLanguageRespond.m_oBody.m_vecData.push_back(l_CData);
        }

        l_CGetLanguageRespond.m_oBody.m_strAllCount = m_pString->Number(l_mapLanguage.size());
        l_CGetLanguageRespond.m_oBody.m_strCount = m_pString->Number(l_mapLanguage.size());
        JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
        std::string l_strMessage = l_CGetLanguageRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
    }
}

void CBusinessImpl::OnCNotifiDeleteLanguageRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CDeleteLanguageRequest l_CDeleteLanguageRequest;
	if (!l_CDeleteLanguageRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
        return;
    }

	CLanguageInfo l_oLanguage;
	l_oLanguage.m_strGuid = l_CDeleteLanguageRequest.m_oBody.m_strGuid;
	SyncLanguage(l_oLanguage, SYNC_TYPE_DELETE);
}


bool CBusinessImpl::SyncLanguage(const CLanguageInfo &p_oLanguage, std::string p_strSyncType)
{
    PROTOCOL::CLanguageSync l_CLanguageSync;
    l_CLanguageSync.m_oHeader.m_strSystemID = "ICC";
    l_CLanguageSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_CLanguageSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
    l_CLanguageSync.m_oHeader.m_strRelatedID = "";
    l_CLanguageSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
    l_CLanguageSync.m_oHeader.m_strCmd = LANGUAGE_SYNC;
    l_CLanguageSync.m_oHeader.m_strRequest = "topic_basedata_sync";
    l_CLanguageSync.m_oHeader.m_strRequestType = "1";
    l_CLanguageSync.m_oHeader.m_strResponse = "";
    l_CLanguageSync.m_oHeader.m_strResponseType = "";

    l_CLanguageSync.m_oBody.m_strSyncTyp = p_strSyncType;
    l_CLanguageSync.m_oBody.m_strGuid = p_oLanguage.m_strGuid;
    l_CLanguageSync.m_oBody.m_strCode = p_oLanguage.m_strCode;
    l_CLanguageSync.m_oBody.m_strName = p_oLanguage.m_strName;
    l_CLanguageSync.m_oBody.m_strSort = p_oLanguage.m_strSort;
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_CLanguageSync.ToString(l_pIJson);
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send:[%s]", l_strMessage.c_str());
    return true;
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


bool CBusinessImpl::SetRedisLanguageInfo(CLanguageInfo& p_oLanguageInfo)
{
    JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
    if (l_pJson)
    {
		std::string l_strVal = p_oLanguageInfo.ToJson(l_pJson);
		return m_pRedisClient->HSet(LANGUAGE_KEY, p_oLanguageInfo.m_strGuid, l_strVal);
    }
    return false;
}

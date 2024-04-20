#include "Boost.h"
#include "BusinessImpl.h"

#define pi 3.1415926535897932384626433832795
#define EARTH_RADIUS 6378.137 //地球半径 KM

#define TIMER_CMD_NAME	"get_login_token"

const int HOUHEADRELCODE = 99;
const unsigned long THREADID_RECEIVENOTIFY = 10000;
void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_PERSONINFODATA_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();

	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

	m_pJsonFty = ICCGetIJsonFactory();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();

	m_uProcThreadCount = 0;
	m_uCurrentThreadIndex = 0;
	m_bFlag = true;
	_CreateThreads();

	_InitProcNotifys();
}

void CBusinessImpl::_InitProcNotifys()
{
	m_mapFuncs.insert(std::make_pair(TIMER_CMD_NAME, &CBusinessImpl::OnNotifGetLoginToken));
	m_mapFuncs.insert(std::make_pair("query_id_info", &CBusinessImpl::OnNotifQueryIdInfo));
	m_mapFuncs.insert(std::make_pair("query_name_case_data", &CBusinessImpl::OnNotifQueryNameCaseData));
	m_mapFuncs.insert(std::make_pair("query_veh_illegal_info", &CBusinessImpl::OnNotifQueryVehIllegalInfo));
	m_mapFuncs.insert(std::make_pair("query_case_info", &CBusinessImpl::OnNotifQueryCaseInfo));
	m_mapFuncs.insert(std::make_pair("query_cred_info", &CBusinessImpl::OnNotifQueryCredInfo));
	m_mapFuncs.insert(std::make_pair("query_cert_info", &CBusinessImpl::OnNotifQueryCertInfo));
	m_mapFuncs.insert(std::make_pair("query_case_veh_info", &CBusinessImpl::OnNotifQueryCaseVehInfo));
	m_mapFuncs.insert(std::make_pair("query_person_tag_info", &CBusinessImpl::OnNotifQueryPersonTagInfo));
	m_mapFuncs.insert(std::make_pair("query_addr_info", &CBusinessImpl::OnNotifQueryAddrInfo));
	m_mapFuncs.insert(std::make_pair("query_comp_info", &CBusinessImpl::OnNotifQueryCompInfo));
	m_mapFuncs.insert(std::make_pair("query_hous", &CBusinessImpl::OnNotifQueryHousInfo));
	m_mapFuncs.insert(std::make_pair("query_person_info", &CBusinessImpl::OnNotifQueryPersonInfo));

	m_mapFuncs.insert(std::make_pair("query_tibetan_related_groups_info", &CBusinessImpl::OnNotifQueryTibetanRelatedGroupsInfo));
	m_mapFuncs.insert(std::make_pair("query_drug_info", &CBusinessImpl::OnNotifQueryDrugInfo));
	m_mapFuncs.insert(std::make_pair("query_escape_info", &CBusinessImpl::OnNotifQueryEscapeInfo));
	m_mapFuncs.insert(std::make_pair("query_person_key_info", &CBusinessImpl::OnNotifQueryPersonKeyInfo));
	m_mapFuncs.insert(std::make_pair("query_person_photo_info", &CBusinessImpl::OnNotifQueryPersonPhotoInfo));

	m_mapFuncs.insert(std::make_pair("query_name_case_data_and_case_info", &CBusinessImpl::OnNotifQueryNameCaseDataAndCaseInfo));
	m_mapFuncs.insert(std::make_pair("query_cert_info_and_person_info", &CBusinessImpl::OnNotifQueryCertInfoAndPersonInfo));
	m_mapFuncs.insert(std::make_pair("query_cred_info_and_person_info", &CBusinessImpl::OnNotifQueryCredInfoAndPersonInfo));
	m_mapFuncs.insert(std::make_pair("query_case_veh_info_and_case_info", &CBusinessImpl::OnNotifQueryCaseVehInfoAndCaseInfo));

	m_mapFuncs.insert(std::make_pair("query_address_info", &CBusinessImpl::OnNotifQueryAddressInfo));

	m_mapFuncs.insert(std::make_pair("syn_nacos_params", &CBusinessImpl::OnReceiveSynNacosParams));

	m_mapFuncs.insert(std::make_pair("get_service_info", &CBusinessImpl::OnNotifGetServiceInfo));

}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	if (!ReadConfig())
	{
		ICC_LOG_ERROR(m_pLog, "failed to read PersonInfoData configuration file");
		ICC_LOG_INFO(m_pLog, "failed to start PersonInfoData");
		return;
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_id_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_name_case_data", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_veh_illegal_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_case_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_cred_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_cert_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_case_veh_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_person_tag_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_addr_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_comp_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_hous", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_person_info", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_tibetan_related_groups_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_drug_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_escape_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_person_key_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_person_photo_info", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_name_case_data_and_case_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_cert_info_and_person_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_cred_info_and_person_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_case_veh_info_and_case_info", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_address_info", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_service_info", _OnReceiveNotify);

	GetHouHeadRelFromConfig();
	//行政区划名称转code
	GetDistrictCodeMapFromConfig();

	GetPostUrlMapFromConfig();

	GetKeyInfoMapFromConfig();
	//添加定时器 3500s调用一次
	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, 3500, 1);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

bool CBusinessImpl::ReadConfig()
{
	m_strIsUsing = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/IsUsing", "0");
	if (m_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		//return false;
	}

	m_strServerIP = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/serverip", "");         // 服务IP
	m_strServerPort = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/serverport", "80");   // 服务端口

	m_strtype = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/type", "");                 // 用户登录id：511023198201089876
	m_strpoliceNo = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/policeno", "");         // policeNo
	m_strUserId = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/userid", "");             // 用户id
	m_strAppId = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/appid", "");               // appId
	m_strSenderId = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/senderid", "");         // 角色id
	m_strGroupId = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/groupid", "");           // 角色组id

	m_strpublicKey = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/publicKey", "");       // 公钥信息
	m_strTagAppId = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/tagAppId", "");         // tagappid
	m_strIp = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/ip", "");                     // 公钥信息
	m_strMac = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/mac", "");                   // tagappid
	m_strareaCode = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/areaCode", "");
	// 模拟数据与现场真实数据切换开关 : 1 为 现场真是数据， 其他为模拟数据,
	m_strConfig = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/config", "0");
	m_strEscapeInfo = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/escapeinfo", "0");

	m_strNewKeyInfo = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/newkeyinfo", "0");

	//m_strcar = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/carKeyName", "");    //号牌
	m_strbody = m_pString->Format("{\"userId\":\"%s\",\"ip\":\"%s\",\"mac\":\"%s\"}", m_strUserId.c_str(), m_strIp.c_str(), m_strMac.c_str());

	m_intTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/PersonInfoData/TimeOut", "5"));

	m_strAiaAlarmServerName = m_pConfig->GetValue("ICC/Plugin/PersonInfoData/aia_alarm_server", "aia-alarm-service");

	return true;
}

void CBusinessImpl::OnStop()
{
	_DestoryThreads();
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin OnDestroy.", MODULE_NAME);
}

void CBusinessImpl::OnNotifGetLoginToken(ObserverPattern::INotificationPtr p_pNotify)
{
	//登录 定时调用
	ICC_LOG_DEBUG(m_pLog, "OnNotifGetLoginToken start");

	int l_ncount = 0;
	while (l_ncount < 3)
	{
		if (GetLoginToken())
		{
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "Login failed");
		l_ncount++;
	}
}

void CBusinessImpl::OnNotifGetServiceInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	//定时调用
	ICC_LOG_DEBUG(m_pLog, "OnNotifGetServiceInfo start");
	int l_ncount = 0;

	m_bFlag = true;

	while (l_ncount < 3)
	{
		if (GetServiceInfo(m_strAiaAlarmServerName, m_strAiaAlarmSendServerIp, m_strAiaAlarmSendServerPort))
		{
			m_bFlag = false;
			return;
		}
		ICC_LOG_ERROR(m_pLog, "get GetServiceInfo info failed");
		l_ncount++;
	}
}

void CBusinessImpl::OnNotifQueryIdInfo(ObserverPattern::INotificationPtr p_pNotify)
{

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryIdInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息
	PROTOCOL::CGetQueryIdInfoRequest l_IDRequest;

	if (!l_IDRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_IDRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	//回复
	PROTOCOL::CPushIdInfoRequest l_oPushIdInfoRequest;
	PostQueryIdInfo(l_IDRequest, l_oPushIdInfoRequest);

	//构建消息回复前端页面
	BuildGetQueryIdInfoRespond(l_oPushIdInfoRequest, p_pNotify);
}

void CBusinessImpl::PostQueryIdInfo(PROTOCOL::CGetQueryIdInfoRequest& p_pRequest, PROTOCOL::CPushIdInfoRequest& p_pSponse)
{

	if (p_pRequest.m_oBody.m_strIdNo.empty() && p_pRequest.m_oBody.m_strmobile.empty())
	{
		p_pSponse.m_strstatus = "499";
		p_pSponse.m_strmsg = "receive message  /body/IdNo empty or /body/mobile empty";
		return;
	}
	std::string strCmd = "query_id_info";
	std::string strIdInfo;

	std::string strTarget = GetPostUrl(strCmd);

	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00026");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "6628124917760000";
	}

	//构建header
	PROTOCOL::InputParm p_pInputParm;
	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	p_pInputParm.mapkey.insert(std::make_pair("idno", p_pRequest.m_oBody.m_strIdNo));
	p_pInputParm.mapkey.insert(std::make_pair("mobile", p_pRequest.m_oBody.m_strmobile));
	//构建消息体 
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryIdInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strIdInfo = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryIdInfo strIdInfo[%s]", strIdInfo.c_str());

	if (strIdInfo.empty())
	{
		p_pSponse.m_strstatus = "500";
		p_pSponse.m_strmsg = "Failed";
		return;
	}

	//解析  Parse
	if (!p_pSponse.ParseString(strIdInfo, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strstatus = "501";
		p_pSponse.m_strmsg = strIdInfo;
		return;
	}
}

void CBusinessImpl::BuildGetQueryIdInfoRespond(PROTOCOL::CPushIdInfoRequest p_pPushIdInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_ERROR(m_pLog, "PersonInfoData BuildGetQueryIdInfoRespond start");
	//回复
	PROTOCOL::CGetIdInfoRespond Respond;
	PROTOCOL::CGetIdInfoRespond::Data data;
	Respond.m_strmessage = p_pPushIdInfoRequest.m_strmsg;
	if (0 == p_pPushIdInfoRequest.m_strcode.compare("200"))
	{
		Respond.m_strcode = p_pPushIdInfoRequest.m_strcode;
		for (size_t i = 0; i < p_pPushIdInfoRequest.m_vecData.size(); i++)
		{
			data.m_strmobile = p_pPushIdInfoRequest.m_vecData.at(i).m_strmobile;
			data.m_stridno = p_pPushIdInfoRequest.m_vecData.at(i).m_stridno;
			data.m_strlevel = p_pPushIdInfoRequest.m_vecData.at(i).m_strlevel;
			data.m_strsex = p_pPushIdInfoRequest.m_vecData.at(i).m_strsex;
			data.m_strage = p_pPushIdInfoRequest.m_vecData.at(i).m_strage;
			data.m_strchname = p_pPushIdInfoRequest.m_vecData.at(i).m_strchname;
			data.m_strhplace = p_pPushIdInfoRequest.m_vecData.at(i).m_strhplace;
			data.m_strbirthday = p_pPushIdInfoRequest.m_vecData.at(i).m_strbirthday;
			Respond.m_vecData.push_back(data);
		}
	}
	else
	{
		Respond.m_strcode = p_pPushIdInfoRequest.m_strstatus;
	}

	std::string l_strMessage = Respond.ToString(ICCGetIJsonFactory()->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryIdInfoRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryNameCaseData(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryNameCaseData receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  QueryNameCaseData
	PROTOCOL::CGetQueryNameCaseDataRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryNameCaseDataRequest l_PostQueryNameCaseDataRequest;
	PostQueryNameCaseData(l_oRequest, l_PostQueryNameCaseDataRequest);

	//构建消息回复前端页面
	BuildGetQueryNameCaseDataRespond(l_PostQueryNameCaseDataRequest, p_pNotify);
}

void CBusinessImpl::BuildGetQueryNameCaseDataRespond(PROTOCOL::CPostQueryNameCaseDataRequest p_pPostQueryNameCaseDataRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryNameCaseDataRespond Respond;
	PROTOCOL::CGetQueryNameCaseDataRespond::Results results;
	Respond.m_strcode = p_pPostQueryNameCaseDataRequest.m_strstatus;
	Respond.m_strmessage = p_pPostQueryNameCaseDataRequest.m_strmsg;
	Respond.m_Data.m_strtotalRows = p_pPostQueryNameCaseDataRequest.m_Data.m_strtotalRows;
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildGetQueryNameCaseDataRespond strQueryNameCaseData");
	for (int i = 0; i < std::atoi(Respond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		results.m_strcaseNo = p_pPostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseNo;
		results.m_strcacsName = p_pPostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcacsName;
		results.m_strcaseTime = ToStringTime(p_pPostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseTime);
		results.m_strcertNum = p_pPostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcertNum;
		results.m_strname = p_pPostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strname;
		Respond.m_Data.m_vecResults.push_back(results);
	}
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryNameCaseDataRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryVehIllegalInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryVehIllegalInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  QueryNameCaseData
	PROTOCOL::CGetQueryVehIllegalInfoRequest l_Request;

	if (!l_Request.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
	    return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_Request.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryVehIllegalInfoRequest m_oPostQueryVehIllegalInfoRequest;
	do
	{
		if (l_Request.m_oBody.m_strvehPlateNum.empty())
		{
			m_oPostQueryVehIllegalInfoRequest.m_strstatus = "499";
			m_oPostQueryVehIllegalInfoRequest.m_strmsg = "receive message /body/vehPlateNum empty";
			break;
		}
		std::string strCmd = "query_veh_illegal_info";
		std::string strQueryVehIllegalInfo;

		std::string strTarget = GetPostUrl(strCmd);
		if (strTarget.empty())
		{
			strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00045");
		}

		std::string strServiceId = GetPostServiceId(strCmd);
		if (strServiceId.empty())
		{
			strServiceId = "9790273664057344";
		}

		std::string strContent;                             //请求内容
		std::string strErrorMessage;                        //请求返回错误信息
		PROTOCOL::InputParm p_pInputParm;
		//组装Condition结构
		if (!l_Request.m_oBody.m_strvehLicTname.empty())
		{
			int p = l_Request.m_oBody.m_strvehLicTname.find(m_strcar.c_str());
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryVehIllegalInfo p [%d]   strvehLicTname[%d]", p, m_strcar.size());
			if (p > 0)
			{	
				l_Request.m_oBody.m_strvehLicTname.erase(p, m_strcar.size());
				ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryVehIllegalInfo m_strvehLicTname[%s] ", l_Request.m_oBody.m_strvehLicTname.c_str());
			}
			p_pInputParm.Condition = m_pString->Format("(VEH_PLATE_NUM = '%s' and VEH_LIC_TNAME = '%s')", l_Request.m_oBody.m_strvehPlateNum.c_str(), l_Request.m_oBody.m_strvehLicTname.c_str());
		}
		else
		{
			p_pInputParm.Condition = m_pString->Format("(VEH_PLATE_NUM = '%s')", l_Request.m_oBody.m_strvehPlateNum.c_str());
		}
		p_pInputParm.Parafs = { "VEH_LIC_TNAME","VEH_PLATE_NUM","ILLE_PLAC_ADDR_NAME","TRAF_ILLREC_TCODE","TRAF_ILLREC_TNAME","ILLE_TIME", "PROC_TIME", "ILLE_MAN_HAND_RCODE" };
	    p_pInputParm.maporderParafs.insert(std::make_pair("ILLE_TIME", "desc"));
		//构建消息体
		strContent = BuildPostContentRespond(l_Request.m_oBody.m_oBodyInfo, p_pInputParm);

		std::string strData = "";
		if (0 == m_strConfig.compare("1"))
		{
			std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
			//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
			strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryVehIllegalInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
				strServiceId.c_str(), strContent.c_str());
		}
		else
		{
			strData = GetPostTextByContent(strCmd);
		}
		strQueryVehIllegalInfo = Subreplace(strData, "\"null\"", "\"\"");
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryVehIllegalInfo strQueryVehIllegalInf[%s]", strQueryVehIllegalInfo.c_str());

		if (strQueryVehIllegalInfo.empty())
		{
			m_oPostQueryVehIllegalInfoRequest.m_strstatus = "500";
			m_oPostQueryVehIllegalInfoRequest.m_strmsg = "Failed";

			break;
		}

		//解析  Parse
		if (!m_oPostQueryVehIllegalInfoRequest.ParseString(strQueryVehIllegalInfo, m_pJsonFty->CreateJson()))
		{
			m_oPostQueryVehIllegalInfoRequest.m_strstatus = "501";
			m_oPostQueryVehIllegalInfoRequest.m_strmsg = strQueryVehIllegalInfo;

			break;
		}

	} while (0);

	//构建消息回复前端页面
	BuildGetQueryVehIllegalInfoRespond(m_oPostQueryVehIllegalInfoRequest, p_pNotify);
}

void CBusinessImpl::BuildGetQueryVehIllegalInfoRespond(PROTOCOL::CPostQueryVehIllegalInfoRequest p_pPostQueryVehIllegalInfoRequest, ObserverPattern::INotificationPtr p_pNotify)\
{
	//回复
	PROTOCOL::CGetQueryVehIllegalInfoRespond Respond;
	PROTOCOL::CGetQueryVehIllegalInfoRespond::Results results;
	Respond.m_strcode = p_pPostQueryVehIllegalInfoRequest.m_strstatus;
	Respond.m_strmessage = p_pPostQueryVehIllegalInfoRequest.m_strmsg;

	//去重
	auto ite = std::unique(p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.begin(), p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.end(),
		[](const PROTOCOL::CPostQueryVehIllegalInfoRequest::Results& d1, const PROTOCOL::CPostQueryVehIllegalInfoRequest::Results& d2)
		{
			return d1.m_strilleTime == d2.m_strilleTime;
		});

	p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.erase(ite, p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.end());

	Respond.m_Data.m_strtotalRows = std::to_string(p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.size());

	for (int i = 0; i < std::atoi(Respond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		results.m_strilleManHandRcode = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strilleManHandRcode;
		results.m_strilleTime = ToStringTime(p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strilleTime);
		results.m_strprocTime = ToStringTime(p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strprocTime);

		results.m_strtrafIllrecTcode = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strtrafIllrecTcode;
		results.m_strtrafIllrecTname = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strtrafIllrecTname;
		results.m_strIllePlacAddrName = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strIllePlacAddrName;

		results.m_strvehLicTname = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strvehLicTname;
		results.m_strvehPlateNum = p_pPostQueryVehIllegalInfoRequest.m_Data.m_vecResults.at(i).m_strvehPlateNum;

		Respond.m_Data.m_vecResults.push_back(results);
	}
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryVehIllegalInfoRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryCaseVehInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCaseVehInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetQueryCaseVehInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryCaseVehInfoRequest p_pPostQueryCaseVehInfoRequest;
	PostQueryCaseVehInfo(l_oRequest, p_pPostQueryCaseVehInfoRequest);

	// 发送响应消息
	BuildGetCaseVehInfoRespond(p_pPostQueryCaseVehInfoRequest, p_pNotify);
}

void CBusinessImpl::PostQueryCaseVehInfo(PROTOCOL::CGetQueryCaseVehInfoRequest& p_pRequest, PROTOCOL::CPostQueryCaseVehInfoRequest& p_pSponse)
{
	if (p_pRequest.m_strVehPlateNum.empty())
	{
		p_pSponse.m_strCode = "499";
		p_pSponse.m_strMessage = "receive message /body/VehPlateNum empty";
		return;
	}

	std::string strCmd = "query_case_veh_info";

	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00050");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790367410946048";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;
	p_pInputParm.Condition = m_pString->Format("(VEH_PLATE_NUM = '%s')", p_pRequest.m_strVehPlateNum.c_str());
	p_pInputParm.Parafs = { "CASE_NO","VEH_LIC_TCODE","VEH_PLATE_NUM","DIS_PLACE" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBodyInfo, p_pInputParm);

	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCaseVehInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCaseVehInfo strReceive:%s\n", strReceive.c_str());

	if (strReceive.empty())
	{
		p_pSponse.m_strCode = "500";
		p_pSponse.m_strMessage = "Failed";

		return;
	}

	//解析  Parse
	if (!p_pSponse.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strCode = "501";
		p_pSponse.m_strMessage = strReceive;

		return;
	}
}

void CBusinessImpl::BuildGetCaseVehInfoRespond(PROTOCOL::CPostQueryCaseVehInfoRequest& p_oRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryCaseVehInfoRespond Respond;
	PROTOCOL::CGetQueryCaseVehInfoRespond::CData::CList results;

	Respond.m_strCode = p_oRequest.m_strCode;
	Respond.m_strMessage = p_oRequest.m_strMessage;
	Respond.m_strData.m_strCount = p_oRequest.m_strData.m_strCount;

	for (int i = 0; i < std::atoi(Respond.m_strData.m_strCount.c_str()); ++i)
	{
		results.m_strCaseNo = p_oRequest.m_strData.m_vecLists.at(i).m_strCaseNo;
		results.m_strDisPlace = p_oRequest.m_strData.m_vecLists.at(i).m_strDisPlace;
		results.m_strVehPlateNum = p_oRequest.m_strData.m_vecLists.at(i).m_strVehPlateNum;
		results.m_strVehLicTcode = p_oRequest.m_strData.m_vecLists.at(i).m_strVehLicTcode;
		Respond.m_strData.m_vecLists.push_back(results);
	}

	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetCaseVehInfoRespond send message:%s", l_strMessage.c_str());

}

void CBusinessImpl::OnNotifQueryPersonTagInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonTagInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetQueryPersonTagInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonTagInfo start");

	// 发送响应消息
	PROTOCOL::CPostQueryPersonTagInfoRespond l_oRespond;
	PostQueryPersonTagInfo(l_oRequest.m_oBodyInfo, l_oRequest.m_strCardId, l_oRespond);

	// 判断涉藏 吸毒 在逃 重点人员便签
	BuildPersonTagInfo(l_oRequest.m_oBodyInfo, l_oRequest.m_strCardId, l_oRespond);

	//回复前端页面
	BuildGetPersonTagRespond(l_oRespond, p_pNotify);
}

void CBusinessImpl::PostQueryPersonTagInfo(PROTOCOL::BodyInfo& p_oRequestInfo, const std::string strIDCard, PROTOCOL::CPostQueryPersonTagInfoRespond& p_pPostRequest)
{
	if (p_oRequestInfo.m_struserId.empty() || strIDCard.empty())
	{
		p_pPostRequest.m_strCode = "499";
		p_pPostRequest.m_strMessage = "receive message /body/userId empty or /body/CertNum empty";
		return;
	}
	std::string strCmd = "query_person_tag_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00128");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "19637802295623680";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;
	p_pInputParm.mapkey.insert(std::make_pair("appId", m_strTagAppId));
	p_pInputParm.mapkey.insert(std::make_pair("methodName", "getArchiveTags"));
	p_pInputParm.mapkey.insert(std::make_pair("authSign", p_oRequestInfo.m_struserId));
	p_pInputParm.mapkey.insert(std::make_pair("publicKey", m_strpublicKey));
	p_pInputParm.mapkey.insert(std::make_pair("time", std::to_string(m_pDateTime->CurrentDateTime() / 1000)));
	p_pInputParm.mapkey.insert(std::make_pair("module", "ren"));
	p_pInputParm.mapkey.insert(std::make_pair("ZJHM", strIDCard));

	//构建消息体
	strContent = BuildPostContentRespond(p_oRequestInfo, p_pInputParm);

	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildPersonTagRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "BuildPersonTagRespond strReceive:[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_pPostRequest.m_strCode = "500";
		p_pPostRequest.m_strMessage = "Failed";
		return;
	}

	//解析  Parse
	if (!p_pPostRequest.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_pPostRequest.m_strCode = "501";
		p_pPostRequest.m_strMessage = strReceive;
		return;
	}
}

void CBusinessImpl::BuildPersonTagInfo(PROTOCOL::BodyInfo& p_oBodyInfo, const std::string strIdCard, PROTOCOL::CPostQueryPersonTagInfoRespond& l_oRespond)
{
	//解析涉藏请求消息  
	PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRequest strRelatedRequest;
	strRelatedRequest.m_oBody.m_oBodyInfo = p_oBodyInfo;
	strRelatedRequest.m_oBody.m_strCertNum = strIdCard;
	PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest strTibetanRespond;
	PostQueryTibetanRelatedGroupsInfo(strRelatedRequest, strTibetanRespond);
	l_oRespond.m_bTibetanRespond = std::atoi(strTibetanRespond.m_Data.m_strtotalRows.c_str()) > 0 ? true : false;

	//解析吸毒请求消息  
	PROTOCOL::CGetQueryDrugInfoRequest strDrugRequest;
	strDrugRequest.m_oBody.m_oBodyInfo = p_oBodyInfo;
	strDrugRequest.m_oBody.m_strCertNum = strIdCard;
	PROTOCOL::CPostQueryDrugInfoRequest strDrugRespond;
	PostQueryDrugInfo(strDrugRequest, strDrugRespond);
	l_oRespond.m_bDrugRespond = std::atoi(strDrugRespond.m_Data.m_strtotalRows.c_str()) > 0 ? true : false;

	//解析在逃请求消息  

	PROTOCOL::CGetQueryEscapeInfoRequest stryEscapequest;
	stryEscapequest.m_oBodyInfo = p_oBodyInfo;
	stryEscapequest.m_strCerdNum = strIdCard;
	PROTOCOL::CEscapeInfoRespond strEscapeRespond;
	if (0 == m_strEscapeInfo.compare("1"))
	{
		BuildEscapeInfoRespond(stryEscapequest, strEscapeRespond);
	}
	l_oRespond.m_bEscapeRespond = std::atoi(strEscapeRespond.m_strData.m_strCount.c_str()) > 0 ? true : false;

	//解析重点人员请求消息  
	PROTOCOL::CGetQueryPersonKeyInfoRequest strPersonKeyRequest;
	strPersonKeyRequest.m_oBodyInfo = p_oBodyInfo;
	strPersonKeyRequest.m_strCerdNum = strIdCard;
	PROTOCOL::CPersonKeyInfoRespond strPersonKeyRespond;
	BuildPersonKeyInfoRespond(strPersonKeyRequest, strPersonKeyRespond);

	// 添加个字段，返回关注人员标签  m_mapKeyInfo
	l_oRespond.m_bPersonKeyRespond = std::atoi(strPersonKeyRespond.m_strData.m_strCount.c_str()) > 0 ? true : false;

	if (!strPersonKeyRespond.m_strData.m_vecLists.empty() && l_oRespond.m_bPersonKeyRespond)
	{
		for (size_t i = 0; i < strPersonKeyRespond.m_strData.m_vecLists.size(); i++)
		{
			l_oRespond.m_vecAttentionTag.push_back(GetKeyInfo(strPersonKeyRespond.m_strData.m_vecLists.at(i).m_strPersonKeyTypeName));
		}
	}
}

void CBusinessImpl::BuildGetPersonTagRespond(PROTOCOL::CPostQueryPersonTagInfoRespond& p_pPostQueryPersonTagInfo, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryPersonTagInfoRespond Respond;
	PROTOCOL::CGetQueryPersonTagInfoRespond::CData results;
	PROTOCOL::CGetQueryPersonTagInfoRespond::CData::CTag resultstag;
	Respond.m_strCode = p_pPostQueryPersonTagInfo.m_strCode;
	Respond.m_strMessage = p_pPostQueryPersonTagInfo.m_strMessage;
	Respond.m_strCount = p_pPostQueryPersonTagInfo.m_strCount;
	ICC_LOG_DEBUG(m_pLog, "BuildGetPersonTagRespond m_strCount");

	if (0 == Respond.m_strCode.compare("200"))
	{
		for (int i = 0; i < std::atoi(Respond.m_strCount.c_str()); ++i)
		{
			results.m_strTagType = p_pPostQueryPersonTagInfo.m_strData.at(i).m_strTagType;
			results.m_strTagTypeName = p_pPostQueryPersonTagInfo.m_strData.at(i).m_strTagTypeName;

			for (size_t j = 0; j < p_pPostQueryPersonTagInfo.m_strData.at(i).m_vecTags.size(); ++j)
			{
				resultstag.m_strTagCode = p_pPostQueryPersonTagInfo.m_strData.at(i).m_vecTags[j].m_strTagCode;
				resultstag.m_strTagName = p_pPostQueryPersonTagInfo.m_strData.at(i).m_vecTags[j].m_strTagName;
				results.m_vecTags.push_back(resultstag);
			}
			Respond.m_strData.push_back(results);
			results.m_vecTags.clear();
		}
	}
	else
	{
		Respond.m_strState = p_pPostQueryPersonTagInfo.m_strState;
		Respond.m_strErrcode = p_pPostQueryPersonTagInfo.m_strErrcode;
	}

	if (p_pPostQueryPersonTagInfo.m_bTibetanRespond)
	{
		resultstag.m_strTagCode = "01";
		resultstag.m_strTagName = m_strChineseLib["Tibetan"];
		results.m_vecTags.push_back(resultstag);
		results.m_strTagType = "rybq";
		results.m_strTagTypeName = m_strChineseLib["PersonTag"];
		Respond.m_strData.push_back(results);
		results.m_vecTags.clear();
	}

	if (p_pPostQueryPersonTagInfo.m_bDrugRespond)
	{
		resultstag.m_strTagCode = "02";
		resultstag.m_strTagName = m_strChineseLib["Drug"];
		results.m_vecTags.push_back(resultstag);
		results.m_strTagType = "rybq";
		results.m_strTagTypeName = m_strChineseLib["PersonTag"];
		Respond.m_strData.push_back(results);
		results.m_vecTags.clear();
	}

	if (p_pPostQueryPersonTagInfo.m_bEscapeRespond)
	{
		resultstag.m_strTagCode = "03";
		resultstag.m_strTagName = m_strChineseLib["Escape"];
		results.m_vecTags.push_back(resultstag);
		results.m_strTagType = "rybq";
		results.m_strTagTypeName = m_strChineseLib["PersonTag"];
		Respond.m_strData.push_back(results);
		results.m_vecTags.clear();
	}

	if (p_pPostQueryPersonTagInfo.m_bPersonKeyRespond)
	{
		resultstag.m_strTagCode = "04";
		resultstag.m_strTagName = m_strChineseLib["PersonKey"];
		results.m_vecTags.push_back(resultstag);
		results.m_strTagType = "rybq";
		results.m_strTagTypeName = m_strChineseLib["PersonTag"];
		Respond.m_strData.push_back(results);
		results.m_vecTags.clear();
	}

	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetPersonTagRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryCaseInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive messages:[%s]", strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetQueryCaseInfoRequest l_request;
	if (!l_request.ParseString(strMsg, l_pIJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Json Error!!!");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_request.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryCaseInfoRequest m_PostQueryCaseInfoRequest;
	PostQueryCaseInfo(l_request, m_PostQueryCaseInfoRequest);

	// 发送响应消息
	BuildQueryCaseInfoRespond(m_PostQueryCaseInfoRequest, p_pNotify);
}

void CBusinessImpl::BuildQueryCaseInfoRespond(PROTOCOL::CPostQueryCaseInfoRequest& p_pPostCaseVehInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	//回复
	PROTOCOL::CQueryCaseInfoResponse Respond;
	PROTOCOL::CQueryCaseInfoResponse::CData::CList results;
	Respond.m_oBody.code = p_pPostCaseVehInfoRequest.m_oBody.code;
	Respond.m_oBody.message = p_pPostCaseVehInfoRequest.m_oBody.message;
	Respond.m_oBody.m_Data.m_strCount = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_strCount;

	for (int i = 0; i < std::atoi(Respond.m_oBody.m_Data.m_strCount.c_str()); ++i)
	{
		results.m_accepTime = ToStringTime(p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_accepTime);

		results.m_accepUnitPsag = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_accepUnitPsag;
		results.m_arrsceTime = ToStringTime(p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_arrsceTime);
		results.m_casClaName = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_casClaName;
		results.m_caseAddrAddrCode = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseAddrAddrCode;
		results.m_caseClasCode = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseClasCode;
		results.m_caseDate = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseDate;
		results.m_caseName = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseName;
		results.m_caseNo = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseNo;
		results.m_caseOrgaPsag = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseOrgaPsag;
		results.m_caseOrgaPsagCode = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseOrgaPsagCode;
		results.m_caseSourDesc = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseSourDesc;
		results.m_caseTimeTimper = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseTimeTimper;
		results.m_caseTypeCode = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_caseTypeCode;
		results.m_crimPurpDesc = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_crimPurpDesc;
		results.m_discTime = ToStringTime(p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_discTime);
		results.m_mainCaseCaseNo = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_mainCaseCaseNo;
		results.m_minorCaseCaseNo = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_minorCaseCaseNo;
		results.m_repoCaseUnitAddrName = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_repoCaseUnitAddrName;
		results.m_setLawsDate = p_pPostCaseVehInfoRequest.m_oBody.m_Data.m_vecList.at(i).m_setLawsDate;
		Respond.m_oBody.m_Data.m_vecList.push_back(results);
	}
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildQueryCaseInfoRespond send message:%s", l_strMessage.c_str());

}

void CBusinessImpl::OnNotifQueryCredInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryCredInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CGetQueryCredInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryCredInfoRequest l_oPostQueryCredInfoRequest;
	PostQueryCredInfo(l_oRequest, l_oPostQueryCredInfoRequest);
	//构建消息回复前端页面
	BuildGetQueryCredInfoRespond(l_oPostQueryCredInfoRequest, p_pNotify);
}

void CBusinessImpl::PostQueryCredInfo(PROTOCOL::CGetQueryCredInfoRequest& p_pRequest, PROTOCOL::CPostQueryCredInfoRequest& p_pSponse)
{
	if (p_pRequest.m_oBody.m_credNum.empty() && p_pRequest.m_oBody.m_vehPlateNum.empty())
	{
		p_pSponse.m_strstatus = "499";
		p_pSponse.m_strmsg = "receive message /body/CredNum empty or /body/VehPlateNum empty";
		return;
	}
	std::string strCmd = "query_cred_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00039");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790245784518656";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;
	//车牌号与证件号二选一

	if (!p_pRequest.m_oBody.m_credNum.empty())
	{
		p_pInputParm.Condition = m_pString->Format("(CRED_NUM = '%s')", p_pRequest.m_oBody.m_credNum.c_str());
	}
	else if (!p_pRequest.m_oBody.m_vehPlateNum.empty())
	{
		p_pInputParm.Condition = m_pString->Format("(VEH_PLATE_NUM = '%s')", p_pRequest.m_oBody.m_vehPlateNum.c_str());
	}

	p_pInputParm.Parafs = { "MOB","CRED_NUM","VEH_LIC_TNAME","VEH_PLATE_NUM","VEHC_BRAND_MODEL",
		"VEH_TYPE","VEH_TNAME","VEH_COLOR_NAME","ENGINE_NO","VEH_USAG_NAME","VEH_STAT_CODE","VEH_STAT_NAME",
		"INSP_VE_DATE","INS_END_DATE","FIRST_REG_DATE","REG_COM_PSAG" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCredInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else {
		strData = GetPostTextByContent(strCmd);
	}
	std::string  strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCredInfo strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_pSponse.m_strstatus = "500";
		p_pSponse.m_strmsg = "Failed";

		return;
	}
	//解析  Parse

	if (!p_pSponse.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strstatus = "501";
		p_pSponse.m_strmsg = strReceive;

		return;
	}
}

void CBusinessImpl::BuildGetQueryCredInfoRespond(PROTOCOL::CPostQueryCredInfoRequest p_pPostQueryCredInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CQueryCredInfoResponse respond;
	PROTOCOL::CQueryCredInfoResponse::Results results;
	respond.m_oBody.code = p_pPostQueryCredInfoRequest.m_strstatus;
	respond.m_oBody.message = p_pPostQueryCredInfoRequest.m_strmsg;
	
	for (int i = 0; i < std::atoi(p_pPostQueryCredInfoRequest.m_Data.m_strtotalRows.c_str()); ++i)
	{
		if (0 == p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatCode.compare("E"))
		{
			continue;
		}

		results.m_insEndDate = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_insEndDate;
		results.m_credNum = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_credNum;
		results.m_mob = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob;
		results.m_vehLicTname = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehLicTname;
		results.m_strVehLicTcode = GetCarCodeFromDataBase(p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehLicTname);
		results.m_vehPlateNum = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehPlateNum;
		results.m_vehcBrandModel = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehcBrandModel;
		results.m_vehType = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehType;

		results.m_m_vehColorName = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_m_vehColorName;
		results.m_engineNo = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_engineNo;
		results.m_vehUsagName = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehUsagName;
		results.m_vehStatCode = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatCode;
		results.m_vehStatName = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatName;
		results.m_inspVeDate = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_inspVeDate;
		results.m_firstRegDate = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_firstRegDate;

		results.m_regComPsag = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_regComPsag;
		results.m_vehTname = p_pPostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehTname;

		respond.m_oBody.m_vecData.push_back(results);
	}
	respond.m_oBody.m_strtotalRows = std::to_string(respond.m_oBody.m_vecData.size());
	std::string l_strMessage = respond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryCredInfoRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryCertInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryCertInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetQueryCertInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryCertInfoRequest l_oSponse;

	PostQueryCertInfo(l_oRequest, l_oSponse);
	if (std::atoi(l_oSponse.m_Data.m_strtotalRows.c_str()) > 0)
	{
		//排序 LAST_TIME
		std::sort(l_oSponse.m_Data.m_vecResults.begin(), l_oSponse.m_Data.m_vecResults.end(),
			[](const PROTOCOL::CPostQueryCertInfoRequest::Results& dept1, const PROTOCOL::CPostQueryCertInfoRequest::Results& dept2)
			{
				return atoll(dept1.m_lastTime.c_str()) > atoll(dept2.m_lastTime.c_str());
			});

		if (!l_oSponse.m_Data.m_vecResults.at(0).m_domicNum.empty() && !l_oSponse.m_Data.m_vecResults.at(0).m_domicAdmDiv.empty())
		{
			l_oRequest.m_oBody.m_strCertNum = "";
			l_oRequest.m_oBody.m_strDomicNum = l_oSponse.m_Data.m_vecResults.at(0).m_domicNum.c_str();
			l_oRequest.m_oBody.m_strDomic_Adm_Div = l_oSponse.m_Data.m_vecResults.at(0).m_domicAdmDiv.c_str();
			l_oSponse.m_Data.m_vecResults.clear();
			PostQueryCertInfo(l_oRequest, l_oSponse);
		}
	}

	std::sort(l_oSponse.m_Data.m_vecResults.begin(), l_oSponse.m_Data.m_vecResults.end(),
		[](const PROTOCOL::CPostQueryCertInfoRequest::Results& dept1, const PROTOCOL::CPostQueryCertInfoRequest::Results& dept2)
		{
			return atoll(dept1.m_certNum.c_str()) < atoll(dept2.m_certNum.c_str());
		});

	//去重
	auto ite = std::unique(l_oSponse.m_Data.m_vecResults.begin(), l_oSponse.m_Data.m_vecResults.end(),
		[](const PROTOCOL::CPostQueryCertInfoRequest::Results& d1, const PROTOCOL::CPostQueryCertInfoRequest::Results& d2)
		{
			return d1.m_certNum == d2.m_certNum;
		});

	l_oSponse.m_Data.m_vecResults.erase(ite, l_oSponse.m_Data.m_vecResults.end());

	//构建消息回复前端页面
	BuildGetQueryCertInfoRespond(l_oSponse, p_pNotify);
}

void CBusinessImpl::PostQueryCertInfo(PROTOCOL::CGetQueryCertInfoRequest& p_pRequest, PROTOCOL::CPostQueryCertInfoRequest& p_pSponse)
{

	std::string strQueryCertInfo;
	std::string strCmd = "query_cert_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00052");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790457701728256";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	if (!p_pRequest.m_oBody.m_strCertNum.empty())
	{
		p_pInputParm.Condition = m_pString->Format("(CERT_NUM = '%s')", p_pRequest.m_oBody.m_strCertNum.c_str());
	}
	else
	{
		p_pInputParm.Condition = m_pString->Format("(DOMIC_NUM = '%s' and DOMIC_ADM_DIV = '%s')", p_pRequest.m_oBody.m_strDomicNum.c_str(), p_pRequest.m_oBody.m_strDomic_Adm_Div.c_str());
	}

	p_pInputParm.Parafs = { "PREDICT","CERT_NUM","DOMIC_NUM","HOU_HEAD_REL","DOMIC_ADDR","DOMIC_ADM_DIV","REL_TYPE" ,"LAST_TIME" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCertInfo strContent[%s]", strContent.c_str());
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCertInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strQueryCertInfo = Subreplace(strData, "\"null\"", "\"\"");
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCertInfo strQueryCertInfo[%s]", strQueryCertInfo.c_str());

	if (strQueryCertInfo.empty())
	{
		p_pSponse.m_strstatus = "500";
		p_pSponse.m_strmsg = "Failed";

		return;
	}

	//解析  Parse
	if (!p_pSponse.ParseString(strQueryCertInfo, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strstatus = "501";
		p_pSponse.m_strmsg = strQueryCertInfo;

		return;
	}
}

void CBusinessImpl::BuildGetQueryCertInfoRespond(PROTOCOL::CPostQueryCertInfoRequest p_pPostQueryCertInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CQueryCertInfoResponse respond;
	PROTOCOL::CQueryCertInfoResponse::Results results;
	respond.m_oBody.code = p_pPostQueryCertInfoRequest.m_strstatus;
	respond.m_oBody.message = p_pPostQueryCertInfoRequest.m_strmsg;
	respond.m_oBody.m_strtotalRows = std::to_string(p_pPostQueryCertInfoRequest.m_Data.m_vecResults.size());
	//respond.m_oBody.m_strtotalRows = p_pPostQueryCertInfoRequest.m_Data.m_strtotalRows;
	for (int i = 0; i < std::atoi(respond.m_oBody.m_strtotalRows.c_str()); ++i)
	{
		results.m_predict = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_predict;
		results.m_relType = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_relType;
		results.m_certNum = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_certNum;
		results.m_domicNum = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicNum;
		auto it = m_strHouHeadRel.find(p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_houHeadRel);
		if (it != m_strHouHeadRel.end()) {
			results.m_houHeadRel = it->second;
		}
		else {
			results.m_houHeadRel = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_houHeadRel;
		}
		results.m_domicAddr = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicAddr;
		results.m_domicAdmDiv = p_pPostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicAdmDiv;
		respond.m_oBody.m_vecData.push_back(results);
	}
	std::string l_strMessage = respond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryCertInfoRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryAddrInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryAddrInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CAddrInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CAddrInfoRespond l_oRespond;
	BuildAddrInfoRespond(l_oRequest, l_oRespond);
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryAddrInfo send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::BuildAddrInfoRespond(PROTOCOL::CAddrInfoRequest& p_oRequest, PROTOCOL::CAddrInfoRespond& p_oRespond)
{
	if (p_oRequest.m_strAddrName.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/AddrName empty";
		return;
	}
	std::string strCmd = "query_addr_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00042");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790272791642112";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.Condition = m_pString->Format("(ADDR_NAME = '%s')", p_oRequest.m_strAddrName.c_str());

	p_pInputParm.Parafs = { "INFO_DELE_JUDGE_FLAG","ADDR_NAME","LON","LAT","NORMA_LON","NORMA_LAT" };
	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);

	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildAddrInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else 
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");
	
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildAddrInfoRespond strReceive[%s]", strReceive.c_str());
	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}
}

void CBusinessImpl::OnNotifQueryCompInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCompInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CCompInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CCompInfoRespond l_oRespond;
	BuildCompInfoRespond(l_oRequest, l_oRespond);
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCompInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::BuildCompInfoRespond(PROTOCOL::CCompInfoRequest& p_oRequest, PROTOCOL::CCompInfoRespond& p_oRespond)
{
	if (p_oRequest.m_strCompName.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/CompName empty";
		return;
	}
	std::string strCmd = "query_comp_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00040");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790272233799680";
	}
	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.Condition = m_pString->Format("(COMP_NAME = '%s' )", p_oRequest.m_strCompName.c_str());

	p_pInputParm.Parafs = { "COMP_NAME","UNIT_TCODE","UNIT_TNAME","COM_ADDI_DETAIL_ADDR","COMP_ADDR_LON","COMP_ADDR_LAT",
		"OWN_POL_STAT_PSAG_CODE","OWN_POL_STAT_PSAG","NORMA_COMP_ADDR_LON","NORMA_COMP_ADDR_LAT" };

	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strTarget, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildCompInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else 
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");
	
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildCompInfoRespond strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}
}

void CBusinessImpl::OnNotifQueryHousInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryHousInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CHousInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CHousInfoRespond l_oRespond;
	BuildHousInfoRespond(l_oRequest, l_oRespond);

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryHousInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::BuildHousInfoRespond(PROTOCOL::CHousInfoRequest& p_oRequest, PROTOCOL::CHousInfoRespond& p_oRespond)
{
	if (p_oRequest.m_strHousOwnerAddrName.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/HousOwnerAddrName empty";
		return;
	}

	std::string strCmd = "query_hous";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00057");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790459194900480";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.Condition = m_pString->Format("(HOUS_OWNER_ADDR_NAME = '%s')", p_oRequest.m_strHousOwnerAddrName.c_str());

	p_pInputParm.Parafs = { "HOUS_CLAS_CODE","HOUS_CLAS_NAME","HOUS_TCODE","HOUS_TNAME","HOUS_OWNER_ADDR_NAME","OWN_POL_STAT_PSAG_CODE",
		"OWN_POL_STAT_PSAG","LON","LAT","DOPLA_NO_SERNUM","HOUSE_NO","CANC_JUDGE_FLAG","NORMA_LON","NORMA_LAT" };
	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);
;	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildHousInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else 
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildHousInfoRespond strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}

	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}
}

void CBusinessImpl::OnNotifQueryPersonInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryPersonInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  QueryNameCaseData
	PROTOCOL::CGetQueryPersonInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryPersonInfoRequest l_oPostQueryPersonInfoRequest;
	PROTOCOL::CPersonPhotoInfoRespond l_oPersonPhotoInfoRespond;
	if (!l_oRequest.m_oBody.m_strmobile.empty())
	{
		PROTOCOL::CGetQueryIdInfoRequest l_IDRequest;
		l_IDRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBody.m_oBodyInfo;
		l_IDRequest.m_oBody.m_strmobile = l_oRequest.m_oBody.m_strmobile;
		PROTOCOL::CPushIdInfoRequest l_oPushIdInfoRequest;
		PostQueryIdInfo(l_IDRequest, l_oPushIdInfoRequest);
		if (l_oPushIdInfoRequest.m_vecData.size() > 0)
		{
			std::sort(l_oPushIdInfoRequest.m_vecData.begin(), l_oPushIdInfoRequest.m_vecData.end(),
				[](const PROTOCOL::CPushIdInfoRequest::Data& dept1, const PROTOCOL::CPushIdInfoRequest::Data& dept2)
				{
					return atof(dept1.m_strlevel.c_str()) > atof(dept2.m_strlevel.c_str());
				});
			
			if (!l_oPushIdInfoRequest.m_vecData.at(0).m_stridno.empty())
			{
				l_oRequest.m_oBody.m_strCertNum = l_oPushIdInfoRequest.m_vecData.at(0).m_stridno;
			}

			l_oPushIdInfoRequest.m_vecData.clear();
		}
	}
	// 发送响应消息
	PostQueryPersonInfo(l_oRequest, l_oPostQueryPersonInfoRequest);
	if (!l_oPostQueryPersonInfoRequest.m_Data.m_vecResults.empty())
	{
		BuildPersonPhotoInfoRespond(l_oRequest.m_oBody.m_oBodyInfo, l_oRequest.m_oBody.m_strCertNum, l_oPersonPhotoInfoRespond);
		if (!l_oPersonPhotoInfoRespond.m_strData.m_vecLists.empty()) 
		{
			l_oPostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPhoto = l_oPersonPhotoInfoRespond.m_strData.m_vecLists.at(0).m_strPhoto;
		}
		l_oPersonPhotoInfoRespond.m_strData.m_vecLists.clear();
		PostQueryPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oRequest.m_oBody.m_strCertNum, l_oPostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);
		// 判断涉藏 吸毒 在逃 重点人员便签
		BuildPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oRequest.m_oBody.m_strCertNum, l_oPostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);	
		//构建消息回复前端页面
		BuildGetQueryPersonInfoRespond(l_oPostQueryPersonInfoRequest, p_pNotify);

	}
	else
	{

		PROTOCOL::CPostQueryAiaPersonInfoRequest l_oPostQueryAiaPersonInfoRequest;
		PostQueryAiaPersonInfo(l_oRequest, l_oPostQueryAiaPersonInfoRequest);

		// 返回前端
		BuildGetQueryAiaPersonInfoRespond(l_oPostQueryAiaPersonInfoRequest, p_pNotify);
	}
}

void CBusinessImpl::PostQueryPersonInfo(PROTOCOL::CGetQueryPersonInfoRequest& p_pRequest, PROTOCOL::CPostQueryPersonInfoRequest& p_pSponse)
{
	if (p_pRequest.m_oBody.m_strCertNum.empty() && p_pRequest.m_oBody.m_strmobile.empty())
	{
		p_pSponse.m_strstatus = "499";
		p_pSponse.m_strmsg = "receive message /body/Idno empty or /body/Mobile empty";
		return;
	}
	std::string strQueryPersonInfo;
	std::string strCmd = "query_person_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00053");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790458041466880";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;
	if (!p_pRequest.m_oBody.m_strCertNum.empty())
	{
		p_pInputParm.Condition = m_pString->Format("(IDNO = '%s')", p_pRequest.m_oBody.m_strCertNum.c_str());
	}
	else if (!p_pRequest.m_oBody.m_strmobile.empty())
	{
		p_pInputParm.Condition = m_pString->Format("(MOBILE = '%s')", p_pRequest.m_oBody.m_strmobile.c_str());
	}

	p_pInputParm.Parafs = { "ESCU","MOBILE","DOMPLACE","HOMEADDR","WORKADDR","VEH","IDNO","CHNAME","SEX","BIRTHDAY","BPLACE","HPLACE","NATION","EDUDEGREE","MARR","POLI","RELI","PROF","SERVICEPLACE", "HPLACEAREA" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryPersonInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strQueryPersonInfo = Subreplace(strData, "\"null\"", "\"\"");
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryPersonInfo strQueryPersonInfo[%s]", strQueryPersonInfo.c_str());

	if (strQueryPersonInfo.empty())
	{
		p_pSponse.m_strstatus = "500";
		p_pSponse.m_strmsg = "Failed";

		return;
	}

	if (!p_pSponse.ParseString(strQueryPersonInfo, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strstatus = "501";
		p_pSponse.m_strmsg = strQueryPersonInfo;

		return;
	}
}

void CBusinessImpl::PostQueryAiaPersonInfo(PROTOCOL::CGetQueryPersonInfoRequest& p_pRequest, PROTOCOL::CPostQueryAiaPersonInfoRequest& p_pSponse)
{
	if (m_bFlag)
	{
		if (GetServiceInfo(m_strAiaAlarmServerName, m_strAiaAlarmSendServerIp, m_strAiaAlarmSendServerPort))
		{
			m_bFlag = false;
		}
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	std::map<std::string, std::string> mapHeaders;
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strTarget;
	if (!p_pRequest.m_oBody.m_strCertNum.empty() && !p_pRequest.m_oBody.m_strmobile.empty())
	{
		strTarget = m_pString->Format("/iccQuery/person?idNo=%s&mobile=%s", p_pRequest.m_oBody.m_strCertNum.c_str(), p_pRequest.m_oBody.m_strmobile.c_str());
	}
	else if (!p_pRequest.m_oBody.m_strCertNum.empty() && p_pRequest.m_oBody.m_strmobile.empty())
	{
		strTarget = m_pString->Format("/iccQuery/person?idNo=%s", p_pRequest.m_oBody.m_strCertNum.c_str());
	}
	else if (p_pRequest.m_oBody.m_strCertNum.empty() && !p_pRequest.m_oBody.m_strmobile.empty())
	{
		strTarget = m_pString->Format("/iccQuery/person?mobile=%s", p_pRequest.m_oBody.m_strmobile.c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "receive message /body/Idno empty or /body/Mobile empty");
	}

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAiaPersonInfo ip:port[%s-%s] Target[%s]", m_strAiaAlarmSendServerIp.c_str(), m_strAiaAlarmSendServerPort.c_str(), strTarget.c_str());
	std::string strData = m_pHttpClient->GetWithTimeout(m_strAiaAlarmSendServerIp, m_strAiaAlarmSendServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_intTimeOut);

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAiaPersonInfo msgid[%s] \n data[%s]", p_pRequest.m_oHeader.m_strMsgid.c_str(), strData.c_str());

	if (strData.empty())
	{
		p_pSponse.m_strcode = "500";
		ICC_LOG_ERROR(m_pLog, "strData empty %s", strErrorMessage.c_str());
		m_bFlag = true;
		return;
	}

	if (!p_pSponse.ParseString(strData, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strcode = "501";
		ICC_LOG_ERROR(m_pLog, "strData ParseString failed");
		return;
	}

}

void CBusinessImpl::BuildGetQueryPersonInfoRespond(PROTOCOL::CPostQueryPersonInfoRequest p_strRespond, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryPersonInfoRespond Respond;
	PROTOCOL::CGetQueryPersonInfoRespond::Results results;
	Respond.m_strcode = p_strRespond.m_strstatus;
	Respond.m_strmessage = p_strRespond.m_strmsg;
	Respond.m_Data.m_strtotalRows = p_strRespond.m_Data.m_strtotalRows;

	for (int i = 0; i < std::atoi(Respond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		results.m_strEscu = p_strRespond.m_Data.m_vecResults.at(i).m_strEscu;
		results.m_strHomeaddr = p_strRespond.m_Data.m_vecResults.at(i).m_strHomeaddr;
		results.m_strSex = p_strRespond.m_Data.m_vecResults.at(i).m_strSex;
		results.m_strEdudegree = p_strRespond.m_Data.m_vecResults.at(i).m_strEdudegree;
		results.m_strDomplace = p_strRespond.m_Data.m_vecResults.at(i).m_strDomplace;

		results.m_strBplace = p_strRespond.m_Data.m_vecResults.at(i).m_strBplace;
		results.m_strNation = p_strRespond.m_Data.m_vecResults.at(i).m_strNation;
		results.m_strServiceplace = p_strRespond.m_Data.m_vecResults.at(i).m_strServiceplace;
		results.m_strReli = p_strRespond.m_Data.m_vecResults.at(i).m_strReli;
		results.m_strMarr = p_strRespond.m_Data.m_vecResults.at(i).m_strMarr;

		results.m_strVeh = p_strRespond.m_Data.m_vecResults.at(i).m_strVeh;
		results.m_strHplace = p_strRespond.m_Data.m_vecResults.at(i).m_strHplace;
		results.m_strPoli = p_strRespond.m_Data.m_vecResults.at(i).m_strPoli;
		results.m_strProf = p_strRespond.m_Data.m_vecResults.at(i).m_strProf;
		results.m_strWorkaddr = p_strRespond.m_Data.m_vecResults.at(i).m_strWorkaddr;

		results.m_strIdno = p_strRespond.m_Data.m_vecResults.at(i).m_strIdno;
		results.m_strBirthday = p_strRespond.m_Data.m_vecResults.at(i).m_strBirthday;
		results.m_strMobile = p_strRespond.m_Data.m_vecResults.at(i).m_strMobile;
		results.m_strChname = p_strRespond.m_Data.m_vecResults.at(i).m_strChname;
		results.m_strHplaceArea = p_strRespond.m_Data.m_vecResults.at(i).m_strHplaceArea;

		auto it = m_strDistrictCodeMap.find(results.m_strHplaceArea);
		if (it != m_strDistrictCodeMap.end())
		{
			results.m_strHplaceAreaCode = it->second;
		}
		else 
		{
			results.m_strHplaceAreaCode = results.m_strHplaceArea;
		}

		results.m_strPhoto = p_strRespond.m_Data.m_vecResults.at(i).m_strPhoto;

		PROTOCOL::CGetQueryPersonTagInfoRespond strTagInfo;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData strData;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData::CTag strDataTag;

		if (0 == p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strCode.compare("200"))
		{
			for (size_t index = 0; index < p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.size(); ++index)
			{
				strData.m_strTagType = p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagType;
				strData.m_strTagTypeName = p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagTypeName;

				for (size_t j = 0; j < p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.size(); ++j)
				{
					strDataTag.m_strTagCode = p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagCode;
					strDataTag.m_strTagName = p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagName;
					strData.m_vecTags.push_back(strDataTag);
				}
				strTagInfo.m_strData.push_back(strData);
				strData.m_vecTags.clear();
			}
		}

		if (p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_bTibetanRespond)
		{
			strDataTag.m_strTagCode = "01";
			strDataTag.m_strTagName = m_strChineseLib["Tibetan"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_bDrugRespond)
		{
			strDataTag.m_strTagCode = "02";
			strDataTag.m_strTagName = m_strChineseLib["Drug"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_bEscapeRespond)
		{
			strDataTag.m_strTagCode = "03";
			strDataTag.m_strTagName = m_strChineseLib["Escape"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_bPersonKeyRespond)
		{
			strDataTag.m_strTagCode = "04";
			strDataTag.m_strTagName = m_strChineseLib["PersonKey"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (!p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_vecAttentionTag.empty())
		{
			for (size_t index = 0; index < p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_vecAttentionTag.size(); index++)
			{
				results.m_vecAttentionTag.push_back(p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_vecAttentionTag.at(index));
			}
		}

		results.PersonTagInfo = strTagInfo;
		Respond.m_Data.m_vecResults.push_back(results);
	}

	p_strRespond.m_Data.m_vecResults.clear();
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryPersonInfoRespond send message:%s", l_strMessage.c_str());

	if (Respond.m_strcode == "200" && !Respond.m_Data.m_vecResults.empty())
	{
		Respond.m_oHeader.m_strSystemID = "ICC";
		Respond.m_oHeader.m_strSubsystemID = "PersonInfoData";
		Respond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		Respond.m_oHeader.m_strRelatedID = "";
		Respond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		Respond.m_oHeader.m_strCmd = "sync_topic_person";
		Respond.m_oHeader.m_strRequest = "topic_person";
		Respond.m_oHeader.m_strRequestType = "1";
		Respond.m_oHeader.m_strResponse = "";
		Respond.m_oHeader.m_strResponseType = "";
	
		std::string l_strSendSyncMsg = Respond.SyncToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "sync_topic_person send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::BuildGetQueryAiaPersonInfoRespond(PROTOCOL::CPostQueryAiaPersonInfoRequest p_strRespond, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryPersonInfoRespond Respond;
	PROTOCOL::CGetQueryPersonInfoRespond::Results results;
	Respond.m_strcode = p_strRespond.m_strcode;
	//Respond.m_strmessage = p_strRespond.m_strmsg;
	//Respond.m_Data.m_strtotalRows = p_strRespond.m_Data.m_strtotalRows;

	for (size_t i = 0; i < p_strRespond.m_Data.m_vecResults.size(); ++i)
	{
		results.m_strEscu = p_strRespond.m_Data.m_vecResults.at(i).m_strEscu;
		results.m_strHomeaddr = p_strRespond.m_Data.m_vecResults.at(i).m_strHomeaddr;
		results.m_strSex = p_strRespond.m_Data.m_vecResults.at(i).m_strSex;
		results.m_strEdudegree = p_strRespond.m_Data.m_vecResults.at(i).m_strEdudegree;
		results.m_strDomplace = p_strRespond.m_Data.m_vecResults.at(i).m_strDomplace;

		results.m_strBplace = p_strRespond.m_Data.m_vecResults.at(i).m_strBplace;
		results.m_strNation = p_strRespond.m_Data.m_vecResults.at(i).m_strNation;
		results.m_strServiceplace = p_strRespond.m_Data.m_vecResults.at(i).m_strServiceplace;
		results.m_strReli = p_strRespond.m_Data.m_vecResults.at(i).m_strReli;
		results.m_strMarr = p_strRespond.m_Data.m_vecResults.at(i).m_strMarr;

		results.m_strVeh = p_strRespond.m_Data.m_vecResults.at(i).m_strVeh;
		results.m_strHplace = p_strRespond.m_Data.m_vecResults.at(i).m_strHplace;
		results.m_strPoli = p_strRespond.m_Data.m_vecResults.at(i).m_strPoli;
		results.m_strProf = p_strRespond.m_Data.m_vecResults.at(i).m_strProf;
		results.m_strWorkaddr = p_strRespond.m_Data.m_vecResults.at(i).m_strWorkaddr;

		results.m_strIdno = p_strRespond.m_Data.m_vecResults.at(i).m_strIdno;
		results.m_strBirthday = p_strRespond.m_Data.m_vecResults.at(i).m_strBirthday;
		results.m_strMobile = p_strRespond.m_Data.m_vecResults.at(i).m_strMobile;
		results.m_strChname = p_strRespond.m_Data.m_vecResults.at(i).m_strChname;
		results.m_strHplaceArea = p_strRespond.m_Data.m_vecResults.at(i).m_strHplaceArea;
		results.m_strHplaceAreaCode = p_strRespond.m_Data.m_vecResults.at(i).m_strHplaceAreaCode;
		results.m_strPhoto = p_strRespond.m_Data.m_vecResults.at(i).m_strPhoto;

		PROTOCOL::CGetQueryPersonTagInfoRespond strTagInfo;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData strData;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData::CTag strDataTag;

		//if (0 == p_strRespond.m_Data.m_vecResults.at(i).PersonTagInfo.m_strCode.compare("200"))
		{
			for (size_t index = 0; index < p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.size(); ++index)
			{
				strData.m_strTagType = p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.at(index).m_strTagType;
				strData.m_strTagTypeName = p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.at(index).m_strTagTypeName;

				for (size_t j = 0; j < p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.at(index).m_vecTags.size(); ++j)
				{
					strDataTag.m_strTagCode = p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.at(index).m_vecTags.at(j).m_strTagCode;
					strDataTag.m_strTagName = p_strRespond.m_Data.m_vecResults.at(i).m_vecdataTag.at(index).m_vecTags.at(j).m_strTagName;
					strData.m_vecTags.push_back(strDataTag);
				}
				strTagInfo.m_strData.push_back(strData);
				strData.m_vecTags.clear();
			}
		}

		if (!p_strRespond.m_Data.m_vecResults.at(i).m_vecAttentionTag.empty())
		{
			for (size_t index = 0; index < p_strRespond.m_Data.m_vecResults.at(i).m_vecAttentionTag.size(); index++)
			{
				results.m_vecAttentionTag.push_back(p_strRespond.m_Data.m_vecResults.at(i).m_vecAttentionTag.at(index));
			}
		}

		results.PersonTagInfo = strTagInfo;
		Respond.m_Data.m_vecResults.push_back(results);
	}

	p_strRespond.m_Data.m_vecResults.clear();
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryPersonInfoRespond send message:%s", l_strMessage.c_str());

	if (Respond.m_strcode == "200" && !Respond.m_Data.m_vecResults.empty())
	{
		Respond.m_oHeader.m_strSystemID = "ICC";
		Respond.m_oHeader.m_strSubsystemID = "PersonInfoData";
		Respond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		Respond.m_oHeader.m_strRelatedID = "";
		Respond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		Respond.m_oHeader.m_strCmd = "sync_topic_person";
		Respond.m_oHeader.m_strRequest = "topic_person";
		Respond.m_oHeader.m_strRequestType = "1";
		Respond.m_oHeader.m_strResponse = "";
		Respond.m_oHeader.m_strResponseType = "";

		std::string l_strSendSyncMsg = Respond.SyncToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "sync_topic_person send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifQueryTibetanRelatedGroupsInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryTibetanRelatedGroupsInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
	    return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest l_oPostRequest;
	//post 请求
	PostQueryTibetanRelatedGroupsInfo(l_oRequest, l_oPostRequest);
	//构建消息回复前端页面
	BuildGetQueryTibetanRelatedGroupsInfoRespond(l_oPostRequest, p_pNotify);
}

void CBusinessImpl::PostQueryTibetanRelatedGroupsInfo(PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRequest& p_pGetRequest, PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest& p_pPostRequest)
{
	if (p_pGetRequest.m_oBody.m_strCertNum.empty())
	{
		p_pPostRequest.m_strstatus = "499";
		p_pPostRequest.m_strmsg = "receive message /body/CertNum empty";
		return;
	}
	std::string strQueryTibetanRelatedGroupsInfo;
	std::string strCmd = "query_tibetan_related_groups_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00063");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790545752752128";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.mapkey.insert(std::make_pair("ZJHM", p_pGetRequest.m_oBody.m_strCertNum));
	p_pInputParm.Parafs = { "ZJHM","XM","LXDH","SACYLXDM" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pGetRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryTibetanRelatedGroupsInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strQueryTibetanRelatedGroupsInfo = Subreplace(strData, "\"null\"", "\"\"");
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryTibetanRelatedGroupsInfo strQueryNameCaseData[%s]", strQueryTibetanRelatedGroupsInfo.c_str());

	if (strQueryTibetanRelatedGroupsInfo.empty())
	{
		p_pPostRequest.m_strstatus = "500";
		p_pPostRequest.m_strmsg = "Failed";

		return;
	}

	if (!p_pPostRequest.ParseString(strQueryTibetanRelatedGroupsInfo, m_pJsonFty->CreateJson()))
	{
		p_pPostRequest.m_strstatus = "501";
		p_pPostRequest.m_strmsg = strQueryTibetanRelatedGroupsInfo;

		return;
	}
}

void CBusinessImpl::BuildGetQueryTibetanRelatedGroupsInfoRespond(PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest p_pPostQueryTibetanRelatedGroupsInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRespond Respond;
	PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRespond::Results results;
	Respond.m_strcode = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_strstatus;
	Respond.m_strmessage = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_strmsg;
	Respond.m_Data.m_strtotalRows = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_Data.m_strtotalRows;

	for (int i = 0; i < std::atoi(Respond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		results.m_strXm = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_Data.m_vecResults.at(i).m_strXm;
		results.m_strSacylxdm = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_Data.m_vecResults.at(i).m_strSacylxdm;
		results.m_strLxdh = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_Data.m_vecResults.at(i).m_strLxdh;
		results.m_strZjhm = p_pPostQueryTibetanRelatedGroupsInfoRequest.m_Data.m_vecResults.at(i).m_strZjhm;

		Respond.m_Data.m_vecResults.push_back(results);
	}
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryTibetanRelatedGroupsInfoRespond send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryDrugInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryDrugInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CGetQueryDrugInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CPostQueryDrugInfoRequest l_oPostRequest;
	//post 请求
	PostQueryDrugInfo(l_oRequest, l_oPostRequest);
	//构建消息回复前端页面
	BuildGetQueryDrugInfoRespond(l_oPostRequest, p_pNotify);
}

void CBusinessImpl::PostQueryDrugInfo(PROTOCOL::CGetQueryDrugInfoRequest& p_pGetRequest, PROTOCOL::CPostQueryDrugInfoRequest& p_pPostRequest)
{
	if (p_pGetRequest.m_oBody.m_strCertNum.empty() && p_pGetRequest.m_oBody.m_strXm.empty() && p_pGetRequest.m_oBody.m_strHjdzXzqhmc.empty() 
		&& p_pGetRequest.m_oBody.m_strXbdm.empty())
	{
		p_pPostRequest.m_strstatus = "499";
		p_pPostRequest.m_strmsg = "receive message /body/CertNum  xm  hjdzxzqhmc  xbdm empty";
		return;
	}

	std::string strQueryDrugInfo;
	std::string strCmd = "query_drug_info";
	std::string strTarget = GetPostUrl(strCmd);

	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00066");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790546549669888";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.mapkey.insert(std::make_pair("SFZHM18", p_pGetRequest.m_oBody.m_strCertNum));
	p_pInputParm.mapkey.insert(std::make_pair("XM", p_pGetRequest.m_oBody.m_strXm));
	p_pInputParm.mapkey.insert(std::make_pair("HJDZ_XZQHMC", p_pGetRequest.m_oBody.m_strHjdzXzqhmc));
	p_pInputParm.mapkey.insert(std::make_pair("XBDM", p_pGetRequest.m_oBody.m_strXbdm));

	p_pInputParm.Parafs = { "XM","BMCH","XBDM","CSRQ","MZDM","SG","HJDZ_XZQHMC","SJJZD_XZQHMC","SJJZD_GAJGMC","LRSJ","SFZHM18","HJDZ_DZMC","HJDZ_GAJGMC","SJJZD_DZMC","LRDW_DWMC" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pGetRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	

	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryDrugInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strQueryDrugInfo = Subreplace(strData, "\"null\"", "\"\"");
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryDrugInfo strQueryDrugInfo[%s]", strQueryDrugInfo.c_str());

	if (strQueryDrugInfo.empty())
	{
		p_pPostRequest.m_strstatus = "500";
		p_pPostRequest.m_strmsg = "Failed";

		return;
	}

	if (!p_pPostRequest.ParseString(strQueryDrugInfo, m_pJsonFty->CreateJson()))
	{
		p_pPostRequest.m_strstatus = "501";
		p_pPostRequest.m_strmsg = strQueryDrugInfo;

		return;
	}
}

void CBusinessImpl::BuildGetQueryDrugInfoRespond(PROTOCOL::CPostQueryDrugInfoRequest p_pPostQueryDrugInfoRRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CGetQueryDrugInfoRespond Respond;
	PROTOCOL::CGetQueryDrugInfoRespond::Results results;
	Respond.m_strcode = p_pPostQueryDrugInfoRRequest.m_strstatus;
	Respond.m_strmessage = p_pPostQueryDrugInfoRRequest.m_strmsg;
	Respond.m_Data.m_strtotalRows = p_pPostQueryDrugInfoRRequest.m_Data.m_strtotalRows;

	for (int i = 0; i < std::atoi(Respond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		results.m_strBmch = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strBmch;
		results.m_strCsrq = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strCsrq;
		results.m_strHjdzXzqhmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strHjdzXzqhmc;
		results.m_strSjjzdxzqhmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strSjjzdxzqhmc;
		results.m_strSjjzdgajgmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strSjjzdgajgmc;
		results.m_strLrsj = ToStringTime(p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strLrsj);
		results.m_strSg = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strSg;
		results.m_strHjdzgajgmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strHjdzgajgmc;
		results.m_strSjjzddzmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strSjjzddzmc;
		results.m_strLrdwdwmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strLrdwdwmc;
		results.m_strXm = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strXm;
		results.m_strSfzhm18 = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strSfzhm18;
		results.m_strHjdzdzmc = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strHjdzdzmc;
		results.m_strXbdm = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strXbdm;
		results.m_strMzdm = p_pPostQueryDrugInfoRRequest.m_Data.m_vecResults.at(i).m_strMzdm;

		Respond.m_Data.m_vecResults.push_back(results);
	}
	std::string l_strMessage = Respond.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "BuildGetQueryDrugInfoRespondsend message:%s", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifQueryEscapeInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryEscapeInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息
	PROTOCOL::CGetQueryEscapeInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CEscapeInfoRespond l_oRespond;
	BuildEscapeInfoRespond(l_oRequest, l_oRespond);

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryEscapeInfo send l_oRespond :%s\n", l_strMessage.c_str());
}

void CBusinessImpl::BuildEscapeInfoRespond(PROTOCOL::CGetQueryEscapeInfoRequest& p_oRequest, PROTOCOL::CEscapeInfoRespond& p_oRespond)
{
	if (p_oRequest.m_strCerdNum.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/zjhm  empty";
		return;
	}
	std::string strCmd = "query_escape_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00064");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790545991827456";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.Condition = m_pString->Format("(ZJHM = '%s')", p_oRequest.m_strCerdNum.c_str());

	p_pInputParm.Parafs = { "DBJBDM","KY2_DM","JYAQ","TP2_RQ","ZTRYBH","XM","XBDM","SG","XZD_XZQHDM","XZD_QHNXXDZ","AJBH",
		"AJLBDM","LADW_XZQHDM","LADW_GAJGJGDM","SG","ZBDW_LXDH","DBJB_DM","TJLBH","HJDZ_XZQHDM","HJDZ_QHNXXDZ","ZJHM","LA_RQSJ" };

	p_pInputParm.maporderParafs.insert(std::make_pair("LA_RQSJ", "desc"));
	p_pInputParm.m_strMaxReturnNum = "1";
	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);

	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildEscapeInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildEscapeInfoRespond strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}
}

void CBusinessImpl::OnNotifQueryPersonKeyInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonKeyInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetQueryPersonKeyInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonKeyInfo start");

	// 发送响应消息
	PROTOCOL::CPersonKeyInfoRespond l_oRespond;
	BuildPersonKeyInfoRespond(l_oRequest, l_oRespond);
	std::string l_strMessage = PersonKeyToString(l_oRespond);

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, " OnNotifQueryPersonKeyInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::BuildPersonKeyInfoRespond(PROTOCOL::CGetQueryPersonKeyInfoRequest& p_oRequest, PROTOCOL::CPersonKeyInfoRespond& p_oRespond)
{
	if (p_oRequest.m_strCerdNum.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/zjhm empty";
		return;
	}
	std::string strCmd = "query_person_key_info";
	std::string strTarget = GetPostUrl(strCmd);

	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00071");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790972992946176";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	if (m_strNewKeyInfo == "1")
	{
		p_pInputParm.mapkey.insert(std::make_pair("GMSFHM", p_oRequest.m_strCerdNum));
	}
	else
	{
		p_pInputParm.Condition = m_pString->Format("(GMSFHM = '%s')", p_oRequest.m_strCerdNum.c_str());
	}

	p_pInputParm.Parafs = { "BJ1_ZDRY_BH","XM","WWXM","XBDM","CSRQ","GJDM","GMSFHM","QTZJ_CYZJDM","QTZJ_CYZJHM","MZDM",
		"JGGJDQDM","HJDZ_XZQHDM","HJDZ_QHNXXDZ","HJDPCS_GAJGMC","HJDPCS_GAJGJGDM","XZZ_XZQHDM","XZZ_QHNXXDZ","XZZPCS_GAJGMC",
		"XZZPCS_GAJGJGDM","GXDW_GAJGMC","GXDW_GAJGJGDM","LADW_GAJGMC","LADW_GAJGJGDM","ZX5_LARQ","BJ1_ZDRY_XXRKSJ","ZDRY_LB1_BS",
		"ZDRY_XL_DM","ZDRYYWJL_GXSJ","ZDRYYWJL_XZSC_RQSJ","ZDRY_LB1_MC","ZDRY_XL_MC"};

	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildPersonKeyInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), 
			strTarget.c_str(), strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildPersonKeyInfoRespond strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}

}

void CBusinessImpl::OnNotifQueryPersonPhotoInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonPhotoInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetQueryPersonPhotoInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryPersonPhotoInfo start");

	// 发送响应消息
	PROTOCOL::CPersonPhotoInfoRespond l_oRespond;
	BuildPersonPhotoInfoRespond(l_oRequest.m_oBodyInfo, l_oRequest.m_strCerdNum, l_oRespond);
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, " OnNotifQueryPersonPhotoInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::BuildPersonPhotoInfoRespond(PROTOCOL::BodyInfo& p_oRequestInfo, const std::string strIdCard, PROTOCOL::CPersonPhotoInfoRespond& p_oRespond)
{
	if (strIdCard.empty())
	{
		p_oRespond.m_strCode = "499";
		p_oRespond.m_strMessage = "receive message /body/sfzh empty";
		return;
	}
	std::string strContent;
	std::string strErrorMessage;
	std::string strCmd = "query_person_photo_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00019");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "4068888580980736";
	}

	PROTOCOL::InputParm p_pInputParm;
	p_pInputParm.mapkey.insert(std::make_pair("idCard", strIdCard));
	//构建消息体 
	strContent = BuildPostContentRespond(p_oRequestInfo, p_pInputParm);
	
	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryPhotoInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(), 
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	ICC_LOG_DEBUG(m_pLog, "PersonInfo CPersonPhotoInfoRespond strReceive[%s]", strData.c_str());

	if (strData.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "no find photo info";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strData, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strData;
		return;
	}
}

std::string CBusinessImpl::PersonKeyToString(PROTOCOL::CPersonKeyInfoRespond& p_oRespond)
{
	JsonParser::IJsonPtr p_pJson = ICCGetIJsonFactory()->CreateJson();
	if (nullptr == p_pJson)
	{
		return "";
	}
	p_pJson->SetNodeValue("/code", p_oRespond.m_strCode);
	p_pJson->SetNodeValue("/message", p_oRespond.m_strMessage);
	p_pJson->SetNodeValue("/data/Count", p_oRespond.m_strData.m_strCount);
	int iCount = p_oRespond.m_strData.m_vecLists.size();
	for (int i = 0; i < iCount; ++i)
	{
		std::string l_strPrefixPath("/data/List/" + std::to_string(i));
		p_pJson->SetNodeValue(l_strPrefixPath + "/PoliceStationCode", p_oRespond.m_strData.m_vecLists.at(i).m_strPoliceStationCode);                // 现住地派出所代码
		p_pJson->SetNodeValue(l_strPrefixPath + "/Jurisdiction", p_oRespond.m_strData.m_vecLists.at(i).m_strJurisdiction);                // 管辖单位
		p_pJson->SetNodeValue(l_strPrefixPath + "/JurisdictionCode", p_oRespond.m_strData.m_vecLists.at(i).m_strJurisdictionCode);                // 管辖单位机构代码
		p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepart", p_oRespond.m_strData.m_vecLists.at(i).m_strFilingDepart);                // 立案单位
		p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepartCode", p_oRespond.m_strData.m_vecLists.at(i).m_strFilingDepartCode);                // 立案单位机构代码
		p_pJson->SetNodeValue(l_strPrefixPath + "/InputTime", ToStringTime(p_oRespond.m_strData.m_vecLists.at(i).m_strInputTime));				// 纳入部级重点人员库时间
		p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyInfo", p_oRespond.m_strData.m_vecLists.at(i).m_strPersonKeyInfo);                // 重点人员细类
		p_pJson->SetNodeValue(l_strPrefixPath + "/ChangeTime", ToStringTime(p_oRespond.m_strData.m_vecLists.at(i).m_strChangeTime));                // 业务记录变更时间
		p_pJson->SetNodeValue(l_strPrefixPath + "/IncreaseTime", ToStringTime(p_oRespond.m_strData.m_vecLists.at(i).m_strIncreaseTime));                // 业务记录新增时间
		p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyNum", p_oRespond.m_strData.m_vecLists.at(i).m_strPersonKeyNum);                // 部级重点人员编号
		p_pJson->SetNodeValue(l_strPrefixPath + "/EnglishName", p_oRespond.m_strData.m_vecLists.at(i).m_strEnglishName);                // 外文姓名
		p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", p_oRespond.m_strData.m_vecLists.at(i).m_strBirthday);                // 出生日期
		p_pJson->SetNodeValue(l_strPrefixPath + "/CerdNum", p_oRespond.m_strData.m_vecLists.at(i).m_strCerdNum);                // 公民身份号码
		p_pJson->SetNodeValue(l_strPrefixPath + "/Address", p_oRespond.m_strData.m_vecLists.at(i).m_strAddress);                // 现住地详址
		p_pJson->SetNodeValue(l_strPrefixPath + "/PoliceStation", p_oRespond.m_strData.m_vecLists.at(i).m_strPoliceStation);                // 现住地派出所
		p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyType", p_oRespond.m_strData.m_vecLists.at(i).m_strPersonKeyType);                // 重点人员类别标记(最后一位是1的为涉恐、第一位是1的为涉稳、第二刑事、第三治安)
		p_pJson->SetNodeValue(l_strPrefixPath + "/FilingTime", ToStringTime(p_oRespond.m_strData.m_vecLists.at(i).m_strFilingTime));                // 最近立案时间
		p_pJson->SetNodeValue(l_strPrefixPath + "/Name", p_oRespond.m_strData.m_vecLists.at(i).m_strName);                // 姓名
		p_pJson->SetNodeValue(l_strPrefixPath + "/OtherCerdNum", p_oRespond.m_strData.m_vecLists.at(i).m_strOtherCerdNum);                // 其它证件号码
		p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredAddress", p_oRespond.m_strData.m_vecLists.at(i).m_strRegisteredAddress);                // 户籍地详址
		p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredPolice", p_oRespond.m_strData.m_vecLists.at(i).m_strRegisteredPolice);                // 户籍地派出所
		p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredPoliceCode", p_oRespond.m_strData.m_vecLists.at(i).m_strRegisteredPoliceCode);                // 户籍地派出所代码
		p_pJson->SetNodeValue(l_strPrefixPath + "/NativePlace", p_oRespond.m_strData.m_vecLists.at(i).m_strNativePlace);                // 籍贯
		p_pJson->SetNodeValue(l_strPrefixPath + "/Nationality", p_oRespond.m_strData.m_vecLists.at(i).m_strNationality);                // 国籍
		p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredArea", p_oRespond.m_strData.m_vecLists.at(i).m_strRegisteredArea);                // 户籍地区划
		p_pJson->SetNodeValue(l_strPrefixPath + "/NativeArea", p_oRespond.m_strData.m_vecLists.at(i).m_strNativeArea);                // 现住地区划
		p_pJson->SetNodeValue(l_strPrefixPath + "/OtherCerdType", p_oRespond.m_strData.m_vecLists.at(i).m_strOtherCerdType);                // 其它证件类型
		p_pJson->SetNodeValue(l_strPrefixPath + "/Gender", p_oRespond.m_strData.m_vecLists.at(i).m_strGender);                // 性别
		p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", p_oRespond.m_strData.m_vecLists.at(i).m_strNation);                // 民族

		p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyInfoName", p_oRespond.m_strData.m_vecLists.at(i).m_strPersonKeyInfoName);               
		p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyType", p_oRespond.m_strData.m_vecLists.at(i).m_strPersonKeyTypeName);              

	}
	return p_pJson->ToString();
}

bool CBusinessImpl::GetLoginToken()
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData GetLoginToken start");
	std::string strTokenContent;
	std::string strLoginToken;

	std::string strTarget = GetPostUrl("get_login_token");

	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/api/v1/token/create");
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::CGetLoginTokenRespond l_oLoginTokenRespond;

	l_oLoginTokenRespond.apptoken.m_struserToken.m_strtype = m_strtype;
	l_oLoginTokenRespond.apptoken.m_struserToken.m_strpoliceNo = m_strpoliceNo;
	l_oLoginTokenRespond.apptoken.m_struserToken.m_struserId = m_strUserId;

	strTokenContent = l_oLoginTokenRespond.ToString(m_pJsonFty->CreateJson());

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData GetLoginToken strUserToken[%s]", strTokenContent.c_str());

	JsonParser::IJsonPtr pJsonuserToken = ICCGetIJsonFactory()->CreateJson();
	pJsonuserToken->SetNodeValue("/type", m_strtype);
	pJsonuserToken->SetNodeValue("/policeNo", m_strpoliceNo);
	pJsonuserToken->SetNodeValue("/userId", m_strUserId);
	std::string struserToken = pJsonuserToken->ToString();
	JsonParser::IJsonPtr pJsonappToken = ICCGetIJsonFactory()->CreateJson();
	pJsonappToken->SetNodeValue("/appId", m_strAppId);
	pJsonappToken->SetNodeValue("/userToken", strTokenContent);
	std::string strappToken = pJsonappToken->ToString();

	std::string strbody = m_pString->Format("userToken=%s&appToken=%s", struserToken.c_str(), strappToken.c_str());
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));

	//strLoginToken = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strbody, strErrorMessage);
	strLoginToken = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strbody, strErrorMessage, m_intTimeOut);
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData GetLoginToken strTarget[%s] \n strbody[%s] \n strLoginToken[%s]", strTarget.c_str(),strbody.c_str(),strLoginToken.c_str());

	if (strLoginToken.empty())
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData post Login error!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}

	//解析  Parse
	PROTOCOL::CGetLoginTokenRequest strLoginTokenRequest;
	if (!strLoginTokenRequest.ParseString(strLoginToken, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData GetLoginToken[%s] ParseString Failed", strLoginToken.c_str());
		return false;
	}

	PROTOCOL::CGetappTokenRequest strAppTokenRequest;
	if (!strAppTokenRequest.ParseString(strLoginTokenRequest.m_oappTokenId, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData GetLoginToken[%s] ParseString Failed", strLoginTokenRequest.m_oappTokenId.c_str());
		return false;
	}

	PROTOCOL::CGetuserTokenRequest strUserTokenequest;
	if (!strUserTokenequest.ParseString(strLoginTokenRequest.m_ouserTokenId, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData GetLoginToken[%s] ParseString Failed", strLoginTokenRequest.m_oappTokenId.c_str());
		return false;
	}

	if (0 == strLoginTokenRequest.m_strstatus.compare("500"))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData GetLoginToken Parse [%s]", strLoginTokenRequest.m_strstatus.c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData GetLoginToken m_strappToken[%s]\n  m_struserToken[%s]\n", strAppTokenRequest.m_oappToken.c_str(), strUserTokenequest.m_ouserToken.c_str());
		m_strapptoken = strAppTokenRequest.m_oappToken;
		m_strusertoken = strUserTokenequest.m_ouserToken;
	}

	return true;
}

std::map<std::string, std::string> CBusinessImpl::PostHeader(std::string p_pServiceId, std::string p_pcmd)
{
	
	std::map<std::string, std::string> mapHeader;
	mapHeader.insert(std::make_pair("Content-Type", "application/json"));
	mapHeader.insert(std::make_pair("apptoken", m_strapptoken));
	mapHeader.insert(std::make_pair("usertoken", m_strusertoken));
	mapHeader.insert(std::make_pair("userInfo", m_strbody));
	mapHeader.insert(std::make_pair("subid", m_strAppId));
	mapHeader.insert(std::make_pair("serviceid", p_pServiceId));
	mapHeader.insert(std::make_pair("senderId", m_strSenderId));
	mapHeader.insert(std::make_pair("groupId", m_strGroupId));

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostHeader m_strserviceid[%s]  p_pcmd[%s] ", p_pServiceId.c_str(), p_pcmd.c_str());
	return mapHeader;
}

std::string CBusinessImpl::BuildPostContentRespond(PROTOCOL::BodyInfo& m_oBody, PROTOCOL::InputParm& p_pInputParm)
{
	JsonParser::IJsonPtr p_pJson = ICCGetIJsonFactory()->CreateJson();
	p_pJson->SetNodeValue("/regId", m_oBody.m_strregId);
	p_pJson->SetNodeValue("/userId", m_oBody.m_struserId);
	p_pJson->SetNodeValue("/userName", m_oBody.m_struserName);
	p_pJson->SetNodeValue("/userType", m_oBody.m_struserType);
	p_pJson->SetNodeValue("/organization", m_oBody.m_strorganization);
	p_pJson->SetNodeValue("/organizationId", m_oBody.m_strorganizationId);
	p_pJson->SetNodeValue("/operateTime", m_pDateTime->CurrentDateTimeStr());

	p_pJson->SetNodeValue("/terminalType", m_oBody.m_strterminalType);

	p_pJson->SetNodeValue("/terminalIp", m_strIp);
	p_pJson->SetNodeValue("/inquireType", m_oBody.m_strinquireType);

	p_pJson->SetNodeValue("/messageSequence", m_pString->CreateGuid());
	if (!p_pInputParm.Condition.empty()) {
		p_pJson->SetNodeValue("/condition", p_pInputParm.Condition);
	}
	if (!p_pInputParm.m_strMaxReturnNum.empty())
	{
		p_pJson->SetNodeValue("/maxReturnNum", p_pInputParm.m_strMaxReturnNum);
	}
	else
	{
		p_pJson->SetNodeValue("/maxReturnNum", "100");
	}
	if (!p_pInputParm.mapkey.empty()) {
		for (auto itr_const = p_pInputParm.mapkey.begin(); itr_const != p_pInputParm.mapkey.end(); ++itr_const)
		{
			std::string strKey = m_pString->Format("/%s", itr_const->first.c_str());
			p_pJson->SetNodeValue(strKey, itr_const->second);
		}
	}

	if (!p_pInputParm.maporderParafs.empty()) 
	{
		for (auto itr_const = p_pInputParm.maporderParafs.begin(); itr_const != p_pInputParm.maporderParafs.end(); ++itr_const)
		{
			std::string strParafs("/orderParafs");
			std::string strKey = m_pString->Format("/%s", itr_const->first.c_str());
			p_pJson->SetNodeValue(strParafs + strKey, itr_const->second);
		}
	}

	int size = p_pInputParm.Parafs.size();
	for (int i = 0; i < size; ++i)
	{
		std::string strParafs("/parafs/" + std::to_string(i));
		p_pJson->SetNodeValue(strParafs, p_pInputParm.Parafs.at(i));
	}

	p_pJson->SetNodeValue("/areaCode", m_strareaCode);

	return p_pJson->ToString();
}

void CBusinessImpl::OnNotifQueryNameCaseDataAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryNameCaseDatAndCaseInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  QueryNameCaseData
	PROTOCOL::CGetQueryNameCaseDataRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CGetQueryNameCaseDataAndCaseInfoRespond l_oRespond;
	PostQueryNameCaseData(l_oRequest, l_oRespond.m_PostQueryNameCaseDataRequest);

	PROTOCOL::CGetQueryCaseInfoRequest l_oGetQueryCaseInfoRequest;
	l_oGetQueryCaseInfoRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBody.m_oBodyInfo;
	l_oRespond.m_strCode = l_oRespond.m_PostQueryNameCaseDataRequest.m_strstatus;
	l_oRespond.m_strMessage = l_oRespond.m_PostQueryNameCaseDataRequest.m_strmsg;
	l_oRespond.m_Data.m_strtotalRows = l_oRespond.m_PostQueryNameCaseDataRequest.m_Data.m_strtotalRows;
	for (int i = 0; i < std::atoi(l_oRespond.m_PostQueryNameCaseDataRequest.m_Data.m_strtotalRows.c_str()); ++i)
	{
		if (!l_oRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseNo.empty())
		{
			l_oGetQueryCaseInfoRequest.m_oBody.m_CaseNo = l_oRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseNo;
			PostQueryCaseInfo(l_oGetQueryCaseInfoRequest, l_oRespond.m_PostQueryCaseInfoRequest);
		}
		BuildGetQueryNameCaseDataAndCaseInfoRespond(l_oRespond,i);
	}
	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryNameCaseDataAndCaseInfo send message:%s", strmsg.c_str());
}

void CBusinessImpl::PostQueryNameCaseData(PROTOCOL::CGetQueryNameCaseDataRequest& p_pRequest, PROTOCOL::CPostQueryNameCaseDataRequest& p_pSponse)
{
	if (p_pRequest.m_oBody.m_strCertNum.empty())
	{
		p_pSponse.m_strstatus = "499";
		p_pSponse.m_strmsg = "receive message /body/CertNum empty";
		return;
	}

	std::string strCmd = "query_name_case_data";
	std::string strcaseNo;
	std::string strQueryNameCaseData;

	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00051");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790371823353856";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	p_pInputParm.Condition = m_pString->Format("(CERT_NUM = '%s')", p_pRequest.m_oBody.m_strCertNum.c_str());
	p_pInputParm.Parafs = { "CASE_NAME","CASE_TIME","NAME","CERT_NUM","CASE_NO" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParm);
	
	std::string strData;
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryNameCaseData strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		strData = GetPostTextByContent(strCmd);
	}
	strQueryNameCaseData = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryNameCaseData strQueryNameCaseData[%s]", strQueryNameCaseData.c_str());

	if (strQueryNameCaseData.empty())
	{
		p_pSponse.m_strstatus = "500";
		p_pSponse.m_strmsg = "Failed";

		return;
	}

	if (!p_pSponse.ParseString(strQueryNameCaseData, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_strstatus = "501";
		p_pSponse.m_strmsg = strQueryNameCaseData;

		return;
	}
}

void CBusinessImpl::PostQueryCaseInfo(PROTOCOL::CGetQueryCaseInfoRequest& p_pRequest, PROTOCOL::CPostQueryCaseInfoRequest& p_pSponse)
{
	if (p_pRequest.m_oBody.m_CaseNo.empty())
	{
		p_pSponse.m_oBody.code = "499";
		p_pSponse.m_oBody.message = "receive message /body/CaseNo empty";
		return;
	}
	std::string strCmd = "query_case_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00044");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790273391427584";
	}

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::InputParm p_pInputParmCaseInfo;
	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	p_pInputParmCaseInfo.Condition = m_pString->Format("(CASE_NO = '%s')", p_pRequest.m_oBody.m_CaseNo.c_str());
	p_pInputParmCaseInfo.Parafs = { "CASE_NO","CASE_NAME","CASE_TYPE_CODE","CASCLA_NAME","CASE_SOUR_DESC","ACCEP_UNIT_PSAG",
		"ACCEP_TIME","DISC_TIME","CASE_TIME_TIMPER","CASE_ADDR_ADDR_CODE","CASE_ORGA_PSAG_CODE","CASE_ORGA_PSAG",
		"MAIN_CASE_CASE_NO","MINOR_CASE_CASE_NO","REPO_CASE_UNIT_ADDR_NAME","ARRSCE_TIME","CASE_DATE","SET_LAWS_DATE",
		"REPO_CASE_TIME","CASE_CLAS_CODE","CASE_CLASS_CODE","CRIM_PURP_DESC" };
	//构建消息体
	strContent = BuildPostContentRespond(p_pRequest.m_oBody.m_oBodyInfo, p_pInputParmCaseInfo);
	std::string strData;
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeaderCaseInfo = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeaderCaseInfo, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeaderCaseInfo, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCaseInfo strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
	}
	else 
	{
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCaseInfo strReceive[%s]", strReceive.c_str());

	if (strReceive.empty())
	{
		p_pSponse.m_oBody.code = "500";
		p_pSponse.m_oBody.message = "Failed";

		return;
	}

	//解析  Parse
	if (!p_pSponse.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_pSponse.m_oBody.code = "501";
		p_pSponse.m_oBody.message = strReceive;

		return;
	}
}

void CBusinessImpl::BuildGetQueryNameCaseDataAndCaseInfoRespond(PROTOCOL::CGetQueryNameCaseDataAndCaseInfoRespond& p_pRespond,int i)
{
	PROTOCOL::CGetQueryNameCaseDataAndCaseInfoRespond::Results results;
	results.m_strcaseNo = p_pRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseNo;
	results.m_strcacsName = p_pRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcacsName;
	results.m_strcaseTime = ToStringTime(p_pRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcaseTime);
	results.m_strcertNum = p_pRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strcertNum;
	results.m_strname = p_pRespond.m_PostQueryNameCaseDataRequest.m_Data.m_vecResults.at(i).m_strname;

	if (p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.size() > 0)
	{
		results.m_caseClasCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseClasCode;

		results.m_repoCaseTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_repoCaseTime);
		results.m_caseName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseName;
		results.m_mainCaseCaseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_mainCaseCaseNo;
		results.m_casClaName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_casClaName;
		results.m_repoCaseUnitAddrName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_repoCaseUnitAddrName;
		results.m_discTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_discTime);
		results.m_accepUnitPsag = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_accepUnitPsag;
		results.m_minorCaseCaseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_minorCaseCaseNo;
		results.m_caseClassCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseClassCode;
		results.m_caseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseNo;
		results.m_caseOrgaPsag = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseOrgaPsag;
		results.m_caseDate = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseDate;
		results.m_accepTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_accepTime);
		results.m_caseTimeTimper = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseTimeTimper;
		results.m_caseTypeCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseTypeCode;

		results.m_crimPurpDesc = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_crimPurpDesc;
		results.m_caseSourDesc = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseSourDesc;
		results.m_caseAddrAddrCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseAddrAddrCode;
		results.m_caseOrgaPsagCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseOrgaPsagCode;
		results.m_arrsceTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_arrsceTime);
		results.m_setLawsDate = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_setLawsDate;
	}
	p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.clear();
	p_pRespond.m_Data.m_vecResults.push_back(results);
}

void CBusinessImpl::OnNotifQueryAddressInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryAddressInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CAddressInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	PROTOCOL::CAddressInfoRespond l_oRespond;
	PROTOCOL::CAddrInfoRespond l_oAddrInfoRespond;
	PROTOCOL::CCompInfoRespond l_oCompInfoRespond;

	do
	{
		if ((l_oRequest.m_strAddrName.empty() && l_oRequest.m_strLat.empty() && l_oRequest.m_strLon.empty()))
		{
			l_oRespond.m_strCode = "499";
			l_oRespond.m_strMessage = "receive message /body/AddrName or /body/Lon or /body/Lat empty";
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryAddressInfo msgid[%s] ", l_oRequest.m_oHeader.m_strMsgid.c_str());
		std::string l_strGuid = m_pString->CreateGuid();
		//另用一个线程请求  四川省标准地址层户地址查询
		//boost::thread l_oThreadQueryAlarmCount(boost::bind(&CBusinessImpl::PostQueryAddrInfoRespond, this, l_oRequest, l_oAddrInfoRespond, l_strGuid));

		// 四川省标准地址层户地址查询
		PostQueryAddrInfoRespond(l_oRequest, l_oAddrInfoRespond);
		// 四川省实有单位查询

		PostQueryCompInfoRespond(l_oRequest, l_oCompInfoRespond);
		ICC_LOG_DEBUG(m_pLog, "wait for the returning of PostQueryCompInfoRespond msgid[%s] == l_strGuid[%s]", l_oRespond.m_oHeader.m_strMsgid.c_str(), l_strGuid.c_str());
		//l_oThreadQueryAlarmCount.join();//等四川省标准地址层户地址查询返回

		//{
		//	std::lock_guard<std::mutex> lock(m_mutexPostQueryAddrInfoRespond);
		//	l_oAddrInfoRespond = m_mapPostQueryAddrInfoRespond[l_strGuid];
		//}

		ICC_LOG_DEBUG(m_pLog, "PostQueryCompInfoRespond returned msgid[%s] == l_strGuid[%s]", l_oRequest.m_oHeader.m_strMsgid.c_str(), l_strGuid.c_str());

		//计算经纬度距离，按照距离排序  -- 根据经纬度查询
		if (!l_oRequest.m_strLat.empty() && !l_oRequest.m_strLon.empty())
		{
			BuildRealDistanceAndSort(l_oRequest, l_oAddrInfoRespond, l_oCompInfoRespond);
		}

		if (l_oAddrInfoRespond.m_strCode == "200" || l_oCompInfoRespond.m_strCode == "200")
		{
			l_oRespond.m_strCode = "200";
			l_oRespond.m_strMessage = "success";
		}
		else
		{
			l_oRespond.m_strCode = l_oAddrInfoRespond.m_strCode;
			l_oRespond.m_strMessage = l_oAddrInfoRespond.m_strMessage;
			break;
		}

		BuildGetQueryAddressRespond(l_oAddrInfoRespond, l_oCompInfoRespond, l_oRespond);

		//{
		//	std::lock_guard<std::mutex> lock(m_mutexPostQueryAddrInfoRespond);
		//	m_mapPostQueryAddrInfoRespond.erase(l_strGuid);
		//}

	} while (false);

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryAddressInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::PostQueryAddrInfoRespond(PROTOCOL::CAddressInfoRequest& p_oRequest, PROTOCOL::CAddrInfoRespond& p_oRespond, std::string p_strGuid)
{
	ICC_LOG_DEBUG(m_pLog, "PostQueryAddrInfoRespond msgid[%s] == l_strGuid[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(), p_strGuid.c_str());
	std::string strCmd = "query_addr_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00042");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790272791642112";
	}

	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;

	if (m_strnormaLocation == "1")
	{
		if (!p_oRequest.m_strLat.empty() && !p_oRequest.m_strLon.empty())
		{
			// 走经纬度查询	 LON 经度   LAT 纬度
			// begin
			// end
			std::string strbeginLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) - 0.00045);
			std::string strendLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) + 0.00045);
			std::string strbeginLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) - 0.00045);
			std::string strendLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) + 0.00045);

			p_pInputParm.Condition = m_pString->Format("(NORMA_LON between %s and %s and NORMA_LAT between %s and %s)", strbeginLon.c_str(), strendLon.c_str(), strbeginLat.c_str(), strendLat.c_str());
			//p_pInputParm.Condition = "(LON between " + strbeginLon + "and "  + strendLon  + "LAT like '" + strLat + "%')";
		}
		else if (!p_oRequest.m_strAddrName.empty())
		{
			// 走地址查询	
			if (!m_strCityName.empty())
			{
				int p = p_oRequest.m_strAddrName.find(m_strCityName.c_str());

				if (p > 0)
				{
					p_pInputParm.Condition = "(ADDR_NAME like '%" + p_oRequest.m_strAddrName + "%' and NORMA_LON > 0 and NORMA_LAT > 0 )";
				}
				else
				{
					p_pInputParm.Condition = "(ADDR_NAME like '%" + m_strCityName + "%" + p_oRequest.m_strAddrName + "%' and NORMA_LON > 0 and NORMA_LAT > 0 )";
				}
			}
			else
			{
				p_pInputParm.Condition = "(ADDR_NAME like '%" + p_oRequest.m_strAddrName + "%' and NORMA_LON > 0 and NORMA_LAT > 0 )";
			}
		}
	}
	else
	{
		if (!p_oRequest.m_strLat.empty() && !p_oRequest.m_strLon.empty())
		{
			// 走经纬度查询	 LON 经度   LAT 纬度
			// begin
			// end
			std::string strbeginLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) - 0.0009);
			std::string strendLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) + 0.0009);
			std::string strbeginLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) - 0.0009);
			std::string strendLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) + 0.0009);

			p_pInputParm.Condition = m_pString->Format("(LON between %s and %s and LAT between %s and %s)", strbeginLon.c_str(), strendLon.c_str(), strbeginLat.c_str(), strendLat.c_str());
			//p_pInputParm.Condition = "(LON between " + strbeginLon + "and "  + strendLon  + "LAT like '" + strLat + "%')";
		}
		else if (!p_oRequest.m_strAddrName.empty())
		{
			// 走地址查询	
			if (!m_strCityName.empty())
			{
				int p = p_oRequest.m_strAddrName.find(m_strCityName.c_str());

				if (p > 0)
				{
					p_pInputParm.Condition = "(ADDR_NAME like '%" + p_oRequest.m_strAddrName + "%' and LON > 0 and LAT > 0 )";
				}
				else
				{
					p_pInputParm.Condition = "(ADDR_NAME like '%" + m_strCityName + "%" + p_oRequest.m_strAddrName + "%' and LON > 0 and LAT > 0 )";
				}
			}
			else
			{
				p_pInputParm.Condition = "(ADDR_NAME like '%" + p_oRequest.m_strAddrName + "%' and LON > 0 and LAT > 0 )";
			}
		}
	}

	p_pInputParm.Parafs = { "INFO_DELE_JUDGE_FLAG","ADDR_NAME","LON","LAT","NORMA_LON","NORMA_LAT" };

	p_pInputParm.m_strMaxReturnNum = m_strReturnNum;

	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);

	std::string strData = "";
	do
	{
		if (0 == m_strConfig.compare("1"))
		{
			std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
			//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
			strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);

			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond  msgid[%s] \n strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(),
				strCmd.c_str(), strTarget.c_str(), strServiceId.c_str(), strContent.c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
				strServiceId.c_str(), strContent.c_str());
			strData = GetPostTextByContent(strCmd);
		}
		std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond msgid[%s] \n strReceive[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(), strReceive.c_str());
		if (strReceive.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond end msgid[%s] \n strReceive[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(), strReceive.c_str());
			p_oRespond.m_strCode = "500";
			p_oRespond.m_strMessage = "Failed";

			break;
		}

		//解析  Parse
		if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
		{
			p_oRespond.m_strCode = "501";
			p_oRespond.m_strMessage = strReceive;

			break;
		}

	} while (false);
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond-->m_mutexPostQueryAddrInfoRespond begin msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
	if (!p_strGuid.empty())
	{
		std::lock_guard<std::mutex> lock(m_mutexPostQueryAddrInfoRespond);
		m_mapPostQueryAddrInfoRespond[p_strGuid] = p_oRespond;
	}
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond end msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
}

void CBusinessImpl::PostQueryCompInfoRespond(PROTOCOL::CAddressInfoRequest& p_oRequest, PROTOCOL::CCompInfoRespond& p_oRespond)
{
	ICC_LOG_DEBUG(m_pLog, "PostQueryCompInfoRespond msgid[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str());
	std::string strCmd = "query_comp_info";
	std::string strTarget = GetPostUrl(strCmd);
	if (strTarget.empty())
	{
		strTarget = m_pString->Format("/restcloud/fhopenapiplatform/S-510000000000-0100-00040");
	}

	std::string strServiceId = GetPostServiceId(strCmd);
	if (strServiceId.empty())
	{
		strServiceId = "9790272233799680";
	}
	std::string strContent;                             //请求内容
	std::string strErrorMessage;                        //请求返回错误信息
	PROTOCOL::InputParm p_pInputParm;
	if (m_strnormaLocation == "1")
	{
		if (!p_oRequest.m_strLat.empty() && !p_oRequest.m_strLon.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond m_strnormaLocation m_strLat  begin msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
			// 走经纬度查询	 LON 经度   LAT 纬度
			std::string strbeginLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) - 0.0009);
			std::string strendLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) + 0.0009);
			std::string strbeginLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) - 0.0009);
			std::string strendLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) + 0.0009);

			p_pInputParm.Condition = m_pString->Format("(NORMA_COMP_ADDR_LON between %s and %s and NORMA_COMP_ADDR_LAT between %s and %s)", strbeginLon.c_str(), strendLon.c_str(), strbeginLat.c_str(), strendLat.c_str());
		}
		else if (!p_oRequest.m_strAddrName.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond m_strnormaLocation m_strAddrName begin msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
			if (!m_strCityName.empty())
			{
				// 走地址查询	
				p_pInputParm.Condition = "((COMP_NAME like '%" + p_oRequest.m_strAddrName + "%' or  COM_ADDI_DETAIL_ADDR like '%" + p_oRequest.m_strAddrName
					+ "%') and COM_ADDI_DETAIL_ADDR like '%" + m_strCityName + "%' and NORMA_COMP_ADDR_LON > 0 and NORMA_COMP_ADDR_LAT > 0 )";
			}
			else
			{
				p_pInputParm.Condition = "((COMP_NAME like '%" + p_oRequest.m_strAddrName + "%' or  COM_ADDI_DETAIL_ADDR like '%" + p_oRequest.m_strAddrName
					+ "%') and NORMA_COMP_ADDR_LON > 0 and NORMA_COMP_ADDR_LAT > 0 )";
			}
		}
	}
	else
	{
		if (!p_oRequest.m_strLat.empty() && !p_oRequest.m_strLon.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond  m_strLat begin msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
			// 走经纬度查询	 LON 经度   LAT 纬度
			std::string strbeginLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) - 0.00045);
			std::string strendLon = std::to_string(m_pString->ToDouble(p_oRequest.m_strLon) + 0.00045);
			std::string strbeginLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) - 0.00045);
			std::string strendLat = std::to_string(m_pString->ToDouble(p_oRequest.m_strLat) + 0.00045);

			p_pInputParm.Condition = m_pString->Format("(COMP_ADDR_LON between %s and %s and COMP_ADDR_LAT between %s and %s)", strbeginLon.c_str(), strendLon.c_str(), strbeginLat.c_str(), strendLat.c_str());
		}
		else if (!p_oRequest.m_strAddrName.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryAddrInfoRespond  m_strAddrName begin msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
			if (!m_strCityName.empty())
			{
				// 走地址查询	
				p_pInputParm.Condition = "((COMP_NAME like '%" + p_oRequest.m_strAddrName + "%' or  COM_ADDI_DETAIL_ADDR like '%" + p_oRequest.m_strAddrName
					+ "%') and COM_ADDI_DETAIL_ADDR like '%" + m_strCityName + "%' and COMP_ADDR_LON > 0 and COMP_ADDR_LAT > 0 )";
			}
			else
			{
				p_pInputParm.Condition = "((COMP_NAME like '%" + p_oRequest.m_strAddrName + "%' or  COM_ADDI_DETAIL_ADDR like '%" + p_oRequest.m_strAddrName
					+ "%') and COMP_ADDR_LON > 0 and COMP_ADDR_LAT > 0 )";
			}
		}
	}
	p_pInputParm.Parafs = { "COMP_NAME","UNIT_TCODE","UNIT_TNAME","COM_ADDI_DETAIL_ADDR","COMP_ADDR_LON","COMP_ADDR_LAT",
		"OWN_POL_STAT_PSAG_CODE","OWN_POL_STAT_PSAG","NORMA_COMP_ADDR_LON","NORMA_COMP_ADDR_LAT" };

	p_pInputParm.m_strMaxReturnNum = m_strReturnNum;
	//构建消息体
	strContent = BuildPostContentRespond(p_oRequest.m_oBodyInfo, p_pInputParm);

	std::string strData = "";
	if (0 == m_strConfig.compare("1"))
	{
		std::map<std::string, std::string> mapHeader = PostHeader(strServiceId, strCmd);
		//strData = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage);
		strData = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeader, strContent, strErrorMessage, m_intTimeOut);
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCompInfoRespond msgid[%s] \n strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(),
			strCmd.c_str(), strTarget.c_str(), strServiceId.c_str(), strContent.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCompInfoRespond  strCmd[%s] \n strTarget[%s] \n strServiceId[%s] \n strContent[%s]", strCmd.c_str(), strTarget.c_str(),
			strServiceId.c_str(), strContent.c_str());
		strData = GetPostTextByContent(strCmd);
	}
	std::string strReceive = Subreplace(strData, "\"null\"", "\"\"");

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCompInfoRespond msgid[%s] \n strReceive[%s]", p_oRequest.m_oHeader.m_strMsgid.c_str(), strReceive.c_str());

	if (strReceive.empty())
	{
		p_oRespond.m_strCode = "500";
		p_oRespond.m_strMessage = "Failed";

		return;
	}
	//解析  Parse
	if (!p_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		p_oRespond.m_strCode = "501";
		p_oRespond.m_strMessage = strReceive;

		return;
	}
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData PostQueryCompInfoRespond end msgid[%s] ", p_oRequest.m_oHeader.m_strMsgid.c_str());
}


void CBusinessImpl::OnNotifQueryCertInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryCertInfoAndPersonInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CGetQueryCertInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	//用身份证号来查询户号  DOMIC_NUM 与 行政区划 DOMIC_ADM_DIV
	PROTOCOL::CGetQueryCertInfoAndPersonInfoRespond l_oRespond;
	PostQueryCertInfo(l_oRequest, l_oRespond.m_PostQueryCertInfoRequest);
	
	if (std::atoi(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_strtotalRows.c_str()) > 0)
	{
		//排序 LAST_TIME
		std::sort(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.begin(), l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.end(),
			[](const PROTOCOL::CPostQueryCertInfoRequest::Results& dept1, const PROTOCOL::CPostQueryCertInfoRequest::Results& dept2)
			{
				return atoll(dept1.m_lastTime.c_str()) > atoll(dept2.m_lastTime.c_str());
			});

		//用户号  DOMIC_NUM 与 行政区划 DOMIC_ADM_DIV 来查询户籍信息
		if (!l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(0).m_domicNum.empty() && !l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(0).m_domicAdmDiv.empty())
		{
			l_oRequest.m_oBody.m_strCertNum = "";
			l_oRequest.m_oBody.m_strDomicNum = l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(0).m_domicNum.c_str();
			l_oRequest.m_oBody.m_strDomic_Adm_Div = l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(0).m_domicAdmDiv.c_str();
			l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.clear();
			PostQueryCertInfo(l_oRequest, l_oRespond.m_PostQueryCertInfoRequest);
		}
	}
	PROTOCOL::CPersonPhotoInfoRespond l_oPersonPhotoInfoRespond;
	PROTOCOL::CGetQueryPersonInfoRequest l_oGetQueryPersonInfoRequest;
	l_oGetQueryPersonInfoRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBody.m_oBodyInfo;
	l_oRespond.m_strCode = l_oRespond.m_PostQueryCertInfoRequest.m_strstatus;
	l_oRespond.m_strMessage = l_oRespond.m_PostQueryCertInfoRequest.m_strmsg;

	std::sort(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.begin(), l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.end(),
		[](const PROTOCOL::CPostQueryCertInfoRequest::Results& dept1, const PROTOCOL::CPostQueryCertInfoRequest::Results& dept2)
		{
			return atoll(dept1.m_certNum.c_str()) < atoll(dept2.m_certNum.c_str());
		});

	//去重
	auto ite = std::unique(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.begin(), l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.end(),
		[](const PROTOCOL::CPostQueryCertInfoRequest::Results& d1, const PROTOCOL::CPostQueryCertInfoRequest::Results& d2)
		{
			return d1.m_certNum == d2.m_certNum;
		});

	l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.erase(ite, l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.end());

	l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_strtotalRows = std::to_string(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.size());
	l_oRespond.m_Data.m_strtotalRows = l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_strtotalRows;

	// 用身份证号请求个人身份信息
	for (int i = 0; i < std::atoi(l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_strtotalRows.c_str()); ++i)
	{	
		if (!l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_certNum.empty())
		{
			l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum = l_oRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_certNum;
			PostQueryPersonInfo(l_oGetQueryPersonInfoRequest, l_oRespond.m_PostQueryPersonInfoRequest);


			if (!l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.empty())
			{
				BuildPersonPhotoInfoRespond(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oPersonPhotoInfoRespond);
				if (!l_oPersonPhotoInfoRespond.m_strData.m_vecLists.empty())
				{
					l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPhoto = l_oPersonPhotoInfoRespond.m_strData.m_vecLists.at(0).m_strPhoto;
				}
				l_oPersonPhotoInfoRespond.m_strData.m_vecLists.clear();
				PostQueryPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);
				// 判断涉藏 吸毒 在逃 重点人员便签
				BuildPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);
			}
		}
		//组装信息
		BuildGetQueryCertInfoAndPersonInfoRespond(l_oRespond, i);
	}
	//回复
	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCertInfoAndPersonInfo send message:%s", strmsg.c_str());
}


void CBusinessImpl::BuildGetQueryAddressRespond(PROTOCOL::CAddrInfoRespond p_AddrInfoRespond, PROTOCOL::CCompInfoRespond p_CompInfoRespond, PROTOCOL::CAddressInfoRespond& p_strRespond)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildGetQueryAddressRespond begin");
	// 组装数据
	// 从两个结果，取出十个距离最近的结果返回（取值规则，如果两个接口都返回超过五个，各取五个，如果某个接口返回少于5个，从其他接口中补充，补满10个，补不满就有多少返回多少）

	if (p_AddrInfoRespond.m_strData.m_vecLists.size() >= m_intaddrSum / 2 && p_CompInfoRespond.m_strData.m_vecLists.size() >= m_intaddrSum / 2)
	{
		size_t index = 0;
		for (size_t i = 0; i < p_AddrInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName.empty())
			{
				continue;
			}

			index++;

			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName;
			results.m_strLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLat;
			results.m_strLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLon;
			results.m_strNormaLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLat;
			results.m_strNormaLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLon;
			results.m_strInfoDeleJudgeFlag = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strInfoDeleJudgeFlag;
			p_strRespond.m_Data.m_vecResults.push_back(results);

			if (index >= m_intaddrSum / 2)
			{
				break;
			}
		}

		for (size_t i = 0; i < p_CompInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr.empty())
			{
				continue;
			}

			index++;

			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr;
			results.m_strLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLat;
			results.m_strLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLon;
			results.m_strNormaLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLat;
			results.m_strNormaLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLon;
			results.m_strCompName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompName;
			results.m_strUnitTcode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTcode;
			results.m_strUnitTname = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTname;
			results.m_strOwnPolStatPsagCode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode;
			results.m_strOwnPolStatPsag = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsag;
			p_strRespond.m_Data.m_vecResults.push_back(results);

			if (index >= m_intaddrSum / 2)
			{
				break;
			}
		}
	}
	else if (p_AddrInfoRespond.m_strData.m_vecLists.size() >= m_intaddrSum / 2 && p_CompInfoRespond.m_strData.m_vecLists.size() < m_intaddrSum / 2)
	{
		size_t index = 0;
		for (size_t i = 0; i < p_CompInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr.empty())
			{
				continue;
			}
			index++;
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr;
			results.m_strLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLat;
			results.m_strLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLon;
			results.m_strNormaLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLat;
			results.m_strNormaLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLon;
			results.m_strCompName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompName;
			results.m_strUnitTcode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTcode;
			results.m_strUnitTname = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTname;
			results.m_strOwnPolStatPsagCode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode;
			results.m_strOwnPolStatPsag = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsag;
			p_strRespond.m_Data.m_vecResults.push_back(results);

		}

		for (size_t i = 0; i < p_AddrInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName.empty())
			{
				continue;
			}
			index++;
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName;
			results.m_strLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLat;
			results.m_strLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLon;
			results.m_strNormaLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLat;
			results.m_strNormaLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLon;
			results.m_strInfoDeleJudgeFlag = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strInfoDeleJudgeFlag;
			p_strRespond.m_Data.m_vecResults.push_back(results);

			if (index == m_intaddrSum)
			{
				break;
			}
		}
	}
	else if (p_AddrInfoRespond.m_strData.m_vecLists.size() < m_intaddrSum / 2 && p_CompInfoRespond.m_strData.m_vecLists.size() >= m_intaddrSum / 2)
	{
		int index = 0;
		for (size_t i = 0; i < p_AddrInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName.empty())
			{
				continue;
			}
			index++;
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName;
			results.m_strLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLat;
			results.m_strLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLon;
			results.m_strNormaLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLat;
			results.m_strNormaLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLon;
			results.m_strInfoDeleJudgeFlag = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strInfoDeleJudgeFlag;
			p_strRespond.m_Data.m_vecResults.push_back(results);

		}
		for (size_t i = 0; i < p_CompInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr.empty())
			{
				continue;
			}
			index++;
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr;
			results.m_strLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLat;
			results.m_strLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLon;
			results.m_strNormaLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLat;
			results.m_strNormaLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLon;
			results.m_strCompName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompName;
			results.m_strUnitTcode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTcode;
			results.m_strUnitTname = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTname;
			results.m_strOwnPolStatPsagCode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode;
			results.m_strOwnPolStatPsag = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsag;
			p_strRespond.m_Data.m_vecResults.push_back(results);
			if (index == m_intaddrSum)
			{
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < p_AddrInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName.empty())
			{
				continue;
			}
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strAddrName;
			results.m_strLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLat;
			results.m_strLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strLon;
			results.m_strNormaLat = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLat;
			results.m_strNormaLon = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strNormaLon;
			results.m_strInfoDeleJudgeFlag = p_AddrInfoRespond.m_strData.m_vecLists.at(i).m_strInfoDeleJudgeFlag;
			p_strRespond.m_Data.m_vecResults.push_back(results);
		}

		for (size_t i = 0; i < p_CompInfoRespond.m_strData.m_vecLists.size(); i++)
		{
			if (p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr.empty())
			{
				continue;
			}
			PROTOCOL::CAddressInfoRespond::Results results;
			results.m_strAddrName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strComAddiDetailAddr;
			results.m_strLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLat;
			results.m_strLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompAddrLon;
			results.m_strNormaLat = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLat;
			results.m_strNormaLon = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strNormaCompAddrLon;
			results.m_strCompName = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strCompName;
			results.m_strUnitTcode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTcode;
			results.m_strUnitTname = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strUnitTname;
			results.m_strOwnPolStatPsagCode = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode;
			results.m_strOwnPolStatPsag = p_CompInfoRespond.m_strData.m_vecLists.at(i).m_strOwnPolStatPsag;
			p_strRespond.m_Data.m_vecResults.push_back(results);
		}
	}
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildGetQueryAddressRespond end");
}

void CBusinessImpl::BuildRealDistanceAndSort(PROTOCOL::CAddressInfoRequest p_pRequest, PROTOCOL::CAddrInfoRespond p_AddrInfoRespond, PROTOCOL::CCompInfoRespond p_CompInfoRespond)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildRealDistanceAndSort begin");
	// 计算距离
	for (size_t i = 0; i < p_AddrInfoRespond.m_strData.m_vecLists.size(); i++)
	{
		if (!p_AddrInfoRespond.m_strData.m_vecLists[i].m_strLat.empty() && !p_AddrInfoRespond.m_strData.m_vecLists[i].m_strLon.empty())
		{
			p_AddrInfoRespond.m_strData.m_vecLists[i].m_strDistance = std::to_string(RealDistance(m_pString->ToDouble(p_pRequest.m_strLat), m_pString->ToDouble(p_pRequest.m_strLon),
				m_pString->ToDouble(p_AddrInfoRespond.m_strData.m_vecLists[i].m_strLat), m_pString->ToDouble(p_AddrInfoRespond.m_strData.m_vecLists[i].m_strLon)));

		}
	}

	for (size_t i = 0; i < p_CompInfoRespond.m_strData.m_vecLists.size(); i++)
	{
		if (!p_CompInfoRespond.m_strData.m_vecLists[i].m_strCompAddrLat.empty() && !p_CompInfoRespond.m_strData.m_vecLists[i].m_strCompAddrLon.empty())
		{
			p_CompInfoRespond.m_strData.m_vecLists[i].m_strDistance = std::to_string(RealDistance(m_pString->ToDouble(p_pRequest.m_strLat), m_pString->ToDouble(p_pRequest.m_strLon),
				m_pString->ToDouble(p_CompInfoRespond.m_strData.m_vecLists[i].m_strCompAddrLat), m_pString->ToDouble(p_CompInfoRespond.m_strData.m_vecLists[i].m_strCompAddrLon)));
		}
	}

	// 按照距离排序
	std::sort(p_AddrInfoRespond.m_strData.m_vecLists.begin(), p_AddrInfoRespond.m_strData.m_vecLists.end(),
		[](const PROTOCOL::CAddrInfoRespond::CData::CList& dept1, const PROTOCOL::CAddrInfoRespond::CData::CList& dept2)
		{
			return atof(dept1.m_strDistance.c_str()) < atof(dept2.m_strDistance.c_str());
		});

	std::sort(p_CompInfoRespond.m_strData.m_vecLists.begin(), p_CompInfoRespond.m_strData.m_vecLists.end(),
		[](const PROTOCOL::CCompInfoRespond::CData::CList& dept1, const PROTOCOL::CCompInfoRespond::CData::CList& dept2)
		{
			return atof(dept1.m_strDistance.c_str()) < atof(dept2.m_strDistance.c_str());
		});

	ICC_LOG_DEBUG(m_pLog, "PersonInfoData BuildRealDistanceAndSort end");
}

void CBusinessImpl::BuildGetQueryCertInfoAndPersonInfoRespond(PROTOCOL::CGetQueryCertInfoAndPersonInfoRespond &p_pRespond, int i)
{
	PROTOCOL::CGetQueryCertInfoAndPersonInfoRespond::Results results;
	results.m_predict = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_predict;
	results.m_relType = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_relType;

	results.m_certNum = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_certNum;
	results.m_domicNum = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicNum;

	auto it = m_strHouHeadRel.find(p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_houHeadRel);
	if (it != m_strHouHeadRel.end())
	{
		results.m_houHeadRel = it->second;
	}
	else 
	{
		results.m_houHeadRel = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_houHeadRel;
	}

	results.m_domicAddr = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicAddr;
	results.m_domicAdmDiv = p_pRespond.m_PostQueryCertInfoRequest.m_Data.m_vecResults.at(i).m_domicAdmDiv;

	if (std::atoi(p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_strtotalRows.c_str()) > 0)
	{
		results.m_strEscu = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strEscu;
		results.m_strHomeaddr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strHomeaddr;
		results.m_strSex = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strSex;
		results.m_strEdudegree = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strEdudegree;
		results.m_strDomplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strDomplace;

		results.m_strBplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strBplace;
		results.m_strNation = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strNation;
		results.m_strServiceplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strServiceplace;
		results.m_strReli = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strReli;
		results.m_strMarr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strMarr;
		results.m_strVeh = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strVeh;

		results.m_strPoli = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPoli;
		results.m_strProf = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strProf;
		results.m_strWorkaddr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strWorkaddr;
		results.m_strBirthday = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strBirthday;
		results.m_strMobile = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strMobile;
		results.m_strChname = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strChname;
		results.m_strHplaceArea = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strHplaceArea;
		
		auto it = m_strDistrictCodeMap.find(results.m_strHplaceArea);
		if (it != m_strDistrictCodeMap.end())
		{
			results.m_strHplaceAreaCode = it->second;
		}
		else
		{
			results.m_strHplaceAreaCode = results.m_strHplaceArea;
		}

		results.m_strPhoto = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPhoto;

		PROTOCOL::CGetQueryPersonTagInfoRespond strTagInfo;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData strData;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData::CTag strDataTag;

		if (0 == p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strCode.compare("200"))
		{
			for (size_t index = 0; index < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData.size(); ++index)
			{
				strData.m_strTagType = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_strTagType;
				strData.m_strTagTypeName = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_strTagTypeName;

				for (size_t j = 0; j < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags.size(); ++j)
				{
					strDataTag.m_strTagCode = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagCode;
					strDataTag.m_strTagName = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagName;
					strData.m_vecTags.push_back(strDataTag);
				}
				strTagInfo.m_strData.push_back(strData);
				strData.m_vecTags.clear();
			}
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bTibetanRespond)
		{
			strDataTag.m_strTagCode = "01";
			strDataTag.m_strTagName = m_strChineseLib["Tibetan"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bDrugRespond)
		{
			strDataTag.m_strTagCode = "02";
			strDataTag.m_strTagName = m_strChineseLib["Drug"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bEscapeRespond)
		{
			strDataTag.m_strTagCode = "03";
			strDataTag.m_strTagName = m_strChineseLib["Escape"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bPersonKeyRespond)
		{
			strDataTag.m_strTagCode = "04";
			strDataTag.m_strTagName = m_strChineseLib["PersonKey"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (!p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.empty())
		{
			for (size_t index = 0; index < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.size(); index++)
			{
				results.m_vecAttentionTag.push_back(p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.at(index));
			}
		}

		results.PersonTagInfo = strTagInfo;
	}
	p_pRespond.m_Data.m_vecResults.push_back(results);
	p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.clear();
}

void CBusinessImpl::OnNotifQueryCredInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryCredInfoAndPersonInfo receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CGetQueryCredInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 优先根据车牌号查询机动车信息 
	PROTOCOL::CGetQueryCredInfoAndPersonInfoRespond l_oRespond;
	PostQueryCredInfo(l_oRequest, l_oRespond.m_PostQueryCredInfoRequest);
	
	PROTOCOL::CGetQueryPersonInfoRequest l_oGetQueryPersonInfoRequest;

	PROTOCOL::CPersonPhotoInfoRespond l_oPersonPhotoInfoRespond;

	l_oGetQueryPersonInfoRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBody.m_oBodyInfo;
	l_oRespond.m_strCode = l_oRespond.m_PostQueryCredInfoRequest.m_strstatus;
	l_oRespond.m_strMessage = l_oRespond.m_PostQueryCredInfoRequest.m_strmsg;
	//l_oRespond.m_Data.m_strtotalRows = l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_strtotalRows;
	PROTOCOL::CGetQueryIdInfoRequest l_IDRequest;
	l_IDRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBody.m_oBodyInfo;
	for (int i = 0; i < std::atoi(l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_strtotalRows.c_str()); ++i)
	{
		if (0 == l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatCode.compare("E"))
		{
			continue;
		}

		if (!l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_credNum.empty())
		{
			// 身份证号码不对，根据手机号来查询
			if (l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_credNum.size() < 17 && !l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob.empty())
			{
				l_IDRequest.m_oBody.m_strmobile = l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob;
				PROTOCOL::CPushIdInfoRequest l_oPushIdInfoRequest;
				PostQueryIdInfo(l_IDRequest, l_oPushIdInfoRequest);
				if (l_oPushIdInfoRequest.m_vecData.size() > 0)
				{
					std::sort(l_oPushIdInfoRequest.m_vecData.begin(), l_oPushIdInfoRequest.m_vecData.end(),
						[](const PROTOCOL::CPushIdInfoRequest::Data& dept1, const PROTOCOL::CPushIdInfoRequest::Data& dept2)
						{
							return atof(dept1.m_strlevel.c_str()) > atof(dept2.m_strlevel.c_str());
						});

					if (!l_oPushIdInfoRequest.m_vecData.at(0).m_stridno.empty())
					{
						// 获取身份证号
						l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum = l_oPushIdInfoRequest.m_vecData.at(0).m_stridno;
					}
				}
			}
			else
			{
				// 身份证号查询主题信息
				l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum = l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_credNum;
			}
		}
		else if (!l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob.empty())
		{
			// 无身份证号码时，根据手机号查询
			l_IDRequest.m_oBody.m_strmobile = l_oRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob;
			PROTOCOL::CPushIdInfoRequest l_oPushIdInfoRequest;
			PostQueryIdInfo(l_IDRequest, l_oPushIdInfoRequest);
			if (l_oPushIdInfoRequest.m_vecData.size() > 0)
			{
				std::sort(l_oPushIdInfoRequest.m_vecData.begin(), l_oPushIdInfoRequest.m_vecData.end(),
					[](const PROTOCOL::CPushIdInfoRequest::Data& dept1, const PROTOCOL::CPushIdInfoRequest::Data& dept2)
					{
						return atof(dept1.m_strlevel.c_str()) > atof(dept2.m_strlevel.c_str());
					});

				if (!l_oPushIdInfoRequest.m_vecData.at(0).m_stridno.empty())
				{
					l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum = l_oPushIdInfoRequest.m_vecData.at(0).m_stridno;
				}
			}
		}

		PostQueryPersonInfo(l_oGetQueryPersonInfoRequest, l_oRespond.m_PostQueryPersonInfoRequest);

		if (!l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.empty())
		{
			BuildPersonPhotoInfoRespond(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oPersonPhotoInfoRespond);
			if (!l_oPersonPhotoInfoRespond.m_strData.m_vecLists.empty())
			{
				l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPhoto = l_oPersonPhotoInfoRespond.m_strData.m_vecLists.at(0).m_strPhoto;
			}
			l_oPersonPhotoInfoRespond.m_strData.m_vecLists.clear();
			PostQueryPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);
			// 判断涉藏 吸毒 在逃 重点人员便签
			BuildPersonTagInfo(l_oRequest.m_oBody.m_oBodyInfo, l_oGetQueryPersonInfoRequest.m_oBody.m_strCertNum, l_oRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo);
		}

		BuildGetQueryCredInfoAndPersonInfoRespond(l_oRespond, i);
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCredInfoAndPersonInfo send message:%s", strmsg.c_str());

}

void CBusinessImpl::BuildGetQueryCredInfoAndPersonInfoRespond(PROTOCOL::CGetQueryCredInfoAndPersonInfoRespond& p_pRespond, int i)
{
	PROTOCOL::CGetQueryCredInfoAndPersonInfoRespond::Results results;
	results.m_insEndDate = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_insEndDate;
	results.m_credNum = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_credNum;
	results.m_mob = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_mob;
	results.m_vehLicTname = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehLicTname;
	results.m_vehPlateNum = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehPlateNum;
	results.m_vehcBrandModel = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehcBrandModel;
	results.m_vehType = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehType;
	results.m_m_vehColorName = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_m_vehColorName;
	results.m_engineNo = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_engineNo;
	results.m_vehUsagName = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehUsagName;
	results.m_vehStatCode = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatCode;
	results.m_vehStatName = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehStatName;
	results.m_inspVeDate = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_inspVeDate;
	results.m_firstRegDate = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_firstRegDate;
	results.m_regComPsag = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_regComPsag;
	results.m_vehTname = p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehTname;
	results.m_strVehLicTcode = GetCarCodeFromDataBase(p_pRespond.m_PostQueryCredInfoRequest.m_Data.m_vecResults.at(i).m_vehLicTname);

	if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.size() > 0)
	{
		results.m_strEscu = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strEscu;
		results.m_strHomeaddr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strHomeaddr;
		results.m_strSex = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strSex;
		results.m_strEdudegree = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strEdudegree;
		results.m_strDomplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strDomplace;

		results.m_strBplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strBplace;
		results.m_strNation = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strNation;
		results.m_strServiceplace = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strServiceplace;
		results.m_strReli = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strReli;
		results.m_strMarr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strMarr;
		results.m_strVeh = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strVeh;

		results.m_strPoli = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPoli;
		results.m_strProf = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strProf;
		results.m_strWorkaddr = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strWorkaddr;
		results.m_strBirthday = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strBirthday;
		results.m_strMobile = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strMobile;
		results.m_strChname = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strChname;
		results.m_strHplaceArea = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strHplaceArea;
		auto it = m_strDistrictCodeMap.find(results.m_strHplaceArea);
		if (it != m_strDistrictCodeMap.end())
		{
			results.m_strHplaceAreaCode = it->second;
		}
		else
		{
			results.m_strHplaceAreaCode = results.m_strHplaceArea;
		}


		results.m_strPhoto = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).m_strPhoto;

		PROTOCOL::CGetQueryPersonTagInfoRespond strTagInfo;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData strData;
		PROTOCOL::CGetQueryPersonTagInfoRespond::CData::CTag strDataTag;

		if (0 == p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strCode.compare("200"))
		{
			for (size_t index = 0; index < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData.size(); ++index)
			{
				strData.m_strTagType = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_strTagType;
				strData.m_strTagTypeName = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_strTagTypeName;

				for (size_t j = 0; j < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags.size(); ++j)
				{
					strDataTag.m_strTagCode = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagCode;
					strDataTag.m_strTagName = p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagName;
					strData.m_vecTags.push_back(strDataTag);
				}
				strTagInfo.m_strData.push_back(strData);
				strData.m_vecTags.clear();
			}
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bTibetanRespond)
		{
			strDataTag.m_strTagCode = "01";
			strDataTag.m_strTagName = m_strChineseLib["Tibetan"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bDrugRespond)
		{
			strDataTag.m_strTagCode = "02";
			strDataTag.m_strTagName = m_strChineseLib["Drug"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bEscapeRespond)
		{
			strDataTag.m_strTagCode = "03";
			strDataTag.m_strTagName = m_strChineseLib["Escape"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		if (p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_bPersonKeyRespond)
		{
			strDataTag.m_strTagCode = "04";
			strDataTag.m_strTagName = m_strChineseLib["PersonKey"];
			strData.m_vecTags.push_back(strDataTag);
			strData.m_strTagType = "rybq";
			strData.m_strTagTypeName = m_strChineseLib["PersonTag"];
			strTagInfo.m_strData.push_back(strData);
			strData.m_vecTags.clear();
		}

		results.PersonTagInfo = strTagInfo;

		if (!p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.empty())
		{
			for (size_t index = 0; index < p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.size(); index++)
			{
				results.m_vecAttentionTag.push_back(p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.at(0).PersonTagInfo.m_vecAttentionTag.at(index));
			}
		}
	}
	p_pRespond.m_PostQueryPersonInfoRequest.m_Data.m_vecResults.clear();
	p_pRespond.m_Data.m_vecResults.push_back(results);
}

void CBusinessImpl::OnNotifQueryCaseVehInfoAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData OnNotifQueryCaseVehInfoAndCaseInfo receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetQueryCaseVehInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	PROTOCOL::CGetQueryCaseVehInfoAndCaseInfoRespond l_oRespond;
	PostQueryCaseVehInfo(l_oRequest, l_oRespond.m_PostQueryCaseVehInfoRequest);

	PROTOCOL::CGetQueryCaseInfoRequest l_oGetQueryCaseInfoRequest;
	l_oGetQueryCaseInfoRequest.m_oBody.m_oBodyInfo = l_oRequest.m_oBodyInfo;
	l_oRespond.m_strCode = l_oRespond.m_PostQueryCaseVehInfoRequest.m_strCode;
	l_oRespond.m_strMessage = l_oRespond.m_PostQueryCaseVehInfoRequest.m_strMessage;
	l_oRespond.m_Data.m_strtotalRows = l_oRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_strCount;

	for (int i = 0; i < std::atoi(l_oRespond.m_Data.m_strtotalRows.c_str()); ++i)
	{
		if (!l_oRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strCaseNo.empty())
		{
			l_oGetQueryCaseInfoRequest.m_oBody.m_CaseNo = l_oRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strCaseNo;
			PostQueryCaseInfo(l_oGetQueryCaseInfoRequest, l_oRespond.m_PostQueryCaseInfoRequest);
		}
		BuildGetQueryCaseVehInfoAndCaseInfoRespond(l_oRespond,i);
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifQueryCaseVehInfoAndCaseInfo send message:%s", strmsg.c_str());

}

void CBusinessImpl::BuildGetQueryCaseVehInfoAndCaseInfoRespond(PROTOCOL::CGetQueryCaseVehInfoAndCaseInfoRespond& p_pRespond, int i)
{
	PROTOCOL::CGetQueryCaseVehInfoAndCaseInfoRespond::Results results;
	results.m_strCaseNo = p_pRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strCaseNo;
	results.m_strVehPlateNum = p_pRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strVehPlateNum;
	results.m_strVehLicTcode = p_pRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strVehLicTcode;
	results.m_strDisPlace = p_pRespond.m_PostQueryCaseVehInfoRequest.m_strData.m_vecLists.at(i).m_strDisPlace;

	if (p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.size() > 0)
	{
		results.m_caseClasCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseClasCode;

		results.m_repoCaseTime = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_repoCaseTime;
		results.m_caseName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseName;
		results.m_mainCaseCaseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_mainCaseCaseNo;
		results.m_casClaName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_casClaName;
		results.m_repoCaseUnitAddrName = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_repoCaseUnitAddrName;
		results.m_discTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_discTime);
		results.m_accepUnitPsag = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_accepUnitPsag;
		results.m_minorCaseCaseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_minorCaseCaseNo;
		results.m_caseClassCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseClassCode;
		results.m_caseNo = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseNo;
		results.m_caseOrgaPsag = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseOrgaPsag;
		results.m_caseDate = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseDate;
		results.m_accepTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_accepTime);
		results.m_caseTimeTimper = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseTimeTimper;
		results.m_caseTypeCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseTypeCode;

		results.m_crimPurpDesc = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_crimPurpDesc;
		results.m_caseSourDesc = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseSourDesc;
		results.m_caseAddrAddrCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseAddrAddrCode;
		results.m_caseOrgaPsagCode = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_caseOrgaPsagCode;
		results.m_arrsceTime = ToStringTime(p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_arrsceTime);
		results.m_setLawsDate = p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.at(0).m_setLawsDate;
	}
	p_pRespond.m_PostQueryCaseInfoRequest.m_oBody.m_Data.m_vecList.clear();
	p_pRespond.m_Data.m_vecResults.push_back(results);
}

bool CBusinessImpl::GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strServiceName = p_strServiceName;
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strVcsServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strVcsServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy aia-alarm-service send service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy aia-alarm-service send service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	
	return false;
}

std::string CBusinessImpl::Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr)
{
	std::string desStr = souceStr;
	size_t pos = 0;
	while ((pos = desStr.find(subStr)) != std::string::npos) {
		desStr.replace(pos, subStr.length(), newStr);
	}
	return desStr;
}

std::string CBusinessImpl::GetPostTextByContent(const std::string strSearch)
{
	std::string strResult = "";
	std::string strTemp = "select value from public.icc_t_param where name = 'SearchStrId';;;";
	std::string strSql = Subreplace(strTemp, "SearchStrId", strSearch);
	ICC_LOG_DEBUG(m_pLog, "sql = %s\n", strSql.c_str());
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
	do
	{
		ICC_LOG_DEBUG(m_pLog, "GetPostTextByContent , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "ExecQuery Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			strResult = l_result->GetValue("value");
		}
	} while (false);

	return strResult;
}

void CBusinessImpl::GetHouHeadRelFromConfig()
{
	std::string code = "";
	std::string relationship = "";
	std::string results = "99";
	std::string strSql = "select * from public.icc_t_houheadrel;;;";
	ICC_LOG_DEBUG(m_pLog, "sql = %s\n", strSql.c_str());
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
	if (!l_result->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "GetHouHeadRelFromConfig Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
		return;
	}
	while (l_result->Next())
	{
		code = l_result->GetValue("code");
		relationship = l_result->GetValue("relationship");
		m_strHouHeadRel[code] = relationship;
	};
	std::vector<std::string> strVecSource = { "Tibetan","Drug","Escape","PersonKey", "PersonTag"};
	OutputChineseLib(strVecSource, m_strChineseLib);

	StringCar();
}

bool CBusinessImpl::OutputChineseLib(std::vector<std::string> strVecSource, std::map<std::string, std::string>& MapResult)
{
	std::string results = "";
	std::string strSql = "select value from public.icc_t_param where name = 'chinese_library';;;";
	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
		ICC_LOG_DEBUG(m_pLog, "chinese_library sql = %s\n", strSql.c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "chinese_library Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
			return false;
		}
		if (l_result->Next())
		{
			results = l_result->GetValue("value");
		}
	} while (false);
	JsonParser::IJsonPtr p_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!p_pJson->LoadJson(results))
	{
		return false;
	}
	size_t size = strVecSource.size();
	for (size_t i = 0; i < size; ++i)
	{
		MapResult[strVecSource.at(i)] = p_pJson->GetNodeValue("/" + strVecSource.at(i), "");
	}
	return true;
}

std::string CBusinessImpl::GetCarCodeFromDataBase(std::string strvehLicTname)
{
	std::string results = "99";
	std::string strSql = "select code from public.icc_t_carnotype where cartype like '%" + strvehLicTname + "%';;;";
	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
		ICC_LOG_DEBUG(m_pLog, "sql = %s\n", strSql.c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "GetCarCodeFromDataBase Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			results = l_result->GetValue("code");
		}
	} while (false);

	return results;
}

std::string CBusinessImpl::ToStringTime(std::string p_pDateTime)
{
	if (p_pDateTime == "")
	{
		return "";
	}
	std::string  l_strDateTime;
	// 将10位时间戳转化为字符串  
	struct tm* l_tm;
	time_t l_ttime;
	char temp[64];

	l_ttime = atol(p_pDateTime.c_str());
	l_tm = localtime(&l_ttime);

	size_t ret = strftime(temp, 64, "%Y-%m-%d %H:%M:%S", l_tm);
	l_strDateTime = std::string(temp);
	return l_strDateTime;
}

void CBusinessImpl::StringCar()
{
	std::string strSql = "select value from public.icc_t_param where name = 'carplate';;;";
	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
		ICC_LOG_DEBUG(m_pLog, "car sql = %s\n", strSql.c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "car Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
			return ;
		}
		if (l_result->Next())
		{
			m_strcar = l_result->GetValue("value");
		}
	} while (false);
}

void CBusinessImpl::GetDistrictCodeMapFromConfig()
{
	std::string strSql = "select * from public.icc_t_district_code_map;;;";
	
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
	if (!l_result->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "icc_t_district_code_map Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
		return;
	}
	while (l_result->Next())
	{
		m_strDistrictCodeMap[l_result->GetValue("district")] = l_result->GetValue("district_code");
	};
}

void CBusinessImpl::GetPostUrlMapFromConfig()
{
	std::string htt_cmd = "";
	std::string http_url = "";
	std::string http_serviceid = "";

	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "select_icc_t_post_url_map";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(sqlReqeust);

	ICC_LOG_DEBUG(m_pLog, "GetPostUrlMapFromConfig sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "GetPostUrlMapFromConfig Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
		return;
	}
	while (l_result->Next())
	{
		htt_cmd = l_result->GetValue("http_cmd");
		http_url = l_result->GetValue("http_url");
		http_serviceid = l_result->GetValue("http_serviceid");
		m_mapPostUrl[htt_cmd] = http_url;
		m_mapPostServiceId[htt_cmd] = http_serviceid;
	}
	
	return;
}

std::string CBusinessImpl::GetPostUrl(std::string p_pcmd)
{
	auto it = m_mapPostUrl.find(p_pcmd);
	if (it != m_mapPostUrl.end())
	{
		return it->second;
	}
	return "";
}


std::string CBusinessImpl::GetPostServiceId(std::string p_pcmd)
{
	auto it = m_mapPostServiceId.find(p_pcmd);
	if (it != m_mapPostServiceId.end())
	{
		return it->second;
	}
	return "";
}

void CBusinessImpl::GetKeyInfoMapFromConfig()
{
	std::string keyinfo_code = "";
	std::string keyinfo_tag = "";
	std::string attention_tag = "";

	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "select_icc_t_keyinfo_code_map";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(sqlReqeust);

	ICC_LOG_DEBUG(m_pLog, "GetKeyInfoMapFromConfig sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "GetKeyInfoMapFromConfig Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
		return;
	}

	while (l_result->Next())
	{
		keyinfo_code = l_result->GetValue("keyinfo_code");
		keyinfo_tag = l_result->GetValue("keyinfo_tag");
		attention_tag = l_result->GetValue("attention_tag");
		m_mapKeyInfo[keyinfo_tag] = attention_tag;
		if (keyinfo_code == "02000000")
		{
			m_strOtherKeyInfo = attention_tag;
		}
		//m_mapPostServiceId[htt_cmd] = http_serviceid;
	}

	return;
}

std::string CBusinessImpl::GetKeyInfo(std::string p_keyinfo_name)
{
	auto it = m_mapKeyInfo.find(p_keyinfo_name);
	if (it != m_mapKeyInfo.end())
	{
		return it->second;
	}
	else
	{
		return m_strOtherKeyInfo;
	}
	
}

double CBusinessImpl::rad(double d)
{
	return d * pi / 180.0;
}

double CBusinessImpl::RealDistance(double lat1, double lng1, double lat2, double lng2)//lat1第一个点纬度,lng1第一个点经度,lat2第二个点纬度,lng2第二个点经度
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData RealDistance begin");
	double a;
	double b;
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	a = radLat1 - radLat2;
	b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
	s = s * EARTH_RADIUS;
	s = s * 1000;
	ICC_LOG_DEBUG(m_pLog, "PersonInfoData RealDistance end");
	return s;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}


/*****************************************************************
 *
 ****************************************************************/
void CBusinessImpl::_CreateThreads()
{
	Config::IConfigPtr l_pCfgReader = ICCGetIConfigFactory()->CreateConfig();
	if (nullptr == l_pCfgReader)
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfoData create config failed!!!");
		return;
	}
	m_strDispatchMode = l_pCfgReader->GetValue("ICC/Plugin/PersonInfoData/procthreaddispatchmode", "1");
	m_uProcThreadCount = m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/PersonInfoData/procthreadcount", "8"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (m_uProcThreadCount > uConCurrency)
	{
		m_uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "PersonInfoData proc thread count:%d", m_uProcThreadCount);

	for (size_t i = 0; i < m_uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			m_vecProcThreads.push_back(pThread);
		}
	}

	ICC_LOG_INFO(m_pLog, "PersonInfoData real proc thread count:%d", m_vecProcThreads.size());
}

void CBusinessImpl::_DestoryThreads()
{
	for (size_t i = 0; i < m_uProcThreadCount; ++i)
	{
		m_vecProcThreads[i]->Stop(0);
	}
	m_vecProcThreads.clear();
}


CommonWorkThreadPtr CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	if (m_strDispatchMode == "1")
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uCurrentThreadIndex >= m_uProcThreadCount)
		{
			m_uCurrentThreadIndex = 0;
		}
		iIndex = m_uCurrentThreadIndex;
		m_uCurrentThreadIndex++;
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		int iMessageCount = 0;
		for (size_t i = 0; i < m_uProcThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}

	return m_vecProcThreads[iIndex];
}

void CBusinessImpl::_OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	CommonWorkThreadPtr pThread = _GetThread();
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVENOTIFY, 0, 0, 0, p_pNotifiReqeust->GetCmdName(), "", p_pNotifiReqeust);
	}
}

void CBusinessImpl::_DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName)
{
	std::map<std::string, ProcNotify>::const_iterator itr;
	itr = m_mapFuncs.find(strCmdName);
	if (itr != m_mapFuncs.end())
	{
		(this->*itr->second)(p_pNotifiReqeust);
	}
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_RECEIVENOTIFY:
	{
		try
		{
			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY begin!");
			_DispatchNotify(msg_data.pTask, msg_data.str_msg);
			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY end!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception!");
		}
	}
	break;

	default:
		break;
	}
}
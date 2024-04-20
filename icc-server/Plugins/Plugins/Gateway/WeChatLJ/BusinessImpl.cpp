#include "Boost.h"
#include "BusinessImpl.h"
#include "DefineInfo.h"

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_WECHATLJ_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pFact = ICCGetIStringFactory();
	m_pXmlFty = ICCGetIXmlFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/WeChatLJ/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}
	LoadConfig();
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimerHandleRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnWeChatAlarmRespond);
	std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, m_timerInterval, DELAYSTARTTIMER);
	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::LoadConfig()
{
	m_strFilePath = m_pConfig->GetValue("ICC/Plugin/WeChatLJ/ftphomepath", "");
	m_strType = m_pConfig->GetValue("ICC/Plugin/WeChatLJ/type", "");
	m_timerInterval = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/WeChatLJ/timerInterval", ""));
}

void CBusinessImpl::OnTimerHandleRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//扫描请求文件
	std::vector<std::string>l_vFileList;
	if (WeChatScanFilesWithFilter(m_strFilePath, WECHAT_REQUEST_FILE_FLITER, l_vFileList))
	{
		std::vector<std::string>::iterator it = l_vFileList.begin();
		for (; it != l_vFileList.end(); ++it)
		{
			std::string l_strFileName = *it;
			ICC_LOG_DEBUG(m_pLog, "wechat loop file, filename[%s]", l_strFileName.c_str());
			if (WeChaReadRequestFile(l_strFileName))
			{
				WeChatChangeFileExtension(l_strFileName);
			}
		}
	}
}

void CBusinessImpl::OnWeChatAlarmRespond(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//-DIC019040已处警

	std::string l_strMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CAlarmSync l_oSyncAlarmInfo;
	if (!l_oSyncAlarmInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	if (l_oSyncAlarmInfo.m_oBody.m_strSourceType == WECHAT_TYPE_LJ&&
		l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_PROCESSED&&
		SelectThirdAlarmByGuid(l_oSyncAlarmInfo.m_oBody.m_strSourceID))
	{

		PROTOCOL::CWeChatRespond l_oWeChatResp;
		l_oWeChatResp.m_strProcessRes = "";
		l_oWeChatResp.m_strType = m_strType;
		l_oWeChatResp.m_strTypeid = l_oSyncAlarmInfo.m_oBody.m_strID;

		std::string l_strXml = l_oWeChatResp.ToString(m_pXmlFty->CreateXml());
		WeChatGenerateRespFile(l_strXml);
	}
}

bool CBusinessImpl::WeChaReadRequestFile(std::string p_strFileName)
{
	Xml::IXmlPtr l_pXml = m_pXmlFty->CreateXml();
	PROTOCOL::CWeChatRequest l_oCWeChatRequest;
	if (!l_oCWeChatRequest.ParseString(p_strFileName, l_pXml, m_pFact->CreateString()))
	{
		ICC_LOG_ERROR(m_pLog, "wechat load file request failed, xml[%s]", m_strFilePath.c_str());
	}
	if (l_oCWeChatRequest.m_oBody.m_strTypeid.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "wechat handle file request typeid is empty");
		return false;
	}

	if (SelectThirdAlarmByGuid(l_oCWeChatRequest.m_oBody.m_strTypeid))
	{//判断是否重复报警
		ICC_LOG_DEBUG(m_pLog, "wechat handle file request typeid is repeat!");
	}

	//发送报警协议内容
	SendAddWeChatRequest(l_oCWeChatRequest);
	return true;
}

void ICC::CBusinessImpl::SendAddWeChatRequest(const PROTOCOL::CWeChatRequest& p_oWeChatInfo)
{
	PROTOCOL::CAddWeChatAlarm l_oAddWeChatAlarm;
	l_oAddWeChatAlarm.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAddWeChatAlarm.m_oHeader.m_strCmd = ADD_WECHAT_ALARM;
	l_oAddWeChatAlarm.m_oHeader.m_strRequest = WECHART_ALARM_QUEUE;
	l_oAddWeChatAlarm.m_oHeader.m_strRequestType = MQTYPE_QUEUE;
	l_oAddWeChatAlarm.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	l_oAddWeChatAlarm.m_oBody.m_strGuid = p_oWeChatInfo.m_oBody.m_strTypeid;
	l_oAddWeChatAlarm.m_oBody.m_strType = WECHAT_TYPE_LJ;
	l_oAddWeChatAlarm.m_oBody.m_strAddress = p_oWeChatInfo.m_oBody.m_strCaseAddress;
	l_oAddWeChatAlarm.m_oBody.m_strCreateDate = p_oWeChatInfo.m_oBody.m_strCaseTime;
	l_oAddWeChatAlarm.m_oBody.m_strUserName = p_oWeChatInfo.m_oBody.m_strContact;
	l_oAddWeChatAlarm.m_oBody.m_strUserPhone = p_oWeChatInfo.m_oBody.m_strContactNo;
	l_oAddWeChatAlarm.m_oBody.m_strUserAddress = p_oWeChatInfo.m_oBody.m_strContactAddress;
	l_oAddWeChatAlarm.m_oBody.m_strLong = p_oWeChatInfo.m_oBody.m_strLongitude;
	l_oAddWeChatAlarm.m_oBody.m_strLat = p_oWeChatInfo.m_oBody.m_strLatitude;

	std::vector<PROTOCOL::CMaterial> l_vMaterial; 
	PROTOCOL::CMaterial l_oMaterial;
	for (std::string var : p_oWeChatInfo.m_oBody.m_vecimagelist)
	{
		l_oMaterial.m_attachtype = std::to_string(ChType_Image);
		l_oMaterial.m_strattachpath = var;
		l_vMaterial.push_back(l_oMaterial);
	}
	for (std::string var : p_oWeChatInfo.m_oBody.m_vecrecordlist)
	{
		l_oMaterial.m_attachtype = std::to_string(ChType_Record);
		l_oMaterial.m_strattachpath = var;
		l_vMaterial.push_back(l_oMaterial);
	}
	for (std::string var : p_oWeChatInfo.m_oBody.m_vecvideolist)
	{
		l_oMaterial.m_attachtype = std::to_string(ChType_Video);
		l_oMaterial.m_strattachpath = var;
		l_vMaterial.push_back(l_oMaterial);
	}
	l_oAddWeChatAlarm.m_oBody.m_oMaterial = l_vMaterial;

	std::string l_strMsg = l_oAddWeChatAlarm.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

bool CBusinessImpl::WeChatScanFilesWithFilter(const std::string& p_strPath, 
											  const std::string& p_strFilter, 
										      std::vector<std::string>& p_vFileList)
{
	namespace fs = boost::filesystem;
	fs::path l_oPath(p_strPath);
	if (!boost::filesystem::exists(l_oPath))
	{
		return false;
	}
	fs::directory_iterator end_iter;
	for (fs::directory_iterator iter(l_oPath); iter != end_iter; ++iter)
	{
		try
		{
			if (fs::is_regular_file(iter->status()))
			{
				if (iter->path().extension().string() == p_strFilter)
				{
					p_vFileList.push_back(iter->path().string());
				}
			}
			if (fs::is_directory(iter->status()))
			{
				WeChatScanFilesWithFilter(iter->path().string(), p_strFilter, p_vFileList);
			}
		}
		catch (const fs::filesystem_error &ex)
		{
			ICC_LOG_ERROR(m_pLog, "get file name[%s] error[%s]", ex.path1().string().c_str(), ex.what());
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::WeChatChangeFileExtension(std::string p_strFileName)
{
	namespace fs = boost::filesystem;
	fs::path l_oPath(p_strFileName);
	if (!fs::exists(l_oPath))
	{
		return false;
	}
	try
	{
		fs::path l_oDstPath = fs::change_extension(l_oPath, WECHAT_REQUEST_FILE_BAK);
		fs::copy_file(l_oPath, l_oDstPath);
		fs::remove(l_oPath);
	}
	catch (const fs::filesystem_error &ex)
	{
		ICC_LOG_ERROR(m_pLog, "get file name[%s] error[%s]", ex.path1().string().c_str(), ex.what());
		return false;
	}
	return true;
}

bool CBusinessImpl::WeChatGenerateRespFile(std::string p_strRespMsg)
{
	namespace fs = boost::filesystem;
	fs::path l_oPath(m_strFilePath);
	try
	{
		l_oPath = l_oPath / WECHAT_RESPEND_DIR;
		if (!fs::exists(l_oPath))
		{
			fs::create_directory(l_oPath);
		}
		l_oPath = l_oPath / (m_pString->CreateGuid() + WECHAT_RESPEND_FILE_EXTEN);
		fs::save_string_file(l_oPath, p_strRespMsg);
	}
	catch (fs::filesystem_error& ex)
	{
		ICC_LOG_ERROR(m_pLog, "save respond file name[%s] error[%s]", ex.path1().string().c_str(), ex.what());
		return false;
	}
	catch (std::exception& ex)
	{
		ICC_LOG_ERROR(m_pLog, "save respond file exception error[%s]", ex.what());
		return false;
	}

	return true;
}

bool CBusinessImpl::SelectThirdAlarmByGuid(std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_ICC_T_THIRD_ALARM;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_ICC_T_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		return false;
	}
	return true;
}


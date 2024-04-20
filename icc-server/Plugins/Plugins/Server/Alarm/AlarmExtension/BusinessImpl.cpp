#include "Boost.h"
#include "BusinessImpl.h"

using namespace std;

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pLockFty = ICCGetILockFactory();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/AlarmExtension/IsUsing", "1");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_plan_request", OnCNotifiAddOrUpdateAlarmPlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_plan_request", OnCNotifiDeleteAlarmPlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "enable_alarm_plan_request", OnCNotifiEnableAlarmPlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "save_alarm_plan_request", OnCNotifiSaveAlarmPlanRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_plan_request", OnCNotifiSearchAlarmPlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_commonuse_plan_request", OnCNotifiSearchCommonUsePlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_plan_list_request", OnCNotifiSearchPlanListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_recommend_plan_request", OnCNotifiSearchRecommendPlanRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_plan_info_request", OnCNotifiSearchPlanInfoRequest);

	ICC_LOG_DEBUG(m_pLog, "alarmplan start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "alarmplan stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiAddOrUpdateAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiAddOrUpdateAlarmPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  add_alarm_plan_request
	PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiAlarmPlanResponse l_oRespond;

	if (l_oRequest.m_oBody.m_strbaseId.empty())
	{
		// add
		AddAlarmPlan(l_oRequest, l_oRespond);
	}
	else
	{
		// update
		UpdateAlarmPlan(l_oRequest, l_oRespond);
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}


void CBusinessImpl::OnCNotifiDeleteAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiDeleteAlarmPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiDeleteAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiAlarmPlanResponse l_oRespond;

	if (!l_oRequest.m_oBody.m_vecbaseIds.empty())
	{
		DeleteAlarmPlan(l_oRequest, l_oRespond);
	}
	else
	{
		l_oRespond.m_oBody.m_strcode = "401";
		l_oRespond.m_oBody.m_strmessage = "baseIds empty";
	}
	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}

void CBusinessImpl::OnCNotifiEnableAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{

	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiEnableAlarmPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiEnableAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiAlarmPlanResponse l_oRespond;

	DataBase::SQLRequest l_SQLRequest;
	l_oRespond.m_oBody.m_strcode = "200";
	l_oRespond.m_oBody.m_strmessage = "success";
	if (!l_oRequest.m_oBody.m_vecbaseId.empty())
	{
		for (size_t i = 0; i < l_oRequest.m_oBody.m_vecbaseId.size(); i++)
		{
			l_SQLRequest.sql_id = "update_enable_icc_t_plan_base";
			l_SQLRequest.param["id"] = l_oRequest.m_oBody.m_vecbaseId[i];

			l_SQLRequest.set["enable_code"] = l_oRequest.m_oBody.m_strenableCode;

			l_SQLRequest.set["enable_name"] = l_oRequest.m_oBody.m_strenableName;

			DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

			if (!l_Result->IsValid())
			{
				l_oRespond.m_oBody.m_strcode = "401";
				l_oRespond.m_oBody.m_strmessage = "update_enable_icc_t_plan_base failed";
				ICC_LOG_ERROR(m_pLog, "update_enable_icc_t_plan_base id[%s],error msg:[%s]", l_oRequest.m_oBody.m_vecbaseId[i].c_str(), l_Result->GetErrorMsg().c_str());
				break;
			}

			l_oRespond.m_oBody.m_vecdata.push_back(l_oRequest.m_oBody.m_vecbaseId[i]);
		}
	}
	else
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "baseId empty";
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}

void CBusinessImpl::OnCNotifiSaveAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSaveAlarmPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiSaveAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	// 入库
	PROTOCOL::CNotifiSaveAlarmPlanResponse l_oRespond;

	do
	{
		if (l_oRequest.m_oBody.m_strId.empty())
		{
			if (!SaveAddAlarmPlan(l_oRequest, l_oRespond))
			{
				break;
			}
		}
		else
		{
			if (!SaveUpdateAddAlarmPlan(l_oRequest, l_oRespond))
			{
				break;
			}
		}

		if (!SearchSaveAlarmPlan(l_oRequest, l_oRespond))
		{
			l_oRespond.m_oBody.m_strcode = "404";
			l_oRespond.m_oBody.m_strmessage = "Search failed";
		}

	} while (false);

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}

void CBusinessImpl::OnCNotifiSearchAlarmPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSearchAlarmPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiSearchAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	
	//回复
	PROTOCOL::CNotifiSearchAlarmPlanResponse l_oRespond;

	if (!l_oRequest.m_oBody.m_stralarmId.empty())
	{
		SearchAlarmPlan(l_oRequest, l_oRespond);
	}
	else
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "failed，alarmId empty";
		
	}
	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);

}

void CBusinessImpl::OnCNotifiSearchCommonUsePlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSearchCommonUsePlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiSearchCommonUsePlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiSearchCommonUsePlanResponse l_oRespond;

	SearchAlarmCommonUsePlan(l_oRequest, l_oRespond);

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}

void CBusinessImpl::OnCNotifiSearchPlanListRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSearchPlanListRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiSearchPlanListRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiSearchPlanListResponse l_oRespond;

	SearchAlarmPlanList(l_oRequest, l_oRespond);

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);

}

void CBusinessImpl::OnCNotifiSearchRecommendPlanRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSearchRecommendPlanRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息   
	PROTOCOL::CNotifiSearchPlanListRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiSearchCommonUsePlanResponse l_oRespond;

	SearchAlarmRecommendPlan(l_oRequest, l_oRespond);

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);

}

void CBusinessImpl::OnCNotifiSearchPlanInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "AlarmExtension OnCNotifiSearchPlanInfoRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiSearchPlanInfoRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiSearchPlanInfoResponse l_oRespond;
	if (!l_oRequest.m_oBody.m_strbaseId.empty())
	{
		SearchAlarmPlanInfo(l_oRequest, l_oRespond);
	}
	else
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "baseId empty";
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
}

void CBusinessImpl::AddAlarmPlan(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse)
{
	std::string strTransGuid;
	bool bSuccess = false;

	std::string strBasetId = m_pString->CreateGuid();
	strTransGuid = m_pDBConn->BeginTransaction();
	do
	{
		if (strTransGuid.empty())
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "connection failed";
			break;
		}

		//icc_t_plan_base
		if (!InsertPlanBaseInfo(p_pRequest, strBasetId, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "insert insert_icc_t_plan_base failed";
			break;
		}

		//icc_t_plan_detail
		if (!InsertPlanDetail(p_pRequest, strBasetId, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "403";
			p_pResponse.m_oBody.m_strmessage = "insert insert_icc_t_plan_detail failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
		p_pResponse.m_oBody.m_vecdata.push_back(strBasetId);
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}
}

void CBusinessImpl::UpdateAlarmPlan(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse)
{
	std::string strTransGuid;
	bool bSuccess = false;
	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();
		if (strTransGuid.empty())
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "connection failed";
			break;
		}
	
		if (!UpdatePlanBaseInfo(p_pRequest, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "update update_icc_t_plan_base failed";
			break;
		}

		if (!UpdatePlanDetail(p_pRequest, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "403";
			p_pResponse.m_oBody.m_strmessage = "update update_icc_t_plan_detail failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
		p_pResponse.m_oBody.m_vecdata.push_back(p_pRequest.m_oBody.m_strbaseId);
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}
}

void CBusinessImpl::DeleteAlarmPlan(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiAlarmPlanResponse& p_pResponse)
{
	std::string strTransGuid;
	bool bSuccess = false;
	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();
		if (strTransGuid.empty())
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "connection failed";
			break;
		}

		if (!DeletePlanBaseInfo(p_pRequest, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "delete delete_icc_t_plan_base failed";
			break;
		}

		if (!DeletePlanDetail(p_pRequest, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "403";
			p_pResponse.m_oBody.m_strmessage = "delete delete_icc_t_plan_detail failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecbaseIds.size(); ++i)
		{
			p_pResponse.m_oBody.m_vecdata.push_back(p_pRequest.m_oBody.m_vecbaseIds[i]);
		}
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}
}

bool CBusinessImpl::SaveAddAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse)
{
	std::string strTransGuid;
	bool bSuccess = false;

	strTransGuid = m_pDBConn->BeginTransaction();
	do
	{
		if (strTransGuid.empty())
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "connection failed";
			break;
		}

		//icc_t_plan_data
		std::string strguid = m_pString->CreateGuid();
		if (!InsertPlandata(p_pRequest, strguid, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "insert insert_icc_t_plan_data failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}

bool CBusinessImpl::SaveUpdateAddAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse)
{
	std::string strTransGuid;
	bool bSuccess = false;
	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();
		if (strTransGuid.empty())
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "get connection failed";
			break;
		}

		if (!UpdatePlandata(p_pRequest, strTransGuid))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "update update_icc_t_plan_data failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}

void CBusinessImpl::SearchAlarmPlan(const PROTOCOL::CNotifiSearchAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSearchAlarmPlanResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "search_icc_t_plan_data";

	sqlReqeust.param["alarm_id"] = p_pRequest.m_oBody.m_stralarmId;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "search search_icc_t_plan_data failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return;
	}

	if (l_pRSet->Next())
	{
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
		p_pResponse.m_oBody.m_strplanId = l_pRSet->GetValue("plan_id");
		p_pResponse.m_oBody.m_stralarmId = l_pRSet->GetValue("alarm_id");
		p_pResponse.m_oBody.m_stralarmNum = l_pRSet->GetValue("alarm_num");
		p_pResponse.m_oBody.m_strkeyWord = l_pRSet->GetValue("alarm_key_word");
		p_pResponse.m_oBody.m_stralarmLevel = l_pRSet->GetValue("alarm_type");
		p_pResponse.m_oBody.m_stralarmType = l_pRSet->GetValue("alarm_level");
		p_pResponse.m_oBody.m_strplanData = l_pRSet->GetValue("plan_data");
	}
}

void CBusinessImpl::SearchAlarmPlanList(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, PROTOCOL::CNotifiSearchPlanListResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	
	std::string l_strcount;
	DataBase::IResultSetPtr l_pRSetCount;
	DataBase::IResultSetPtr l_pRSet;
	do
	{
		// 获取总数
		if (!SearchPlanList(p_pRequest, "search_icc_t_plan_base_count", sqlReqeust, l_pRSetCount))
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_plan_base_count failed";
			return;
		}

		// 每页个数
		if (!SearchPlanList(p_pRequest, "search_icc_t_plan_base", sqlReqeust, l_pRSet))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_plan_base failed";
			return;
		}

	} while (false);

	if (l_pRSetCount->Next())
	{
		// 总数
		l_strcount = l_pRSetCount->GetValue("count");
	}
    
	int l_isize = m_pString->ToInt(p_pRequest.m_oBody.m_strsize);
	int l_icurrent = m_pString->ToInt(p_pRequest.m_oBody.m_strcurrent);
	int l_icount = m_pString->ToInt(l_strcount);
	
	if (l_isize <= 0)
	{
		l_isize = 10;
	}

	if (l_icurrent < 1)
	{
		l_icurrent = 1;
	}

    if (l_pRSet->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_pRSet->RecordSize();
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";
		p_pResponse.m_oBody.m_data.m_strsize = to_string(l_isize);
		p_pResponse.m_oBody.m_data.m_strcurrent = to_string(l_icurrent);

		p_pResponse.m_oBody.m_data.m_strpages = std::to_string((l_icount / l_isize) + 1); 

		p_pResponse.m_oBody.m_data.m_strtotal = l_strcount; 
		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_data.m_records.m_strbaseId = l_pRSet->GetValue(index, "id");
			p_pResponse.m_oBody.m_data.m_records.m_strenableCode = l_pRSet->GetValue(index, "enable_code");
			p_pResponse.m_oBody.m_data.m_records.m_strenableName = l_pRSet->GetValue(index, "enable_name");
			p_pResponse.m_oBody.m_data.m_records.m_strkeyWord = l_pRSet->GetValue(index, "key_word");
			p_pResponse.m_oBody.m_data.m_records.m_strplanName = l_pRSet->GetValue(index, "name");
			std::vector <std::string>l_vecalarmLevelNames = StrToArray(l_pRSet->GetValue(index, "alarm_level_name"));
			for (std::string l_strCondition : l_vecalarmLevelNames)
			{
				p_pResponse.m_oBody.m_data.m_records.m_vecalarmLevelNames.push_back(l_strCondition);
			}
			
			std::vector <std::string>l_vecalarmTipTitles = StrToArray(l_pRSet->GetValue(index, "alarm_tip_titles"), ",;");
			for (std::string l_strCondition : l_vecalarmTipTitles)
			{
				p_pResponse.m_oBody.m_data.m_records.m_vecalarmTipTitles.push_back(l_strCondition);
			}

			std::vector <std::string>l_vecalarmTypeNames = StrToArray(l_pRSet->GetValue(index, "alarm_type_name"));
			for (std::string l_strCondition : l_vecalarmTypeNames)
			{
				p_pResponse.m_oBody.m_data.m_records.m_vecalarmTypeNames.push_back(l_strCondition);
			}
			p_pResponse.m_oBody.m_data.m_vecrecords.push_back(p_pResponse.m_oBody.m_data.m_records);
			p_pResponse.m_oBody.m_data.m_records.m_vecalarmLevelNames.clear();
			p_pResponse.m_oBody.m_data.m_records.m_vecalarmTypeNames.clear();
		}
	}
}

bool CBusinessImpl::InsertPlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "insert_icc_t_plan_base";

	sqlReqeust.param["id"] = strbaseId;
	sqlReqeust.param["name"] = p_pRequest.m_oBody.m_strplantName;
	sqlReqeust.param["key_word"] = p_pRequest.m_oBody.m_strkeyWord;
	sqlReqeust.param["alarm_type_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmTypes, "code");
	sqlReqeust.param["alarm_type_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmTypes, "name");
	sqlReqeust.param["alarm_level_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLevels, "code");
	sqlReqeust.param["alarm_level_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLevels, "name");
	sqlReqeust.param["alarm_label_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLabels, "code");
	sqlReqeust.param["alarm_label_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLabels, "name");
	sqlReqeust.param["enable_code"] = p_pRequest.m_oBody.m_strenableCode;
	sqlReqeust.param["enable_name"] = p_pRequest.m_oBody.m_strenableName;
	sqlReqeust.param["create_user_id"] = p_pRequest.m_oBody.m_strcreateUserId;
	sqlReqeust.param["create_user_name"] = p_pRequest.m_oBody.m_strcreateUserName;
	sqlReqeust.param["create_org_code"] = p_pRequest.m_oBody.m_strcreateOrgCode;
	sqlReqeust.param["create_org_name"] = p_pRequest.m_oBody.m_strcreateOrgName;
	sqlReqeust.param["create_time"] = p_pRequest.m_oBody.m_strcreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strcreateTime;
	
	sqlReqeust.param["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
	sqlReqeust.param["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
	sqlReqeust.param["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
	sqlReqeust.param["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
	sqlReqeust.param["update_time"] = p_pRequest.m_oBody.m_strupdatetime;
	sqlReqeust.param["validity"] = p_pRequest.m_oBody.m_strvalidity;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_plan_base failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::InsertPlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strbaseId, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "insert_icc_t_plan_detail";
	if (!p_pRequest.m_oBody.m_vecdetails.empty())
	{
		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecdetails.size(); ++i)
		{
			sqlReqeust.param["id"] = m_pString->CreateGuid();
			sqlReqeust.param["base_id"] = strbaseId;
			sqlReqeust.param["type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strtype;
			sqlReqeust.param["title"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strtitle;
			sqlReqeust.param["data"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdata;
			sqlReqeust.param["view_type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strviewType;
			sqlReqeust.param["data_type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataType;
			sqlReqeust.param["config_note_prefix"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNotePre;
			sqlReqeust.param["config_note_suffix"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNoteSuf;
			sqlReqeust.param["data_valid"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataValid;
			sqlReqeust.param["data_view"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataView;
			sqlReqeust.param["sort"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strsort;
			sqlReqeust.param["create_user_id"] = p_pRequest.m_oBody.m_strcreateUserId;
			sqlReqeust.param["create_user_name"] = p_pRequest.m_oBody.m_strcreateUserName;
			sqlReqeust.param["create_org_code"] = p_pRequest.m_oBody.m_strcreateOrgCode;
			sqlReqeust.param["create_org_name"] = p_pRequest.m_oBody.m_strcreateOrgName;
			sqlReqeust.param["create_time"] = p_pRequest.m_oBody.m_strcreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strcreateTime;
			
			sqlReqeust.param["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
			sqlReqeust.param["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
			sqlReqeust.param["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
			sqlReqeust.param["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
			sqlReqeust.param["update_time"] = p_pRequest.m_oBody.m_strupdatetime;

			sqlReqeust.param["validity"] = p_pRequest.m_oBody.m_strvalidity;

			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_plan_base failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
				return false;
			}
		}
	}
	return true;
}

bool CBusinessImpl::InsertPlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strguid, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "insert_icc_t_plan_data";

	sqlReqeust.param["id"] = strguid;
	sqlReqeust.param["plan_id"] = p_pRequest.m_oBody.m_strplanId;
	sqlReqeust.param["alarm_id"] = p_pRequest.m_oBody.m_stralarmId;
	sqlReqeust.param["alarm_num"] = p_pRequest.m_oBody.m_stralarmNum;

	sqlReqeust.param["alarm_key_word"] = p_pRequest.m_oBody.m_strkeyWord;
	sqlReqeust.param["alarm_type"] = p_pRequest.m_oBody.m_stralarmType;
	sqlReqeust.param["alarm_level"] = p_pRequest.m_oBody.m_stralarmLevel;
	sqlReqeust.param["plan_data"] = p_pRequest.m_oBody.m_strplanData;

	sqlReqeust.param["create_user_id"] = p_pRequest.m_oBody.m_strcreateUserId;
	sqlReqeust.param["create_user_name"] = p_pRequest.m_oBody.m_strcreateUserName;
	sqlReqeust.param["create_org_code"] = p_pRequest.m_oBody.m_strcreateOrgCode;
	sqlReqeust.param["create_org_name"] = p_pRequest.m_oBody.m_strcreateOrgName;
	sqlReqeust.param["create_time"] = p_pRequest.m_oBody.m_strcreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strcreateTime;
	
	sqlReqeust.param["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
	sqlReqeust.param["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
	sqlReqeust.param["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
	sqlReqeust.param["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
	sqlReqeust.param["update_time"] = p_pRequest.m_oBody.m_strupdatetime;

	sqlReqeust.param["validity"] = p_pRequest.m_oBody.m_strvalidity;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_plan_base failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::SearchSaveAlarmPlan(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, PROTOCOL::CNotifiSaveAlarmPlanResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "search_icc_t_plan_data";

	sqlReqeust.param["alarm_id"] = p_pRequest.m_oBody.m_stralarmId;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "search search_icc_t_plan_data failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	if (l_pRSet->Next())
	{
		p_pResponse.m_oBody.m_strId = l_pRSet->GetValue("id");
		p_pResponse.m_oBody.m_strplanId = l_pRSet->GetValue("plan_id");
		p_pResponse.m_oBody.m_stralarmId = l_pRSet->GetValue("alarm_id");
		p_pResponse.m_oBody.m_stralarmNum = l_pRSet->GetValue("alarm_num");
		p_pResponse.m_oBody.m_strplanData = l_pRSet->GetValue("plan_data");
		p_pResponse.m_oBody.m_stralarmType = l_pRSet->GetValue("alarm_type");
		p_pResponse.m_oBody.m_stralarmLevel = l_pRSet->GetValue("alarm_level");
		p_pResponse.m_oBody.m_strkeyWord = l_pRSet->GetValue("alarm_key_word");

		p_pResponse.m_oBody.m_strcreateUserId = l_pRSet->GetValue("create_user_id");
		p_pResponse.m_oBody.m_strcreateUserName = l_pRSet->GetValue("create_user_name");
		p_pResponse.m_oBody.m_strcreateOrgCode = l_pRSet->GetValue("create_org_code");
		p_pResponse.m_oBody.m_strcreateOrgName = l_pRSet->GetValue("create_org_name");
		p_pResponse.m_oBody.m_strcreateTime = l_pRSet->GetValue("create_time");

		p_pResponse.m_oBody.m_strupdateUserId = l_pRSet->GetValue("update_user_id");
		p_pResponse.m_oBody.m_strupdateUserName = l_pRSet->GetValue("update_user_name");

		p_pResponse.m_oBody.m_strupdateOrgCode = l_pRSet->GetValue("update_org_code");
		p_pResponse.m_oBody.m_strupdateOrgName = l_pRSet->GetValue("update_org_name");
		p_pResponse.m_oBody.m_strupdatetime = l_pRSet->GetValue("update_time");
		p_pResponse.m_oBody.m_strvalidity = l_pRSet->GetValue("validity");
	}
	p_pResponse.m_oBody.m_strcode = "200";
	p_pResponse.m_oBody.m_strmessage = "success";
	return true;
}

bool CBusinessImpl::UpdatePlanBaseInfo(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "update_icc_t_plan_base";

	sqlReqeust.param["id"] = p_pRequest.m_oBody.m_strbaseId;

	if (!p_pRequest.m_oBody.m_strplantName.empty())
	{
		sqlReqeust.set["name"] = p_pRequest.m_oBody.m_strplantName;
	}
	if (!p_pRequest.m_oBody.m_strkeyWord.empty())
	{
		sqlReqeust.set["key_word"] = p_pRequest.m_oBody.m_strkeyWord;
	}
	if (!p_pRequest.m_oBody.m_vecalarmTypes.empty())
	{
		sqlReqeust.set["alarm_type_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmTypes, "code");
		sqlReqeust.set["alarm_type_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmTypes, "name");
	}

	if (!p_pRequest.m_oBody.m_vecalarmLevels.empty())
	{
		sqlReqeust.set["alarm_level_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLevels, "code");
		sqlReqeust.set["alarm_level_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLevels, "name");
	}
	if (!p_pRequest.m_oBody.m_vecalarmLabels.empty())
	{
		sqlReqeust.set["alarm_label_code"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLabels, "code");
		sqlReqeust.set["alarm_label_name"] = ArrayToStr(p_pRequest.m_oBody.m_vecalarmLabels, "name");
	}
	if (!p_pRequest.m_oBody.m_strenableCode.empty())
	{
		sqlReqeust.set["enable_code"] = p_pRequest.m_oBody.m_strenableCode;
	}
	if (!p_pRequest.m_oBody.m_strenableName.empty())
	{
		sqlReqeust.set["enable_name"] = p_pRequest.m_oBody.m_strenableName;
	}
	sqlReqeust.set["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
	sqlReqeust.set["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
	sqlReqeust.set["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
	sqlReqeust.set["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
	sqlReqeust.set["update_time"] = p_pRequest.m_oBody.m_strupdatetime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strupdatetime;

	if (!p_pRequest.m_oBody.m_strvalidity.empty())
	{
		sqlReqeust.set["validity"] = p_pRequest.m_oBody.m_strvalidity;
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update update_icc_t_plan_base failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::UpdatePlanDetail(const PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "update_icc_t_plan_detail";

	for (size_t i = 0; i < p_pRequest.m_oBody.m_vecdetails.size(); ++i)
	{
		sqlReqeust.param["id"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdetailId;

		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strtype.empty())
		{
			sqlReqeust.set["type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strtype;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strtitle.empty())
		{
			sqlReqeust.set["title"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strtitle;
		}

		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strdata.empty())
		{
			sqlReqeust.set["data"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdata;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strviewType.empty())
		{
			sqlReqeust.set["view_type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strviewType;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataType.empty())
		{
			sqlReqeust.set["data_type"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataType;
		}

		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNotePre.empty())
		{
			sqlReqeust.set["config_note_prefix"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNotePre;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNoteSuf.empty())
		{
			sqlReqeust.set["config_note_suffix"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strconfigNoteSuf;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataValid.empty())
		{
			sqlReqeust.set["data_valid"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataValid;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataView.empty())
		{
			sqlReqeust.set["data_view"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strdataView;
		}
		if (!p_pRequest.m_oBody.m_vecdetails.at(i).m_strsort.empty())
		{
			sqlReqeust.set["sort"] = p_pRequest.m_oBody.m_vecdetails.at(i).m_strsort;
		}

		sqlReqeust.set["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
		sqlReqeust.set["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
		sqlReqeust.set["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
		sqlReqeust.set["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
		sqlReqeust.set["update_time"] = p_pRequest.m_oBody.m_strupdatetime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strupdatetime;
		
		if (!p_pRequest.m_oBody.m_strvalidity.empty())
		{
			sqlReqeust.set["validity"] = p_pRequest.m_oBody.m_strvalidity;
		}
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update update_icc_t_plan_detail failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::UpdatePlandata(const PROTOCOL::CNotifiSaveAlarmPlanRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "update_icc_t_plan_data";

	sqlReqeust.param["id"] = p_pRequest.m_oBody.m_strId;
	sqlReqeust.param["alarm_id"] = p_pRequest.m_oBody.m_stralarmId;

	if (!p_pRequest.m_oBody.m_strplanId.empty())
	{
		sqlReqeust.set["plan_id"] = p_pRequest.m_oBody.m_strplanId;
	}

	if (!p_pRequest.m_oBody.m_stralarmNum.empty())
	{
		sqlReqeust.set["alarm_num"] = p_pRequest.m_oBody.m_stralarmNum;
	}

	if (!p_pRequest.m_oBody.m_strkeyWord.empty())
	{
		sqlReqeust.set["alarm_key_word"] = p_pRequest.m_oBody.m_strkeyWord;
	}

	if (!p_pRequest.m_oBody.m_stralarmNum.empty())
	{
		sqlReqeust.set["alarm_num"] = p_pRequest.m_oBody.m_stralarmNum;
	}

	if (!p_pRequest.m_oBody.m_stralarmType.empty())
	{
		sqlReqeust.set["alarm_type"] = p_pRequest.m_oBody.m_stralarmType;
	}

	if (!p_pRequest.m_oBody.m_stralarmLevel.empty())
	{
		sqlReqeust.set["alarm_level"] = p_pRequest.m_oBody.m_stralarmLevel;
	}

	if (!p_pRequest.m_oBody.m_strplanData.empty())
	{
		sqlReqeust.set["plan_data"] = p_pRequest.m_oBody.m_strplanData;
	}

	sqlReqeust.set["update_user_id"] = p_pRequest.m_oBody.m_strupdateUserId;
	sqlReqeust.set["update_user_name"] = p_pRequest.m_oBody.m_strupdateUserName;
	sqlReqeust.set["update_org_code"] = p_pRequest.m_oBody.m_strupdateOrgCode;
	sqlReqeust.set["update_org_name"] = p_pRequest.m_oBody.m_strupdateOrgName;
	sqlReqeust.set["update_time"] = p_pRequest.m_oBody.m_strupdatetime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strupdatetime;

	if (!p_pRequest.m_oBody.m_strvalidity.empty())
	{
		sqlReqeust.set["validity"] = p_pRequest.m_oBody.m_strvalidity;
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update update_icc_t_plan_data failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::DeletePlanBaseInfo(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;

	if (!p_pRequest.m_oBody.m_vecbaseIds.empty())
	{
		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecbaseIds.size(); i++)
		{
			l_SQLRequest.sql_id = "delete_icc_t_plan_base";
			l_SQLRequest.param["id"] = p_pRequest.m_oBody.m_vecbaseIds[i];

			DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

			if (!l_Result->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "delete_icc_t_plan_base id[%s],error msg:[%s]", p_pRequest.m_oBody.m_vecbaseIds[i].c_str(), l_Result->GetErrorMsg().c_str());
				return false;
			}
		}
	}

	return true;
}

bool CBusinessImpl::DeletePlanDetail(const PROTOCOL::CNotifiDeleteAlarmPlanRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;

	if (!p_pRequest.m_oBody.m_vecbaseIds.empty())
	{
		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecbaseIds.size(); i++)
		{
			l_SQLRequest.sql_id = "delete_icc_t_plan_detail";
			l_SQLRequest.param["base_id"] = p_pRequest.m_oBody.m_vecbaseIds[i];

			DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

			if (!l_Result->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "delete icc_t_plan_detail id[%s],error msg:[%s]", p_pRequest.m_oBody.m_vecbaseIds[i].c_str(), l_Result->GetErrorMsg().c_str());
				return false;
			}
		}
	}

	return true;
}

bool CBusinessImpl::SearchPlanList(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest sqlReqeust, DataBase::IResultSetPtr& p_pRSet)
{
	sqlReqeust.sql_id = p_pSql_id;

	if (!p_pRequest.m_oBody.m_strkeyword.empty())
	{
		sqlReqeust.param["key_word"] = p_pRequest.m_oBody.m_strkeyword;
	}

	if (!p_pRequest.m_oBody.m_strenableCode.empty())
	{
		sqlReqeust.param["enable_code"] = p_pRequest.m_oBody.m_strenableCode;
	}

	if (!p_pRequest.m_oBody.m_vecalarmLevelCodes.empty())
	{
		sqlReqeust.param["alarm_level_code"] = BuildMultiConditions("alarm_level_code", "like", p_pRequest.m_oBody.m_vecalarmLevelCodes);
	}

	if (!p_pRequest.m_oBody.m_vecalarmTypeCodes.empty())
	{
		sqlReqeust.param["alarm_type_code"] = BuildMultiConditions("alarm_type_code", "like", p_pRequest.m_oBody.m_vecalarmTypeCodes);
	}

	if (0 == p_pSql_id.compare("search_icc_t_plan_base"))
	{
		int l_isize = m_pString->ToInt(p_pRequest.m_oBody.m_strsize);

		int l_icurrent = m_pString->ToInt(p_pRequest.m_oBody.m_strcurrent);

		if (l_isize <= 0)
		{
			l_isize = 10;
		}

		if (l_icurrent < 1)
		{
			l_icurrent = 1;
		}

		sqlReqeust.param["limit"] = to_string(l_isize);

		sqlReqeust.param["offset"] = to_string(l_icurrent - 1);
	}

	p_pRSet = m_pDBConn->Exec(sqlReqeust, true);
	ICC_LOG_INFO(m_pLog, "query sql:[%s]", p_pRSet->GetSQL().c_str());

	if (!p_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", p_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::SearchPlanInfo(const PROTOCOL::CNotifiSearchPlanInfoRequest& p_pRequest, std::string p_pSql_id, DataBase::SQLRequest sqlReqeust, DataBase::IResultSetPtr& p_pRSet)
{
	sqlReqeust.sql_id = p_pSql_id;

	if (0 == p_pSql_id.compare("search_icc_t_plan_detail_info"))
	{
		sqlReqeust.param["base_id"] = p_pRequest.m_oBody.m_strbaseId;
	}
	else
	{
		sqlReqeust.param["id"] = p_pRequest.m_oBody.m_strbaseId;
	}	

	p_pRSet = m_pDBConn->Exec(sqlReqeust, true);

	ICC_LOG_INFO(m_pLog, "query sql:[%s]", p_pRSet->GetSQL().c_str());

	if (!p_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", p_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

void CBusinessImpl::SearchAlarmPlanInfo(const PROTOCOL::CNotifiSearchPlanInfoRequest& p_pRequest, PROTOCOL::CNotifiSearchPlanInfoResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	DataBase::IResultSetPtr l_pRSetbase;
	DataBase::IResultSetPtr l_pRSetdetail;
	do
	{
		if (!SearchPlanInfo(p_pRequest, "search_icc_t_plan_base_info", sqlReqeust, l_pRSetbase))
		{
			p_pResponse.m_oBody.m_strcode = "401";
			p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_plan_base_info failed";
			return;
		}

		if (!SearchPlanInfo(p_pRequest, "search_icc_t_plan_detail_info", sqlReqeust, l_pRSetdetail))
		{
			p_pResponse.m_oBody.m_strcode = "402";
			p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_plan_detail_info failed";
			return;
		}

	} while (false);

	if (l_pRSetbase->Next())
	{
		p_pResponse.m_oBody.m_strkeyWord = l_pRSetbase->GetValue("key_word");
		p_pResponse.m_oBody.m_strplantName = l_pRSetbase->GetValue("name");
		p_pResponse.m_oBody.m_strbaseId = l_pRSetbase->GetValue("id");
		std::vector <std::string>l_vecalarmTypeCode = StrToArray(l_pRSetbase->GetValue("alarm_type_code"));
		std::vector <std::string>l_vecalarmTypeName = StrToArray(l_pRSetbase->GetValue("alarm_type_name"));

		for (size_t i = 0; i < l_vecalarmTypeCode.size(); i++)
		{
			p_pResponse.m_oBody.m_keyValuePair.code = l_vecalarmTypeCode[i];
			p_pResponse.m_oBody.m_keyValuePair.name = l_vecalarmTypeName[i];
			p_pResponse.m_oBody.m_vecalarmTypes.push_back(p_pResponse.m_oBody.m_keyValuePair);
		}

		std::vector <std::string>l_vecalarmLevelCode = StrToArray(l_pRSetbase->GetValue("alarm_level_code"));
		std::vector <std::string>l_vecalarmLevelName = StrToArray(l_pRSetbase->GetValue("alarm_level_name"));

		for (size_t i = 0; i < l_vecalarmLevelCode.size(); i++)
		{
			p_pResponse.m_oBody.m_keyValuePair.code = l_vecalarmLevelCode[i];
			p_pResponse.m_oBody.m_keyValuePair.name = l_vecalarmLevelName[i];
			p_pResponse.m_oBody.m_vecalarmLevels.push_back(p_pResponse.m_oBody.m_keyValuePair);
		}

		std::vector <std::string>l_vecalarmLabelCode = StrToArray(l_pRSetbase->GetValue("alarm_label_code"));
		std::vector <std::string>l_vecalarmLabelName = StrToArray(l_pRSetbase->GetValue("alarm_label_name"));

		for (size_t i = 0; i < l_vecalarmLabelCode.size(); i++)
		{
			p_pResponse.m_oBody.m_keyValuePair.code = l_vecalarmLabelCode[i];
			p_pResponse.m_oBody.m_keyValuePair.name = l_vecalarmLabelName[i];
			p_pResponse.m_oBody.m_vecalarmLabels.push_back(p_pResponse.m_oBody.m_keyValuePair);
		}
	}

	size_t l_iRecordSize = l_pRSetdetail->RecordSize();

	if (l_iRecordSize > 0)
	{
		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_planDetail.m_strdetailId = l_pRSetdetail->GetValue(index, "id");
			p_pResponse.m_oBody.m_planDetail.m_strconfigNotePre = l_pRSetdetail->GetValue(index, "config_note_prefix");
			p_pResponse.m_oBody.m_planDetail.m_strconfigNoteSuf = l_pRSetdetail->GetValue(index, "config_note_suffix");
			p_pResponse.m_oBody.m_planDetail.m_strdata = l_pRSetdetail->GetValue(index, "data");
			p_pResponse.m_oBody.m_planDetail.m_strdataType = l_pRSetdetail->GetValue(index, "data_type");
			p_pResponse.m_oBody.m_planDetail.m_strdataValid = l_pRSetdetail->GetValue(index, "data_valid");
			p_pResponse.m_oBody.m_planDetail.m_strdataView = l_pRSetdetail->GetValue(index, "data_view");
			p_pResponse.m_oBody.m_planDetail.m_strsort = l_pRSetdetail->GetValue(index, "sort");
			p_pResponse.m_oBody.m_planDetail.m_strtitle = l_pRSetdetail->GetValue(index, "title");
			p_pResponse.m_oBody.m_planDetail.m_strtype = l_pRSetdetail->GetValue(index, "type");
			p_pResponse.m_oBody.m_planDetail.m_strviewType = l_pRSetdetail->GetValue(index, "view_type");

			p_pResponse.m_oBody.m_vecdetails.push_back(p_pResponse.m_oBody.m_planDetail);

		}
	}

	p_pResponse.m_oBody.m_strcode = "200";
	p_pResponse.m_oBody.m_strmessage = "success";
}

void CBusinessImpl::SearchAlarmCommonUsePlan(const PROTOCOL::CNotifiSearchCommonUsePlanRequest& p_pRequest, PROTOCOL::CNotifiSearchCommonUsePlanResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "search_icc_t_plan_base_info";
	sqlReqeust.param["create_org_code"] = p_pRequest.m_oBody.m_strorgCode;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(sqlReqeust);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "search_icc_t_plan_base_info id[%s],error msg:[%s]", p_pRequest.m_oBody.m_strorgCode.c_str(), l_Result->GetErrorMsg().c_str());
		return ;
	}

	size_t l_iRecordSize = l_Result->RecordSize();

	if (l_iRecordSize > 0)
	{
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";

		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_data.m_strkeyWord = l_Result->GetValue(index, "key_word");
			p_pResponse.m_oBody.m_data.m_strplanName = l_Result->GetValue(index, "name");
			p_pResponse.m_oBody.m_data.m_strbaseId = l_Result->GetValue(index, "id");

			p_pResponse.m_oBody.m_data.m_strenableCode = l_Result->GetValue(index, "enable_code");
			p_pResponse.m_oBody.m_data.m_strenableName = l_Result->GetValue(index, "enable_name");

			std::vector <std::string>l_vecalarmLevelNames = StrToArray(l_Result->GetValue(index, "alarm_level_name"));
			for (std::string l_strCondition : l_vecalarmLevelNames)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmLevelNames.push_back(l_strCondition);
			}

			std::vector <std::string>l_vecalarmTipTitles = StrToArray(l_Result->GetValue(index, "alarm_tip_titles"), ",;");
			for (std::string l_strCondition : l_vecalarmTipTitles)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmTipTitles.push_back(l_strCondition);
			}

			std::vector <std::string>l_vecalarmTypeNames = StrToArray(l_Result->GetValue(index, "alarm_type_name"));
			for (std::string l_strCondition : l_vecalarmTypeNames)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmTypeNames.push_back(l_strCondition);
			}
			p_pResponse.m_oBody.m_vecdata.push_back(p_pResponse.m_oBody.m_data);
			p_pResponse.m_oBody.m_data.m_vecalarmLevelNames.clear();
			p_pResponse.m_oBody.m_data.m_vecalarmTipTitles.clear();
			p_pResponse.m_oBody.m_data.m_vecalarmTypeNames.clear();
		}
	}
}

void CBusinessImpl::SearchAlarmRecommendPlan(const PROTOCOL::CNotifiSearchPlanListRequest& p_pRequest, PROTOCOL::CNotifiSearchCommonUsePlanResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;

	DataBase::IResultSetPtr l_Result;

	if (!SearchPlanList(p_pRequest, "search_icc_t_plan_base", sqlReqeust, l_Result))
	{
		p_pResponse.m_oBody.m_strcode = "402";
		p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_plan_base failed";
		return;
	}

	size_t l_iRecordSize = l_Result->RecordSize();

	if (l_iRecordSize > 0)
	{
		p_pResponse.m_oBody.m_strcode = "200";
		p_pResponse.m_oBody.m_strmessage = "success";

		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_data.m_strkeyWord = l_Result->GetValue(index, "key_word");
			p_pResponse.m_oBody.m_data.m_strplanName = l_Result->GetValue(index, "name");
			p_pResponse.m_oBody.m_data.m_strbaseId = l_Result->GetValue(index, "id");

			p_pResponse.m_oBody.m_data.m_strenableCode = l_Result->GetValue(index, "enable_code");
			p_pResponse.m_oBody.m_data.m_strenableName = l_Result->GetValue(index, "enable_name");

			std::vector <std::string>l_vecalarmLevelNames = StrToArray(l_Result->GetValue(index, "alarm_level_name"));
			for (std::string l_strCondition : l_vecalarmLevelNames)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmLevelNames.push_back(l_strCondition);
			}

			std::vector <std::string>l_vecalarmTipTitles = StrToArray(l_Result->GetValue(index, "alarm_tip_titles"), ",;");
			for (std::string l_strCondition : l_vecalarmTipTitles)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmTipTitles.push_back(l_strCondition);
			}

			std::vector <std::string>l_vecalarmTypeNames = StrToArray(l_Result->GetValue(index, "alarm_type_name"));
			for (std::string l_strCondition : l_vecalarmTypeNames)
			{
				p_pResponse.m_oBody.m_data.m_vecalarmTypeNames.push_back(l_strCondition);
			}
			p_pResponse.m_oBody.m_vecdata.push_back(p_pResponse.m_oBody.m_data);
			p_pResponse.m_oBody.m_data.m_vecalarmLevelNames.clear();
			p_pResponse.m_oBody.m_data.m_vecalarmTypeNames.clear();
		}
	}
}

std::string CBusinessImpl::BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::vector<std::string> p_vecCondition)
{
	std::string l_strResult;
	vector<std::string> l_vecCondition;
	for (std::string l_strCondition : p_vecCondition)
	{
		std::string l_strCondTemp;
		if (p_strOperator == "like")
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '%" + l_strCondition + "%'");
		}
		else
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '" + l_strCondition + "'");
		}
		if (!l_strResult.empty())
		{
			l_strResult.append(" or ").append(l_strCondTemp);
		}
		else
		{
			l_strResult.append(l_strCondTemp);
		}

	}

	return l_strResult;
}

std::string CBusinessImpl::ArrayToStr(std::vector<PROTOCOL::CNotifiAddOrUpdateAlarmPlanRequest::KeyValuePair> p_pKeyValuePair,std::string s)
{
	std::string str = "";
	size_t maxlen = p_pKeyValuePair.size();

	for (size_t i = 0; i < maxlen; ++i)
	{
		if (0 == s.compare("code"))
		{
			str += p_pKeyValuePair.at(i).code;
		}
		else if (0 == s.compare("name"))
		{
			str += p_pKeyValuePair.at(i).name;
		}
		else
		{
			break;
		}
		if (i != maxlen - 1)
		{
			str += ",";
		}
	}

	return str;
}

std::vector<std::string> CBusinessImpl::StrToArray(std::string s, std::string p_pSplit)
{
	vector<std::string> l_vecCondition;

	m_pString->Split(s, p_pSplit, l_vecCondition, false);
	return l_vecCondition;
}
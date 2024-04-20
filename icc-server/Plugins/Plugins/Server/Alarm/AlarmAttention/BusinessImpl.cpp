#include "Boost.h"
#include "BusinessImpl.h"
#define STAFF_ID_NO      "1"
#define STAFF_CODE       "2"

#define ALARM_STATUS_TERMINAL	 "07"    //已终结

#define ALARM_ATTENTION_INFO "AlarmAttentionInfo"

using namespace std;

string CBusinessImpl::m_strCurTime = "";

const unsigned long TIMERID_CHECKEXPIRED = 10000;


struct AttentionInfo
{
	std::string strAlarmId;
	std::string strStaffCode;
};

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);	
	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();	
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_IntervalTime = 86400;
	m_strBeginTime = "1970-01-01 08:00:00";

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

    m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_alarm_attention", OnNotifiAddAlarmAttention);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_alarm_attention", OnNotifiUpdateAlarmAttention);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_attention", OnNotifiDeleteAlarmAttention);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_attention", OnNotifiQueryAlarmAttention);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "judge_alarm_attention", OnNotifiJudgeAlarmAttention);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnNotifiReceiveAlarmSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_process_sync", OnNotifiReceiveProcessAlarmSync);

	//存Redis之前先把Redis节点删除再存
	m_pRedisClient->Del(ALARM_ATTENTION_INFO);

	m_IntervalTime = m_pString->ToInt64(m_pConfig->GetValue("ICC/Plugin/AlarmAttention/interval", "86400"));

	std::string strNormalExpired = m_pConfig->GetValue("ICC/Plugin/AlarmAttention/normalexpired", "7");
	m_lExpiredTime = m_pString->ToInt64(strNormalExpired) * 24 * 60 * 60;
	std::string strTerminalExpired = m_pConfig->GetValue("ICC/Plugin/AlarmAttention/terminalexpired", "24");
	m_lTerminalExpiredTime = m_pString->ToInt64(strTerminalExpired) * 60 * 60;
	this->OnTimer(TIMERID_CHECKEXPIRED);
	m_timerCheckExpired.AddTimer(this, TIMERID_CHECKEXPIRED, m_IntervalTime * 1000);

	ICC_LOG_INFO(m_pLog, "alarmattention start success! normal expired:%s,%lld, terminal expired:%s, %lld，AssistantPolice=%s,Police=%s,CodeMode:[%S]", strNormalExpired.c_str(), m_lExpiredTime, strTerminalExpired.c_str(), m_lTerminalExpiredTime,m_strAssistantPolice.c_str(), m_strPolice.c_str(),m_strCodeMode.c_str());
	
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "alarmattention stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiAddAlarmAttention(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received add attention: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmAttentionAddRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "insert_icc_t_alarm_attention";
	l_SQLRequest.param["alarm_id"] = l_request.m_oBody.m_alarm_id;
	l_SQLRequest.param["attention_staff"] = l_request.m_oBody.m_staff_code;
	std::string l_strAttentionTime = m_pDateTime->CurrentDateTimeStr();
	l_SQLRequest.param["attention_time"] = l_strAttentionTime;
	l_SQLRequest.param["is_update"] = "0";
	std::string l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	l_SQLRequest.param["update_time"] = l_strUpdateTime;

	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute insert_icc_t_alarm_attention failed";
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_alarm_attention failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";		
		//新增数据 添加到 Redis 中
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;
		l_AlarmAttentionInfo.m_strAlarmId = l_request.m_oBody.m_alarm_id;
		l_AlarmAttentionInfo.m_strAttentionStaff = l_request.m_oBody.m_staff_code;
		l_AlarmAttentionInfo.m_strAttentionTime = l_strAttentionTime;
		l_AlarmAttentionInfo.m_strIsUpdate = "0";
		l_AlarmAttentionInfo.m_strUpdateTime = l_strUpdateTime;
		l_AlarmAttentionInfo.m_strTerminalTime = "";
		l_AlarmAttentionInfo.m_strExpireTime = "";

		std::string m_strAlarmAttentionInfo = l_AlarmAttentionInfo.ToJson(m_pJsonFty->CreateJson());
		if (!l_AlarmAttentionInfo.m_strAlarmId.empty())
		{
			if (!m_pRedisClient->HSet(ALARM_ATTENTION_INFO, l_AlarmAttentionInfo.m_strAlarmId, m_strAlarmAttentionInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Hset ALARM_ATTENTION_INFO INFO failed");
			}
		}
	}

	//写流水
	PROTOCOL::CAlarmLogSync l_AlarmLogSync;
	PROTOCOL::AttentionNameAndDeptName response;
	_QueryStaffName(l_request.m_oBody.m_staff_code, response);

	l_AlarmLogSync.m_oBody.m_strID = m_pString->CreateGuid();
	std::vector<std::string> l_vecParamList;
	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(l_request.m_oBody.m_staff_code, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, response.m_strname);

	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(response.m_strdeptname);
	l_AlarmLogSync.m_oBody.m_strAlarmID = l_request.m_oBody.m_alarm_id;
	l_AlarmLogSync.m_oBody.m_strOperate = "BS001005013";
	l_AlarmLogSync.m_oBody.m_strOperateContent = _AlarmLogContent(l_vecParamList);
	l_AlarmLogSync.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	if (!_InsertAlarmLogInfo(l_AlarmLogSync.m_oBody))
	{
		l_oRespond.m_strResult = "2";
		l_oRespond.m_strMsg = "execute insert alarm log info failed";
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
	}
	else
	{
		_SyncAlarmLogInfo(l_AlarmLogSync.m_oBody);
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

void CBusinessImpl::OnNotifiDeleteAlarmAttention(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received delete attention: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmAttentionDeleteRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;
	if (!_DeleteAttention(l_request.m_oBody.m_alarm_id, l_request.m_oBody.m_staff_code, l_oRespond.m_strMsg))
	{
		l_oRespond.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}

	/*DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "delete_icc_t_alarm_attention_by_staff";
	l_SQLRequest.param["alarm_id"] = l_request.m_oBody.m_alarm_id;
	l_SQLRequest.param["attention_staff"] = l_request.m_oBody.m_staff_code;	

	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute delete_icc_t_alarm_attention_by_staff failed";
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_alarm_attention_by_staff failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}*/

	//写流水
	PROTOCOL::CAlarmLogSync l_AlarmLogSync;
	PROTOCOL::AttentionNameAndDeptName response;
	_QueryStaffName(l_request.m_oBody.m_staff_code, response);

	l_AlarmLogSync.m_oBody.m_strID = m_pString->CreateGuid();
	std::vector<std::string> l_vecParamList;
	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(l_request.m_oBody.m_staff_code, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, response.m_strname);

	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(response.m_strdeptname);

	l_AlarmLogSync.m_oBody.m_strOperate = "BS001005014";
	l_AlarmLogSync.m_oBody.m_strAlarmID = l_request.m_oBody.m_alarm_id;
	l_AlarmLogSync.m_oBody.m_strOperateContent = _AlarmLogContent(l_vecParamList);
	l_AlarmLogSync.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	if (!_InsertAlarmLogInfo(l_AlarmLogSync.m_oBody))
	{
		l_oRespond.m_strResult = "2";
		l_oRespond.m_strMsg = "execute insert alarm log info failed";
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
	}
	else
	{
		_SyncAlarmLogInfo(l_AlarmLogSync.m_oBody);
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

void CBusinessImpl::OnNotifiUpdateAlarmAttention(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received update attention: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmAttentionUpdateRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_icc_t_alarm_attention_by_alarm_id_and_staff";
	l_SQLRequest.param["alarm_id"] = l_request.m_oBody.m_alarm_id;
	l_SQLRequest.param["attention_staff"] = l_request.m_oBody.m_staff_code;

	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute update_icc_t_alarm_attention_by_alarm_id_and_staff failed";
		ICC_LOG_ERROR(m_pLog, "update_icc_t_alarm_attention_by_alarm_id_and_staff failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
		// 更新 Redis 中对应数据
		// 先获取，在更新
		std::string l_strAlarmAttentionInfo;
		if (!m_pRedisClient->HGet(ALARM_ATTENTION_INFO, l_request.m_oBody.m_alarm_id, l_strAlarmAttentionInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "HGet ALARM_ATTENTION_INFO Failed!!!");
			
		}
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;

		if (!l_AlarmAttentionInfo.Parse(l_strAlarmAttentionInfo, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Parse ALARM_ATTENTION_INFO failed");
		}
		l_AlarmAttentionInfo.m_strIsUpdate = "0";

		std::string m_strAlarmAttentionInfo = l_AlarmAttentionInfo.ToJson(m_pJsonFty->CreateJson());
		if (!l_AlarmAttentionInfo.m_strAlarmId.empty())
		{
			if (!m_pRedisClient->HSet(ALARM_ATTENTION_INFO, l_AlarmAttentionInfo.m_strAlarmId, m_strAlarmAttentionInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Hset ALARM_ATTENTION_INFO INFO failed");
			}
		}
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

void CBusinessImpl::OnNotifiQueryAlarmAttention(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received query attention: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmAttentionQueryRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	PROTOCOL::CAlarmAttentionQueryResponse response;
	response.m_oHeader = l_request.m_oHeader;
	DateTime::CDateTime curTime = m_pDateTime->CurrentDateTime();

	std::vector<std::string> vecDeletes;

	// 从  Redis 中  取数据

	std::map<std::string, std::string>mapAttentionInfo;
	if (!m_pRedisClient->HGetAll(ALARM_ATTENTION_INFO, mapAttentionInfo))
	{
		ICC_LOG_ERROR(m_pLog, "ALARM_ATTENTION_INFO HGetALL failed");
	}

	for (auto it = mapAttentionInfo.begin(); it != mapAttentionInfo.end(); it++)
	{
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;
		if (!l_AlarmAttentionInfo.Parse(it->second, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "ALARM_ATTENTION_INFO Parse failed");
			continue;
		}
		if (l_request.m_oBody.m_staff_code.compare(l_AlarmAttentionInfo.m_strAttentionStaff.c_str()) == 0)
		{
			if (!l_AlarmAttentionInfo.m_strTerminalTime.empty())
			{
				DateTime::CDateTime terminalTime = m_pDateTime->FromString(l_AlarmAttentionInfo.m_strTerminalTime);
				if ((curTime - terminalTime) >= m_lTerminalExpiredTime * 1000 * 1000)
				{
					vecDeletes.push_back(l_AlarmAttentionInfo.m_strAlarmId);
					continue;
				}
			}

			if (!l_AlarmAttentionInfo.m_strAttentionTime.empty())
			{
				DateTime::CDateTime attentionTime = m_pDateTime->FromString(l_AlarmAttentionInfo.m_strAttentionTime);
				if ((curTime - attentionTime) >= m_lExpiredTime * 1000 * 1000)
				{
					vecDeletes.push_back(l_AlarmAttentionInfo.m_strAlarmId);
					continue;
				}
			}

			PROTOCOL::CAlarmAttentionQueryResponse::CData data;
			data.m_alarm_id = l_AlarmAttentionInfo.m_strAlarmId;
			data.m_is_update = l_AlarmAttentionInfo.m_strIsUpdate;

			response.m_oBody.m_vecData.push_back(data);
			
		}
	}

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	response.m_oBody.m_vecData.clear();
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());

	for (size_t i = 0; i < vecDeletes.size(); ++i)
	{
		std::string strMsg;
		_DeleteAttention(vecDeletes[i], l_request.m_oBody.m_staff_code, strMsg);
	}
}

void CBusinessImpl::OnNotifiReceiveAlarmSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received alarm changed: [%s]", p_strMsg.c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmSync l_CAlarmSync;

	if (!l_CAlarmSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	DataBase::SQLRequest l_SQLRequest;

	std::string l_strTerminalTime = "";

	if (l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strState == ALARM_STATUS_TERMINAL)
	{
		l_SQLRequest.sql_id = "update_icc_t_alarm_attention_terminal_by_alarm_id";
		l_SQLRequest.param["alarm_id"] = l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strID;
		l_strTerminalTime = m_pDateTime->CurrentDateTimeStr();
		l_SQLRequest.param["terminal_time"] = l_strTerminalTime;
	}
	else
	{
		l_SQLRequest.sql_id = "update_icc_t_alarm_attention_by_alarm_id";
		l_SQLRequest.param["alarm_id"] = l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strID;
	}		

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{		
		ICC_LOG_ERROR(m_pLog, "%s failed,error msg:[%s]", l_SQLRequest.sql_id.c_str(), l_Result->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s success", l_SQLRequest.sql_id.c_str());
		
		// 更新 Redis 中对应数据
		// 先获取，在更新
		std::string l_strAlarmAttentionInfo;
		if (!m_pRedisClient->HGet(ALARM_ATTENTION_INFO, l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strID, l_strAlarmAttentionInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "HGet ALARM_ATTENTION_INFO Failed!!!");

		}
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;

		if (!l_AlarmAttentionInfo.Parse(l_strAlarmAttentionInfo, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Parse ALARM_ATTENTION_INFO failed");
		}
		l_AlarmAttentionInfo.m_strIsUpdate = "1";
		if (!l_strTerminalTime.empty())
		{
			l_AlarmAttentionInfo.m_strTerminalTime = l_strTerminalTime;
		}
		std::string m_strAlarmAttentionInfo = l_AlarmAttentionInfo.ToJson(m_pJsonFty->CreateJson());
		if (!l_AlarmAttentionInfo.m_strAlarmId.empty())
		{
			if (!m_pRedisClient->HSet(ALARM_ATTENTION_INFO, l_AlarmAttentionInfo.m_strAlarmId, m_strAlarmAttentionInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Hset ALARM_ATTENTION_INFO INFO failed");
			}
		}
	}

}

void CBusinessImpl::OnNotifiReceiveProcessAlarmSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received process changed: [%s]", p_strMsg.c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmProcessSync l_CAlarmProcessSync;

	if (!l_CAlarmProcessSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_icc_t_alarm_attention_by_alarm_id";
	l_SQLRequest.param["alarm_id"] = l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strAlarmID;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_alarm_attention_by_alarm_id failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_alarm_attention_by_alarm_id success");

		// 更新 Redis 中对应数据
		// 先获取，在更新
		std::string l_strAlarmAttentionInfo;
		if (!m_pRedisClient->HGet(ALARM_ATTENTION_INFO, l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strAlarmID, l_strAlarmAttentionInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "HGet ALARM_ATTENTION_INFO Failed!!!");

		}
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;

		if (!l_AlarmAttentionInfo.Parse(l_strAlarmAttentionInfo, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Parse ALARM_ATTENTION_INFO failed");
		}
		l_AlarmAttentionInfo.m_strIsUpdate = "1";

		std::string m_strAlarmAttentionInfo = l_AlarmAttentionInfo.ToJson(m_pJsonFty->CreateJson());
		if (!l_AlarmAttentionInfo.m_strAlarmId.empty())
		{
			if (!m_pRedisClient->HSet(ALARM_ATTENTION_INFO, l_AlarmAttentionInfo.m_strAlarmId, m_strAlarmAttentionInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Hset ALARM_ATTENTION_INFO INFO failed");
			}
		}
	}
}

bool CBusinessImpl::_DeleteAttention(const std::string& strAlarmId, const std::string& strStaffCode, std::string& strMsg)
{
	// 删除SQL中的数据
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "delete_icc_t_alarm_attention_by_staff";
	l_SQLRequest.param["alarm_id"] = strAlarmId;
	l_SQLRequest.param["attention_staff"] = strStaffCode;	

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{		
		strMsg = "execute delete_icc_t_alarm_attention_by_staff failed";
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_alarm_attention_by_staff failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}
	
	// 删除 Redis 中的数据
	if (!m_pRedisClient->HDel(ALARM_ATTENTION_INFO, strAlarmId))
	{
		ICC_LOG_ERROR(m_pLog, "redis delete failed,AlarmId is [%s]", strAlarmId.c_str());
		return false;
	}

	return true;
}

void CBusinessImpl::_CheckExpired()
{
	ICC_LOG_DEBUG(m_pLog, "_CheckExpired begin");
	std::string l_strEndTime = m_pDateTime->ToString(m_pDateTime->AddSeconds(m_pDateTime->CurrentDateTime(), -1));
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_icc_t_alarm_attention_all";
	// 添加 begin_time 和 end_time    
	// begin_time 取 上一次 检测时间， end_time 取 当前时间 -1
	l_SQLRequest.param["begin_time"] = m_strBeginTime;
	l_SQLRequest.param["end_time"] = l_strEndTime;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "_CheckExpired sql:[%s]", l_Result->GetSQL().c_str());
	m_strBeginTime = l_strEndTime;
	DateTime::CDateTime curTime = m_pDateTime->CurrentDateTime();

	std::vector<AttentionInfo> vecDeletes;
	
	while (l_Result->Next())
	{
		// 查询是来的数据，添加到 Redis 中 
		PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;
		l_AlarmAttentionInfo.m_strAlarmId = l_Result->GetValue("alarm_id");
		l_AlarmAttentionInfo.m_strAttentionStaff = l_Result->GetValue("attention_staff");
		l_AlarmAttentionInfo.m_strAttentionTime = l_Result->GetValue("attention_time");
		l_AlarmAttentionInfo.m_strIsUpdate = l_Result->GetValue("is_update");
		l_AlarmAttentionInfo.m_strUpdateTime = l_Result->GetValue("update_time");
		l_AlarmAttentionInfo.m_strTerminalTime = l_Result->GetValue("terminal_time");
		l_AlarmAttentionInfo.m_strExpireTime = l_Result->GetValue("expire_time");
		
		std::string m_strAlarmAttentionInfo = l_AlarmAttentionInfo.ToJson(m_pJsonFty->CreateJson());
		if (!l_AlarmAttentionInfo.m_strAlarmId.empty())
		{
			if (!m_pRedisClient->HSet(ALARM_ATTENTION_INFO, l_AlarmAttentionInfo.m_strAlarmId, m_strAlarmAttentionInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Hset ALARM_ATTENTION_INFO INFO failed");
			}
		}
	}

	// 从  Redis 中  取数据
	PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;
	std::map<std::string, std::string>mapAttentionInfo;
	if (!m_pRedisClient->HGetAll(ALARM_ATTENTION_INFO, mapAttentionInfo))
	{
		ICC_LOG_ERROR(m_pLog, "ALARM_ATTENTION_INFO HGetALL failed");
	}

	for (auto it = mapAttentionInfo.begin(); it != mapAttentionInfo.end(); it++)
	{
		if (!l_AlarmAttentionInfo.Parse(it->second, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "ALARM_ATTENTION_INFO Parse failed");
			continue;
		}

		if (!l_AlarmAttentionInfo.m_strTerminalTime.empty())
		{
			DateTime::CDateTime terminalTime = m_pDateTime->FromString(l_AlarmAttentionInfo.m_strTerminalTime);
			if ((curTime - terminalTime) >= m_lTerminalExpiredTime * 1000 * 1000)
			{
				AttentionInfo info;
				info.strAlarmId = l_AlarmAttentionInfo.m_strAlarmId;
				info.strStaffCode = l_AlarmAttentionInfo.m_strAttentionStaff;
				vecDeletes.push_back(info);
				continue;
			}
		}

		if (!l_AlarmAttentionInfo.m_strAttentionTime.empty())
		{
			DateTime::CDateTime attentionTime = m_pDateTime->FromString(l_AlarmAttentionInfo.m_strAttentionTime);
			if ((curTime - attentionTime) >= m_lExpiredTime * 1000 * 1000)
			{
				AttentionInfo info;
				info.strAlarmId = l_AlarmAttentionInfo.m_strAlarmId;
				info.strStaffCode = l_AlarmAttentionInfo.m_strAttentionStaff;
				vecDeletes.push_back(info);
				continue;
			}
		}
	}

	for (size_t i = 0; i < vecDeletes.size(); ++i)
	{
		std::string strMsg;
		_DeleteAttention(vecDeletes[i].strAlarmId, vecDeletes[i].strStaffCode, strMsg);
	}

	ICC_LOG_DEBUG(m_pLog, "_CheckExpired end");
}

void CBusinessImpl::OnTimer(unsigned long uMsgId)
{
	if (uMsgId == TIMERID_CHECKEXPIRED)
	{
		_CheckExpired();
	}
}


void CBusinessImpl::OnNotifiJudgeAlarmAttention(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received Judge attention: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmAttentionJudgeRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	PROTOCOL::CAlarmAttentionJudgeResponse response;
	response.m_oHeader = l_request.m_oHeader;

	//查询
	std::string l_strAlarmAttentionInfo;
	if (!m_pRedisClient->HGet(ALARM_ATTENTION_INFO, l_request.m_oBody.m_alarm_id, l_strAlarmAttentionInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "HGet ALARM_ATTENTION_INFO Failed!!!");

	}
	PROTOCOL::CAlarmAttentionInfo l_AlarmAttentionInfo;

	if (!l_AlarmAttentionInfo.Parse(l_strAlarmAttentionInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse ALARM_ATTENTION_INFO failed");
	}

	if (l_request.m_oBody.m_staff_code.compare(l_AlarmAttentionInfo.m_strAttentionStaff.c_str()) == 0)
	{
		response.m_oBody.m_result = "1";
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Alarm is not been attention!");
		response.m_oBody.m_result = "0";
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	std::string tmp_strMsg=response.ToString(tmp_spJson);
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

bool CBusinessImpl::_InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		//TODO::select_icc_t_jjdb_jjsj 查询jjsj
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_strAlarmID);

		if (strTime != "")
		{
			DataBase::SQLRequest l_sqlReqeust;
			l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
			l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
			DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
			if (!l_pRSetPtr->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
			}
			if (l_pRSetPtr->Next())
			{
				p_AlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
			}
		}
	}

	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		p_AlarmLogInfo.m_strReceivedTime = l_strCurTime;
	}

	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		return false;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	m_pHelpTool->Sleep(1);
	return true;
}

void CBusinessImpl::_SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;

	l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}


std::string CBusinessImpl::_AlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

void CBusinessImpl::_QueryStaffName(const std::string& strStaffCode, PROTOCOL::AttentionNameAndDeptName& response, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_staff_dept";
	l_SQLRequest.param["user_code"] = strStaffCode;
	
	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "select_icc_t_staff_dept sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_staff_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return ;
	}

	if (l_Result->Next())
	{
		response.m_strname = l_Result->GetValue("name");
		response.m_strdeptname = l_Result->GetValue("deptname");
	}

}

bool CBusinessImpl::_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo)
{
	std::string strStaffInfo;
	if (m_strCodeMode == STAFF_ID_NO)
	{
		if (!m_pRedisClient->HGet("StaffIdInfo", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffIdInfo failed,staff_id_no:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	else if (m_strCodeMode == STAFF_CODE)
	{
		if (!m_pRedisClient->HGet("StaffInfoMap", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed,staff_code:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	if (!l_oStaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse staff info failed!!!");
		return false;
	}
	return true;
}

std::string	CBusinessImpl::_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName)
{
	std::string strEndStaffName;
	if (strStaffType == "JZLX101")
	{
		strEndStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), strStaffName.c_str());
	}
	else
	{
		strEndStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), strStaffName.c_str());
	}
	return strEndStaffName;
}

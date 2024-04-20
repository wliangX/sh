#include "Boost.h"
#include "BusinessImpl.h"
#define STAFF_ID_NO      "1"
#define STAFF_CODE       "2"

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
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
	m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/CodeMode", "1");
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_shift_request", OnCNotifiUpdateShift);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_shift_request", OnCNotifiGetShift);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_shift_request_ex", OnReceiveUpdateShiftNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_shift_request_ex", OnReceiveQueryShiftNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_shift_relation_request", OnReceiveQueryShiftRelationNotify);

	ICC_LOG_DEBUG(m_pLog, "shift start success，AssistantPolice=%s,Police=%s,CodeMode:[%s]",m_strAssistantPolice.c_str(), m_strPolice.c_str(), m_strCodeMode.c_str());
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "shift stop success");
}

void CBusinessImpl::OnDestroy()
{

}

std::string CBusinessImpl::_QueryDeptPath(const std::string& strDeptCode, const std::string& strTransGuid,bool name_path)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_name_path_from_icc_t_dept";
	l_SQLRequest.param["dept_code"] = strDeptCode;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest,false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "query_name_path_from_icc_t_dept sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return "";
	}

	if (l_Result->Next())
	{		
		if (name_path)
		{
			return l_Result->GetValue("name");
		}
		else
		{
			return l_Result->GetValue("path");
		}
	}

	return "";
}

bool CBusinessImpl::_InsertShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "insert_icc_t_shift";
	sqlReqeust.param["guid"] = strShiftId;
	sqlReqeust.param["start_time"] = request.m_strStartTime;
	sqlReqeust.param["end_time"] = request.m_strEndTime;
	sqlReqeust.param["dept_code"] = request.m_strDeptCode;
	sqlReqeust.param["dept_code_path"] = _QueryDeptPath(request.m_strDeptCode,strTransGuid);
	sqlReqeust.param["duty_code"] = request.m_strDutyCode;
	sqlReqeust.param["duty_name"] = request.m_strDutyName;
	sqlReqeust.param["duty_leader_code"] = request.m_strDutyLeaderCode;
	sqlReqeust.param["duty_leader_name"] = request.m_strDutyLeaderName;
	sqlReqeust.param["receiver_code"] = request.m_strReceiverCode;
	sqlReqeust.param["receiver_name"] = request.m_strReceiverName;
	sqlReqeust.param["receive_state"] = "0";
	sqlReqeust.param["work_content"] = request.m_strWorkContent;
	sqlReqeust.param["important_content"] = request.m_strImportantContent;
	sqlReqeust.param["shift_content"] = request.m_strShiftContent;
	sqlReqeust.param["create_user"] = request.m_strCreateUser;
	sqlReqeust.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	sqlReqeust.param["update_user"] = request.m_strUpdateUser;
	sqlReqeust.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "insert_icc_t_shift sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{		
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_shift failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::_InsertReleation(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "insert_icc_t_shift_relation";

	unsigned int iCount = request.m_vecDatas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		sqlReqeust.param["shift_id"] = strShiftId;
		sqlReqeust.param["alarm_id"] = request.m_vecDatas[i];

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);
		ICC_LOG_DEBUG(m_pLog, "insert_icc_t_shift_relation sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "insert_icc_t_shift_relation failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return false;
		}
	}
	
	return true;
}

bool CBusinessImpl::_AlarmLogSync(const PROTOCOL::CShiftAddOrUpdateReqeust& request,const std::string& strTransGuid)
{
	PROTOCOL::CAlarmLogSync l_oTHLData;
	std::vector<std::string> l_vecParamList;
	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(request.m_strDutyCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, request.m_strDutyName);
	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(request.m_strDutyCode);
	l_vecParamList.push_back(_QueryDeptPath(request.m_strDeptCode, strTransGuid,true));

	Data::CStaffInfo l_StaffInfo;
	if (!_GetStaffInfo(request.m_strReceiverCode, l_StaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	l_strStaffName = _GetPoliceTypeName(l_StaffInfo.m_strType, request.m_strReceiverName);
	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(request.m_strReceiverCode);
	l_vecParamList.push_back(_QueryDeptPath(request.m_strDeptCode, strTransGuid,true));
	l_oTHLData.m_oBody.m_strOperate = "BS001005012";
	l_oTHLData.m_oBody.m_strOperateContent = _AlarmLogContent(l_vecParamList);
	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	for (int i = 0; i < request.m_vecDatas.size(); ++i)
	{
		l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
		l_oTHLData.m_oBody.m_strAlarmID = request.m_vecDatas[i];
		if (!_InsertAlarmLogInfo(l_oTHLData.m_oBody))
		{
			ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
			return false;
		}
		else
		{
			_SyncAlarmLogInfo(l_oTHLData.m_oBody);
		}
	}

	return true;
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

bool CBusinessImpl::_ProcessAddShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, PROTOCOL::CShiftAddOrUpdateRespond& response)
{
	std::string strTransGuid; 
	bool bSuccess = false;
	
	std::string strShiftId = m_pString->CreateGuid();
	strTransGuid = m_pDBConn->BeginTransaction();
	do 
	{
		if (strTransGuid.empty())
		{
			response.m_strCode = "3";
			response.m_strMessage = "get trans connection failed";
			break;
		}
		
		if (!_InsertShift(request, strShiftId, strTransGuid ))
		{
			response.m_strCode = "1";
			response.m_strMessage = "insert shift failed";
			break;
		}

		if (!_InsertReleation(request, strShiftId, strTransGuid))
		{
			response.m_strCode = "2";
			response.m_strMessage = "insert relation failed";
			break;
		}
		//写流水
		if (!_AlarmLogSync(request, strTransGuid))
		{
			response.m_strCode = "4";
			response.m_strMessage = "insert alarm log info failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		_SynShift(request, strShiftId, strTransGuid);

		m_pDBConn->Commit(strTransGuid);
		response.m_strCode = "200";
		response.m_strMessage = "success";
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}
	
	return bSuccess;
}

bool CBusinessImpl::_DeleteShiftRelation(const std::string& strShiftId, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "delete_icc_t_shift_relation";
	sqlReqeust.param["shift_id"] = strShiftId;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_shift_relation failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::_UpdateShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "update_icc_t_shift";
	sqlReqeust.param["guid"] = request.m_strGuid;
	if (!request.m_strStartTime.empty())
	{
		sqlReqeust.set["start_time"] = request.m_strStartTime;
	}

	if (!request.m_strEndTime.empty())
	{
		sqlReqeust.set["end_time"] = request.m_strEndTime;
	}
	
	if (!request.m_strDeptCode.empty())
	{
		sqlReqeust.set["dept_code"] = request.m_strDeptCode;
	}	
	
	if (!request.m_strDeptCode.empty())
	{
		sqlReqeust.param["dept_code_path"] = _QueryDeptPath(request.m_strDeptCode, strTransGuid);
	}

	if (!request.m_strDutyCode.empty())
	{
		sqlReqeust.set["duty_code"] = request.m_strDutyCode;
	}
	
	if (!request.m_strDutyName.empty())
	{
		sqlReqeust.set["duty_name"] = request.m_strDutyName;
	}

	if (!request.m_strDutyLeaderCode.empty())
	{
		sqlReqeust.set["duty_leader_code"] = request.m_strDutyLeaderCode;
	}
	
	if (!request.m_strDutyLeaderName.empty())
	{
		sqlReqeust.set["duty_leader_name"] = request.m_strDutyLeaderName;
	}
	
	if (!request.m_strReceiverCode.empty())
	{
		sqlReqeust.set["receiver_code"] = request.m_strReceiverCode;
	}

	if (!request.m_strReceiverName.empty())
	{
		sqlReqeust.set["receiver_name"] = request.m_strReceiverName;
	}
	
	if (!request.m_strReceiveState.empty())
	{
		sqlReqeust.set["receive_state"] = request.m_strReceiveState;
	}
	
	if (!request.m_strWorkContent.empty())
	{
		sqlReqeust.set["work_content"] = request.m_strWorkContent;
	}
	
	if (!request.m_strImportantContent.empty())
	{
		sqlReqeust.set["important_content"] = request.m_strImportantContent;
	}
	
	if (!request.m_strShiftContent.empty())
	{
		sqlReqeust.set["shift_content"] = request.m_strShiftContent;
	}
	
	if (!request.m_strUpdateUser.empty())
	{
		sqlReqeust.set["update_user"] = request.m_strUpdateUser;
	}	
	
	sqlReqeust.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_shift failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::_ProcessUpdateShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, PROTOCOL::CShiftAddOrUpdateRespond& response)
{
	std::string strTransGuid;
	bool bSuccess = false;
	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();
		if (strTransGuid.empty())
		{
			response.m_strCode = "3";
			response.m_strMessage = "get trans connection failed";
			break;
		}		

		if (!_UpdateShift(request, strTransGuid))
		{
			response.m_strCode = "1";
			response.m_strMessage = "update shift failed";
			break;
		}

		if (request.m_vecDatas.size() != 0)
		{
			if (!_DeleteShiftRelation(request.m_strGuid, strTransGuid))
			{
				response.m_strCode = "2";
				response.m_strMessage = "delete relation failed";
				break;
			}

			if (!_InsertReleation(request, request.m_strGuid, strTransGuid))
			{
				response.m_strCode = "4";
				response.m_strMessage = "insert relation failed";
				break;
			}
		}	

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		response.m_strCode = "200";
		response.m_strMessage = "success";
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}

void CBusinessImpl::OnReceiveUpdateShiftNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	if (!p_pNotify)
	{
		ICC_LOG_ERROR(m_pLog, "OnReceiveUpdateShiftNotify Request is nullptr");
		return;
	}
	std::string strMessage = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnReceiveUpdateShiftNotify message:[%s]", strMessage.c_str());

	PROTOCOL::CShiftAddOrUpdateReqeust request;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnReceiveUpdateShiftNotify]Parse request error.");
		return;
	}	

	PROTOCOL::CShiftAddOrUpdateRespond response;
	response.m_strMsgId = request.m_oHeader.m_strMsgId;
	
	if (request.m_strGuid.empty())
	{
		_ProcessAddShift(request, response);
	}
	else
	{
		_ProcessUpdateShift(request, response);
	}

	std::string strMsg(response.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(strMsg);

	ICC_LOG_DEBUG(m_pLog, "OnReceiveUpdateShiftNotify complete. [%s]", strMsg.c_str());
}

void CBusinessImpl::_SynShift(const PROTOCOL::CShiftAddOrUpdateReqeust& request, const std::string& strShiftId, const std::string& strTransGuid)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CShiftSyn shiftSync;
	shiftSync.m_oHeader.m_strSystemID = "ICC";
	shiftSync.m_oHeader.m_strSubsystemID = "shift";
	shiftSync.m_oHeader.m_strMsgid = l_strGuid;
	shiftSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	shiftSync.m_oHeader.m_strCmd = "shift_syn";
	shiftSync.m_oHeader.m_strRequest = "topic_notice_syn";
	shiftSync.m_oHeader.m_strRequestType = "1";
	shiftSync.m_oHeader.m_strResponse = "";
	shiftSync.m_oHeader.m_strResponseType = "";

	shiftSync.m_strShiftId = strShiftId;
	shiftSync.m_strReceiverCode = request.m_strReceiverCode;
	shiftSync.m_strDeptCode = request.m_strDeptCode;
	shiftSync.m_strDeptName = _QueryDeptPath(request.m_strDeptCode, strTransGuid, true);
	shiftSync.m_strDutyCode = request.m_strDutyCode;
	shiftSync.m_strDutyName = request.m_strDutyName;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = shiftSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send _SynShift message:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::_CheckQueryShiftParam(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response)
{
	if (request.m_strStartTime.empty())
	{
		response.m_strCode = "1";
		response.m_strMessage = "start time is empty!";
		return false;
	}

	if (request.m_strEndTime.empty())
	{
		response.m_strCode = "1";
		response.m_strMessage = "end time is empty!";
		return false;
	}

	if (request.m_strDeptCode.empty())
	{
		response.m_strCode = "1";
		response.m_strMessage = "dept code is empty!";
		return false;
	}

	if (request.m_strPageIndex.empty() || std::stoi(request.m_strPageIndex) < 1)
	{
		response.m_strCode = "1";
		response.m_strMessage = "page index is invalid!";
		return false;
	}

	if (request.m_strPageSize.empty())
	{
		response.m_strCode = "1";
		response.m_strMessage = "page size is invalid";
		return false;
	}

	int iPageSize = std::stoi(request.m_strPageSize);
	if (iPageSize < 1 || iPageSize > 1000)
	{
		response.m_strCode = "1";
		response.m_strMessage = "page size is invalid";
		return false;
	}

	return true;
}

bool CBusinessImpl::_QueryShiftCount(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response)
{
	DataBase::SQLRequest l_tSQLReqCnt;
	l_tSQLReqCnt.sql_id = "select_shift_count";
	l_tSQLReqCnt.param["start_time"] = request.m_strStartTime;
	l_tSQLReqCnt.param["end_time"] = request.m_strEndTime;
	if (!request.m_strDutyCode.empty())
	{
		l_tSQLReqCnt.param["duty_code"] = request.m_strDutyCode;
	}

	if (!request.m_strReceiverCode.empty())
	{
		l_tSQLReqCnt.param["receiver_code"] = request.m_strReceiverCode;
	}

	if (!request.m_isSelectEx.empty())
	{
		l_tSQLReqCnt.param["enable_alarm"] = request.m_isSelectEx;
	}

	std::string strIsRecursive = "0";
	if (!request.m_strIsRecursive.empty())
	{
		strIsRecursive = request.m_strIsRecursive;
	}
	l_tSQLReqCnt.param["is_recursive"] = strIsRecursive;
	l_tSQLReqCnt.param["dept_code"] = request.m_strDeptCode;
	
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		response.m_strCode = "1";
		response.m_strMessage = "select_shift_count failed!";
		return false;;
	}
	ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_result->GetSQL().c_str());

	response.m_strAllCount = l_result->GetValue(0, "num");

	return true;
}

bool CBusinessImpl::_QueryShift(const PROTOCOL::CShiftQueryReqeust& request, PROTOCOL::CShiftQueryRespond& response)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "select_shift";
	sqlReqeust.param["start_time"] = request.m_strStartTime;
	sqlReqeust.param["end_time"] = request.m_strEndTime;

	if (!request.m_strDutyCode.empty())
	{
		sqlReqeust.param["duty_code"] = request.m_strDutyCode;
	}

	if (!request.m_strReceiverCode.empty())
	{
		sqlReqeust.param["receiver_code"] = request.m_strReceiverCode;
	}

	if (!request.m_isSelectEx.empty())
	{
		sqlReqeust.param["enable_alarm"] = request.m_isSelectEx;
	}

	std::string strIsRecursive = "0";
	if (!request.m_strIsRecursive.empty())
	{
		strIsRecursive = request.m_strIsRecursive;
	}
	sqlReqeust.param["is_recursive"] = strIsRecursive;
	sqlReqeust.param["dept_code"] = request.m_strDeptCode;

	sqlReqeust.param["page_size"] = request.m_strPageSize;
	sqlReqeust.param["page_index"] = request.m_strPageIndex;

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(sqlReqeust, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

		response.m_strCode = "1";
		response.m_strMessage = "select_shift failed!";	

		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_result->GetSQL().c_str());

	std::vector<std::string> vecCloumns = l_result->GetFieldNames();
	unsigned int iCloumnCount = vecCloumns.size();
	while (l_result->Next())
	{		
		std::map<std::string, std::string> mapTmps;
		for (unsigned int i = 0; i < iCloumnCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecCloumns[i], l_result->GetValue(vecCloumns[i])));
		}		

		response.m_vecDatas.push_back(mapTmps);
	}

	response.m_strCount = m_pString->Format("%d", response.m_vecDatas.size());

	return true;
}

void CBusinessImpl::OnReceiveQueryShiftNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	if (!p_pNotify)
	{
		ICC_LOG_ERROR(m_pLog, "OnReceiveQueryShiftNotify Request is nullptr");
		return;
	}
	std::string strMessage = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnReceiveQueryShiftNotify message:[%s]", strMessage.c_str());

	PROTOCOL::CShiftQueryReqeust request;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnReceiveQueryShiftNotify]Parse request error.");
		return;
	}

	PROTOCOL::CShiftQueryRespond response;
	response.m_strMsgId = request.m_oHeader.m_strMsgId;

	do 
	{
		if (!_CheckQueryShiftParam(request, response))
		{
			break;
		}

		if (!_QueryShiftCount(request, response))
		{
			break;
		}

		if (!_QueryShift(request, response))
		{
			break;
		}	
		
		response.m_strCode = "200";
		response.m_strMessage = "success";

	} while (false);

	std::string strMsg(response.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(strMsg);
	ICC_LOG_DEBUG(m_pLog, "OnReceiveQueryShiftNotify complete. [%s]", strMsg.c_str());
}

bool CBusinessImpl::_QueryShiftRelation(const PROTOCOL::CShiftQueryRelationReqeust& request, PROTOCOL::CShiftQueryRelationRespond& response)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "select_icc_t_shift_relation_by_shift_id";
	sqlReqeust.param["shift_id"] = request.m_strShiftId;

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(sqlReqeust, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

		response.m_strCode = "1";
		response.m_strMessage = "select_shift failed!";

		return false;
	}

	std::vector<std::string> vecCloumns = l_result->GetFieldNames();
	unsigned int iCloumnCount = vecCloumns.size();
	while (l_result->Next())
	{
		std::map<std::string, std::string> mapTmps;
		for (unsigned int i = 0; i < iCloumnCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecCloumns[i], l_result->GetValue(vecCloumns[i])));
		}

		response.m_vecDatas.push_back(mapTmps);
	}

	response.m_strCount = m_pString->Format("%d", response.m_vecDatas.size());

	response.m_strCode = "200";
	response.m_strMessage = "success";

	return true;
}

void CBusinessImpl::OnReceiveQueryShiftRelationNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	if (!p_pNotify)
	{
		ICC_LOG_ERROR(m_pLog, "OnReceiveQueryShiftRelationNotify Request is nullptr");
		return;
	}
	std::string strMessage = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnReceiveQueryShiftRelationNotify message:[%s]", strMessage.c_str());


	PROTOCOL::CShiftQueryRelationReqeust request;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnReceiveQueryShiftNotify]Parse request error.");
		return;
	}

	PROTOCOL::CShiftQueryRelationRespond response;
	response.m_strMsgId = request.m_oHeader.m_strMsgId;

	_QueryShiftRelation(request, response);

	std::string strMsg(response.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(strMsg);
	ICC_LOG_DEBUG(m_pLog, "OnReceiveQueryShiftRelationNotify complete. [%s]", strMsg.c_str());
}


void CBusinessImpl::OnCNotifiUpdateShift(ObserverPattern::INotificationPtr p_pNotify)
{
	if (!p_pNotify)
	{
		ICC_LOG_ERROR(m_pLog, "UpdateTodayAlarm Request is nullptr");

		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receive UpdateShift message:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CShiftProtocol request;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[UpdateShift]Parse request error.");
		return;
	}

	PROTOCOL::CShiftProtocol response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	
	if (!request.m_oBody.m_vecData.empty())
	{
		DataBase::SQLRequest l_tSQLReq;
		PROTOCOL::CShiftProtocol::CShift shift = request.m_oBody.m_vecData[0];
		string strCurrentTime = m_pDateTime->CurrentDateTimeStr();
		if (shift.m_strGuid.empty())//新增
		{
			l_tSQLReq.sql_id = "insert_icc_t_shift";
			l_tSQLReq.param["guid"] = m_pString->CreateGuid();
			l_tSQLReq.param["start_time"] = shift.m_strStartTime;
			l_tSQLReq.param["end_time"] = shift.m_strEndTime;
			l_tSQLReq.param["dept_code"] = shift.m_strDeptCode;
			l_tSQLReq.param["duty_code"] = shift.m_strDutyCode;
			l_tSQLReq.param["duty_name"] = shift.m_strDutyName;
			l_tSQLReq.param["duty_leader_code"] = shift.m_strDutyLeaderCode;
			l_tSQLReq.param["duty_leader_name"] = shift.m_strDutyLeaderName;
			l_tSQLReq.param["work_content"] = shift.m_strWorkContent;
			l_tSQLReq.param["important_content"] = shift.m_strImportantContent;
			l_tSQLReq.param["shift_content"] = shift.m_strShiftContent;
			l_tSQLReq.param["create_user"] = "";
			l_tSQLReq.param["create_time"] = strCurrentTime;
			l_tSQLReq.param["update_user"] = "";
			l_tSQLReq.param["update_time"] = strCurrentTime;
		}
		else//修改
		{
			l_tSQLReq.sql_id = "update_icc_t_shift";
			l_tSQLReq.param["guid"] = shift.m_strGuid;
			l_tSQLReq.set["start_time"] = shift.m_strStartTime;
			l_tSQLReq.set["end_time"] = shift.m_strEndTime;
			l_tSQLReq.set["dept_code"] = shift.m_strDeptCode;
			l_tSQLReq.set["duty_code"] = shift.m_strDutyCode;
			l_tSQLReq.set["duty_name"] = shift.m_strDutyName;
			l_tSQLReq.set["duty_leader_code"] = shift.m_strDutyLeaderCode;
			l_tSQLReq.set["duty_leader_name"] = shift.m_strDutyLeaderName;
			l_tSQLReq.set["work_content"] = shift.m_strWorkContent;
			l_tSQLReq.set["important_content"] = shift.m_strImportantContent;
			l_tSQLReq.set["shift_content"] = shift.m_strShiftContent;
			l_tSQLReq.set["update_user"] = "";
			l_tSQLReq.set["update_time"] = strCurrentTime;
		}

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
		if (!l_pRSet->IsValid())
		{
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "UpdateShift failed";
			ICC_LOG_ERROR(m_pLog, "UpdateShift failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		}
	}
	else
	{
		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "Request empty";
	}

	std::string strMsg(response.UpToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(strMsg);
	ICC_LOG_INFO(m_pLog, "UpdateShift sucess.");
}

void CBusinessImpl::OnCNotifiGetShift(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CShiftProtocol request;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[GetShift]Parse request error.");
		return;
	}

	PROTOCOL::CShiftProtocol response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	response.m_oBody.m_strPageIndex = request.m_strPageIndex;
	response.m_oBody.m_strAllCount = "0";
	response.m_oBody.m_strCount = "0";

	ICC_LOG_DEBUG(m_pLog, "shift page_size=%s,page_index=%s,receive_state=%s", request.m_strPageSize.c_str(), request.m_strPageIndex.c_str(), request.m_oBody.m_strReceiveState.c_str());
	if (!request.m_oBody.m_vecData.empty() && !request.m_strPageSize.empty() && !request.m_strPageIndex.empty())
	{
		PROTOCOL::CShiftProtocol::CShift shift = request.m_oBody.m_vecData[0];
		//查询记录总数
		DataBase::SQLRequest l_tSQLReqCnt;
		l_tSQLReqCnt.sql_id = "select_shift_count";
		l_tSQLReqCnt.param["start_time"] = shift.m_strStartTime;
		l_tSQLReqCnt.param["end_time"] = shift.m_strEndTime;
		if (!shift.m_strDutyCode.empty())
		{
			l_tSQLReqCnt.param["duty_code"] = shift.m_strDutyCode;
		}
		if (!shift.m_strDeptCode.empty())
		{
			l_tSQLReqCnt.param["dept_code"] = shift.m_strDeptCode;
		}
		if (!request.m_oBody.m_strReceiveState.empty())
		{
			l_tSQLReqCnt.param["receive_state"] = request.m_oBody.m_strReceiveState;
		}
		if (!request.m_oBody.m_isSelectEx.empty())
		{
			l_tSQLReqCnt.param["enable_alarm"] = request.m_oBody.m_isSelectEx;
		}
		if (!request.m_oBody.m_strReceiveCode.empty())
		{
			l_tSQLReqCnt.param["receiver_code"] = request.m_oBody.m_strReceiveCode;
		}

		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt, true);
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "ExecQuery Failed";

			std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "Send shift Info Msg [%s]", l_strMessage.c_str());

			return;
		}
		response.m_oBody.m_strAllCount = l_result->GetValue(0, "num");
		ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_result->GetSQL().c_str());

		//查询当前页记录
		DataBase::SQLRequest l_tSQLReq;
		l_tSQLReq.sql_id = "select_shift";
		l_tSQLReq.param["start_time"] = shift.m_strStartTime;
		l_tSQLReq.param["end_time"] = shift.m_strEndTime;
		l_tSQLReq.param["page_size"] = request.m_strPageSize;
		l_tSQLReq.param["page_index"] = request.m_strPageIndex;
		if (!shift.m_strDutyCode.empty())
		{
			l_tSQLReq.param["duty_code"] = shift.m_strDutyCode;
		}
		if (!shift.m_strDeptCode.empty())
		{
			l_tSQLReq.param["dept_code"] = shift.m_strDeptCode;
		}
		if (!request.m_oBody.m_strReceiveState.empty())
		{
			l_tSQLReq.param["receive_state"] = request.m_oBody.m_strReceiveState;
		}
		if (!request.m_oBody.m_isSelectEx.empty())
		{
			l_tSQLReq.param["enable_alarm"] = request.m_oBody.m_isSelectEx;
		}
		if (!request.m_oBody.m_strReceiveCode.empty())
		{
			l_tSQLReqCnt.param["receiver_code"] = request.m_oBody.m_strReceiveCode;
		}

		l_result = m_pDBConn->Exec(l_tSQLReq, true);
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "ExecQuery Failed";

			std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "Send shift Info Msg [%s]", l_strMessage.c_str());

			return;
		}
		ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_result->GetSQL().c_str());

		while (l_result->Next())
		{
			PROTOCOL::CShiftProtocol::CShift shift;
			shift.m_strGuid = l_result->GetValue("guid");
			shift.m_strStartTime = l_result->GetValue("start_time");
			shift.m_strEndTime = l_result->GetValue("end_time");
			shift.m_strDeptCode = l_result->GetValue("dept_code");
			shift.m_strDeptName = l_result->GetValue("name");
			shift.m_strDutyCode = l_result->GetValue("duty_code");
			shift.m_strDutyName = l_result->GetValue("duty_name");
			shift.m_strDutyLeaderCode = l_result->GetValue("duty_leader_code");
			shift.m_strDutyLeaderName = l_result->GetValue("duty_leader_name");
			shift.m_strWorkContent = l_result->GetValue("work_content");
			shift.m_strImportantContent = l_result->GetValue("important_content");
			shift.m_strShiftContent = l_result->GetValue("shift_content");
			
			response.m_oBody.m_vecData.push_back(shift);
		}

		response.m_oBody.m_strCount = std::to_string(response.m_oBody.m_vecData.size());
	}
	else
	{
		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "Request empty";
	}

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	response.m_oBody.m_vecData.clear();

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send shift Info Msg [%s]", l_strMessage.c_str());
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

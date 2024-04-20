#include "Boost.h"
#include "ViolationManager.h"
#include <Protocol/ParamInfo.h>

#define Param_INFO "ParamInfo"

class CViolationSeatInfo
{
public:
	CViolationSeatInfo();
	~CViolationSeatInfo();
public:
	bool Parse(std::string p_strSeatInfo, JsonParser::IJsonPtr p_pJson);

public:
	std::string m_strNo;
	std::string m_strName;
	std::string m_strDeptCode;
	std::string m_strDeptName;
};

CViolationSeatInfo::CViolationSeatInfo()
{

}

CViolationSeatInfo::~CViolationSeatInfo()
{

}



bool CViolationSeatInfo::Parse(std::string p_strStaffInfo, JsonParser::IJsonPtr p_pJson)
{
	if (p_pJson && p_pJson->LoadJson(p_strStaffInfo))
	{
		m_strNo = p_pJson->GetNodeValue("/no", "");
		m_strName = p_pJson->GetNodeValue("/name", "");
		m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
		m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");

		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
CViolationCheckData::CViolationCheckData()
{
	m_nVolationCheckType = VIOLATIONCHECK_UNKNOWN;
	m_nTick = 0;
	m_nTimeOutCount = 0;
	m_nTimeOutValue = 0;
	m_strViolationId = "";
	m_strUserID = "";
	m_strStartTime = "";
}

CViolationCheckData::~CViolationCheckData()
{
	//
}

//每秒调用一次
bool CViolationCheckData::ViolationCheck()
{
	if (m_nTimeOutValue > 0)
	{
		if (++m_nTick >= m_nTimeOutValue)
		{
			m_nTick = 0;
			m_nTimeOutCount++;
			return true;
		}
	}
	return false;
}

void CViolationCheckData::ChangeViolationCheck(const std::string& p_strViolationId, int p_nCheckType,int p_nTimeOutValue)
{
	if (p_nCheckType < VIOLATIONCHECK_UNKNOWN)
	{
		m_nVolationCheckType = p_nCheckType;
		m_nTick = 0;
		if (p_nTimeOutValue < 5)
		{
			//最小值5s
			p_nTimeOutValue = 5;
		}
		m_nTimeOutValue = p_nTimeOutValue;

		m_strViolationId = p_strViolationId;
		if (p_nCheckType != m_nVolationCheckType)
		{
			m_nTimeOutCount = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CViolationManager> CViolationManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CViolationManager> CViolationManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CViolationManager>();
	}
	return m_pManagerInstance;
}
void CViolationManager::ExitInstance()
{
	//this->ClearConferenceList();
	this->ClearViolationCheckList();
}

void CViolationManager::OnStart()
{
	ICC_LOG_DEBUG(m_LogPtr, "CViolationManager::OnStart, CallRingTimeOut:[%d],CallTalkTimeOut:[%d],BusyTimeOut:[%d]", 
		m_nCallRingTimeOutValue, m_nCallTalkTimeOutValue, m_nBusyTimeOutValue);
}

CViolationManager::CViolationManager(void)
{
	m_LogPtr = nullptr;
	m_DateTimePtr = nullptr;
	m_StringUtilPtr = nullptr;
	m_oLastCheckTime = 0;
	m_pTimerManager = nullptr;
	m_pRequestDBConn = nullptr;

	//违规超时检测默认30s
	m_nCallRingTimeOutValue = 30;
	m_nCallTalkTimeOutValue = 30;
	m_nBusyTimeOutValue = 30;
}

CViolationManager::~CViolationManager(void)
{
	//
}

void CViolationManager::ClearViolationCheckList()
{
	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	m_mViolationCheckList.clear();
}

void CViolationManager::SetViolationCheck(int p_nCheckType, const std::string& p_strState,
	const std::string& p_strDevice,
	const std::string& p_strUserID,
	const std::string& p_strViolationID)
{
	int l_nViolationCheckType = VIOLATIONCHECK_UNKNOWN;
	unsigned int l_nTimeOutValue = 0;
	if (0 == p_nCheckType) 
	{
		//呼叫状态类违规
		if (p_strState.compare(CallStateString[STATE_RING]) == 0)		//震铃状态
		{
			l_nViolationCheckType = VIOLATIONCHECK_CALL_RING;
			l_nTimeOutValue = m_nCallRingTimeOutValue;
			if (m_nCallRingTimeOutValue == 0)
			{
				return;
			}
		}
		else if (p_strState.compare(CallStateString[STATE_TALK]) == 0)	//通话状态
		{
			l_nViolationCheckType = VIOLATIONCHECK_CALL_TALK;
			l_nTimeOutValue = m_nCallTalkTimeOutValue;
			if (m_nCallTalkTimeOutValue == 0)
			{
				return;
			}
		}
		else
		{
			if (p_strState.compare(CallStateString[STATE_DIALERROR]) == 0 ||
				p_strState.compare(CallStateString[STATE_RELEASE]) == 0 ||
				p_strState.compare(CallStateString[STATE_HANGUP]) == 0 ||
				p_strState.compare(CallStateString[STATE_CALLOVER]) == 0 ||
				p_strState.compare(CallStateString[STATE_FREE]) == 0 ||
				p_strState.compare(CallStateString[STATE_WAITHANGUP]) == 0)
			{
				l_nViolationCheckType = VIOLATIONCHECK_UNKNOWN;
			}
			else
			{
				return;
			}
		}
	}
	else 
	{
		//坐席状态类违规
		if (p_strState == ReadyState_Busy)
		{
			l_nViolationCheckType = VIOLATIONCHECK_BUSY;
			l_nTimeOutValue = m_nBusyTimeOutValue;
			if (m_nBusyTimeOutValue == 0)
			{
				return;
			}
		}
	}

	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	auto iter = m_mViolationCheckList.find(p_strViolationID);
	if (iter != m_mViolationCheckList.end())
	{
		if (VIOLATIONCHECK_UNKNOWN == l_nViolationCheckType)
		{
			ICC_LOG_DEBUG(m_LogPtr, "Delete Violation Check CallRefId:[%s],Device:[%s],CheckType:[%d]", p_strViolationID.c_str(), p_strDevice.c_str(), l_nViolationCheckType);
			//删除违规检测
			m_mViolationCheckList.erase(iter);
		}
		else
		{
			iter->second->ChangeViolationCheck(p_strViolationID, l_nViolationCheckType, l_nTimeOutValue);
		}
	}
	else
	{
		if ( VIOLATIONCHECK_UNKNOWN != l_nViolationCheckType)
		{
			//添加违规检测
			boost::shared_ptr<CViolationCheckData> l_pViolationCheckData = boost::make_shared<CViolationCheckData>();
			//l_pViolationCheckData->SetStartTime(p_strStateTime);
			l_pViolationCheckData->SetViolationTarget(p_strDevice);
			l_pViolationCheckData->SetViolationUserID(p_strUserID);
			l_pViolationCheckData->ChangeViolationCheck(p_strViolationID, l_nViolationCheckType, l_nTimeOutValue);
			m_mViolationCheckList[p_strViolationID] = l_pViolationCheckData;

			ICC_LOG_DEBUG(m_LogPtr, "Add Violation Check ViolationID:[%s],Device:[%s],CheckType:[%d],TimeOutValue:[%d]", p_strViolationID.c_str(), p_strDevice.c_str(), l_nViolationCheckType, l_nTimeOutValue);
		}
	}
}

void CViolationManager::ViolationCheckTimerProcess()
{
	std::map<std::string, boost::shared_ptr<CViolationCheckData>> violationCheckList;
	{
		std::lock_guard<std::mutex> guard(m_violationCheckMutex);
		violationCheckList = m_mViolationCheckList;
	}

	std::vector<std::string> l_timeoutKeys;
	//std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	for (auto l_oCheckObj: violationCheckList)
	{
		if (l_oCheckObj.second->ViolationCheck())
		{
			l_timeoutKeys.push_back(l_oCheckObj.first);
			int l_nCheckType = l_oCheckObj.second->GetCheckType();
			std::string p_strVoilationType;
			std::string p_strVoilationID;

			int l_nTimeOutValue = 0;
			if (VIOLATIONCHECK_BUSY == l_nCheckType)
			{
				p_strVoilationType = "DIC050001"; //置忙超时
				l_nTimeOutValue = m_nBusyTimeOutValue;
			}
			else if (VIOLATIONCHECK_CALL_RING == l_nCheckType)
			{
				p_strVoilationType = "DIC050002"; //震铃超时
				l_nTimeOutValue = m_nCallRingTimeOutValue;
			}
			else if (VIOLATIONCHECK_CALL_TALK == l_nCheckType)
			{
				p_strVoilationType = "DIC050003"; //通话超时
				l_nTimeOutValue = m_nCallTalkTimeOutValue;
			}
			else
			{
				continue;
			}
			std::string l_strTimeOutValue = m_StringUtilPtr->Format("%d", l_nTimeOutValue);
			p_strVoilationID = l_oCheckObj.first + "_" + p_strVoilationType;
			std::string p_strCurrentTime = m_DateTimePtr->CurrentDateTimeStr();
			DataBase::SQLRequest l_strViolationSql;

			int l_nViolationCount = l_oCheckObj.second->GetTimeOutCount();
			//修改成违规超时一次增加一条记录一次,不更新
			/*
			if (l_oCheckObj.second->GetTimeOutCount() <= 1)
			{
				l_strViolationSql.sql_id = " insert_icc_t_violation";
				l_strViolationSql.param.insert(std::pair<std::string, std::string>("guid", m_StringUtilPtr->CreateGuid()));
				l_strViolationSql.param.insert(std::pair<std::string, std::string>("create_time", p_strCurrentTime));
				l_strViolationSql.param.insert(std::pair<std::string, std::string>("timeout_value", l_strTimeOutValue));
				l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_type", p_strVoilationType));
				l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_target", l_oCheckObj.second->GetViolationTarget()));
			}
			else
			{
				l_strViolationSql.sql_id = "update_icc_t_violation";
			}
			*/
			//
			CViolationSeatInfo l_oSeatData;
			if (m_pRedisClient)
			{
				std::string l_strVal;
				if (m_pRedisClient->HGet("SeatInfo", l_oCheckObj.second->GetViolationTarget(), l_strVal))
				{
					l_oSeatData.Parse(l_strVal, m_JsonFactoryPtr->CreateJson());
				}
			}


			std::string l_strGUID = m_StringUtilPtr->CreateGuid();
			l_strViolationSql.sql_id = "insert_icc_t_violation";
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("guid", l_strGUID));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("create_time", p_strCurrentTime));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("timeout_value", l_strTimeOutValue));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_type", p_strVoilationType));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_target", l_oCheckObj.second->GetViolationTarget()));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_user", l_oCheckObj.second->GetViolationUserID()));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("dept_code", l_oSeatData.m_strDeptCode));

			l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_id", p_strVoilationID));
			//l_strViolationSql.param.insert(std::pair<std::string, std::string>("update_time", p_strCurrentTime));
			l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_time", p_strCurrentTime));

			DataBase::IResultSetPtr l_pSqlResult;
			l_pSqlResult = m_pRequestDBConn->Exec(l_strViolationSql);
			if (!l_pSqlResult->IsValid())
			{
				std::string l_strSQL = l_pSqlResult->GetSQL();
				ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
					l_strViolationSql.sql_id.c_str(),
					l_strSQL.c_str(),
					l_pSqlResult->GetErrorMsg().c_str());

				return;
			}
			else
			{
				ICC_LOG_DEBUG(m_LogPtr, "%s table success,ViolationCount=%d, SQL: [%s]",
					l_strViolationSql.sql_id.c_str(),
					l_nViolationCount,
					l_pSqlResult->GetSQL().c_str());
			}
			//发同步消息
			PROTOCOL::CViolationSync l_CViolationSync;
			l_CViolationSync.m_oHeader.m_strMsgid = m_StringUtilPtr->CreateGuid();
			l_CViolationSync.m_oHeader.m_strCmd = "topic_violation_sync";
			l_CViolationSync.m_oHeader.m_strRequest = "topic_violation";
			l_CViolationSync.m_oHeader.m_strRequestType = "1";//主题
			l_CViolationSync.m_oHeader.m_strSendTime = p_strCurrentTime;

			l_CViolationSync.m_oBody.m_strGUID = l_strGUID;
			l_CViolationSync.m_oBody.m_strTimeOutValue = l_strTimeOutValue;
			l_CViolationSync.m_oBody.m_strVoilationType = p_strVoilationType;
			l_CViolationSync.m_oBody.m_strViolationTarget = l_oCheckObj.second->GetViolationTarget();
			l_CViolationSync.m_oBody.m_strViolationUser = l_oCheckObj.second->GetViolationUserID();
			l_CViolationSync.m_oBody.m_strViolationTime = p_strCurrentTime;
			l_CViolationSync.m_oBody.m_strDeptCode = l_oSeatData.m_strDeptCode;
			l_CViolationSync.m_oBody.m_strViolationID = p_strVoilationID;
			l_CViolationSync.m_oBody.m_strSyncType = "1";

			std::string l_strSyncMessage = l_CViolationSync.ToString(m_JsonFactoryPtr->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

			ICC_LOG_DEBUG(m_LogPtr, "Send Violation Sync: [%s]", l_strSyncMessage.c_str());

			//(测试)推送至消息中心
			std::string l_deptCode = l_oSeatData.m_strDeptCode;
			MessageSendDTOData l_CDTOData;
			l_CDTOData.appCode = "icc";
			l_CDTOData.businessCode = "violation";
			l_CDTOData.compensateType = 0;
			l_CDTOData.delayDuration = "";
			l_CDTOData.delayType = 0;

			if (VIOLATIONCHECK_BUSY == l_nCheckType)
			{
				l_CDTOData.title = BuildViolationMsg(VIOLATIONCHECK_BUSY);
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oCheckObj.second->GetViolationTarget());
			}
			else if (VIOLATIONCHECK_CALL_RING == l_nCheckType)
			{
				l_CDTOData.title = BuildViolationMsg(VIOLATIONCHECK_CALL_RING);
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oCheckObj.second->GetViolationTarget());
			}
			else if (VIOLATIONCHECK_CALL_TALK == l_nCheckType)
			{
				l_CDTOData.title = BuildViolationMsg(VIOLATIONCHECK_CALL_TALK);
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oCheckObj.second->GetViolationTarget());
			}
			ICC_LOG_DEBUG(m_LogPtr, "cur Ip:%s Port:%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());

			l_CDTOData.moduleCode = "violation";
			l_CDTOData.needStorage = "true";
			l_CDTOData.sendType = "0";
			l_CDTOData.sync = "true";

			if (l_oSeatData.m_strDeptCode.empty())
			{
				DataBase::SQLRequest l_strDeptSql;
				l_strDeptSql.sql_id = "select_icc_t_seat_by_no";
				l_strDeptSql.param["no"] = l_oCheckObj.second->GetViolationTarget();

				DataBase::IResultSetPtr l_pSqlResult;
				l_pSqlResult = m_pRequestDBConn->Exec(l_strDeptSql);

				if (!l_pSqlResult->IsValid())
				{
					std::string l_strSQL = l_pSqlResult->GetSQL();
					ICC_LOG_ERROR(m_LogPtr, "sql failed, SQL: [%s], Error: [%s]",
						l_strSQL.c_str(),
						l_pSqlResult->GetErrorMsg().c_str());

					return;
				}
				else if (l_pSqlResult->Next())
				{
					std::string l_strSQL = l_pSqlResult->GetSQL();
					ICC_LOG_DEBUG(m_LogPtr, "sql success,dept_code=%s, SQL: [%s]",
						l_pSqlResult->GetValue("dept_code").c_str(),
						l_pSqlResult->GetSQL().c_str());

					l_deptCode = l_pSqlResult->GetValue("dept_code");
				}
			}

			std::string l_deptGuid = DeptCodeToDeptGuid(l_deptCode);
			std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR");
			if (l_curNoticeList.empty())
			{

				ReceiveObject l_recObj;
				l_recObj.type = "role";
				l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
				l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

				l_CDTOData.receiveObjects.push_back(l_recObj);
			}
			else
			{
				ReceiveObject l_recObj;
				l_recObj.type = "user";

				for (int index = 0; index < l_curNoticeList.size(); index++)
				{
					l_recObj.noticeList.push_back(l_curNoticeList[index]);
					l_recObj.syncList.push_back(l_curNoticeList[index]);
				}

				l_CDTOData.receiveObjects.push_back(l_recObj);
			}

			if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
			{
				ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list");
				return;
			}

			l_CDTOData.message = BuildSendMsg(l_CDTOData);
			std::string l_sendMsg = BuildSendMsg(l_CDTOData);
			m_pMsgCenter->Send(l_sendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
		}
	}

	for (int i = 0; i < l_timeoutKeys.size(); i++)
	{
		m_mViolationCheckList.erase(l_timeoutKeys[i]);
	}
}

bool CViolationManager::GetViolationList(const PROTOCOL::CGetViolationRequest& p_oGetViolationRequest, PROTOCOL::CGetViolationRespond& p_oGetViolationRespond)
{
	DataBase::SQLRequest l_tSQLReq;

	//先查条目
	l_tSQLReq.sql_id = "select_icc_t_violation_count";

	if (!p_oGetViolationRequest.m_oBody.m_strViolationType.empty())
	{
		l_tSQLReq.param["violation_type"] = p_oGetViolationRequest.m_oBody.m_strViolationType;
	}

	l_tSQLReq.param["begin_time"] = p_oGetViolationRequest.m_oBody.m_strStartTime;
	l_tSQLReq.param["end_time"] = p_oGetViolationRequest.m_oBody.m_strEndTime;

	if (!p_oGetViolationRequest.m_oBody.m_strDeptCode.empty())
	{
		l_tSQLReq.param["dept_code"] = p_oGetViolationRequest.m_oBody.m_strDeptCode;
	}
	
	DataBase::IResultSetPtr l_resultCount = m_pRequestDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_LogPtr, "sql:[%s]", l_resultCount->GetSQL().c_str());
	if (!l_resultCount->IsValid())
	{
		ICC_LOG_ERROR(m_LogPtr, "ExecQuery Error ,Error Message :[%s]", l_resultCount->GetErrorMsg().c_str());

		p_oGetViolationRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	std::string tmp_strAllCount;

	if (l_resultCount->Next())
	{
		tmp_strAllCount = l_resultCount->GetValue("num");

		ICC_LOG_DEBUG(m_LogPtr, "Get all monitorapply size[%s]", tmp_strAllCount.c_str());
	}

	//表示没有记录
	if (tmp_strAllCount.empty() || "0" == tmp_strAllCount)
	{
		ICC_LOG_DEBUG(m_LogPtr, "no record");
		return true;
	}

	int l_nPageSize = m_StringUtilPtr->ToInt(p_oGetViolationRequest.m_oBody.m_strPageSize.c_str());
	int l_nPageIndex = m_StringUtilPtr->ToInt(p_oGetViolationRequest.m_oBody.m_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;
	l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;


	l_tSQLReq.sql_id = "select_icc_t_violation";
	l_tSQLReq.param["limit"] = std::to_string(l_nPageSize);
	l_tSQLReq.param["offset"] = std::to_string(l_nPageIndex);
	
	
	l_tSQLReq.param["orderby"] = "violation_time desc";

	DataBase::IResultSetPtr l_result = m_pRequestDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_LogPtr, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_LogPtr, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		p_oGetViolationRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	p_oGetViolationRespond.m_oBody.m_strAllCount = tmp_strAllCount;

	while (l_result->Next())
	{
		PROTOCOL::CGetViolationRespond::CBody::CData l_CData;
		l_CData.m_strGuid = l_result->GetValue("guid");
		l_CData.m_strViolationTime = l_result->GetValue("violation_time");
		l_CData.m_strViolationType = l_result->GetValue("violation_type");
		l_CData.m_strViolationTarget = l_result->GetValue("violation_target");
		l_CData.m_strViolationUser = l_result->GetValue("violation_user");
		l_CData.m_strViolationID = l_result->GetValue("violation_id");
		l_CData.m_strDeptCode = l_result->GetValue("dept_code");
		p_oGetViolationRespond.m_oBody.m_vecData.push_back(l_CData);
	}

	p_oGetViolationRespond.m_oBody.m_strCount = std::to_string(p_oGetViolationRespond.m_oBody.m_vecData.size());

	ICC_LOG_DEBUG(m_LogPtr, "Get success, size[%s]", p_oGetViolationRespond.m_oBody.m_strCount.c_str());

	return true;

}

bool CViolationManager::GetMsgCenterService(std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strMsgCenterServiceName = m_pConfig->GetValue("ICC/Plugin/DBProcess/vcsservicename", "commandcenter-umc-service");
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strMsgCenterServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/DBProcess/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strMsgCenterServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_LogPtr, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_LogPtr, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strMsgCenterServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_LogPtr, "find healthy msg center service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, "not find healthy msg center service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

bool CViolationManager::MessageSendDTO(std::string l_sendMsg)
{
	std::string strTarget = "/msg/send";
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;

	std::string m_strMsgCenterServerIp;
	std::string m_strMsgCenterServerPort;

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

	GetMsgCenterService(m_strMsgCenterServerIp, m_strMsgCenterServerPort);

	ICC_LOG_DEBUG(m_LogPtr, "send message to msg center: %s", l_sendMsg.c_str());

	std::string strReceive = m_pHttpClient->PostEx(m_strMsgCenterServerIp, m_strMsgCenterServerPort, strTarget, mapHeaders, l_sendMsg, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "post msg center error!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "get message from msg center: %s", strReceive.c_str());
	}

	return true;
}

std::string CViolationManager::BuildViolationMsg(int l_violationcheckType)
{
	std::string strName;
	if (l_violationcheckType == VIOLATIONCHECK_BUSY)
	{
		strName = "BusyOverTime";
	}
	else if (l_violationcheckType == VIOLATIONCHECK_CALL_RING)
	{
		strName = "CallRingOverTime";
	}
	else if (l_violationcheckType == VIOLATIONCHECK_CALL_TALK)
	{
		strName = "CallTalkOverTime";
	}

	std::string strParamInfo;
	if (!m_pRedisClient->HGet(Param_INFO, strName, strParamInfo))
	{
		ICC_LOG_DEBUG(m_LogPtr, "HGet Param INFO Failed!!!");
		return "";
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	JsonParser::IJsonPtr l_pJson = m_JsonFactoryPtr->CreateJson();
	if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Parse Param Info failed!!!");
		return "";
	}

	return l_ParamInfo.m_strValue;
}

std::string CViolationManager::BuildSendMsg(MessageSendDTOData l_sendData)
{
	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();

	pJson->SetNodeValue("/appCode", l_sendData.appCode);
	pJson->SetNodeValue("/businessCode", l_sendData.businessCode);
	pJson->SetNodeValue("/compensateType", l_sendData.compensateType);
	pJson->SetNodeValue("/delayDuration", l_sendData.delayDuration);
	pJson->SetNodeValue("/delayType", l_sendData.compensateType);
	pJson->SetNodeValue("/moduleCode", l_sendData.moduleCode);
	pJson->SetNodeValue("/needStorage", l_sendData.needStorage);
	pJson->SetNodeValue("/sendType", l_sendData.sendType);
	pJson->SetNodeValue("/sync", l_sendData.sync);
	pJson->SetNodeValue("/title", l_sendData.title);
	pJson->SetNodeValue("/message", l_sendData.message);

	unsigned int l_curIndex = 0;
	for (std::vector<ReceiveObject>::iterator it = l_sendData.receiveObjects.begin(); it != l_sendData.receiveObjects.end(); it++)
	{
		JsonParser::IJsonPtr l_pIJson = m_JsonFactoryPtr->CreateJson();
		unsigned int l_iIndex = 0;
		for (auto it_notice = it->noticeList.cbegin(); it_notice != it->noticeList.cend(); it_notice++)
		{
			pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/noticeList/" + std::to_string(l_iIndex), *it_notice);
			l_iIndex++;
		}

		l_iIndex = 0;
		for (auto it_notice = it->syncList.cbegin(); it_notice != it->syncList.cend(); it_notice++)
		{
			pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/syncList/" + std::to_string(l_iIndex), *it_notice);
			l_iIndex++;
		}

		pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/type", it->type);
		l_curIndex++;
		//pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex++), l_pIJson->ToString());
	}
	
	return pJson->ToString();
}

void CViolationManager::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

std::string CViolationManager::DeptCodeToDeptGuid(std::string l_deptCode)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_dept_by_code";
	l_strDeptSql.param["code"] = l_deptCode;
	DataBase::IResultSetPtr l_sqlResult = m_pRequestDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return "";
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_LogPtr, "sql success,SQL: [%s]",
			l_sqlResult->GetSQL().c_str());

		return l_sqlResult->GetValue("guid");
	}

	return "";
}

std::vector<std::string> CViolationManager::GetGuidbByDept(std::string l_orgGuid, std::string l_roleAttr)
{
	std::vector<std::string> l_resDatas;
	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	std::string l_sendMsg;

	pJson->SetNodeValue("/orgGuid", l_orgGuid);
	pJson->SetNodeValue("/roleAttr", l_roleAttr);
	pJson->SetNodeValue("/orgType", "belong");

	l_sendMsg = pJson->ToString();

	std::string strTarget = "/basic/staff/queryGuidByOrgGuidAndRoleType?orgGuid=" + l_orgGuid + "&roleAttr=" + l_roleAttr + "&orgType=belong";
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;

	std::string m_strSMPServerIp;
	std::string m_strSMPServerPort;

	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));

	GetSmpService(m_strSMPServerIp, m_strSMPServerPort, "center-smp-northface-service");

	ICC_LOG_DEBUG(m_LogPtr, "%s, %s, send message to smp service: %s", l_orgGuid.c_str(), l_roleAttr.c_str(), l_sendMsg.c_str());

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strSMPServerIp, m_strSMPServerPort, strTarget, mapHeaders, l_sendMsg, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "post smp service error!!!!err[%s]", strErrorMessage.c_str());
		return l_resDatas;
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "get message from smp service: %s", strReceive.c_str());

		JsonParser::IJsonPtr l_pJson = m_JsonFactoryPtr->CreateJson();
		l_pJson->LoadJson(strReceive);

		int iCount = l_pJson->GetCount("/data");

		for (int i = 0; i < iCount; i++)
		{
			std::string l_strPrefixPath("/data/" + std::to_string(i));

			l_resDatas.push_back(l_pJson->GetNodeValue(l_strPrefixPath, ""));

			ICC_LOG_DEBUG(m_LogPtr, "get message: %s", l_pJson->GetNodeValue(l_strPrefixPath, "").c_str());
		}

		return l_resDatas;
	}
}

bool CViolationManager::GetSmpService(std::string& p_strIp, std::string& p_strPort, std::string l_serverName)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strMsgCenterServiceName = l_serverName;
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strMsgCenterServiceHealthyFlag = "1";

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strMsgCenterServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_LogPtr, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_LogPtr, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strMsgCenterServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_LogPtr, "find healthy msg center service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, "not find healthy msg center service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}
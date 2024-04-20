#include "Boost.h"
#include "BusinessImpl.h"
#include "Protocol/CWebSignRequest.h"
#include "Protocol/SyncNacosParams.h"
#include "DeptInfo.h"

#include <fstream>
#define CODE_SUCCESS "200"
void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_VCS_OBSERVER_CENTER);

	m_pObserverCenterProcessVcsMessage = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_VCS_OBSERVER_CENTER_PROCESS_RECV_VCS_MESSAGE);

	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_LockFacPtr = ICCGetILockFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();

	m_pJsonFty = ICCGetIJsonFactory();

	// l_strCallExpire单位是秒，m_uCallExpire单位是微秒
	std::string l_strCallExpire = m_pConfig->GetValue("ICC/Plugin/VCS/CallExpire", "300");
	m_uCallExpire = m_pString->ToInt64(l_strCallExpire) * 1e6L;

	//m_bStartedTimerFlag = false;
	m_bFirstVcsSynFlag = true;
	m_strVcsSyncStartTime = "";
	m_bIsVcsSyning = false;
	m_bVcsSynSuccess = false;

	m_nVcsSyncUpdateFlag = 0;
	m_nFirstSyncDaysAgo = 30;
	m_iPackageSize = 100;

	m_bStartedTimerFlag = false;
}

void CBusinessImpl::_GetVcsSyncConfig()
{
	m_strServiceID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "");

	m_strNacosServerIp = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerIp", "127.0.0.1");
	m_strNacosServerPort = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerPort", "8848");
	m_strNacosNamespace = m_pConfig->GetValue("ICC/Component/HttpServer/NacosNamespace", "dev");
	m_strVcsServiceName = m_pConfig->GetValue("ICC/Plugin/VCS/vcsservicename", "commandcenter-vcs-incident");
	m_strNacosGroupName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosGroupName", "master");
	m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	//m_strNacosSuccessFlag = m_pConfig->GetValue("ICC/Component/HttpServer/successflag", "ok");
	m_strVcsServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/VCS/servicehealthyflag", "1");

	std::string strPackageSize = m_pConfig->GetValue("ICC/Plugin/VCS/syndatapackagesize", "");
	if (strPackageSize.empty())
	{
		strPackageSize = "100";
	}
	m_iPackageSize = std::stoi(strPackageSize);

	//是否补偿更新的数据,默认只对新增加的数据进行补偿
	std::string strSyncUpdateFlag = m_pConfig->GetValue("ICC/Plugin/VCS/VcsSyncUpdateFlag", "0");
	if (strSyncUpdateFlag.empty())
	{
		strSyncUpdateFlag = "0";
	}
	m_nVcsSyncUpdateFlag = std::stoi(strSyncUpdateFlag);

	m_strIgnoreHttpError = m_pConfig->GetValue("ICC/Plugin/VCS/ignorehttperror", "0");

	std::string l_strFirstSyncDaysAgo = m_pConfig->GetValue("ICC/Plugin/VCS/FirstVcsSyncDaysAgo", "1");
	if (l_strFirstSyncDaysAgo.empty())
	{
		strSyncUpdateFlag = "1";
	}
	m_nFirstSyncDaysAgo = std::stoi(l_strFirstSyncDaysAgo);

	std::string l_strVcsSyncFlag = m_pConfig->GetValue("ICC/Plugin/VCS/VcsSyncStartFlag", "0");

	m_bEnbleVcsSyncFlag = false;
	if (l_strVcsSyncFlag == "1")
	{
		m_bEnbleVcsSyncFlag = true;
	}

	std::string l_strVcsSyncInterval = m_pConfig->GetValue("ICC/Plugin/VCS/VcsSyncInterval", "30");  //补偿间隔时间
	std::string strSynCheckInterval = m_pConfig->GetValue("ICC/Plugin/VCS/SynCheckInterval", "60");  //定时器定时触发
	ICC_LOG_DEBUG(m_pLog, "read vcs sync config,enbleVcsSyncFlag=%s,SyncUpdateFlag=%d,Timer=%s,VcsSyncInterval=%s,FirstSyncDaysAgo=%d,PackageSize=%d",
		l_strVcsSyncFlag.c_str(),
		m_nVcsSyncUpdateFlag,
		strSynCheckInterval.c_str(),
		l_strVcsSyncInterval.c_str(),
		m_nFirstSyncDaysAgo, m_iPackageSize);

	m_nTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/VCS/TimeOut", "3"));

	m_pMsgCenter->Start();
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/VCS/IsUsing", "1");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}
	m_queueToVCS = "queue_alarm_to_vcs";//警情队列名称，发送到VCS
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_server_lock", OnVcsNotifiDoubleServerSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", SendAlarmChange);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_process_sync", SendProcessChange);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_feedback_sync", SendFeedBackChange);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_call_state_sync", SaveCallInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "device_state_sync", SendCallInfoChange);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_info", RecvAlarmUpdate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_log_sync", RecvAlarmLogSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "admin_dept_request", OnAdminDeptRequset); //icc
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_temporary_hold_request", OnSetTemporaryHoldRequest);
	
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "feedback_info", RecvFeedBackUpdate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "device_info", RecvDeviceUpdate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "textmsg_info", RecvTextMsgUpdate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "record_info", RecvRecordUpdate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "CASEINFO_VCSREQUEST", AlarmInfoRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "CASEDISPOSAL_VCSREQUEST", AlarmProcessInfoRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "CASEFEEDBACK_VCSREQUEST", AlarmFeedBackInfoRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "forward_remark_request", SendRemarkToFeedBack);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "gps_sync", SendAlarmPosition);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_gps_respond", GetGpsRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, VCS_TIMER, OnTimer);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "caller_info_sync_request", SendAlarmCallerInfoSync);

	// 附件信息同步  icc -> vcs
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_attach_syn", OnNotifiAlarmAttachSyn);
	//VCS->ICC 同步附件
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "topic_alarm_attach_sync", OnVcsAlarmAttachSync);
	
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_case", RecvAlarmUpdateCase);  //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_postion", RecvUpdatePostion); //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_masterorg", RecvUpdateMasterorg);//vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_disposal", RecvUpdateDisposal); //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "delete_disposal", RecvDeleteDisposal); //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "topic_bll_status_sync", OnRecvVcsBllStatusSync); //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_linked_dispatch", RecvUpdateLinkedDispatch); //vcs

	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_feedback", RecvUpdateFeedback);//vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "notify_incomingcall", RecvNotifyIncomingCall); //vcs
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_log", OnRecvUpdateLog); //vcs
	
	ADDOBSERVER(m_pObserverCenterProcessVcsMessage, CBusinessImpl, "update_involved_party", OnRecvUpdateCallerInfo); //vcs

	//
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqsacl_request", OnVcsAlarmRelatedCarsRequest); //vcs
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqsacl_request", OnVcsAlarmRelatedCarsRequest); //vcs
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqsacl_request", OnVcsAlarmRelatedCarsRequest);//vcs

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqdsr_request", OnVcsAlarmRelatedPersonsRequest);//vcs
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqdsr_request", OnVcsAlarmRelatedPersonsRequest);//vcs
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqdsr_request", OnVcsAlarmRelatedPersonsRequest);//vcs

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "topic_jqsacl_sync", OnRecvAlarmRelatedCarsSync); //vcs + icc
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "topic_jqdsr_sync", OnRecvAlarmRelatedPersonsSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_vcs_data_request", OnRecvSyncVcsDataRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "vcs_sync_timer", OnTimerVcsSyncRequestInfo);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "linked_dispatch_sync", OnReceiveSynLinkedDispatch);

	_GetVcsSyncConfig();

	std::string l_strTimerName = m_pTimerMgr->AddTimer(VCS_TIMER, 60, 10);
	int nRet = m_pRedisClient->Del(ACDCALLSTATEKEY);
	m_mCallRefId.clear();
	ICC_LOG_DEBUG(m_pLog, "Del return %d", nRet);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	_SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::_SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosNamespace = strNameSpace;
	m_strNacosGroupName = strGroupName;
}

void CBusinessImpl::_GetNacosParams(std::string& strNacosIp, std::string& strNacosPort, std::string& strNameSpace, std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	strNacosIp = m_strNacosServerIp;
	strNacosPort = m_strNacosServerPort;
	strNameSpace = m_strNacosNamespace;
	strGroupName = m_strNacosGroupName;
}


void CBusinessImpl::OnReceiveSynLinkedDispatch(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string p_strMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received OnReceiveSynLinkedDispatch From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmLinkedSync l_oAlarmLinkedSync;
	if (!l_oAlarmLinkedSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	PROTOCOL::CAlarmLinkedSync l_CProcessInfoChange;
	GenRespondHeader(CMD_LINKED_DISPATCH, l_oAlarmLinkedSync.m_oHeader, l_CProcessInfoChange.m_oHeader);
	l_CProcessInfoChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CProcessInfoChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;
	l_CProcessInfoChange.m_oBody = l_oAlarmLinkedSync.m_oBody;
	std::string l_strMsg = l_CProcessInfoChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "OnReceiveSynLinkedDispatch send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnVcsNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncServerLock l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	std::string strServerId(m_pConfig->ServerFlag());
	if (l_oSyncObj.m_oBody.m_strServiceID != m_strServiceID)
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "Server Role Sync,Config ServerFalg:%s, clientid:%s", strServerId.c_str(), l_oSyncObj.m_oBody.m_strClientID.c_str());
	if (l_oSyncObj.m_oBody.m_strServiceState.compare("master") == 0)
	{
		std::string l_strVcsSyncFlag = m_pConfig->GetValue("ICC/Plugin/VCS/VcsSyncStartFlag", "0");
		if (l_strVcsSyncFlag != "1")
		{
			return;
		}

		if (l_oSyncObj.m_oBody.m_strClientID == strServerId)
		{
			////同步间隔时间,当前时间之前多少秒的数据
			std::string l_strVcsSyncInterval = m_pConfig->GetValue("ICC/Plugin/VCS/VcsSyncInterval", "30");
			if (l_strVcsSyncInterval.empty())
			{
				l_strVcsSyncInterval = "0";
			}
			m_nVcsSyncInterval = std::stoi(l_strVcsSyncInterval);

			if (!m_bStartedTimerFlag)
			{
				m_bStartedTimerFlag = true;
				std::string strSynCheckInterval = m_pConfig->GetValue("ICC/Plugin/VCS/SynCheckInterval", "60");
				if (strSynCheckInterval.empty())
				{
					strSynCheckInterval = "60";
				}
				int iSynCheckInterval = std::stoi(strSynCheckInterval);
				std::string l_strRequestInfoTimerName = m_pTimerMgr->AddTimer("vcs_sync_timer", iSynCheckInterval, 3);
				ICC_LOG_DEBUG(m_pLog, "start vcs sync timer complete,SynCheckInterval=%d,VcsSyncInterval=%d!", iSynCheckInterval, m_nVcsSyncInterval);
			}
		}
		else
		{
			m_nVcsSyncInterval = 0; //为零，不去定时补偿VCS数据
		}
	}
	else
	{
		m_nVcsSyncInterval = 0; //为零，不去定时补偿VCS数据
	}
}

void CBusinessImpl::OnTimerVcsSyncRequestInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "OnTimerVcsSyncRequestInfo---------!");
	if (m_bFirstVcsSynFlag)
	{
		m_bFirstVcsSynFlag = false;
		ICC_LOG_DEBUG(m_pLog, "server start, first vcs sync data!");
		m_strVcsSyncStartTime = "";
		RequestVcsSyncFunc();
	}
	else
	{
		if (!m_bVcsSynSuccess)
		{
			ICC_LOG_DEBUG(m_pLog, "server start, first vcs syn not success!!! will start vcs syn data!");
			RequestVcsSyncFunc();
		}
		else
		{
			if (m_nVcsSyncInterval > 0)
			{
				std::string strCurDate = m_pDateTime->CurrentDateTimeStr();
				m_strVcsSyncStartTime = m_pDateTime->ToString(m_pDateTime->AddSeconds(m_pDateTime->CurrentDateTime(), m_nVcsSyncInterval * (-1)));

				ICC_LOG_DEBUG(m_pLog, "vcs sync current time [%s],IntervalTime:%s", strCurDate.c_str(), m_strVcsSyncStartTime.c_str());
				RequestVcsSyncFunc();
			}
		}
	}
}

void CBusinessImpl::SendAlarmChange(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received AlarmChange From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmSync l_CAlarmSync;

	if (!l_CAlarmSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	//if (l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource == MSG_SOURCE_VCS || !l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strMergeID.empty())
	if (l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource == MSG_SOURCE_VCS) //合并的警情，通知VCS  2022-7-14
	{ // VCS传过来的更新，合并的警情，不再通知VCS
		return;
	}

	if ("vcs_relocated" == l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource)
	{
		// VCS传过来的更新
		return;
	}

	PROTOCOL::CAlarmInfoChange l_CAlarmInfoChange;
	l_CAlarmInfoChange.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CAlarmInfoChange.m_oHeader.m_strRelatedID = l_CAlarmInfoChange.m_oHeader.m_strMsgid;
	l_CAlarmInfoChange.m_oHeader.m_strCmd = CMD_UPDATE_CASE/*CASEINFO_CHANGED*/;
	l_CAlarmInfoChange.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmInfoChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_CAlarmInfoChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;

	CallEventInfo l_tCallEventInfo;
	QueryCallevent(l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strSourceID, l_tCallEventInfo);

	PROTOCOL::CAlarmInfoChange::CBody::CData l_oAlarmInfo;
	l_oAlarmInfo.m_oAlarm = l_CAlarmSync.m_oBody.m_oAlarmInfo;

	//std::string l_strCallNo = l_CAlarmSync.m_oBody.m_strCalledNoType;
	//l_oAlarmInfo.m_strID = l_CAlarmSync.m_oBody.m_strID;
	//l_oAlarmInfo.m_strTitle = l_CAlarmSync.m_oBody.m_strTitle;
	//l_oAlarmInfo.m_strMerge_ID = l_CAlarmSync.m_oBody.m_strMergeID;
	//l_oAlarmInfo.m_strCallerNo = l_CAlarmSync.m_oBody.m_strCallerNo;
	//l_oAlarmInfo.m_strCalledNo = l_strCallNo;/*(l_strCallNo == POLICE_TYPE_110) ? POLICE_NO_110 : ((l_strCallNo == POLICE_TYPE_122) ? POLICE_NO_122 : POLICE_NO_119);*/
	//l_oAlarmInfo.m_strCalledNoType = l_strCallNo;
	//l_oAlarmInfo.m_strCallingTime = l_CAlarmSync.m_oBody.m_strTime;
	l_oAlarmInfo.m_oAlarm.m_strReceivedTime = l_tCallEventInfo.m_strAnsweRingTime;

	//l_oAlarmInfo.m_strHandleType = l_CAlarmSync.m_oBody.m_strHandleType;
	//屏蔽接警席位号的重新赋值20230308
	/*if (!l_tCallEventInfo.m_strSeatNo.empty())
	{
		l_oAlarmInfo.m_oAlarm.m_strSeatNo = l_tCallEventInfo.m_strSeatNo;
	}*/		

	/*l_oAlarmInfo.m_strContact = l_CAlarmSync.m_oBody.m_strContactName;
	l_oAlarmInfo.m_strContactNo = l_CAlarmSync.m_oBody.m_strContactNo;
	l_oAlarmInfo.m_strContactSex = l_CAlarmSync.m_oBody.m_strContactGender;
	l_oAlarmInfo.m_strCaseDestrict = l_CAlarmSync.m_oBody.m_strAdminDeptDistrictCode;
	l_oAlarmInfo.m_strAdminDeptCode = l_CAlarmSync.m_oBody.m_strAdminDeptCode;
	l_oAlarmInfo.m_strCaseAddress = l_CAlarmSync.m_oBody.m_strAddr;
	l_oAlarmInfo.m_strLongitude = l_CAlarmSync.m_oBody.m_strLongitude;
	l_oAlarmInfo.m_strLatitude = l_CAlarmSync.m_oBody.m_strLatitude;
	l_oAlarmInfo.m_strCaseTime = l_CAlarmSync.m_oBody.m_strActualOccurTime;
	l_oAlarmInfo.m_strCaseType = l_CAlarmSync.m_oBody.m_strFirstType;
	l_oAlarmInfo.m_strCaseSubType = l_CAlarmSync.m_oBody.m_strSecondType;
	l_oAlarmInfo.m_strCaseThreeType = l_CAlarmSync.m_oBody.m_strThirdType;
	l_oAlarmInfo.m_strCaseSource = l_CAlarmSync.m_oBody.m_strSourceType;
	l_oAlarmInfo.m_strCaseLevel = l_CAlarmSync.m_oBody.m_strLevel;
	l_oAlarmInfo.m_strCaseDesc = l_CAlarmSync.m_oBody.m_strContent;
	l_oAlarmInfo.m_strCreateTime = l_CAlarmSync.m_oBody.m_strCreateTime;
	l_oAlarmInfo.m_strCreateUser = l_CAlarmSync.m_oBody.m_strCreateUser;
	l_oAlarmInfo.m_strUpdateTime = l_CAlarmSync.m_oBody.m_strUpdateTime;
	l_oAlarmInfo.m_strUpdateUser = l_CAlarmSync.m_oBody.m_strUpdateUser;*/

	//如果是同步删除，赋值被删除
	if ("3" == l_CAlarmSync.m_oBody.m_strSyncType)
	{
		l_oAlarmInfo.m_oAlarm.m_strDeleteFlag = "1";
	}
	else
	{
		l_oAlarmInfo.m_oAlarm.m_strDeleteFlag = "0";
	}
	/*l_oAlarmInfo.m_strStatus = l_CAlarmSync.m_oBody.m_strState;

	l_oAlarmInfo.m_strReceiptDeptCode = l_CAlarmSync.m_oBody.m_strReceiptDeptCode;
	l_oAlarmInfo.m_strRecordId = l_CAlarmSync.m_oBody.m_strSourceID;
	l_oAlarmInfo.m_strReceiptCode = l_CAlarmSync.m_oBody.m_strReceiptCode;
	l_oAlarmInfo.m_strReceiptName = l_CAlarmSync.m_oBody.m_strReceiptName;
	l_oAlarmInfo.m_strCommitTime = l_CAlarmSync.m_oBody.m_strTime;
	l_oAlarmInfo.m_strPhoneName = l_CAlarmSync.m_oBody.m_strCallerName;
	l_oAlarmInfo.m_strPhoneId = l_CAlarmSync.m_oBody.m_strCallerID;
	l_oAlarmInfo.m_strPhoneAddr = l_CAlarmSync.m_oBody.m_strCallerAddr;
	l_oAlarmInfo.m_strCallerName = l_CAlarmSync.m_oBody.m_strCallerName;
	l_oAlarmInfo.m_strCallerId = l_CAlarmSync.m_oBody.m_strCallerID;
	l_oAlarmInfo.m_strCallerGender = l_CAlarmSync.m_oBody.m_strCallerGender;
	l_oAlarmInfo.m_strAdminDeptCode = l_CAlarmSync.m_oBody.m_strAdminDeptCode;
	l_oAlarmInfo.m_strAdminDeptName = l_CAlarmSync.m_oBody.m_strAdminDeptName;*/
	//l_oAlarmInfo.m_strSignNum = l_CAlarmSync.m_oBody.m_strSignNum;
	//mpa退单特殊处理，方便mpa刷新警单列表
	if (l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource == "mpa" && l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strState == "02")
	{
		l_oAlarmInfo.m_oAlarm.m_strState = "DIC020073";
	}
	
	//增加警情处置信息
	std::vector<Process> tmp_vecProcess;
	if (QueryAlarmProcessByAlarmID(l_oAlarmInfo.m_oAlarm.m_strID, tmp_vecProcess))
	{
		ICC_LOG_DEBUG(m_pLog, "query alram id: %s, process size: %d", l_oAlarmInfo.m_oAlarm.m_strID.c_str(), tmp_vecProcess.size());

		for (Process tmp_oProccedata : tmp_vecProcess)
		{
			PROTOCOL::CAddOrUpdateProcessRequest::CProcessData tmp_oData;
			tmp_oData = tmp_oProccedata.m_ProcessData;
			l_oAlarmInfo.m_vecProcessData.push_back(tmp_oData);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Query alarm processby alarmid failed, alarm id:[%s]", l_oAlarmInfo.m_oAlarm.m_strID.c_str());
	}

	l_CAlarmInfoChange.m_oBody.m_vecData.push_back(l_oAlarmInfo);

	std::string l_strMsg = l_CAlarmInfoChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SendProcessChange(ObserverPattern::INotificationPtr p_pRequest)
{
	return;
	//警情变更已经通知，后面的不发
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received ProcessChange From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmProcessSync l_CAlarmProcessSync;

	if (!l_CAlarmProcessSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	if (!l_CAlarmProcessSync.m_oBody.m_strMsgSource.compare(MSG_SOURCE_VCS))
	{
		return;
	}

	PROTOCOL::CProcessInfoChange l_CProcessInfoChange;
	GenRespondHeader(CMD_UPDATE_DISPOSAL, l_CAlarmProcessSync.m_oHeader, l_CProcessInfoChange.m_oHeader);
	l_CProcessInfoChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CProcessInfoChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;

	CallEventInfo l_tCallEventInfo;
	PROTOCOL::CAlarmInfo l_tAlarmInfo;

	if (!QueryAlarm(l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strAlarmID, l_tAlarmInfo))
	{
		ICC_LOG_ERROR(m_pLog, "QueryAlarm Fail ! AlarmID: [%s]", l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strAlarmID.c_str());
		return;
	}
	QueryCallevent(l_tAlarmInfo.m_strSourceID, l_tCallEventInfo);

	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_oProcessData;
	l_oProcessData = l_CAlarmProcessSync.m_oBody.m_ProcessData;
	if (l_tCallEventInfo.m_strSeatNo.empty())
	{
		l_oProcessData.m_strSeatCode = l_tAlarmInfo.m_strSeatNo;
	}
	else
	{
		l_oProcessData.m_strSeatCode = l_tCallEventInfo.m_strSeatNo;
	}
	//l_oProcessData.m_strDispatchDeptCode = l_CAlarmProcessSync.m_oBody.m_strDispatchDeptCode;
	//l_oProcessData.m_strProcessCode = l_CAlarmProcessSync.m_oBody.m_strProcessCode;
	l_oProcessData.m_strDelFlag = "0";
	l_oProcessData.m_strProcessCode = "";
	if (l_CAlarmProcessSync.m_oBody.m_strMsgSource == "mpa")
	{
		l_oProcessData.m_strProcessCode = l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strProcessCode;
	}
	l_CProcessInfoChange.m_oBody.m_vecData.push_back(l_oProcessData);

	std::string l_strMsg = l_CProcessInfoChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SendFeedBackChange(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received FeedBackChange From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CFeedBackSync l_CFeedBackSync;

	if (!l_CFeedBackSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	PROTOCOL::CFeedBackInfoChange l_CFeedBackInfoChange;
	GenRespondHeader(CMD_UPDATE_FEEDBACK, l_CFeedBackSync.m_oHeader, l_CFeedBackInfoChange.m_oHeader);
	l_CFeedBackInfoChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CFeedBackInfoChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;

	PROTOCOL::CFeedBackInfoChange::CBody::CData l_oFeedBackData;
	l_oFeedBackData.m_strID = l_CFeedBackSync.m_oBody.m_strID;
	l_oFeedBackData.m_strCaseId = l_CFeedBackSync.m_oBody.m_strAlarmID;
	l_oFeedBackData.m_strDisPosalId = l_CFeedBackSync.m_oBody.m_strProcessID;
	l_oFeedBackData.m_strCaseDispatchStatus = l_CFeedBackSync.m_oBody.m_strResultType;

	l_oFeedBackData.m_strFBContent = l_CFeedBackSync.m_oBody.m_strResultContent;
	l_oFeedBackData.m_strFeedBacker = l_CFeedBackSync.m_oBody.m_strFeedbackName;
	l_oFeedBackData.m_strFeedBackerOrg = l_CFeedBackSync.m_oBody.m_strFeedbackDeptCode;
	l_oFeedBackData.m_strFBTime = l_CFeedBackSync.m_oBody.m_strTimeSigned;

	l_oFeedBackData.m_strCaseType = l_CFeedBackSync.m_oBody.m_strAlarmFirstType;
	l_oFeedBackData.m_strCaseSubType = l_CFeedBackSync.m_oBody.m_strAlarmSecondType;
	l_oFeedBackData.m_strCaseThreeType = l_CFeedBackSync.m_oBody.m_strAlarmThirdType;
	l_oFeedBackData.m_strCaseProcessor = l_CFeedBackSync.m_oBody.m_strProcessName;
	l_oFeedBackData.m_strCreateTime = l_CFeedBackSync.m_oBody.m_strCreateTime;
	l_oFeedBackData.m_strCreateUser = l_CFeedBackSync.m_oBody.m_strCreateUser;
	l_oFeedBackData.m_strUpdateTime = l_CFeedBackSync.m_oBody.m_strUpdateTime;
	l_oFeedBackData.m_strUpdateUser = l_CFeedBackSync.m_oBody.m_strUpdateUser;
	l_oFeedBackData.m_strDelFlag = "0";

	l_CFeedBackInfoChange.m_oBody.m_vecData.push_back(l_oFeedBackData);

	std::string l_strMsg = l_CFeedBackInfoChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


void CBusinessImpl::SendAlarmCallerInfoSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSearchCallerRequest l_oCallerInfoRequest;
	ICC_LOG_DEBUG(m_pLog, "Received AlarmCallerInfoSync From ICC: [%s]", p_strMsg.c_str());
	if (!l_oCallerInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Received AlarmCallerInfoSync From ICC:  failed");
		return;
	}
	PROTOCOL::CSearchCallerRequest l_CAlarmRelatedCarsChangeSync;
	l_CAlarmRelatedCarsChangeSync = l_oCallerInfoRequest;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strCmd = "update_involved_party";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestType = "1";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";

	//同步消息
	std::string l_strMsg(l_CAlarmRelatedCarsChangeSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


void CBusinessImpl::OnRecvUpdateCallerInfo(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSearchCallerRequest l_oCallerInfoRequest;
	ICC_LOG_DEBUG(m_pLog, "Received AlarmCallerInfoSync From VCS: [%s]", p_strMsg.c_str());
	if (!l_oCallerInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Received AlarmCallerInfoSync From ICC:  failed");
		return;
	}
	PROTOCOL::CSearchCallerRequest l_CAlarmRelatedCarsChangeSync;
	l_CAlarmRelatedCarsChangeSync = l_oCallerInfoRequest;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRelatedID = "";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strCmd = "update_caller_info_request";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestType = "0";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";

	//同步消息
	std::string l_strMsg(l_CAlarmRelatedCarsChangeSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


void CBusinessImpl::SendCallInfoChange(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CDeviceStateEvent l_CDeviceStateEvent;
	ICC_LOG_DEBUG(m_pLog, "Received CallInfoChange From ICC: [%s]", p_strMsg.c_str());
	if (!l_CDeviceStateEvent.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	//挂机状态
	if ("hangupstate" == l_CDeviceStateEvent.m_oBody.m_strState)
	{
		PROTOCOL::CTelHangupRespond l_CTelHangupRespond;

		std::map<std::string, std::string> l_mapACDCallState;
		m_pRedisClient->HGetAll(ACDCALLSTATEKEY, l_mapACDCallState);
		auto it = l_mapACDCallState.find(l_CDeviceStateEvent.m_oBody.m_strCallRefId);
		if (it != l_mapACDCallState.end())
		{
			ACDCallState l_ACDCallState;
			l_ACDCallState.ParseString(it->second, ICCGetIJsonFactory()->CreateJson());

			l_CTelHangupRespond.m_oBody.m_strCaseID = l_ACDCallState.m_strAlarmID;
		}

		GenRespondHeader("notify_hangupcall", l_CDeviceStateEvent.m_oHeader, l_CTelHangupRespond.m_oHeader);
		l_CTelHangupRespond.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
		l_CTelHangupRespond.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;

		l_CTelHangupRespond.m_oBody.m_strSeatNo = l_CDeviceStateEvent.m_oBody.m_strDevice;
		l_CTelHangupRespond.m_oBody.m_strCallerId = l_CDeviceStateEvent.m_oBody.m_strCallerId;
		l_CTelHangupRespond.m_oBody.m_strCalledId = l_CDeviceStateEvent.m_oBody.m_strCalledId;
		l_CTelHangupRespond.m_oBody.m_strHangupTime = l_CDeviceStateEvent.m_oBody.m_strTime;
		l_CTelHangupRespond.m_oBody.m_strCallID = l_CDeviceStateEvent.m_oBody.m_strCallRefId;
		//l_CTelHangupRespond.m_oBody.m_strCaseID = l_ACDCallState.m_strAlarmID;
		std::string l_strMsg = l_CTelHangupRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());

		m_pRedisClient->HDel(ACDCALLSTATEKEY, l_CDeviceStateEvent.m_oBody.m_strCallRefId);
	}
/*
	else  if ("talkstate" == l_CDeviceStateEvent.m_oBody.m_strState)
	{
		ACDCallState l_ACDCallState;
		std::map<std::string, std::string> l_mapACDCallState;
		m_pRedisClient->HGetAll(ACDCALLSTATEKEY, l_mapACDCallState);
		auto it = l_mapACDCallState.find(l_CDeviceStateEvent.m_oBody.m_strCallRefId);
		if (it == l_mapACDCallState.end())
		{
			l_ACDCallState.m_strCallRefId = l_CDeviceStateEvent.m_oBody.m_strCallRefId;
			l_ACDCallState.m_strAnsweringtTime = l_CDeviceStateEvent.m_oBody.m_strTime;
			l_ACDCallState.m_strSeatNo = l_CDeviceStateEvent.m_oBody.m_strDevice;

			l_ACDCallState.m_strCallerNum = l_CDeviceStateEvent.m_oBody.m_strCallerId;
			l_ACDCallState.m_strCalledNum = l_CDeviceStateEvent.m_oBody.m_strCalledId;
			std::string l_strVal = l_ACDCallState.ToString(ICCGetIJsonFactory()->CreateJson());
			bool bRes = m_pRedisClient->HSet(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId, l_strVal);

			ICC_LOG_ERROR(m_pLog, "No ACDCallState,AddRedis ! CallrefID : [%s],res=%d", l_CDeviceStateEvent.m_oBody.m_strCallRefId.c_str(), bRes);
		}
		else
		{
			l_ACDCallState.ParseString(it->second, ICCGetIJsonFactory()->CreateJson());
			l_ACDCallState.m_strAnsweringtTime = l_CDeviceStateEvent.m_oBody.m_strTime;

			std::string l_strVal = l_ACDCallState.ToString(ICCGetIJsonFactory()->CreateJson());
			bool bRes = m_pRedisClient->HSet(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId, l_strVal);
		}

		//
	}
*/

#if 0
	//通话态
	if ("talkstate" != l_CDeviceStateEvent.m_oBody.m_strState)
	{
		return;
	}
	//坐席分机号与被叫一致
	if (l_CDeviceStateEvent.m_oBody.m_strDevice != l_CDeviceStateEvent.m_oBody.m_strCalledId)
	{
		return;
	}

	
	std::map<std::string, std::string> l_mapACDCallState;
	m_pRedisClient->HGetAll(ACDCALLSTATEKEY, l_mapACDCallState);
	auto it = l_mapACDCallState.find(l_CDeviceStateEvent.m_oBody.m_strCallRefId);
	if (it == l_mapACDCallState.end())
	{
		ICC_LOG_ERROR(m_pLog, "No ACDCallState ! CallrefID : [%s]", l_CDeviceStateEvent.m_oBody.m_strCallRefId.c_str());
		return;
	}

	ACDCallState l_ACDCallState;
	l_ACDCallState.ParseString(it->second, ICCGetIJsonFactory()->CreateJson());
	PROTOCOL::CTelinComingChange l_CTelinComingChange;
	GenRespondHeader(CMD_NOTIFY_INCOMINGCALL, l_CDeviceStateEvent.m_oHeader, l_CTelinComingChange.m_oHeader);
	l_CTelinComingChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CTelinComingChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;
    

	l_CTelinComingChange.m_oBody.m_strID = l_CDeviceStateEvent.m_oBody.m_strCallRefId;
	l_CTelinComingChange.m_oBody.m_strSeatNo = l_CDeviceStateEvent.m_oBody.m_strDevice;
	l_CTelinComingChange.m_oBody.m_strAnsweringTime = l_CDeviceStateEvent.m_oBody.m_strTime;

	l_CTelinComingChange.m_oBody.m_strCallingTime = l_ACDCallState.m_strTime;
	l_CTelinComingChange.m_oBody.m_strPhone = l_ACDCallState.m_strCallerId;

	l_CTelinComingChange.m_oBody.m_strCaseID = "";
	l_CTelinComingChange.m_oBody.m_strLatitude = "";
	l_CTelinComingChange.m_oBody.m_strLongitude = "";
	l_CTelinComingChange.m_oBody.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_CTelinComingChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());

	l_ACDCallState.m_strAnsweringtTime = l_CDeviceStateEvent.m_oBody.m_strTime;
	l_ACDCallState.m_strSeatNo = l_CDeviceStateEvent.m_oBody.m_strDevice;
	std::string l_strVal = l_ACDCallState.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pRedisClient->HSet(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId, l_strVal);
#endif
}

void CBusinessImpl::SendAlarmPosition(ObserverPattern::INotificationPtr p_pRequest)
{
    std::string p_strMsg = p_pRequest->GetMessages();
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CGpsSync l_oGpsSync;

    if (!l_oGpsSync.ParseString(p_strMsg, l_pIJson))
    {
        return;
    }

	ICC_LOG_DEBUG(m_pLog, "Received SendAlarmPosition From ICC: [%s]", p_strMsg.c_str());
    std::map<std::string, std::string> l_mapACDCallState;
	m_pRedisClient->HGetAll(ACDCALLSTATEKEY, l_mapACDCallState);
    auto it = l_mapACDCallState.find(l_oGpsSync.m_oBody.m_strCallrefID);
    if (it == l_mapACDCallState.end())
    {
        ICC_LOG_ERROR(m_pLog, "No ACDCallState ! CallrefID : [%s]", l_oGpsSync.m_oBody.m_strCallrefID.c_str());
        return;
    }
    ACDCallState l_ACDCallState;
    l_ACDCallState.ParseString(it->second, ICCGetIJsonFactory()->CreateJson());
    if (l_ACDCallState.m_strAlarmID.empty())
    {
		ICC_LOG_ERROR(m_pLog, "AlarmID is empty ! CallrefID : [%s]", l_oGpsSync.m_oBody.m_strCallrefID.c_str());
        return;
    }

	PROTOCOL::CTelinComingChange l_CTelinComingChange;
	GenRespondHeader(CMD_UPDATE_POSITION, l_oGpsSync.m_oHeader, l_CTelinComingChange.m_oHeader);
	l_CTelinComingChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS;
	l_CTelinComingChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE;

    l_CTelinComingChange.m_oBody.m_strLatitude = l_oGpsSync.m_oBody.m_strLatitude;
    l_CTelinComingChange.m_oBody.m_strLongitude = l_oGpsSync.m_oBody.m_strLongitude;

	l_CTelinComingChange.m_oBody.m_strCaseID = l_ACDCallState.m_strAlarmID;
	l_CTelinComingChange.m_oBody.m_strAddress = l_ACDCallState.m_strAddr;
	l_CTelinComingChange.m_oBody.m_strPersonName = l_ACDCallState.m_strPersonName;
	l_CTelinComingChange.m_oBody.m_strPersonSex = l_ACDCallState.m_strPersonSex;
	l_CTelinComingChange.m_oBody.m_strPersonID = l_ACDCallState.m_strPersonID;

	l_CTelinComingChange.m_oBody.m_strSeatNo = l_ACDCallState.m_strSeatNo;
	l_CTelinComingChange.m_oBody.m_strAnsweringTime = l_ACDCallState.m_strAnsweringtTime;
	l_CTelinComingChange.m_oBody.m_strCallingTime = l_ACDCallState.m_strTime;
	l_CTelinComingChange.m_oBody.m_strPhone = l_ACDCallState.m_strCallerNum;

	l_CTelinComingChange.m_oBody.m_strCallerNum = l_ACDCallState.m_strCallerNum;
	l_CTelinComingChange.m_oBody.m_strCalledNum = l_ACDCallState.m_strCalledNum;

	l_CTelinComingChange.m_oBody.m_strCallID = l_ACDCallState.m_strCallRefId;  //对应m_strCallRefId

	l_CTelinComingChange.m_oBody.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

    std::string  l_strMsg = l_CTelinComingChange.ToString(ICCGetIJsonFactory()->CreateJson());
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	//m_pRedisClient->HDel(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId); //改到挂机的时候删除
}

void CBusinessImpl::AlarmInfoRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received AlarmInfoRequest From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVCSRequestICCInfo l_CVCSRequestICCInfo;

	if (!l_CVCSRequestICCInfo.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	std::vector<Alarm> l_vecAlarmInfo;
	bool l_isQuery = QueryAlarmUpdateTime(l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime, l_vecAlarmInfo);
	if (!l_isQuery)
	{
		ICC_LOG_ERROR(m_pLog, "query alarm updatetime > [%s] is fail", l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime.c_str());
		return;
	}
	PROTOCOL::CCaseInfoVCSRespond l_CCaseInfoVCSRespond;
	GenRespondHeader(CASEINFO_VCSRESPONSE, l_CVCSRequestICCInfo.m_oHeader, l_CCaseInfoVCSRespond.m_oHeader);
	l_CCaseInfoVCSRespond.m_oHeader.m_strRequest = CASEINFO_VCSRESPONSE;
	l_CCaseInfoVCSRespond.m_oHeader.m_strRequestType = "0";
	SendAlarmInfoRespond(l_CCaseInfoVCSRespond.m_oHeader, l_vecAlarmInfo);
}

void CBusinessImpl::AlarmProcessInfoRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received AlarmProcessInfoRequest From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVCSRequestICCInfo l_CVCSRequestICCInfo;

	if (!l_CVCSRequestICCInfo.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	std::vector<Process> l_vecProcessInfo;
	bool l_isQuery = QueryProcessUpdateTime(l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime, l_vecProcessInfo);
	if (!l_isQuery)
	{
		ICC_LOG_ERROR(m_pLog, "query alarmprocess updatetime > [%s] is fail", l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime.c_str());
		return;
	}
	PROTOCOL::CCaseDisposalVCSRespond l_CCaseDisposalVCSRespond;
	GenRespondHeader(CASEDISPOSAL_VCSRESPONSE, l_CVCSRequestICCInfo.m_oHeader, l_CCaseDisposalVCSRespond.m_oHeader);
	l_CCaseDisposalVCSRespond.m_oHeader.m_strRequest = CASEDISPOSAL_VCSRESPONSE;
	l_CCaseDisposalVCSRespond.m_oHeader.m_strRequestType = "0";
	SendAlarmProcessInfoRespond(l_CCaseDisposalVCSRespond.m_oHeader, l_vecProcessInfo);
}

void CBusinessImpl::AlarmFeedBackInfoRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received AlarmFeedBackInfoRequest From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVCSRequestICCInfo l_CVCSRequestICCInfo;

	if (!l_CVCSRequestICCInfo.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	std::vector<FeedBack> l_vecFeedBackInfo;
	bool l_isQuery = QueryFeedBackUpdateTime(l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime, l_vecFeedBackInfo);
	if (!l_isQuery)
	{
		ICC_LOG_ERROR(m_pLog, "query alarmfeedback updatetime > [%s] is fail", l_CVCSRequestICCInfo.m_oBody.m_strUpdateTime.c_str());
		return;
	}
	PROTOCOL::CCaseFeedBackVCSRespond l_CCaseFeedBackVCSRespond;
	GenRespondHeader(CASEFEEDBACK_VCSRESPONSE, l_CVCSRequestICCInfo.m_oHeader, l_CCaseFeedBackVCSRespond.m_oHeader);
	l_CCaseFeedBackVCSRespond.m_oHeader.m_strRequest = CASEFEEDBACK_VCSRESPONSE;
	l_CCaseFeedBackVCSRespond.m_oHeader.m_strRequestType = "0";
	SendAlarmFeedBackInfoRespond(l_CCaseFeedBackVCSRespond.m_oHeader, l_vecFeedBackInfo);
}

void CBusinessImpl::SendAlarmInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<Alarm>& p_vecAlarmInfo)
{
	PROTOCOL::CCaseInfoVCSRespond l_CCaseInfoVCSRespond;
	l_CCaseInfoVCSRespond.m_oHeader = p_pRequestHeader;
	int count = 0;
	for (auto alarm : p_vecAlarmInfo)
	{
		PROTOCOL::CAlarmInfo l_oData;

		CallEventInfo l_tCallEventInfo;
		QueryCallevent(alarm.m_strSourceID, l_tCallEventInfo);

		l_oData = alarm;

		std::string l_strCallNo = alarm.m_strCalledNoType;
		/*l_oData.m_strID = alarm.m_strID;
		l_oData.m_strID = alarm.m_strMergeID;
		l_oData.m_strCallerNo = alarm.m_strCallerNo;*/
		l_oData.m_strCalledNoType = (l_strCallNo == POLICE_TYPE_110) ? POLICE_NO_110 : ((l_strCallNo == POLICE_TYPE_122) ? POLICE_NO_122 : POLICE_NO_119);
		l_oData.m_strTime = l_tCallEventInfo.m_strRingTime;
		l_oData.m_strReceivedTime = l_tCallEventInfo.m_strAnsweRingTime;
		l_oData.m_strSeatNo = l_tCallEventInfo.m_strSeatNo;
		l_oData.m_strDeleteFlag = "0";
		/*l_oData.m_strCallingTime = l_tCallEventInfo.m_strRingTime;
		l_oData.m_strAnsweringTime = l_tCallEventInfo.m_strAnsweRingTime;
		l_oData.m_strSeatNo = l_tCallEventInfo.m_strSeatNo;
		l_oData.m_strContact = alarm.m_strContactName;
		l_oData.m_strContactNo = alarm.m_strContactNo;
		l_oData.m_strContactSex = alarm.m_strContactGender;
		l_oData.m_strCaseDestrict = alarm.m_strAdminDeptDistrictCode;
		l_oData.m_strAreaOrg = alarm.m_strAdminDeptCode;
		l_oData.m_strCaseAddress = alarm.m_strAddr;
		l_oData.m_strLongitude = alarm.m_strLongitude;
		l_oData.m_strLatitude = alarm.m_strLatitude;
		l_oData.m_strCaseTime = alarm.m_strActualOccurTime;
		l_oData.m_strCaseType = alarm.m_strFirstType;
		l_oData.m_strCaseSubType = alarm.m_strSecondType;
		l_oData.m_strCaseThreeType = alarm.m_strThirdType;
		l_oData.m_strCaseSource = alarm.m_strSourceType;
		l_oData.m_strCaseLevel = alarm.m_strLevel;
		l_oData.m_strCaseDesc = alarm.m_strContent;
		l_oData.m_strCreateTime = alarm.m_strCreateTime;
		l_oData.m_strCreateUser = alarm.m_strCreateUser;
		l_oData.m_strUpdateTime = alarm.m_strUpdateTime;
		l_oData.m_strUpdateUser = alarm.m_strUpdateUser;*/
		//l_oData.m_strDelFlag = "0";
		//l_oData.m_strStatus = alarm.m_strState;

		l_CCaseInfoVCSRespond.m_oBody.m_vecData.push_back(l_oData);

		++count;
		if (count == MAX_COUNT)
		{
			l_CCaseInfoVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
			std::string l_strMessage = l_CCaseInfoVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			count = 0;
			l_CCaseInfoVCSRespond.m_oBody.m_vecData.clear();
		}
	}
	if (count != 0)
	{
		l_CCaseInfoVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
		std::string l_strMessage = l_CCaseInfoVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::SendAlarmProcessInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<Process>& p_vecProcessInfo)
{
	PROTOCOL::CCaseDisposalVCSRespond l_CCaseDisposalVCSRespond;
	l_CCaseDisposalVCSRespond.m_oHeader = p_pRequestHeader;
	int count = 0;
	for (auto process : p_vecProcessInfo)
	{
		CallEventInfo l_tCallEventInfo;
		PROTOCOL::CAlarmInfo l_tAlarmInfo;

		if (!QueryAlarm(process.m_ProcessData.m_strAlarmID, l_tAlarmInfo))
		{
			ICC_LOG_ERROR(m_pLog, "QueryAlarm Fail ! AlarmID: [%s]", process.m_ProcessData.m_strAlarmID.c_str());
			return;
		}
		QueryCallevent(l_tAlarmInfo.m_strSourceID, l_tCallEventInfo);
		PROTOCOL::CCaseDisposalVCSRespond::CData l_oData;

		l_oData.m_strID = process.m_ProcessData.m_strID;
		l_oData.m_strCaseId = process.m_ProcessData.m_strAlarmID;
		l_oData.m_strSeatNo = l_tCallEventInfo.m_strSeatNo;
		l_oData.m_strDispatchOrgId = process.m_ProcessData.m_strProcessDeptCode;
		l_oData.m_strDispatchTime = process.m_ProcessData.m_strTimeSigned;
		l_oData.m_strStatus = process.m_ProcessData.m_strState;
		l_oData.m_strCreateTime = process.m_ProcessData.m_strCreateTime;
		l_oData.m_strCreateUser = process.m_ProcessData.m_strCreateUser;
		l_oData.m_strUpdateTime = process.m_ProcessData.m_strUpdateTime;
		l_oData.m_strUpdateUser = process.m_ProcessData.m_strUpdateUser;
		l_oData.m_strDelFlag = "0";

		l_CCaseDisposalVCSRespond.m_oBody.m_vecData.push_back(l_oData);

		++count;
		if (count == MAX_COUNT)
		{
			l_CCaseDisposalVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
			std::string l_strMessage = l_CCaseDisposalVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			count = 0;
			l_CCaseDisposalVCSRespond.m_oBody.m_vecData.clear();
		}
	}
	if (count != 0)
	{
		l_CCaseDisposalVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
		std::string l_strMessage = l_CCaseDisposalVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::SendAlarmFeedBackInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<FeedBack>& p_vecFeedBackInfo)
{
	PROTOCOL::CCaseFeedBackVCSRespond l_CCaseFeedBackVCSRespond;
	l_CCaseFeedBackVCSRespond.m_oHeader = p_pRequestHeader;
	int count = 0;
	for (auto feedback : p_vecFeedBackInfo)
	{
		PROTOCOL::CAlarmInfo l_tAlarmInfo;

		if (!QueryAlarm(feedback.m_strAlarmID, l_tAlarmInfo))
		{
			ICC_LOG_ERROR(m_pLog, "QueryAlarm Fail ! AlarmID: [%s]", feedback.m_strAlarmID.c_str());
			return;
		}

		PROTOCOL::CCaseFeedBackVCSRespond::CData l_oData;
		l_oData.m_strID = feedback.m_strID;
		l_oData.m_strCaseId = feedback.m_strAlarmID;
		l_oData.m_strDisPosalId = feedback.m_strProcessID;
		l_oData.m_strCaseDispatchStatus = feedback.m_strResultType;

		//l_oData.m_strInterphoneId = ;
		//l_oData.m_strFBWay = ;

		l_oData.m_strFBContent = feedback.m_strResultContent;
		l_oData.m_strFeedBacker = feedback.m_strFeedbackName;
		l_oData.m_strFBTime = feedback.m_strTimeSigned;

		l_oData.m_strCaseType = feedback.m_strAlarmFirstType;
		l_oData.m_strCaseSubType = feedback.m_strAlarmSecondType;
		l_oData.m_strCaseThreeType = feedback.m_strAlarmThirdType;
		l_oData.m_strCaseProcessor = feedback.m_strProcessName;
		l_oData.m_strCreateTime = feedback.m_strCreateTime;
		l_oData.m_strCreateUser = feedback.m_strCreateUser;
		l_oData.m_strUpdateTime = feedback.m_strUpdateTime;
		l_oData.m_strUpdateUser = feedback.m_strUpdateUser;
		l_oData.m_strDelFlag = "0";
		l_CCaseFeedBackVCSRespond.m_oBody.m_vecData.push_back(l_oData);

		++count;
		if (count == MAX_COUNT)
		{
			l_CCaseFeedBackVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
			std::string l_strMessage = l_CCaseFeedBackVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			count = 0;
			l_CCaseFeedBackVCSRespond.m_oBody.m_vecData.clear();
		}
	}
	if (count != 0)
	{
		l_CCaseFeedBackVCSRespond.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
		std::string l_strMessage = l_CCaseFeedBackVCSRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::GetGpsRespond(ObserverPattern::INotificationPtr p_pRequest)
{
    std::string l_strMessage = p_pRequest->GetMessages();
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

    PROTOCOL::CGetGpsRespond l_oGetGpsRespond;
    if (!l_oGetGpsRespond.ParseString(l_strMessage, l_pIJson))
    {
        return;
    }

    std::string l_strResult = l_oGetGpsRespond.m_oBody.m_strResult;
    if (l_strResult != "0")
    {
        ICC_LOG_ERROR(m_pLog, "Get Gps Fail!!!");
        return;
    }
    ICC_LOG_DEBUG(m_pLog, "Get Gps Success!!!");
}

void CBusinessImpl::RecvAlarmUpdate(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvAlarmUpdate From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsSetAlarmInfoRequest l_CVcsSetAlarmInfoRequest;

	if (!l_CVcsSetAlarmInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	Alarm l_oAlarm;
	if (!QueryAlarm(l_CVcsSetAlarmInfoRequest.m_oBody.m_strId, l_oAlarm))
	{
		ICC_LOG_ERROR(m_pLog, "query alarm fail alarm_id = [%s]", l_CVcsSetAlarmInfoRequest.m_oBody.m_strId.c_str());
		return;
	}

	bool l_bAcceptAdminDeptCode = false;
	if (!l_CVcsSetAlarmInfoRequest.m_oBody.m_strAdminDeptCode.empty())
	{
		DataBase::SQLRequest l_Sql;
		l_Sql.sql_id = "check_admin_dept_by_alarm";
		l_Sql.param["alarm_id"] = l_CVcsSetAlarmInfoRequest.m_oBody.m_strId;
		l_Sql.param["new_admin_dept_code"] = l_CVcsSetAlarmInfoRequest.m_oBody.m_strAdminDeptCode;

		std::string strTime = m_pDateTime->GetAlarmIdTime(l_CVcsSetAlarmInfoRequest.m_oBody.m_strId);
		if (strTime != "")
		{
			l_Sql.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_Sql.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
		}

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{ // 执行SQL失败
			ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", "check_admin_dept_by_alarm", l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			if (l_pResult->Next())
			{
				int l_iFind = std::atoi(l_pResult->GetValue("find_result").c_str());
				if (l_iFind > 0)
				{ // 新管辖单位能被接警员看得到
					l_bAcceptAdminDeptCode = true;
					ICC_LOG_ERROR(m_pLog, "exec sql[%s] success", "check_admin_dept_by_alarm");
				}
			}
		}		
	}
	if (l_bAcceptAdminDeptCode)
	{ // 新管辖单位能被接警员看得到
		DataBase::SQLRequest l_Sql;
		l_Sql.sql_id = "select_icc_t_dept";
		l_Sql.param["code"] = l_CVcsSetAlarmInfoRequest.m_oBody.m_strAdminDeptCode;

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{ // 执行SQL失败
			ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", "select_icc_t_dept", l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			if (l_pResult->Next())
			{
				l_oAlarm.m_strAdminDeptCode = l_CVcsSetAlarmInfoRequest.m_oBody.m_strAdminDeptCode;
				l_oAlarm.m_strAdminDeptName = l_pResult->GetValue("name");
				ICC_LOG_ERROR(m_pLog, "exec sql[%s] success", "select_icc_t_dept");
			}
		}
	}
	if (l_oAlarm.m_strAddr.empty())
	{ 
		// 如果ICC地址为空，则取VCS地址
		l_oAlarm.m_strAddr = l_CVcsSetAlarmInfoRequest.m_oBody.m_strCaseAddress;
	}
	l_oAlarm.m_strLongitude = l_CVcsSetAlarmInfoRequest.m_oBody.m_strLongitude;
	l_oAlarm.m_strLatitude = l_CVcsSetAlarmInfoRequest.m_oBody.m_strLatitude;

	//l_oAlarm.m_strOperatorCode = l_CVcsSetAlarmInfoRequest.m_oBody.m_strOperatorCode;
	//l_oAlarm.m_strOperatorName = l_CVcsSetAlarmInfoRequest.m_oBody.m_strOperatorName;

	std::vector<Process> l_vecProcess;
	if (!QueryAlarmProcessByAlarmID(l_CVcsSetAlarmInfoRequest.m_oBody.m_strId, l_vecProcess))
	{
		ICC_LOG_ERROR(m_pLog, "query process fail alarm_id = [%s]", l_CVcsSetAlarmInfoRequest.m_oBody.m_strId.c_str());
		return;
	}

	SendAlarmUpdateRequest(l_CVcsSetAlarmInfoRequest.m_oHeader, l_oAlarm, l_vecProcess);
}


void CBusinessImpl::_BuildRequestHeader(PROTOCOL::CHeader& p_pRequestHeader, std::string p_strCmd, std::string p_strQueueName)
{
	p_pRequestHeader.m_strSystemID = "ICC";
	p_pRequestHeader.m_strSubsystemID = "VCS";
	//p_pRequestHeader.m_strMsgid = p_pRequestHeader.m_strMsgid;
	p_pRequestHeader.m_strRelatedID = "";
	p_pRequestHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRequestHeader.m_strCmd = p_strCmd;
	p_pRequestHeader.m_strRequest = p_strQueueName;
	p_pRequestHeader.m_strRequestType = "0";
	p_pRequestHeader.m_strResponse = "";
	p_pRequestHeader.m_strResponseType = "";
}

//根据反馈人警号 查询 部门编码
std::string CBusinessImpl::_QueryDeptCodeByProcesserCode(const std::string& strProcesserCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_staff_processer_deptcode_by_processercode";
	l_SqlRequest.param["code"] = strProcesserCode;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	if (l_pResult == nullptr)
	{		
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_SqlRequest.sql_id.c_str());
		return "";
	}

	if (!l_pResult->IsValid())
	{		
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return "";
	}
	
	ICC_LOG_DEBUG(m_pLog, "exec sql success!, sql = %s", l_pResult->GetSQL().c_str());


	if (l_pResult->Next())
	{
		std::string strDeptCode = l_pResult->GetValue("code");
		ICC_LOG_DEBUG(m_pLog, "query success!, code = %s", strDeptCode.c_str());
		return 	strDeptCode;
	}
	return "";
}

#define ASSIGN_ERROR     -1
#define ASSIGN_ASSIGNED   1
#define ASSIGN_UNASSIGNED 0

int CBusinessImpl::_Asigned(const std::string& strState)
{
	if (strState.empty())
	{
		return ASSIGN_ERROR;
	}

	if (strState >= "DIC020030")
	{
		return ASSIGN_ASSIGNED;
	}
	return ASSIGN_UNASSIGNED;
}

std::string CBusinessImpl::_QueryProcessIdByAlarmIdAndDeptCode(const std::string& strAlarmId, const std::string& strDeptCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_process_id_by_alarm_id_and_deptcode";
	l_SqlRequest.param["alarm_id"] = strAlarmId;
	l_SqlRequest.param["dept_code"] = strDeptCode;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	if (l_pResult == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_SqlRequest.sql_id.c_str());
		return "";
	}

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return "";
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success!, sql = %s", l_pResult->GetSQL().c_str());


	if (l_pResult->Next())
	{
		std::string strProcessId = l_pResult->GetValue("id");
		ICC_LOG_DEBUG(m_pLog, "query success!, state = %s", strProcessId.c_str());
		return 	strProcessId;
	}
	return "";
}

bool CBusinessImpl::_QueryProcessIdAndDeptCodeByAlarmId(const std::string& strAlarmId, std::string& strProcessId, std::string& strDeptCode, std::string& strDeptName)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_process_id_and_deptcode_by_alarm_id";
	l_SqlRequest.param["alarm_id"] = strAlarmId;	

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	if (l_pResult == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_SqlRequest.sql_id.c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success!, sql = %s", l_pResult->GetSQL().c_str());

	int iCount = 0;
	while (l_pResult->Next())
	{
		iCount++;
		strProcessId = l_pResult->GetValue("processid");
		strDeptCode = l_pResult->GetValue("deptcode");
		strDeptName = l_pResult->GetValue("deptname");
	}

	if (iCount == 1)
	{
		return true;
	}

	return false;
}

bool CBusinessImpl::_IsNeedSendAssigned(const std::string& strAlarmId, const std::string& strDeptCode)
{
	std::string strState; //= _QueryProcessStateByAlarmIdAndDeptCode(strAlarmId, strDeptCode);
	return (_Asigned(strState) == ASSIGN_UNASSIGNED);
}

void CBusinessImpl::RecvFeedBackUpdate(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvFeedBackUpdate From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsSetFeedBackInfoRequest l_CVcsSetFeedBackInfoRequest;

	if (!l_CVcsSetFeedBackInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	if (l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseDispatchStatus == "1")//vcs签收
	{
		if (!l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackDeptCode.empty())
		{
			PROTOCOL::CWebSignRequest p_oRequest;
			_BuildRequestHeader(p_oRequest.m_oHeader, "add_alarm_sign_request", "queue_alarm");

			// 构建更新处警单信息
			p_oRequest.m_oBody.m_oProcess.m_strID = _QueryProcessIdByAlarmIdAndDeptCode(l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseId, l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackDeptCode);
			if (!p_oRequest.m_oBody.m_oProcess.m_strID.empty())
			{
				p_oRequest.m_oBody.m_oProcess.m_strAlarmID = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseId;
				p_oRequest.m_oBody.m_oProcess.m_strProcessDeptCode = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackDeptCode;//_QueryDeptCodeByProcesserCode(l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackerCode);
				p_oRequest.m_oBody.m_oProcess.m_strProcessDeptName = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackDept;
				p_oRequest.m_oBody.m_oProcess.m_strProcessCode = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackerCode;
				p_oRequest.m_oBody.m_oProcess.m_strProcessName = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBacker;
				p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderCode = "";
				p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderName = "";
				p_oRequest.m_oBody.m_oProcess.m_strMsgSource = "vcs_relocated";
				std::string l_strMessage = p_oRequest.ToString(ICCGetIJsonFactory()->CreateJson());
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
				ICC_LOG_DEBUG(m_pLog, "send assigned, [%s]", l_strMessage.c_str());
			}
			else
			{
				
				ICC_LOG_ERROR(m_pLog, "%s", "not find process id by alarm id and dept code, not send assign message");
			}
		}			
		else
		{
			std::string strProcessId;
			std::string strDeptCode;
			std::string strDeptName;

			if (_QueryProcessIdAndDeptCodeByAlarmId(l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseId, strProcessId, strDeptCode, strDeptName))
			{
				PROTOCOL::CWebSignRequest p_oRequest;
				_BuildRequestHeader(p_oRequest.m_oHeader, "add_alarm_sign_request", "queue_alarm");
				p_oRequest.m_oBody.m_oProcess.m_strID = strProcessId;
				p_oRequest.m_oBody.m_oProcess.m_strAlarmID = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseId;
				p_oRequest.m_oBody.m_oProcess.m_strProcessDeptCode = strDeptCode;
				p_oRequest.m_oBody.m_oProcess.m_strProcessDeptName = strDeptName;
				p_oRequest.m_oBody.m_oProcess.m_strProcessCode = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackerCode;
				p_oRequest.m_oBody.m_oProcess.m_strProcessName = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBacker;
				p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderCode = "";
				p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderName = "";
				p_oRequest.m_oBody.m_oProcess.m_strMsgSource = "vcs_relocated";

				std::string l_strMessage = p_oRequest.ToString(ICCGetIJsonFactory()->CreateJson());
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

				ICC_LOG_DEBUG(m_pLog, "send assigned, [%s]", l_strMessage.c_str());
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "%s", "not find sure process id and dept code by alarm id, not send assign message");
			}
		}
	}

	
	PROTOCOL::CAddAlarmRemarkRequest request;
	request.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	request.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	request.m_oHeader.m_strCmd = "add_alarm_remark_request";
	request.m_oHeader.m_strRequest = QUEUE_ALARM;
	request.m_oHeader.m_strRequestType = "0";

	request.m_oBody.m_strMsgSorce = MSG_SOURCE_VCS;
	request.m_oBody.m_strGuid = m_pString->CreateGuid();
	request.m_oBody.m_strAlarmID = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strCaseId;
	request.m_oBody.m_strFeedBackCode = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackerCode;
	request.m_oBody.m_strFeedBackName = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBacker;
	request.m_oBody.m_strFeedBackDeptCode = "";
	request.m_oBody.m_strFeedBackDeptName = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFeedBackDept;
	request.m_oBody.m_strFeedBackTime = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFBTime;
	request.m_oBody.m_strContent = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFBContent;
	request.m_oBody.m_strFileType = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFileType;
	request.m_oBody.m_strFileGuid = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strFileGuid;
	request.m_oBody.m_strLongitude = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strLongitude;
	request.m_oBody.m_strLatitude = l_CVcsSetFeedBackInfoRequest.m_oBody.m_strLatitude;

	std::string l_strMessage = request.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::RecvDeviceUpdate(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvDeviceUpdate From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsSetDeviceInfoRequest l_CVcsSetDeviceInfoRequest;

	if (!l_CVcsSetDeviceInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = l_CVcsSetDeviceInfoRequest.m_oBody.m_strCaseID;
	std::vector<std::string> l_vecParamList;
	l_vecParamList.push_back(l_CVcsSetDeviceInfoRequest.m_oBody.m_strDispatchName);
	l_vecParamList.push_back(l_CVcsSetDeviceInfoRequest.m_oBody.m_strDispatchCode);
	l_vecParamList.push_back(l_CVcsSetDeviceInfoRequest.m_oBody.m_strDispatchDept);
	l_vecParamList.push_back(l_CVcsSetDeviceInfoRequest.m_oBody.m_strInterPhoneID);

	std::string l_strStatus = l_CVcsSetDeviceInfoRequest.m_oBody.m_strStatus;
	l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
	l_oAlarmLogInfo.m_strOperate = (l_strStatus == "1") ? ACCEPTE_CONFIRM : ((l_strStatus == "2") ? ARRIVE_DESTINATION : DISPOSAL_COMPLETION);
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = l_CVcsSetDeviceInfoRequest.m_oBody.m_strDispatchName;
	
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	l_oAlarmLogInfo.m_strCreateTime = l_strCurTime;
	if (!InsertDBAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log  failed!");
		return;
	}

	SyncAlarmLogInfo(l_oAlarmLogInfo);
}

void CBusinessImpl::RecvTextMsgUpdate(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvTextMsgUpdate From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsTextMsg l_Request;

	if (!l_Request.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = l_Request.m_oBody.m_strCaseID;
	std::vector<std::string> l_vecParamList;
	l_vecParamList.push_back(l_Request.m_oBody.m_strSender);
	l_vecParamList.push_back(l_Request.m_oBody.m_strReceiver);
	l_vecParamList.push_back(l_Request.m_oBody.m_strContent);

	l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
	l_oAlarmLogInfo.m_strOperate = TEXTMSG_LOG_CODE;
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = MSG_SOURCE_VCS;

	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	l_oAlarmLogInfo.m_strCreateTime = l_strCurTime;
	if (!InsertDBAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log  failed!");
		return;
	}
	
	SyncAlarmLogInfo(l_oAlarmLogInfo);
}

void CBusinessImpl::RecvRecordUpdate(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvRecordUpdate From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsSetRecordInfoRequest l_CVcsSetRecordInfoRequest;

	if (!l_CVcsSetRecordInfoRequest.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = l_CVcsSetRecordInfoRequest.m_oBody.m_strCaseID;
	std::vector<std::string> l_vecParamList;
	//l_vecParamList.push_back(l_CVcsSetRecordInfoRequest.m_oBody.m_strRecordFile);
	l_vecParamList.push_back(l_CVcsSetRecordInfoRequest.m_oBody.m_strCaller);
	l_vecParamList.push_back(l_CVcsSetRecordInfoRequest.m_oBody.m_strCalled);
	l_vecParamList.push_back(l_CVcsSetRecordInfoRequest.m_oBody.m_strCallTime);
	l_vecParamList.push_back(l_CVcsSetRecordInfoRequest.m_oBody.m_strRecordTime);

	l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
	l_oAlarmLogInfo.m_strOperate = RECORD_LOG_CODE;
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strSourceName = "vcs";
	std::string type = RECORD_RESOURSE_TYPE;
	l_oAlarmLogInfo.m_strOperateAttachDesc = GenAlarmLogAttach(type, l_CVcsSetRecordInfoRequest.m_oBody.m_strRecordFile);
	l_oAlarmLogInfo.m_strCreateUser = MSG_SOURCE_VCS;

	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	l_oAlarmLogInfo.m_strCreateTime = l_strCurTime;
	if (!InsertDBAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log  failed!");
		return;
	}

	SyncAlarmLogInfo(l_oAlarmLogInfo);
}

void CBusinessImpl::SendAlarmUpdateRequest(const PROTOCOL::CHeader& p_pRequestHeader, const Alarm& p_Alarm, const std::vector<Process>& p_vecProcess, const std::string p_strVcsFlag)
{
	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest;

	l_oRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();// p_pRequestHeader.m_strMsgid;
	l_oRequest.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
	l_oRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_oRequest.m_oHeader.m_strRequestType = "0";
	l_oRequest.m_oHeader.m_strRequestFlag = "MQ";

	l_oRequest.m_oBody.m_oAlarm = p_Alarm;
	//l_oRequest.m_oHeader = p_pRequestHeader;
	l_oRequest.m_oBody.m_oAlarm.m_strMsgSource = "vcs_relocated";//  VCS重定位导致ICC警情信息变更，ICC更新完数据库，需要重新通知VCS --MSG_SOURCE_VCS;
	l_oRequest.m_oBody.m_oAlarm.m_strLevel = p_Alarm.m_strLevel.empty() ? "03" : p_Alarm.m_strLevel;//默认最低级
	if (l_oRequest.m_oBody.m_oAlarm.m_strReceiptSrvName.empty())
	{
		l_oRequest.m_oBody.m_oAlarm.m_strReceiptSrvName = "vcs";
	}
	for (Process process : p_vecProcess)
	{
		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_ProcessData = process.m_ProcessData;
		l_ProcessData.m_strMsgSource = MSG_SOURCE_VCS;
		l_oRequest.m_oBody.m_vecProcessData.push_back(l_ProcessData);
	}

	if (p_strVcsFlag == "1") //VCS 补偿新增
	{
		l_oRequest.m_oBody.m_strIsVcsSyncFlag = p_strVcsFlag;
	}
	else if (p_strVcsFlag == "2") //VCS 补偿修改
	{
		l_oRequest.m_oBody.m_strIsVcsSyncFlag = p_strVcsFlag;
	}
	std::string l_strMessage = l_oRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendFeedBackUpdateRequest(const PROTOCOL::CHeader& p_pRequestHeader, const FeedBack& p_FeedBack)
{
	PROTOCOL::CSetFeedBackRequest l_oRequest;
	l_oRequest.m_oHeader = p_pRequestHeader;

	l_oRequest.m_oBody.m_strMsgSource = MSG_SOURCE_VCS;
	l_oRequest.m_oBody.m_strID = p_FeedBack.m_strID;
	l_oRequest.m_oBody.m_strAlarmID = p_FeedBack.m_strAlarmID;
	l_oRequest.m_oBody.m_strProcessID = p_FeedBack.m_strProcessID;
	l_oRequest.m_oBody.m_strResultType = p_FeedBack.m_strResultType;
	l_oRequest.m_oBody.m_strResultContent = p_FeedBack.m_strResultContent;
	l_oRequest.m_oBody.m_strLeaderInstruction = p_FeedBack.m_strLeaderInstruction;
	l_oRequest.m_oBody.m_strState = p_FeedBack.m_strState;
	l_oRequest.m_oBody.m_strTimeEdit = p_FeedBack.m_strTimeEdit;
	l_oRequest.m_oBody.m_strTimeSubmit = p_FeedBack.m_strTimeSubmit;
	l_oRequest.m_oBody.m_strTimeArrived = p_FeedBack.m_strTimeArrived;
	l_oRequest.m_oBody.m_strTimeSigned = p_FeedBack.m_strTimeSigned;
	l_oRequest.m_oBody.m_strTimePoliceDispatch = p_FeedBack.m_strTimePoliceDispatch;
	l_oRequest.m_oBody.m_strTimePoliceArrived = p_FeedBack.m_strTimePoliceArrived;
	l_oRequest.m_oBody.m_strActualOccurTime = p_FeedBack.m_strActualOccurTime;
	l_oRequest.m_oBody.m_strActualOccurAddr = p_FeedBack.m_strActualOccurAddr;
	l_oRequest.m_oBody.m_strFeedbackDeptDistrictCode = p_FeedBack.m_strFeedbackDeptDistrictCode;
	l_oRequest.m_oBody.m_strFeedbackDeptCode = p_FeedBack.m_strFeedbackDeptCode;
	l_oRequest.m_oBody.m_strFeedbackDeptName = p_FeedBack.m_strFeedbackDeptName;
	l_oRequest.m_oBody.m_strFeedbackCode = p_FeedBack.m_strFeedbackCode;
	l_oRequest.m_oBody.m_strFeedbackName = p_FeedBack.m_strFeedbackName;
	l_oRequest.m_oBody.m_strFeedbackLeaderCode = p_FeedBack.m_strFeedbackLeaderCode;
	l_oRequest.m_oBody.m_strFeedbackLeaderName = p_FeedBack.m_strFeedbackLeaderName;
	l_oRequest.m_oBody.m_strProcessDeptDistrictCode = p_FeedBack.m_strProcessDeptDistrictCode;
	l_oRequest.m_oBody.m_strProcessDeptCode = p_FeedBack.m_strProcessDeptCode;
	l_oRequest.m_oBody.m_strProcessDeptName = p_FeedBack.m_strProcessDeptName;
	l_oRequest.m_oBody.m_strProcessCode = p_FeedBack.m_strProcessCode;
	l_oRequest.m_oBody.m_strProcessName = p_FeedBack.m_strProcessName;
	l_oRequest.m_oBody.m_strProcessLeaderCode = p_FeedBack.m_strProcessLeaderCode;
	l_oRequest.m_oBody.m_strProcessLeaderName = p_FeedBack.m_strProcessLeaderName;
	l_oRequest.m_oBody.m_strDispatchDeptDistrictCode = p_FeedBack.m_strDispatchDeptDistrictCode;
	l_oRequest.m_oBody.m_strDispatchDeptCode = p_FeedBack.m_strDispatchDeptCode;
	l_oRequest.m_oBody.m_strDispatchDeptName = p_FeedBack.m_strDispatchDeptName;
	l_oRequest.m_oBody.m_strDispatchCode = p_FeedBack.m_strDispatchCode;
	l_oRequest.m_oBody.m_strDispatchName = p_FeedBack.m_strDispatchName;
	l_oRequest.m_oBody.m_strDispatchLeaderCode = p_FeedBack.m_strDispatchLeaderCode;
	l_oRequest.m_oBody.m_strDispatchLeaderName = p_FeedBack.m_strDispatchLeaderName;
	l_oRequest.m_oBody.m_strPersonId = p_FeedBack.m_strPersonId;
	l_oRequest.m_oBody.m_strPersonIdType = p_FeedBack.m_strPersonIdType;
	l_oRequest.m_oBody.m_strPersonNationality = p_FeedBack.m_strPersonNationality;
	l_oRequest.m_oBody.m_strPersonName = p_FeedBack.m_strPersonName;
	l_oRequest.m_oBody.m_strPersonSlaveId = p_FeedBack.m_strPersonSlaveId;
	l_oRequest.m_oBody.m_strPersonSlaveIdType = p_FeedBack.m_strPersonSlaveIdType;
	l_oRequest.m_oBody.m_strPersonSlaveNationality = p_FeedBack.m_strPersonSlaveNationality;
	l_oRequest.m_oBody.m_strPersonSlaveName = p_FeedBack.m_strPersonSlaveName;
	l_oRequest.m_oBody.m_strAlarmCalledNoType = p_FeedBack.m_strAlarmCalledNoType;
	l_oRequest.m_oBody.m_strAlarmFirstType = p_FeedBack.m_strAlarmFirstType;
	l_oRequest.m_oBody.m_strAlarmSecondType = p_FeedBack.m_strAlarmSecondType;
	l_oRequest.m_oBody.m_strAlarmThirdType = p_FeedBack.m_strAlarmThirdType;
	l_oRequest.m_oBody.m_strAlarmFourthType = p_FeedBack.m_strAlarmFourthType;
	l_oRequest.m_oBody.m_strAlarmAddrDeptName = p_FeedBack.m_strAlarmAddrDeptName;
	l_oRequest.m_oBody.m_strAlarmAddrFirstType = p_FeedBack.m_strAlarmAddrFirstType;
	l_oRequest.m_oBody.m_strAlarmAddrSecondType = p_FeedBack.m_strAlarmAddrSecondType;
	l_oRequest.m_oBody.m_strAlarmAddrThirdType = p_FeedBack.m_strAlarmAddrThirdType;
	l_oRequest.m_oBody.m_strAlarmLongitude = p_FeedBack.m_strAlarmLongitude;
	l_oRequest.m_oBody.m_strAlarmLatitude = p_FeedBack.m_strAlarmLatitude;
	l_oRequest.m_oBody.m_strAlarmRegionType = p_FeedBack.m_strAlarmRegionType;
	l_oRequest.m_oBody.m_strAlarmLocationType = p_FeedBack.m_strAlarmLocationType;
	l_oRequest.m_oBody.m_strPeopleNumCapture = p_FeedBack.m_strPeopleNumCapture;
	l_oRequest.m_oBody.m_strPeopleNumRescue = p_FeedBack.m_strPeopleNumRescue;
	l_oRequest.m_oBody.m_strPeopleNumSlightInjury = p_FeedBack.m_strPeopleNumSlightInjury;
	l_oRequest.m_oBody.m_strPeopleNumSeriousInjury = p_FeedBack.m_strPeopleNumSeriousInjury;
	l_oRequest.m_oBody.m_strPeopleNumDeath = p_FeedBack.m_strPeopleNumDeath;
	l_oRequest.m_oBody.m_strPoliceNumDispatch = p_FeedBack.m_strPoliceNumDispatch;
	l_oRequest.m_oBody.m_strPoliceCarNumDispatch = p_FeedBack.m_strPoliceCarNumDispatch;
	l_oRequest.m_oBody.m_strEconomyLoss = p_FeedBack.m_strEconomyLoss;
	l_oRequest.m_oBody.m_strRetrieveEconomyLoss = p_FeedBack.m_strRetrieveEconomyLoss;
	l_oRequest.m_oBody.m_strFirePutOutTime = p_FeedBack.m_strFirePutOutTime;
	l_oRequest.m_oBody.m_strFireBuildingFirstType = p_FeedBack.m_strFireBuildingFirstType;
	l_oRequest.m_oBody.m_strFireBuildingSecondType = p_FeedBack.m_strFireBuildingSecondType;
	l_oRequest.m_oBody.m_strFireBuildingThirdType = p_FeedBack.m_strFireBuildingThirdType;
	l_oRequest.m_oBody.m_strFireSourceType = p_FeedBack.m_strFireSourceType;
	l_oRequest.m_oBody.m_strFireRegionType = p_FeedBack.m_strFireRegionType;
	l_oRequest.m_oBody.m_strFireCauseFirstType = p_FeedBack.m_strFireCauseFirstType;
	l_oRequest.m_oBody.m_strFireCauseSecondType = p_FeedBack.m_strFireCauseSecondType;
	l_oRequest.m_oBody.m_strFireCauseThirdType = p_FeedBack.m_strFireCauseThirdType;
	l_oRequest.m_oBody.m_strFireArea = p_FeedBack.m_strFireArea;
	l_oRequest.m_oBody.m_strTrafficRoadLevel = p_FeedBack.m_strTrafficRoadLevel;
	l_oRequest.m_oBody.m_strTrafficAccidentLevel = p_FeedBack.m_strTrafficAccidentLevel;
	l_oRequest.m_oBody.m_strTrafficVehicleNo = p_FeedBack.m_strTrafficVehicleNo;
	l_oRequest.m_oBody.m_strTrafficVehicleType = p_FeedBack.m_strTrafficVehicleType;
	l_oRequest.m_oBody.m_strTrafficSlaveVehicleNo = p_FeedBack.m_strTrafficSlaveVehicleNo;
	l_oRequest.m_oBody.m_strTrafficSlaveVehicleType = p_FeedBack.m_strTrafficSlaveVehicleType;
	l_oRequest.m_oBody.m_strEventType = p_FeedBack.m_strEventType;
	l_oRequest.m_oBody.m_strCreateTime = p_FeedBack.m_strCreateTime;
	l_oRequest.m_oBody.m_strCreateUser = p_FeedBack.m_strCreateUser;
	l_oRequest.m_oBody.m_strUpdateTime = p_FeedBack.m_strUpdateTime;
	l_oRequest.m_oBody.m_strUpdateUser = p_FeedBack.m_strUpdateUser;

	std::string l_strMessage = l_oRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SaveCallInfo(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCallStateEvent l_CCallStateEvent;

	if (!l_CCallStateEvent.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "SaveCallInfo From CTI: [%s]", p_strMsg.c_str());
	if (l_CCallStateEvent.m_oBody.m_strState != "incoming")
	{
		return;
	}

	ACDCallState l_ACDCallState;
	l_ACDCallState.m_strAgent = l_CCallStateEvent.m_oBody.m_strAgent;
	l_ACDCallState.m_strACD = l_CCallStateEvent.m_oBody.m_strACD;
	l_ACDCallState.m_strCallRefId = l_CCallStateEvent.m_oBody.m_strCallRefId;
	l_ACDCallState.m_strCallerNum = l_CCallStateEvent.m_oBody.m_strCallerId;
	l_ACDCallState.m_strCalledNum = l_CCallStateEvent.m_oBody.m_strCalledId;
	l_ACDCallState.m_strState = l_CCallStateEvent.m_oBody.m_strState;
	l_ACDCallState.m_strTime = l_CCallStateEvent.m_oBody.m_strTime;

	std::string l_strVal = l_ACDCallState.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pRedisClient->HSet(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId, l_strVal);
	m_mCallRefId.insert(std::make_pair(m_pDateTime->CurrentDateTime(), l_ACDCallState.m_strCallRefId));
}

void CBusinessImpl::GenRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = p_pRequestHeader.m_strSystemID;
	p_pRespHeader.m_strSubsystemID = p_pRequestHeader.m_strSubsystemID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
	p_pRespHeader.m_strResponse = "";
	p_pRespHeader.m_strResponseType = "";
}

bool CBusinessImpl::QueryCallevent(std::string p_strID, CallEventInfo& p_tCallEventInfo)
{
	if (p_strID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "ResourceID is Null");
		return false;
	}

	DataBase::SQLRequest l_oSelectCallEvent;

	l_oSelectCallEvent.sql_id = "select_icc_t_callevent";
	l_oSelectCallEvent.param["callref_id"] = p_strID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_strID);
	if (strTime != "")
	{
		l_oSelectCallEvent.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_oSelectCallEvent.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSelectCallEvent);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	while (l_pResult->Next())
	{
		p_tCallEventInfo.m_strRingTime = l_pResult->GetValue("ring_time");
		p_tCallEventInfo.m_strAnsweRingTime = l_pResult->GetValue("talk_time");
		p_tCallEventInfo.m_strSeatNo = l_pResult->GetValue("called_id");
	}
	return true;
}

bool CBusinessImpl::QueryAlarm(std::string p_strID, Alarm& p_tAlarmInfo)
{
	if (p_strID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "AlarmID is Null");
		return false;
	}

	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb";
	l_oSeleteAlarmSQLReq.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		if (!p_tAlarmInfo.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}
	}
	if (!p_tAlarmInfo.m_strID.empty())
	{
		return true;
	}
	return false;
}

bool CBusinessImpl::QueryAlarmProcessByIDEx(std::string p_strID, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData &l_pData)
{
	std::vector<Process> p_vecProcessInfo;
	if (_QueryProcess("id", p_strID, p_vecProcessInfo))
	{
		if (p_vecProcessInfo.size() > 0)
		{
			l_pData = p_vecProcessInfo[0].m_ProcessData;
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::QueryAlarmProcessByID(std::string p_strID, Process& p_oProcess)
{
	std::vector<Process> p_vecProcessInfo;
	if (_QueryProcess("id", p_strID, p_vecProcessInfo))
	{
		if (p_vecProcessInfo.size() > 0)
		{
			p_oProcess = p_vecProcessInfo[0];
		}

		if (!p_oProcess.m_ProcessData.m_strID.empty())
		{
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::QueryAlarmProcessByAlarmID(std::string p_strID, std::vector<Process>& p_vecProcess)
{
	return _QueryProcess("alarm_id", p_strID, p_vecProcess);
}

bool CBusinessImpl::QueryFeedBack(std::string p_strID, FeedBack& p_oFeedBack)
{
	if (p_strID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "p_strID is Null");
		return false;
	}

	DataBase::SQLRequest l_oSeleteAlarmFeedBackSQLReq;
	l_oSeleteAlarmFeedBackSQLReq.sql_id = "select_icc_t_alarm_feedback";
	l_oSeleteAlarmFeedBackSQLReq.param["id"] = p_strID;


	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_oSeleteAlarmFeedBackSQLReq);

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	while (l_Result->Next())
	{
		p_oFeedBack.m_strID = l_Result->GetValue("id");
		p_oFeedBack.m_strAlarmID = l_Result->GetValue("alarm_id");
		p_oFeedBack.m_strProcessID = l_Result->GetValue("process_id");
		p_oFeedBack.m_strResultType = l_Result->GetValue("result_type");
		p_oFeedBack.m_strResultContent = l_Result->GetValue("result_content");
		p_oFeedBack.m_strLeaderInstruction = l_Result->GetValue("leader_instruction");
		p_oFeedBack.m_strState = l_Result->GetValue("state");
		p_oFeedBack.m_strTimeEdit = l_Result->GetValue("time_edit");
		p_oFeedBack.m_strTimeSubmit = l_Result->GetValue("time_submit");
		p_oFeedBack.m_strTimeArrived = l_Result->GetValue("time_arrived");
		p_oFeedBack.m_strTimeSigned = l_Result->GetValue("time_signed");
		p_oFeedBack.m_strTimePoliceDispatch = l_Result->GetValue("time_police_dispatch");
		p_oFeedBack.m_strTimePoliceArrived = l_Result->GetValue("time_police_arrived");
		p_oFeedBack.m_strActualOccurTime = l_Result->GetValue("actual_occur_time");
		p_oFeedBack.m_strActualOccurAddr = l_Result->GetValue("actual_occur_addr");
		p_oFeedBack.m_strFeedbackDeptDistrictCode = l_Result->GetValue("feedback_dept_district_code");
		p_oFeedBack.m_strFeedbackDeptCode = l_Result->GetValue("feedback_dept_code");
		p_oFeedBack.m_strFeedbackDeptName = l_Result->GetValue("feedback_dept_name");
		p_oFeedBack.m_strFeedbackCode = l_Result->GetValue("feedback_code");
		p_oFeedBack.m_strFeedbackName = l_Result->GetValue("feedback_name");
		p_oFeedBack.m_strFeedbackLeaderCode = l_Result->GetValue("feedback_leader_code");
		p_oFeedBack.m_strFeedbackLeaderName = l_Result->GetValue("feedback_leader_name");
		p_oFeedBack.m_strProcessDeptDistrictCode = l_Result->GetValue("process_dept_district_code");
		p_oFeedBack.m_strProcessDeptCode = l_Result->GetValue("process_dept_code");
		p_oFeedBack.m_strProcessDeptName = l_Result->GetValue("process_dept_name");
		p_oFeedBack.m_strProcessCode = l_Result->GetValue("process_code");
		p_oFeedBack.m_strProcessName = l_Result->GetValue("process_name");
		p_oFeedBack.m_strProcessLeaderCode = l_Result->GetValue("process_leader_code");
		p_oFeedBack.m_strProcessLeaderName = l_Result->GetValue("process_leader_name");
		p_oFeedBack.m_strDispatchDeptDistrictCode = l_Result->GetValue("dispatch_dept_district_code");
		p_oFeedBack.m_strDispatchDeptCode = l_Result->GetValue("dispatch_dept_code");
		p_oFeedBack.m_strDispatchDeptName = l_Result->GetValue("dispatch_dept_name");
		p_oFeedBack.m_strDispatchCode = l_Result->GetValue("dispatch_code");
		p_oFeedBack.m_strDispatchName = l_Result->GetValue("dispatch_name");
		p_oFeedBack.m_strDispatchLeaderCode = l_Result->GetValue("dispatch_leader_code");
		p_oFeedBack.m_strDispatchLeaderName = l_Result->GetValue("dispatch_leader_name");
		p_oFeedBack.m_strPersonId = l_Result->GetValue("person_id");
		p_oFeedBack.m_strPersonIdType = l_Result->GetValue("person_id_type");
		p_oFeedBack.m_strPersonNationality = l_Result->GetValue("person_nationality");
		p_oFeedBack.m_strPersonName = l_Result->GetValue("person_name");
		p_oFeedBack.m_strPersonSlaveId = l_Result->GetValue("person_slave_id");
		p_oFeedBack.m_strPersonSlaveIdType = l_Result->GetValue("person_slave_id_type");
		p_oFeedBack.m_strPersonSlaveNationality = l_Result->GetValue("person_slave_nationality");
		p_oFeedBack.m_strPersonSlaveName = l_Result->GetValue("person_slave_name");
		p_oFeedBack.m_strAlarmFirstType = l_Result->GetValue("alarm_first_type");
		p_oFeedBack.m_strAlarmSecondType = l_Result->GetValue("alarm_second_type");
		p_oFeedBack.m_strAlarmThirdType = l_Result->GetValue("alarm_third_type");
		p_oFeedBack.m_strAlarmFourthType = l_Result->GetValue("alarm_fourth_type");
		p_oFeedBack.m_strAlarmAddrDeptName = l_Result->GetValue("alarm_addr_dept_name");
		p_oFeedBack.m_strAlarmAddrFirstType = l_Result->GetValue("alarm_addr_first_type");
		p_oFeedBack.m_strAlarmAddrSecondType = l_Result->GetValue("alarm_addr_second_type");
		p_oFeedBack.m_strAlarmAddrThirdType = l_Result->GetValue("alarm_addr_third_type");
		p_oFeedBack.m_strAlarmLongitude = l_Result->GetValue("alarm_longitude");
		p_oFeedBack.m_strAlarmLatitude = l_Result->GetValue("alarm_latitude");
		p_oFeedBack.m_strAlarmRegionType = l_Result->GetValue("alarm_region_type");
		p_oFeedBack.m_strAlarmLocationType = l_Result->GetValue("alarm_location_type");
		p_oFeedBack.m_strPeopleNumCapture = l_Result->GetValue("people_num_capture");
		p_oFeedBack.m_strPeopleNumRescue = l_Result->GetValue("people_num_rescue");
		p_oFeedBack.m_strPeopleNumSlightInjury = l_Result->GetValue("people_num_slight_injury");
		p_oFeedBack.m_strPeopleNumSeriousInjury = l_Result->GetValue("people_num_serious_injury");
		p_oFeedBack.m_strPeopleNumDeath = l_Result->GetValue("people_num_death");
		p_oFeedBack.m_strPoliceNumDispatch = l_Result->GetValue("police_num_dispatch");
		p_oFeedBack.m_strPoliceCarNumDispatch = l_Result->GetValue("police_car_num_dispatch");
		p_oFeedBack.m_strEconomyLoss = l_Result->GetValue("economy_loss");
		p_oFeedBack.m_strRetrieveEconomyLoss = l_Result->GetValue("retrieve_economy_loss");
		p_oFeedBack.m_strFirePutOutTime = l_Result->GetValue("fire_put_out_time");
		p_oFeedBack.m_strFireBuildingFirstType = l_Result->GetValue("fire_building_first_type");
		p_oFeedBack.m_strFireBuildingSecondType = l_Result->GetValue("fire_building_second_type");
		p_oFeedBack.m_strFireBuildingThirdType = l_Result->GetValue("fire_building_third_type");
		p_oFeedBack.m_strFireSourceType = l_Result->GetValue("fire_source_type");
		p_oFeedBack.m_strFireRegionType = l_Result->GetValue("fire_region_type");
		p_oFeedBack.m_strFireCauseFirstType = l_Result->GetValue("fire_cause_first_type");
		p_oFeedBack.m_strFireCauseSecondType = l_Result->GetValue("fire_cause_second_type");
		p_oFeedBack.m_strFireCauseThirdType = l_Result->GetValue("fire_cause_third_type");
		p_oFeedBack.m_strFireArea = l_Result->GetValue("fire_area");
		p_oFeedBack.m_strTrafficRoadLevel = l_Result->GetValue("traffic_road_level");
		p_oFeedBack.m_strTrafficAccidentLevel = l_Result->GetValue("traffic_accident_level");
		p_oFeedBack.m_strTrafficVehicleNo = l_Result->GetValue("traffic_vehicle_no");
		p_oFeedBack.m_strTrafficVehicleType = l_Result->GetValue("traffic_vehicle_type");
		p_oFeedBack.m_strTrafficSlaveVehicleNo = l_Result->GetValue("traffic_slave_vehicle_no");
		p_oFeedBack.m_strTrafficSlaveVehicleType = l_Result->GetValue("traffic_slave_vehicle_type");
		p_oFeedBack.m_strEventType = l_Result->GetValue("event_type");
		p_oFeedBack.m_strAlarmCalledNoType = l_Result->GetValue("alarm_called_no_type");
		p_oFeedBack.m_strCreateTime = l_Result->GetValue("create_time");
		p_oFeedBack.m_strCreateUser = l_Result->GetValue("create_user");
		p_oFeedBack.m_strUpdateTime = l_Result->GetValue("update_time");
		p_oFeedBack.m_strUpdateUser = l_Result->GetValue("update_uesr");
	}
	if (!p_oFeedBack.m_strID.empty())
	{
		return true;
	}
	return false;
}

bool CBusinessImpl::QueryAlarmUpdateTime(std::string p_strUpdateTime, std::vector<Alarm>& p_vecAlarmInfo)
{
	if (p_strUpdateTime.empty())
	{
		ICC_LOG_ERROR(m_pLog, "p_strUpdateTime is Null");
		return false;
	}

	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb_updatetime";
	l_oSeleteAlarmSQLReq.param["update_time"] = p_strUpdateTime;
	l_oSeleteAlarmSQLReq.param["orderby"] = "gxsj";

	l_oSeleteAlarmSQLReq.param["jjsj_end"] = p_strUpdateTime;

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		Alarm l_oAlarmInfo;
		if (!l_oAlarmInfo.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}

		p_vecAlarmInfo.push_back(l_oAlarmInfo);
	}
	if (p_vecAlarmInfo.size() != 0)
	{
		return true;
	}

	return false;
}

bool CBusinessImpl::QueryProcessUpdateTime(std::string p_strUpdateTime, std::vector<Process>& p_vecProcessInfo)
{
	if (p_strUpdateTime.empty())
	{
		ICC_LOG_ERROR(m_pLog, "p_strUpdateTime is Null");
		return false;
	}

	return _QueryProcess("update_time",p_strUpdateTime, p_vecProcessInfo);

}

bool CBusinessImpl::QueryFeedBackUpdateTime(std::string p_strUpdateTime, std::vector<FeedBack>& p_vecFeedBackInfo)
{
	if (p_strUpdateTime.empty())
	{
		ICC_LOG_ERROR(m_pLog, "p_strUpdateTime is Null");
		return false;
	}

	DataBase::SQLRequest l_oSeleteAlarmFeedBackSQLReq;
	l_oSeleteAlarmFeedBackSQLReq.sql_id = "select_icc_t_alarm_feedback_updatetime";
	l_oSeleteAlarmFeedBackSQLReq.param["update_time"] = p_strUpdateTime;
	l_oSeleteAlarmFeedBackSQLReq.param["orderby"] = "update_time";

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_oSeleteAlarmFeedBackSQLReq);

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	while (l_Result->Next())
	{
		FeedBack l_oFeedBack;
		l_oFeedBack.m_strID = l_Result->GetValue("id");
		l_oFeedBack.m_strAlarmID = l_Result->GetValue("alarm_id");
		l_oFeedBack.m_strProcessID = l_Result->GetValue("process_id");
		l_oFeedBack.m_strResultType = l_Result->GetValue("result_type");
		l_oFeedBack.m_strResultContent = l_Result->GetValue("result_content");
		l_oFeedBack.m_strLeaderInstruction = l_Result->GetValue("leader_instruction");
		l_oFeedBack.m_strState = l_Result->GetValue("state");
		l_oFeedBack.m_strTimeEdit = l_Result->GetValue("time_edit");
		l_oFeedBack.m_strTimeSubmit = l_Result->GetValue("time_submit");
		l_oFeedBack.m_strTimeArrived = l_Result->GetValue("time_arrived");
		l_oFeedBack.m_strTimeSigned = l_Result->GetValue("time_signed");
		l_oFeedBack.m_strTimePoliceDispatch = l_Result->GetValue("time_police_dispatch");
		l_oFeedBack.m_strTimePoliceArrived = l_Result->GetValue("time_police_arrived");
		l_oFeedBack.m_strActualOccurTime = l_Result->GetValue("actual_occur_time");
		l_oFeedBack.m_strActualOccurAddr = l_Result->GetValue("actual_occur_addr");
		l_oFeedBack.m_strFeedbackDeptDistrictCode = l_Result->GetValue("feedback_dept_district_code");
		l_oFeedBack.m_strFeedbackDeptCode = l_Result->GetValue("feedback_dept_code");
		l_oFeedBack.m_strFeedbackDeptName = l_Result->GetValue("feedback_dept_name");
		l_oFeedBack.m_strFeedbackCode = l_Result->GetValue("feedback_code");
		l_oFeedBack.m_strFeedbackName = l_Result->GetValue("feedback_name");
		l_oFeedBack.m_strFeedbackLeaderCode = l_Result->GetValue("feedback_leader_code");
		l_oFeedBack.m_strFeedbackLeaderName = l_Result->GetValue("feedback_leader_name");
		l_oFeedBack.m_strProcessDeptDistrictCode = l_Result->GetValue("process_dept_district_code");
		l_oFeedBack.m_strProcessDeptCode = l_Result->GetValue("process_dept_code");
		l_oFeedBack.m_strProcessDeptName = l_Result->GetValue("process_dept_name");
		l_oFeedBack.m_strProcessCode = l_Result->GetValue("process_code");
		l_oFeedBack.m_strProcessName = l_Result->GetValue("process_name");
		l_oFeedBack.m_strProcessLeaderCode = l_Result->GetValue("process_leader_code");
		l_oFeedBack.m_strProcessLeaderName = l_Result->GetValue("process_leader_name");
		l_oFeedBack.m_strDispatchDeptDistrictCode = l_Result->GetValue("dispatch_dept_district_code");
		l_oFeedBack.m_strDispatchDeptCode = l_Result->GetValue("dispatch_dept_code");
		l_oFeedBack.m_strDispatchDeptName = l_Result->GetValue("dispatch_dept_name");
		l_oFeedBack.m_strDispatchCode = l_Result->GetValue("dispatch_code");
		l_oFeedBack.m_strDispatchName = l_Result->GetValue("dispatch_name");
		l_oFeedBack.m_strDispatchLeaderCode = l_Result->GetValue("dispatch_leader_code");
		l_oFeedBack.m_strDispatchLeaderName = l_Result->GetValue("dispatch_leader_name");
		l_oFeedBack.m_strPersonId = l_Result->GetValue("person_id");
		l_oFeedBack.m_strPersonIdType = l_Result->GetValue("person_id_type");
		l_oFeedBack.m_strPersonNationality = l_Result->GetValue("person_nationality");
		l_oFeedBack.m_strPersonName = l_Result->GetValue("person_name");
		l_oFeedBack.m_strPersonSlaveId = l_Result->GetValue("person_slave_id");
		l_oFeedBack.m_strPersonSlaveIdType = l_Result->GetValue("person_slave_id_type");
		l_oFeedBack.m_strPersonSlaveNationality = l_Result->GetValue("person_slave_nationality");
		l_oFeedBack.m_strPersonSlaveName = l_Result->GetValue("person_slave_name");
		l_oFeedBack.m_strAlarmFirstType = l_Result->GetValue("alarm_first_type");
		l_oFeedBack.m_strAlarmSecondType = l_Result->GetValue("alarm_second_type");
		l_oFeedBack.m_strAlarmThirdType = l_Result->GetValue("alarm_third_type");
		l_oFeedBack.m_strAlarmFourthType = l_Result->GetValue("alarm_fourth_type");
		l_oFeedBack.m_strAlarmAddrDeptName = l_Result->GetValue("alarm_addr_dept_name");
		l_oFeedBack.m_strAlarmAddrFirstType = l_Result->GetValue("alarm_addr_first_type");
		l_oFeedBack.m_strAlarmAddrSecondType = l_Result->GetValue("alarm_addr_second_type");
		l_oFeedBack.m_strAlarmAddrThirdType = l_Result->GetValue("alarm_addr_third_type");
		l_oFeedBack.m_strAlarmLongitude = l_Result->GetValue("alarm_longitude");
		l_oFeedBack.m_strAlarmLatitude = l_Result->GetValue("alarm_latitude");
		l_oFeedBack.m_strAlarmRegionType = l_Result->GetValue("alarm_region_type");
		l_oFeedBack.m_strAlarmLocationType = l_Result->GetValue("alarm_location_type");
		l_oFeedBack.m_strPeopleNumCapture = l_Result->GetValue("people_num_capture");
		l_oFeedBack.m_strPeopleNumRescue = l_Result->GetValue("people_num_rescue");
		l_oFeedBack.m_strPeopleNumSlightInjury = l_Result->GetValue("people_num_slight_injury");
		l_oFeedBack.m_strPeopleNumSeriousInjury = l_Result->GetValue("people_num_serious_injury");
		l_oFeedBack.m_strPeopleNumDeath = l_Result->GetValue("people_num_death");
		l_oFeedBack.m_strPoliceNumDispatch = l_Result->GetValue("police_num_dispatch");
		l_oFeedBack.m_strPoliceCarNumDispatch = l_Result->GetValue("police_car_num_dispatch");
		l_oFeedBack.m_strEconomyLoss = l_Result->GetValue("economy_loss");
		l_oFeedBack.m_strRetrieveEconomyLoss = l_Result->GetValue("retrieve_economy_loss");
		l_oFeedBack.m_strFirePutOutTime = l_Result->GetValue("fire_put_out_time");
		l_oFeedBack.m_strFireBuildingFirstType = l_Result->GetValue("fire_building_first_type");
		l_oFeedBack.m_strFireBuildingSecondType = l_Result->GetValue("fire_building_second_type");
		l_oFeedBack.m_strFireBuildingThirdType = l_Result->GetValue("fire_building_third_type");
		l_oFeedBack.m_strFireSourceType = l_Result->GetValue("fire_source_type");
		l_oFeedBack.m_strFireRegionType = l_Result->GetValue("fire_region_type");
		l_oFeedBack.m_strFireCauseFirstType = l_Result->GetValue("fire_cause_first_type");
		l_oFeedBack.m_strFireCauseSecondType = l_Result->GetValue("fire_cause_second_type");
		l_oFeedBack.m_strFireCauseThirdType = l_Result->GetValue("fire_cause_third_type");
		l_oFeedBack.m_strFireArea = l_Result->GetValue("fire_area");
		l_oFeedBack.m_strTrafficRoadLevel = l_Result->GetValue("traffic_road_level");
		l_oFeedBack.m_strTrafficAccidentLevel = l_Result->GetValue("traffic_accident_level");
		l_oFeedBack.m_strTrafficVehicleNo = l_Result->GetValue("traffic_vehicle_no");
		l_oFeedBack.m_strTrafficVehicleType = l_Result->GetValue("traffic_vehicle_type");
		l_oFeedBack.m_strTrafficSlaveVehicleNo = l_Result->GetValue("traffic_slave_vehicle_no");
		l_oFeedBack.m_strTrafficSlaveVehicleType = l_Result->GetValue("traffic_slave_vehicle_type");
		l_oFeedBack.m_strEventType = l_Result->GetValue("event_type");
		l_oFeedBack.m_strAlarmCalledNoType = l_Result->GetValue("alarm_called_no_type");
		l_oFeedBack.m_strCreateTime = l_Result->GetValue("create_time");
		l_oFeedBack.m_strCreateUser = l_Result->GetValue("create_user");
		l_oFeedBack.m_strUpdateTime = l_Result->GetValue("update_time");
		l_oFeedBack.m_strUpdateUser = l_Result->GetValue("update_uesr");
		p_vecFeedBackInfo.push_back(l_oFeedBack);
	}
	if (p_vecFeedBackInfo.size() != 0)
	{
		return true;
	}
	return false;
}

std::string CBusinessImpl::GenAlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

std::string CBusinessImpl::GenAlarmLogAttach(std::string& type, std::string& id)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

	std::string l_strPath = "/";
	std::string l_strNum = std::to_string(0);

	if (0 == type.compare("7"))
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", "jjlyh");
	}
	else
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", type);
	}

	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/id", id);
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/name", "");
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/path", "");

	return l_pIJson->ToString();
}

void CBusinessImpl::SendRemarkToFeedBack(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received Alarm Remark From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAddAlarmRemarkRequest request;

	if (!request.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	PROTOCOL::CFeedBackInfoChange l_CFeedBackInfoChange;
	GenRespondHeader(CASEFEEDBACK_CHANGED, request.m_oHeader, l_CFeedBackInfoChange.m_oHeader);
	l_CFeedBackInfoChange.m_oHeader.m_strRequest = m_queueToVCS;
	l_CFeedBackInfoChange.m_oHeader.m_strRequestType = "0";

	PROTOCOL::CFeedBackInfoChange::CBody::CData l_oFeedBackData;
	l_oFeedBackData.m_strID = request.m_oBody.m_strGuid;
	l_oFeedBackData.m_strCaseId = request.m_oBody.m_strAlarmID;
	l_oFeedBackData.m_strDisPosalId = "";
	l_oFeedBackData.m_strCaseDispatchStatus = "";

	l_oFeedBackData.m_strFBContent = request.m_oBody.m_strContent;
	l_oFeedBackData.m_strFeedBacker = request.m_oBody.m_strFeedBackName;
	l_oFeedBackData.m_strFeedBackerOrg = request.m_oBody.m_strFeedBackDeptCode;
	l_oFeedBackData.m_strFBTime = request.m_oBody.m_strCreateTime;

	l_oFeedBackData.m_strCaseType = "";
	l_oFeedBackData.m_strCaseSubType = "";
	l_oFeedBackData.m_strCaseThreeType = "";
	l_oFeedBackData.m_strCaseProcessor = "";
	l_oFeedBackData.m_strCreateTime = request.m_oBody.m_strCreateTime;
	l_oFeedBackData.m_strCreateUser = request.m_oBody.m_strCreateUser;
	l_oFeedBackData.m_strUpdateTime = request.m_oBody.m_strCreateTime;
	l_oFeedBackData.m_strUpdateUser = request.m_oBody.m_strCreateUser;
	l_oFeedBackData.m_strDelFlag = "0";

	l_CFeedBackInfoChange.m_oBody.m_vecData.push_back(l_oFeedBackData);

	std::string l_strMsg = l_CFeedBackInfoChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

bool CBusinessImpl::InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo)
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
	return true;
}


void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = SUB_SYSTEMID;
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
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strFromOrgIdentifier = p_rAlarmLogToSync.m_strFromOrgIdentifier;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgIdentifier = p_rAlarmLogToSync.m_strToObjectOrgIdentifier;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateOrg = p_rAlarmLogToSync.m_strCreateOrg;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;

	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_ERROR(m_pLog, " alarm log sync :[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnResponse(std::string strMsg)
{
	ICC_LOG_DEBUG(m_pLog, "Recv Response msg[%s]", strMsg.c_str());
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "m_mCallRefId size=%d]", m_mCallRefId.size());
	std::map<uint64, std::string>::iterator itUpper = m_mCallRefId.upper_bound(m_pDateTime->CurrentDateTime() - m_uCallExpire);
	if (itUpper != m_mCallRefId.end())
	{
		std::map<uint64, std::string>::iterator it = m_mCallRefId.begin();
		while (it != itUpper)
		{
			m_pRedisClient->HDel(ACDCALLSTATEKEY, it->second);
			it++;
			ICC_LOG_DEBUG(m_pLog, "OnTimer HDel[%s]", it->second.c_str());
		}
		m_mCallRefId.erase(m_mCallRefId.begin(), itUpper);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
bool CBusinessImpl::_GetAlarmRelatedPersonsData(const std::string& p_strAlarmID, PROTOCOL::CAlarmRelatedPersonsQueryExRespond& p_AlarmRelatedPersonsRspd)
{
	if (p_strAlarmID.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqdsrdb_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = p_strAlarmID;
	l_tSQLReq.param["deleted"] = "false";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqdsrdb_by_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		p_AlarmRelatedPersonsRspd.m_oBody.m_strResult = "0";
		p_AlarmRelatedPersonsRspd.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "query_icc_t_jqdsrdb_by_alarm_id success! ");

		while (l_pRSet->Next())
		{
			PROTOCOL::CAlarmRelatedPersonsInfoEx l_oData;
			std::vector<std::string> vecFiledNames = l_pRSet->GetFieldNames();
			for (int i = 0; i < vecFiledNames.size(); ++i)
			{
				l_oData.m_mapInfo[vecFiledNames[i]] = l_pRSet->GetValue(vecFiledNames[i]);
			}
			p_AlarmRelatedPersonsRspd.m_oBody.m_vecAlarmRelatedPersonsInfos.push_back(l_oData);
		}

	}
	return true;
}

bool CBusinessImpl::_GetAlarmRelatedCarsData(const std::string& p_strAlarmID, PROTOCOL::CAlarmRelatedCarsQueryExRespond& p_AlarmRelatedCarsRspd)
{
	if (p_strAlarmID.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqclb_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = p_strAlarmID;
	l_tSQLReq.param["deleted"] = "0";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqclb_by_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		p_AlarmRelatedCarsRspd.m_oBody.m_strResult = "0";
		p_AlarmRelatedCarsRspd.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "query_icc_t_jqclb_by_alarm_id success! ");

		while (l_pRSet->Next())
		{
			PROTOCOL::CAlarmRelatedCarsInfoEx l_oData;
			std::vector<std::string> vecFiledNames = l_pRSet->GetFieldNames();
			for (int i = 0; i < vecFiledNames.size(); ++i)
			{
				l_oData.m_mapInfo[vecFiledNames[i]] = l_pRSet->GetValue(vecFiledNames[i]);
			}
			p_AlarmRelatedCarsRspd.m_oBody.m_vecAlarmRelatedCarsInfos.push_back(l_oData);
		}
	}
	return true;
}

void CBusinessImpl::RecvAlarmUpdateCase(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateCase From VCS: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CVcsSetAlarmInfoRequest l_CVcsSetAlarmInfoRequest;
	if (!l_CVcsSetAlarmInfoRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	if (!l_pIJson->LoadJson(p_strMsg))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdateCase parse msg failed!!!");
		return;
	}
	_UpdateCaseSyncTime(l_CVcsSetAlarmInfoRequest.m_oHeader.m_strMsgid);

	std::string l_strId = l_pIJson->GetNodeValue("/body/id", "");
	std::string l_strTime = l_pIJson->GetNodeValue("/body/receiving_time", "");
	std::string l_strReceivedtime = l_pIJson->GetNodeValue("/body/received_time", "");
	std::string l_strReceiptCode = l_pIJson->GetNodeValue("/body/receipt_code", "");
	Alarm l_oTmpAlarm;
	bool bQuery = true;
	if (!QueryAlarm(l_strId, l_oTmpAlarm))
	{
		bQuery = false;
		ICC_LOG_ERROR(m_pLog, "RecvUpdateCase,query alarm fail alarm_id = [%s]", l_strId.c_str());

		DataBase::SQLRequest l_tSQLReqInsertAlarm;
		l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_jjdb";
		l_tSQLReqInsertAlarm.param["id"] = l_strId;
		l_tSQLReqInsertAlarm.param["receiving_time"] = l_strTime;// m_pDateTime->CurrentDateTimeStr();
		l_tSQLReqInsertAlarm.param["create_time"] = l_strTime;
		l_tSQLReqInsertAlarm.param["receipt_code"] = l_strReceiptCode;
		l_tSQLReqInsertAlarm.param["received_time"] = l_strReceivedtime;
		l_tSQLReqInsertAlarm.param["is_invalid"] = "0";
		l_tSQLReqInsertAlarm.param["is_merge"] = "0";
		l_tSQLReqInsertAlarm.param["is_visitor"] = "0";
		l_tSQLReqInsertAlarm.param["is_feedback"] = "0";
		l_tSQLReqInsertAlarm.param["is_delete"] = "0";
		l_tSQLReqInsertAlarm.param["had_push"] = "0";
		l_tSQLReqInsertAlarm.param["is_over"] = "0";

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "RecvUpdateCase1 insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return ;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "RecvUpdateCase1,%s Exec sql success! sql:[%s]", l_tSQLReqInsertAlarm.sql_id.c_str(), l_pRSet->GetSQL().c_str());
		}
	}

	Alarm l_oAlarm;
	l_oAlarm.ParseString("/body", l_pIJson);

	std::string strFlag = "1";
	if (!bQuery)
	{
		//没有查询到 新增一条记录
		strFlag = "0";
	}
	int l_iCount = l_pIJson->GetCount("/body/RELATED_PERSONS");
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateCase1,RELATED_PERSONS count=%d,UpdateType=%s", l_iCount, strFlag.c_str());
	for (int i = 0; i < l_iCount; i++)
	{
		PROTOCOL::CAlarmRelatedPersonsInfoEx tmpPersonsInfo;
		std::string strPre("/body/RELATED_PERSONS/" + std::to_string(i) + "/");
		tmpPersonsInfo.ParseString(strPre, l_pIJson);
		_DBChangeAlarmRelatedPersons(tmpPersonsInfo, strFlag);
	}

	l_iCount = 0;
	l_iCount = l_pIJson->GetCount("/body/RELATED_CARS");
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateCase1,RELATED_CARS count=%d", l_iCount);
	for (int i = 0; i < l_iCount; i++)
	{
		PROTOCOL::CAlarmRelatedCarsInfoEx tmpCarsInfoEx;
		std::string strPre("/body/RELATED_CARS/" + std::to_string(i) + "/");
		tmpCarsInfoEx.ParseString(strPre, l_pIJson);
		_DBChangeAlarmRelatedCars(tmpCarsInfoEx, strFlag);
	}

	std::vector<Process> l_vecProcess;
	if (!QueryAlarmProcessByAlarmID(l_strId, l_vecProcess))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdateCase query process fail alarm_id = [%s]", l_strId.c_str());
		return;
	}

	SendAlarmUpdateRequest(l_CVcsSetAlarmInfoRequest.m_oHeader, l_oAlarm, l_vecProcess);
}

//辖区变更
void CBusinessImpl::RecvUpdateMasterorg(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateMasterorg From VCS: [%s]", p_strMsg.c_str());
	
	PROTOCOL::CVcsSetAlarmInfoRequest l_CVcsSetAlarmInfoRequest;
	if (!l_CVcsSetAlarmInfoRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CVcsSetAlarmInfoRequest.m_oHeader.m_strMsgid);

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_pIJson->LoadJson(p_strMsg))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdateMasterorg parse msg failed!!!");
		return;
	}
	std::string m_strId = l_pIJson->GetNodeValue("/body/caseid", "");
	Alarm l_oAlarm;
	if (!QueryAlarm(m_strId, l_oAlarm))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdateMasterorg query alarm fail alarm_id = [%s]", m_strId.c_str());
		return;
	}
	l_oAlarm.m_strID = m_strId;
	//l_oAlarm.m_strAdminDeptCode = l_pIJson->GetNodeValue("/body/orgid", "");
	l_oAlarm.m_strAdminDeptName = l_pIJson->GetNodeValue("/body/orgname", "");
	l_oAlarm.m_strAdminDeptCode = l_pIJson->GetNodeValue("/body/orgcode", "");
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateMasterorg, id=[%s],AdminDeptName=%s,AdminDeptCode=%s", m_strId.c_str(), l_oAlarm.m_strAdminDeptName.c_str(), l_oAlarm.m_strAdminDeptCode.c_str());

	std::vector<Process> l_vecProcess;
	if (!QueryAlarmProcessByAlarmID(m_strId, l_vecProcess))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdateMasterorg,query process fail alarm_id = [%s]", m_strId.c_str());
		return;
	}

	SendAlarmUpdateRequest(l_CVcsSetAlarmInfoRequest.m_oHeader, l_oAlarm, l_vecProcess);
}

//经纬度变更
void CBusinessImpl::RecvUpdatePostion(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdatePostion From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CVcsSetAlarmInfoRequest l_CVcsSetAlarmInfoRequest;
	if (!l_CVcsSetAlarmInfoRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CVcsSetAlarmInfoRequest.m_oHeader.m_strMsgid);

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_pIJson->LoadJson(p_strMsg))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdatePostion parse msg failed!!!");
		return;
	}
	std::string m_strId = l_CVcsSetAlarmInfoRequest.m_oBody.m_strId;
	Alarm l_oAlarm;
	if (!QueryAlarm(m_strId, l_oAlarm))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdatePostion query alarm fail alarm_id = [%s]", m_strId.c_str());
		return;
	}
	l_oAlarm.m_strID = m_strId;
	l_oAlarm.m_strAddr = l_pIJson->GetNodeValue("/body/Address", "");
	l_oAlarm.m_strManualLongitude = l_pIJson->GetNodeValue("/body/lon", "");
	l_oAlarm.m_strManualLatitude = l_pIJson->GetNodeValue("/body/lat", "");
	ICC_LOG_DEBUG(m_pLog, "RecvUpdatePostion, id=[%s],strLongitude=%s,strLatitude=%s", m_strId.c_str(), l_oAlarm.m_strManualLongitude.c_str(), l_oAlarm.m_strManualLatitude.c_str());
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "select_icc_t_jjdb";
	l_tSQLReqInsertAlarm.param["id"] = l_oAlarm.m_strID;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_DEBUG(m_pLog, "Select Alarm sql:[%s]", l_pRSet->GetSQL().c_str());

	std::string strLongitude;
	std::string strLatitude;
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return ;
	}
	if (l_pRSet->Next())
	{
		if (l_pRSet->GetValue("bjrxzb").empty()&&!l_oAlarm.m_strManualLongitude.empty())
		{
			strLongitude = l_oAlarm.m_strManualLongitude;
		}
		if (l_pRSet->GetValue("bjryzb").empty()&&!l_oAlarm.m_strManualLatitude.empty())
		{
			strLatitude = l_oAlarm.m_strManualLatitude;
		}
	}
	ICC_LOG_LOWDEBUG(m_pLog, "Longitude:[%s],Latitude:[%s]", strLongitude.c_str(), strLatitude.c_str());

	l_tSQLReqInsertAlarm.param.clear();
	std::vector<Process> l_vecProcess;
	if (!QueryAlarmProcessByAlarmID(m_strId, l_vecProcess))
	{
		ICC_LOG_ERROR(m_pLog, "RecvUpdatePostion,query process fail alarm_id = [%s]", l_CVcsSetAlarmInfoRequest.m_oBody.m_strId.c_str());
		return;
	}

	SendAlarmUpdateRequest(l_CVcsSetAlarmInfoRequest.m_oHeader, l_oAlarm, l_vecProcess);
}



// 联合单位 VCS过来的消息
void CBusinessImpl::RecvUpdateLinkedDispatch(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateLinkkedDispatch From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CAddOrUpdateLinkedRequest l_AddOrUpdateLinkedRequest;
	if (!l_AddOrUpdateLinkedRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	l_AddOrUpdateLinkedRequest.m_oHeader.m_strCmd = "add_or_update_linked_dispatch";
	l_AddOrUpdateLinkedRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_AddOrUpdateLinkedRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_AddOrUpdateLinkedRequest.m_oHeader.m_strRequestType = "0";
	l_AddOrUpdateLinkedRequest.m_oHeader.m_strRequestFlag = "MQ";
	l_AddOrUpdateLinkedRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_AddOrUpdateLinkedRequest.m_oBody.m_strMsgSource = "vcs_relocated"; //赋值表示从VCS来的

	std::string l_strMessage = l_AddOrUpdateLinkedRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//派警单新增、更新
void CBusinessImpl::RecvUpdateDisposal(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateDisposal From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CAddOrUpdateProcessRequest l_CAddOrUpdateProcessRequest;
	/*
	//CAddOrUpdateProcessRequest.h
	PROTOCOL::CUpdateDisposalRequest l_CUpdateDisposalRequest;
	if (!l_CUpdateDisposalRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	l_CAddOrUpdateProcessRequest.m_oBody.m_vecData.assign(l_CUpdateDisposalRequest.m_oBody.m_vecProcessData.begin(), l_CUpdateDisposalRequest.m_oBody.m_vecProcessData.end());
	*/
	if (!l_CAddOrUpdateProcessRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgid);

	int i = 0;
	for (auto process : l_CAddOrUpdateProcessRequest.m_oBody.m_vecData)
	{
		std::string l_strID = process.m_strID;
		Process l_oProcess;
		if (QueryAlarmProcessByID(l_strID, l_oProcess))
		{
			ICC_LOG_DEBUG(m_pLog, "QueryAlarmProcessByID return true,id:[%s]", l_strID.c_str());
			l_CAddOrUpdateProcessRequest.m_oBody.m_vecData[i].m_strUpdateType = "1";
		}
		else
		{
			//process.m_strUpdateType = "0";
			l_CAddOrUpdateProcessRequest.m_oBody.m_vecData[i].m_strUpdateType = "0";
			ICC_LOG_DEBUG(m_pLog, "QueryAlarmProcessByID return false,id:[%s],updatetype:[%s]", l_strID.c_str(), l_CAddOrUpdateProcessRequest.m_oBody.m_vecData[i].m_strUpdateType.c_str());
		}

		l_CAddOrUpdateProcessRequest.m_oBody.m_vecData[i].m_strMsgSource = "vcs_relocated"; //标识从VCS过去的
		i++;
	}

	//l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgid = l_CUpdateDisposalRequest.m_oHeader.m_strMsgid;
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strCmd = "add_or_update_process_request";
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestType = "0";
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestFlag = "MQ";
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CAddOrUpdateProcessRequest.m_oBody.m_strMsgSource = "vcs_relocated"; //赋值表示从VCS来的

	std::string l_strMessage = l_CAddOrUpdateProcessRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]",l_strMessage.c_str());

}

//派警单删除
void CBusinessImpl::RecvDeleteDisposal(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvDeleteDisposal From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CVcsDeleteDisposalRequset l_CVcsDeleteDisposalRequset;
	if (!l_CVcsDeleteDisposalRequset.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CVcsDeleteDisposalRequset.m_oHeader.m_strMsgid);

	PROTOCOL::CAddOrUpdateProcessRequest l_CAddOrUpdateProcessRequest;
	for (auto process : l_CVcsDeleteDisposalRequset.m_oBody.m_vecData)
	{
		std::string m_strId = process.m_strID;
		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_pDisposalData;
		if (!QueryAlarmProcessByIDEx(m_strId, l_pDisposalData))
		{
			ICC_LOG_ERROR(m_pLog, "RecvDeleteDisposal query id fail id = [%s]", m_strId.c_str());
			continue;
		}

		l_pDisposalData.m_strID = m_strId;
		l_pDisposalData.m_strState = "17";  //见服务警情处理插件Define.h定义 PROCESS_STATUS_RETURN  //退单state跟旭伟协商，重新定位为17
		l_pDisposalData.m_strUpdateUser = process.m_strUserID;
		l_pDisposalData.m_strUpdateTime = process.m_strUpdateTime;

		l_pDisposalData.m_strMsgSource = "vcs_relocated"; //标识从VCS过去的

		l_pDisposalData.m_strCancelReason = process.m_strCancelReason;
		l_CAddOrUpdateProcessRequest.m_oBody.m_vecData.push_back(l_pDisposalData);
	}

	l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgid = l_CVcsDeleteDisposalRequset.m_oHeader.m_strMsgid;
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strCmd = "add_or_update_process_request";
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestType = "0";
	l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestFlag = "MQ";
	l_CAddOrUpdateProcessRequest.m_oBody.m_strMsgSource = "vcs_relocated"; //赋值表示从VCS来的
	std::string l_strMessage = l_CAddOrUpdateProcessRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//反馈单
void CBusinessImpl::RecvUpdateFeedback(ObserverPattern::INotificationPtr p_pRequest)
{

	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvUpdateFeedback From VCS: [%s]", p_strMsg.c_str());

	PROTOCOL::CFeedBackExSync l_CFeedBackExSync;
	PROTOCOL::CFeedBackExSync l_CUpdateFeedbackRequest;
	if (!l_CUpdateFeedbackRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Message error");
		return;
	}

	l_CFeedBackExSync.m_oBody.m_oFeekBackInfo = l_CUpdateFeedbackRequest.m_oBody.m_oFeekBackInfo;
	bool isUpdateFeedBackFlag = false;
	if (_QueryFeedBack(l_CUpdateFeedbackRequest.m_oBody.m_oFeekBackInfo.m_strID, isUpdateFeedBackFlag))
	{
		if (!isUpdateFeedBackFlag) {
			_InsertFeedBackData(l_CUpdateFeedbackRequest.m_oBody.m_oFeekBackInfo);
			l_CFeedBackExSync.m_oBody.m_strSyncType = "1";
		}
		else {
			_UpdateFeedBackData(l_CUpdateFeedbackRequest.m_oBody.m_oFeekBackInfo);
			l_CFeedBackExSync.m_oBody.m_strSyncType = "2";
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "_QueryFeedBack sql  error or id empty [%s]", l_CUpdateFeedbackRequest.m_oBody.m_oFeekBackInfo.m_strID.c_str());
	}

	//发同步
	{
		l_CFeedBackExSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_CFeedBackExSync.m_oHeader.m_strCmd = "feedback_sync";
		l_CFeedBackExSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_CFeedBackExSync.m_oHeader.m_strRequest = QUEUE_ALARM;
		l_CFeedBackExSync.m_oHeader.m_strRequestType = "0";
		l_CFeedBackExSync.m_oHeader.m_strRequestFlag = "MQ";
		l_CFeedBackExSync.m_oHeader.m_strResponse = "VCS";
		std::string l_strMessage = l_CFeedBackExSync.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::RecvNotifyIncomingCall(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvNotifyIncomingCall From VCS: [%s]", p_strMsg.c_str());

	PROTOCOL::CTelinComingChange l_CTelinComingChange;
	if (!l_CTelinComingChange.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CTelinComingChange.m_oHeader.m_strMsgid);

	std::string m_strId = l_CTelinComingChange.m_oBody.m_strCaseID;
	Alarm l_oAlarm;
	if (!QueryAlarm(m_strId, l_oAlarm))
	{
		ICC_LOG_ERROR(m_pLog, "RecvNotifyIncomingCall query alarm fail alarm_id = [%s]", m_strId.c_str());
		return;
	}
	l_oAlarm.m_strID = m_strId;
	l_oAlarm.m_strAddr = l_CTelinComingChange.m_oBody.m_strAddress;
	l_oAlarm.m_strLongitude = l_CTelinComingChange.m_oBody.m_strLongitude;
	l_oAlarm.m_strLatitude = l_CTelinComingChange.m_oBody.m_strLatitude;
	//l_oAlarm.m_strCityCode = l_CTelinComingChange.m_oBody.m_strRegionCode;
	l_oAlarm.m_strUpdateTime = l_CTelinComingChange.m_oBody.m_strUpdateTime;

	l_oAlarm.m_strAdminDeptCode = l_CTelinComingChange.m_oBody.m_strRegionCode;
	l_oAlarm.m_strAdminDeptName = _GetDeptName(l_oAlarm.m_strAdminDeptCode);
	ICC_LOG_DEBUG(m_pLog, "RecvNotifyIncomingCall, id=[%s],strLongitude=%s,strLatitude=%s", m_strId.c_str(), l_oAlarm.m_strLongitude.c_str(), l_oAlarm.m_strLatitude.c_str());

	std::vector<Process> l_vecProcess;
	if (!QueryAlarmProcessByAlarmID(m_strId, l_vecProcess))
	{
		ICC_LOG_ERROR(m_pLog, "RecvNotifyIncomingCall,query process fail alarm_id = [%s]", m_strId.c_str());
		return;
	}
	SendAlarmUpdateRequest(l_CTelinComingChange.m_oHeader, l_oAlarm, l_vecProcess);
}

std::string CBusinessImpl::_GetDeptName(const std::string& p_strAdminDeptCode)
{
	std::string l_strDeptGuid;
	m_pRedisClient->HGet("DeptCodeGuidMap", p_strAdminDeptCode, l_strDeptGuid);
	if (l_strDeptGuid.empty())
	{
		ICC_LOG_ERROR(m_pLog, "getCodeGuid is empty,AdminDeptCode = [%s]", p_strAdminDeptCode.c_str());
		return "";
	}
	std::string l_strInfo;
	m_pRedisClient->HGet("DeptInfoKey", l_strDeptGuid, l_strInfo);
	CDeptInfo l_oDeptInfo;
	if (l_oDeptInfo.Parse(l_strInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		return l_oDeptInfo.m_strName;
	}
	ICC_LOG_ERROR(m_pLog, "GetDeptName is empty,AdminDeptCode = [%s]", p_strAdminDeptCode.c_str());
	return "";
}

void CBusinessImpl::OnAdminDeptRequset(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	PROTOCOL::CAdminDeptRequset l_CAdminDeptRequset;
	if (!l_CAdminDeptRequset.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "OnAdaminDeptRequset From ICC: [%s]", p_strMsg.c_str());

	PROTOCOL::CAdminDeptRespond l_CAdminDeptRespond;
	l_CAdminDeptRespond.m_oHeader.m_strMsgId = l_CAdminDeptRequset.m_oHeader.m_strMsgId;
	l_CAdminDeptRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMessage = l_CAdminDeptRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	if (p_pRequest)
	{
		p_pRequest->Response(l_strRespondMessage);
	}

	PROTOCOL::CTelinComingChange l_CTelinComingChange;
	l_CTelinComingChange.m_oBody.m_strSeatNo = l_CAdminDeptRequset.m_oBody.m_strStaffCode;// l_CAdminDeptRequset.m_oHeader.m_strSeatNo;
	l_CTelinComingChange.m_oBody.m_strCaseID = l_CAdminDeptRequset.m_oBody.m_strCaseID;
	//std::string m_strPhone;		    		    //报警人号码
	
	l_CTelinComingChange.m_oBody.m_strCallingTime = l_CAdminDeptRequset.m_oBody.m_strCallingtime;
	l_CTelinComingChange.m_oBody.m_strAnsweringTime = l_CAdminDeptRequset.m_oBody.m_strAnsweringtime;
	
	l_CTelinComingChange.m_oBody.m_strLongitude = l_CAdminDeptRequset.m_oBody.m_strLon;
	l_CTelinComingChange.m_oBody.m_strLatitude = l_CAdminDeptRequset.m_oBody.m_strLat;
	l_CTelinComingChange.m_oBody.m_strAddress = l_CAdminDeptRequset.m_oBody.m_strAddress;
	l_CTelinComingChange.m_oBody.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();			//更新时间

	l_CTelinComingChange.m_oBody.m_strPersonName = l_CAdminDeptRequset.m_oBody.m_strAlarmPersonName;
	l_CTelinComingChange.m_oBody.m_strPersonSex = l_CAdminDeptRequset.m_oBody.m_strAlarmPersonSex;
	l_CTelinComingChange.m_oBody.m_strPersonTag = l_CAdminDeptRequset.m_oBody.m_strAlarmPersonTag;
	l_CTelinComingChange.m_oBody.m_strPersonID = l_CAdminDeptRequset.m_oBody.m_strAlarmPersonID;

	l_CTelinComingChange.m_oBody.m_strCallerNum = l_CAdminDeptRequset.m_oBody.m_strCallerno;
	l_CTelinComingChange.m_oBody.m_strCalledNum = l_CAdminDeptRequset.m_oBody.m_strCalledno;

	l_CTelinComingChange.m_oBody.m_strCallID = l_CAdminDeptRequset.m_oBody.m_strCallRefId;

	GenRespondHeader(CMD_NOTIFY_INCOMINGCALL, l_CAdminDeptRequset.m_oHeader, l_CTelinComingChange.m_oHeader);
	l_CTelinComingChange.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_CTelinComingChange.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;

	l_CTelinComingChange.m_oHeader.m_strMsgid = l_CAdminDeptRequset.m_oHeader.m_strMsgid;
	l_CTelinComingChange.m_oHeader.m_strCmd = CMD_NOTIFY_INCOMINGCALL;
	l_CTelinComingChange.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CTelinComingChange.m_oHeader.m_strRequestType = "0";

	std::string  l_strMsg = l_CTelinComingChange.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());

	if (!l_CAdminDeptRequset.m_oBody.m_strCallRefId.empty())
	{
		ACDCallState l_ACDCallState;
		l_ACDCallState.m_strCallRefId = l_CAdminDeptRequset.m_oBody.m_strCallRefId;
		l_ACDCallState.m_strAlarmID = l_CAdminDeptRequset.m_oBody.m_strCaseID;

		std::string l_strVal = l_ACDCallState.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pRedisClient->HSet(ACDCALLSTATEKEY, l_ACDCallState.m_strCallRefId, l_strVal);
	}
	
}

void CBusinessImpl::OnSetTemporaryHoldRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	PROTOCOL::CSetTemporaryHoldRequest l_SetTemporaryHoldRequest;
	if (!l_SetTemporaryHoldRequest.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "OnSetTemporaryHoldRequest From ICC: [%s]", p_strMsg.c_str());

	PROTOCOL::CSetTemporaryHoldRespond l_SetTemporaryHoldRespond;
	l_SetTemporaryHoldRespond.m_oHeader.m_strMsgId = l_SetTemporaryHoldRequest.m_oHeader.m_strMsgId;
	l_SetTemporaryHoldRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMessage = l_SetTemporaryHoldRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	if (p_pRequest)
	{
		p_pRequest->Response(l_strRespondMessage);
	}

	PROTOCOL::CSetTemporaryHoldSync l_SetTemporaryHoldSync;
	l_SetTemporaryHoldSync.m_oBody.m_strCaseID = l_SetTemporaryHoldRequest.m_oBody.m_strCaseID;
	//将消息转发给VCS
	l_SetTemporaryHoldSync.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_SetTemporaryHoldSync.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;
	l_SetTemporaryHoldSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_SetTemporaryHoldSync.m_oHeader.m_strCmd = "notify_temporaryhold";
	l_SetTemporaryHoldSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_SetTemporaryHoldSync.m_oHeader.m_strRequestType = "0";

	std::string  l_strMsg = l_SetTemporaryHoldSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());

	//消息发送至ICC-Server
	//l_SetTemporaryHoldSync.m_oHeader.m_strRequest = "topic_device_state_sync";
	//l_SetTemporaryHoldSync.m_oHeader.m_strRequestType = "1";
	//l_SetTemporaryHoldSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	//l_SetTemporaryHoldSync.m_oHeader.m_strCmd = "close_temporary_alarm";
	//l_SetTemporaryHoldSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	//l_strMsg = l_SetTemporaryHoldSync.ToString(ICCGetIJsonFactory()->CreateJson());
	//m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	//ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnRecvUpdateLog(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnRecvUpdateLog From VCS: [%s]", p_strMsg.c_str());

	PROTOCOL::CVcsUpdateLogRequset l_CVcsUpdateLogRequset;
	if (!l_CVcsUpdateLogRequset.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_CVcsUpdateLogRequset.m_oHeader.m_strMsgid);

	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	//l_oAlarmLogSync.m_oBody.m_strID = m_pString->CreateGuid();
	l_oAlarmLogSync.m_oBody.m_strID = l_CVcsUpdateLogRequset.m_oBody.m_strID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = l_CVcsUpdateLogRequset.m_oBody.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = l_CVcsUpdateLogRequset.m_oBody.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = l_CVcsUpdateLogRequset.m_oBody.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = l_CVcsUpdateLogRequset.m_oBody.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = l_CVcsUpdateLogRequset.m_oBody.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = l_CVcsUpdateLogRequset.m_oBody.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = l_CVcsUpdateLogRequset.m_oBody.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = l_CVcsUpdateLogRequset.m_oBody.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = l_CVcsUpdateLogRequset.m_oBody.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = l_CVcsUpdateLogRequset.m_oBody.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = l_CVcsUpdateLogRequset.m_oBody.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = l_CVcsUpdateLogRequset.m_oBody.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = l_CVcsUpdateLogRequset.m_oBody.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = l_CVcsUpdateLogRequset.m_oBody.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = "vcs";
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = l_CVcsUpdateLogRequset.m_oBody.m_strOperateAttachDesc;

	l_oAlarmLogSync.m_oBody.m_strOperateContent = "{\"param\":" + l_oAlarmLogSync.m_oBody.m_strOperateContent + "}";


	l_oAlarmLogSync.m_oBody.m_strFromOrgIdentifier = l_CVcsUpdateLogRequset.m_oBody.m_strFromOrgIdentifier;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgIdentifier = l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgIdentifier;
	l_oAlarmLogSync.m_oBody.m_strDescription = l_CVcsUpdateLogRequset.m_oBody.m_strDescription;
	l_oAlarmLogSync.m_oBody.m_strCreateOrg = l_CVcsUpdateLogRequset.m_oBody.m_strCreateOrg;
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = l_CVcsUpdateLogRequset.m_oBody.m_strReceivedTime;
	//发消息中心
	SendMessageToMsgCenterOnManualAccept(l_oAlarmLogSync, p_strMsg);

	//TODO::记录警情流水日志
	{
		//使用VCS同步过来的创建时间，如果为空则取本地当前时间
		if (l_oAlarmLogSync.m_oBody.m_strCreateTime.empty())
		{
			std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
			l_oAlarmLogSync.m_oBody.m_strCreateTime = l_strCurTime;
		}
		
		InsertDBAlarmLogInfo(l_oAlarmLogSync.m_oBody);
	}
	
	//发同步
	{
		std::string l_strGuid = m_pString->CreateGuid();
		l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "ICC";// SUBSYSTEMID;
		l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
		l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
		l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
		l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
		l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
		l_oAlarmLogSync.m_oHeader.m_strResponse = "VCS";
		l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, ICCGetIJsonFactory()->CreateJson());

		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s],m_strContent:%s", l_strMessage.c_str(), l_oAlarmLogSync.m_oBody.m_strOperateContent.c_str());
	}
}

void CBusinessImpl::RecvAlarmLogSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "RecvAlarmLogSync: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	if (!l_oAlarmLogSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	if (l_oAlarmLogSync.m_oHeader.m_strResponse == "VCS")
	{
		return;
	}

	PROTOCOL::CVcsUpdateLogRequset l_CVcsUpdateLogRequset;
	l_CVcsUpdateLogRequset.m_oBody.m_strID =l_oAlarmLogSync.m_oBody.m_strID;
	l_CVcsUpdateLogRequset.m_oBody.m_strAlarmID = l_oAlarmLogSync.m_oBody.m_strAlarmID;
	l_CVcsUpdateLogRequset.m_oBody.m_strProcessID = l_oAlarmLogSync.m_oBody.m_strProcessID;
	l_CVcsUpdateLogRequset.m_oBody.m_strFeedbackID = l_oAlarmLogSync.m_oBody.m_strFeedbackID;
	l_CVcsUpdateLogRequset.m_oBody.m_strSeatNo = l_oAlarmLogSync.m_oBody.m_strSeatNo;
	l_CVcsUpdateLogRequset.m_oBody.m_strOperate = l_oAlarmLogSync.m_oBody.m_strOperate;
	l_CVcsUpdateLogRequset.m_oBody.m_strOperateContent = l_oAlarmLogSync.m_oBody.m_strOperateContent;
	l_CVcsUpdateLogRequset.m_oBody.m_strFromType = l_oAlarmLogSync.m_oBody.m_strFromType;
	l_CVcsUpdateLogRequset.m_oBody.m_strFromObject = l_oAlarmLogSync.m_oBody.m_strFromObject;
	l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectName = l_oAlarmLogSync.m_oBody.m_strFromObjectName;
	l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectOrgName = l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName;
	l_CVcsUpdateLogRequset.m_oBody.m_strFromObjectOrgCode = l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode;
	l_CVcsUpdateLogRequset.m_oBody.m_strToType = l_oAlarmLogSync.m_oBody.m_strToType;
	l_CVcsUpdateLogRequset.m_oBody.m_strToObject = l_oAlarmLogSync.m_oBody.m_strToObject;
	l_CVcsUpdateLogRequset.m_oBody.m_strToObjectName = l_oAlarmLogSync.m_oBody.m_strToObjectName;
	l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgName = l_oAlarmLogSync.m_oBody.m_strToObjectOrgName;
	l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgCode = l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode;
	l_CVcsUpdateLogRequset.m_oBody.m_strCreateUser = l_oAlarmLogSync.m_oBody.m_strCreateUser;
	l_CVcsUpdateLogRequset.m_oBody.m_strCreateTime = l_oAlarmLogSync.m_oBody.m_strCreateTime;
	l_CVcsUpdateLogRequset.m_oBody.m_strDeptOrgCode = l_oAlarmLogSync.m_oBody.m_strDeptOrgCode;
	l_CVcsUpdateLogRequset.m_oBody.m_strSourceName = l_oAlarmLogSync.m_oBody.m_strSourceName;
	if (l_CVcsUpdateLogRequset.m_oBody.m_strSourceName.empty() || l_CVcsUpdateLogRequset.m_oBody.m_strSourceName == "icc")
	{
		l_CVcsUpdateLogRequset.m_oBody.m_strSourceName = "3";
	}
	else if (l_CVcsUpdateLogRequset.m_oBody.m_strSourceName == "vcs")
	{
		l_CVcsUpdateLogRequset.m_oBody.m_strSourceName = "1";
	}
	else if (l_CVcsUpdateLogRequset.m_oBody.m_strSourceName == "mpa")
	{
		l_CVcsUpdateLogRequset.m_oBody.m_strSourceName = "2";
	}
	l_CVcsUpdateLogRequset.m_oBody.m_strOperateAttachDesc = l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc;

	l_CVcsUpdateLogRequset.m_oBody.m_strFromOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strFromOrgIdentifier;
	l_CVcsUpdateLogRequset.m_oBody.m_strToObjectOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strToObjectOrgIdentifier;
	l_CVcsUpdateLogRequset.m_oBody.m_strDescription = l_oAlarmLogSync.m_oBody.m_strDescription;
	l_CVcsUpdateLogRequset.m_oBody.m_strCreateOrg = l_oAlarmLogSync.m_oBody.m_strCreateOrg;
	l_CVcsUpdateLogRequset.m_oBody.m_strReceivedTime = l_oAlarmLogSync.m_oBody.m_strReceivedTime;

	l_CVcsUpdateLogRequset.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_CVcsUpdateLogRequset.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;
	l_CVcsUpdateLogRequset.m_oHeader.m_strMsgid = l_oAlarmLogSync.m_oHeader.m_strMsgid;
	l_CVcsUpdateLogRequset.m_oHeader.m_strCmd = "update_log";
	l_CVcsUpdateLogRequset.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CVcsUpdateLogRequset.m_oHeader.m_strRequestType = "0";

	std::string  l_strMsg = l_CVcsUpdateLogRequset.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


bool CBusinessImpl::_DBChangeAlarmRelatedPersons(PROTOCOL::CAlarmRelatedPersonsInfoEx& p_PersonsInfo, const std::string& p_strUpdateType)
{
	DataBase::SQLRequest l_tSQLReq;
	if (p_strUpdateType == "0")
	{
		//插入
		l_tSQLReq.sql_id = "insert_icc_t_jqdsrdb";  //insert_icc_t_alarm_related_persons
		std::map<std::string, std::string>& mapTmp = p_PersonsInfo.m_mapInfo;
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
		{
			l_tSQLReq.param[itr_const->first] = itr_const->second;
		}
	}
	else if (p_strUpdateType == "2")
	{
		//删除
		l_tSQLReq.sql_id = "delete_icc_t_jqdsrdb_by_guid";//delete_icc_t_alarm_related_persons_by_guid
		l_tSQLReq.param[PROTOCOL::Ex_Persons_JQDSRDBH] = p_PersonsInfo.m_mapInfo[PROTOCOL::Ex_Persons_JQDSRDBH];
	}
	else
	{
		l_tSQLReq.sql_id = "update_icc_t_jqdsrdb"; //update_icc_t_alarm_related_persons
		std::map<std::string, std::string>& mapTmp = p_PersonsInfo.m_mapInfo;
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
		{
			if (itr_const->first == PROTOCOL::Ex_Persons_JQDSRDBH)
			{
				l_tSQLReq.param[itr_const->first] = itr_const->second;
			}
			else
			{
				if (!itr_const->second.empty())
				{
					l_tSQLReq.set[itr_const->first] = itr_const->second;
				}
			}
		}
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "%s Exec sql failed! sql:[%s] id=%s", l_tSQLReq.sql_id.c_str(), l_pRSet->GetSQL().c_str(), l_tSQLReq.param[PROTOCOL::Ex_Persons_JQDSRDBH].c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s Exec sql success! sql:[%s]", l_tSQLReq.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}
	return true;
}

bool CBusinessImpl::_DBChangeAlarmRelatedCars(PROTOCOL::CAlarmRelatedCarsInfoEx& p_RelatedCarsInfo, const std::string& p_strUpdateType)
{
	DataBase::SQLRequest l_tSQLReq;
	if (p_strUpdateType == "0")
	{
		//插入
		//l_tSQLReq.sql_id = "insert_icc_t_alarm_related_cars";
		l_tSQLReq.sql_id = "insert_icc_t_jqclb";
		std::map<std::string, std::string>& mapTmp = p_RelatedCarsInfo.m_mapInfo;
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
		{
			l_tSQLReq.param[itr_const->first] = itr_const->second;
		}
	}
	else if (p_strUpdateType == "2")
	{
		//删除
		//l_tSQLReq.sql_id = "delete_icc_t_alarm_related_cars_by_guid";
		l_tSQLReq.sql_id = "delete_icc_t_jqclb_by_guid";
		l_tSQLReq.param[PROTOCOL::Ex_Cars_JQSACLBH] = p_RelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH];
	}
	else
	{
		//l_tSQLReq.sql_id = "update_icc_t_alarm_related_cars";
		l_tSQLReq.sql_id = "update_icc_t_jqclb";
		std::map<std::string, std::string>& mapTmp = p_RelatedCarsInfo.m_mapInfo;
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
		{
			if (itr_const->first == PROTOCOL::Ex_Cars_JQSACLBH)
			{
				l_tSQLReq.param[itr_const->first] = itr_const->second;
			}
			else
			{
				if (!itr_const->second.empty())
				{
					l_tSQLReq.set[itr_const->first] = itr_const->second;
				}
			}
		}
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "%s Exec sql failed! sql:[%s]", l_tSQLReq.sql_id.c_str(), l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s Exec sql success! sql:[%s]", l_tSQLReq.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}

	return true;
}


void CBusinessImpl::OnVcsAlarmRelatedCarsRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnVcsAlarmRelatedCarsRequest From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CAlarmRelatedCarsChangeSync l_oCarsChangeSync;
	if (!l_oCarsChangeSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_oCarsChangeSync.m_oHeader.m_strMsgid);
	//return;
	//PROTOCOL::CAlarmRelatedCarsChangeSync l_oVcsAlarmRelatedCarsRequest;
	//转发给ICC
	l_oCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oCarsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_oCarsChangeSync.m_oHeader.m_strRequestType = "0";
	l_oCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oCarsChangeSync.m_oHeader.m_strResponse = "VCS";
	std::string l_strMessage = l_oCarsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnVcsAlarmRelatedPersonsRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnVcsAlarmRelatedPersonsRequest From VCS: [%s]", p_strMsg.c_str());

	PROTOCOL::CAlarmRelatedPersonsChangeSync l_oPersonsChangeSync;
	if (!l_oPersonsChangeSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	_UpdateCaseSyncTime(l_oPersonsChangeSync.m_oHeader.m_strMsgid);

	//return;
	//PROTOCOL::CAlarmRelatedCarsChangeSync l_oVcsAlarmRelatedCarsRequest;
	//转发给ICC
	l_oPersonsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oPersonsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oPersonsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_oPersonsChangeSync.m_oHeader.m_strRequestType = "0";
	l_oPersonsChangeSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oPersonsChangeSync.m_oHeader.m_strResponse = "VCS";
	std::string l_strMessage = l_oPersonsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnRecvAlarmRelatedCarsSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnRecvAlarmRelatedCarsSync: [%s]", p_strMsg.c_str());
	PROTOCOL::CAlarmRelatedCarsChangeSync l_oCarsChangeSync;
	if (!l_oCarsChangeSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	if (l_oCarsChangeSync.m_oHeader.m_strResponse == "VCS")  //VCS产生的
	{
		//_UpdateCaseSyncTime(l_oCarsChangeSync.m_oHeader.m_strMsgid);
		return;
	}

	if (l_oCarsChangeSync.m_oHeader.m_strRequest == "queue_alarm_to_icc")
	{
		//VCS过来的，转发给ICC
		std::string l_strCmd;
		if (l_oCarsChangeSync.m_oBody.m_strSyncType == "1" || l_oCarsChangeSync.m_oBody.m_strSyncType == "2")
		{
			std::string l_strAlarmRelatedCarsID = l_oCarsChangeSync.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH];
			if (_QueryAlarmRelatedCars(l_strAlarmRelatedCarsID))
			{
				l_strCmd = "update_jqsacl_request";
			}
			else
			{
				l_strCmd = "add_jqsacl_request";
			}
		}
		//else if (l_oCarsChangeSync.m_oBody.m_strSyncType == "2")
		//{
		//	l_strCmd = "update_jqsacl_request";
		//}
		else if (l_oCarsChangeSync.m_oBody.m_strSyncType == "3")
		{
			l_strCmd = "delete_jqsacl_request";
		}
		else
		{
			return;
		}
		l_oCarsChangeSync.m_oBody.m_strSyncType = "";
		l_oCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oCarsChangeSync.m_oHeader.m_strCmd = l_strCmd;
		l_oCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oCarsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM;
		l_oCarsChangeSync.m_oHeader.m_strRequestType = "0";
		l_oCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";
		l_oCarsChangeSync.m_oHeader.m_strResponse = "VCS";
		std::string l_strMessage = l_oCarsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		return;
	}

	/*
	*/
	//将同步消息转发给VCS,命令字不变
	l_oCarsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_oCarsChangeSync.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;
	l_oCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oCarsChangeSync.m_oHeader.m_strCmd = "topic_jqsacl_sync";
	l_oCarsChangeSync.m_oHeader.m_strResponse = "VCS";
	l_oCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oCarsChangeSync.m_oHeader.m_strRequestType = "0";

	std::string  l_strMsg = l_oCarsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnRecvAlarmRelatedPersonsSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnRecvAlarmRelatedPersonsSync: [%s]", p_strMsg.c_str());
	PROTOCOL::CAlarmRelatedPersonsChangeSync l_oPersonsChangeSync;
	if (!l_oPersonsChangeSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	if (l_oPersonsChangeSync.m_oHeader.m_strResponse == "VCS")  //VCS产生的
	{
		//_UpdateCaseSyncTime(l_oPersonsChangeSync.m_oHeader.m_strMsgid);
		return;
	}

	if (l_oPersonsChangeSync.m_oHeader.m_strRequest == "queue_alarm_to_icc")
	{
		std::string l_strCmd;
		if (l_oPersonsChangeSync.m_oBody.m_strSyncType == "1" || l_oPersonsChangeSync.m_oBody.m_strSyncType == "2")
		{
			l_strCmd = "add_jqdsr_request";
			std::string l_strAlarmRelatedPersonsID = l_oPersonsChangeSync.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo[PROTOCOL::Ex_Persons_JQDSRDBH];
			if (_QueryAlarmRelatedPersons(l_strAlarmRelatedPersonsID))
			{
				l_strCmd = "update_jqdsr_request";
			}
			else
			{
				l_strCmd = "add_jqdsr_request";
			}
		}
		//else if (l_oPersonsChangeSync.m_oBody.m_strSyncType == "2")
		//{
		//	l_strCmd = "update_jqdsr_request";
		//}
		else if (l_oPersonsChangeSync.m_oBody.m_strSyncType == "3")
		{
			l_strCmd = "delete_jqdsr_request";
		}
		else
		{
			return;
		}
		l_oPersonsChangeSync.m_oBody.m_strSyncType = "";
		l_oPersonsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oPersonsChangeSync.m_oHeader.m_strCmd = l_strCmd;
		l_oPersonsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oPersonsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM;
		l_oPersonsChangeSync.m_oHeader.m_strRequestType = "0";
		l_oPersonsChangeSync.m_oHeader.m_strRequestFlag = "MQ";
		l_oPersonsChangeSync.m_oHeader.m_strResponse = "VCS";
		std::string l_strMessage = l_oPersonsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		return;
	}
	/*
	
	*/
	//将同步消息转发给VCS,命令字不变
	l_oPersonsChangeSync.m_oHeader.m_strRequest = QUEUE_ALARM_TO_VCS/*m_queueToVCS*/;
	l_oPersonsChangeSync.m_oHeader.m_strRequestType = SEND_TYPE_QUEUE/*"0"*/;
	l_oPersonsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oPersonsChangeSync.m_oHeader.m_strCmd = "topic_jqdsr_sync";
	l_oPersonsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oPersonsChangeSync.m_oHeader.m_strRequestType = "0";

	std::string  l_strMsg = l_oPersonsChangeSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

bool CBusinessImpl::_QueryAlarmRelatedCars(const std::string &p_strID)
{
	if (p_strID.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqclb_by_id";
	l_tSQLReq.param["id"] = p_strID;
	l_tSQLReq.param["deleted"] = "0";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqclb_by_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		while (l_pRSet->Next())
		{
			ICC_LOG_DEBUG(m_pLog, "_QueryAlarmRelatedCars return true");
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::_QueryAlarmRelatedPersons(const std::string &p_strID)
{
	if (p_strID.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqdsrdb_by_id";
	l_tSQLReq.param["id"] = p_strID;
	l_tSQLReq.param["deleted"] = "false";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqdsrdb_by_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		while (l_pRSet->Next())
		{
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::_QueryProcess(const std::string& p_strCmd, const std::string& p_strValue, std::vector<Process>& p_vecProcessInfo)
{
	DataBase::SQLRequest l_oSeleteAlarmProcessSQLReq;
	if (p_strCmd == "update_time")
	{
		l_oSeleteAlarmProcessSQLReq.sql_id = "select_icc_t_pjdb_updatetime";
		l_oSeleteAlarmProcessSQLReq.param["update_time"] = p_strValue;
		l_oSeleteAlarmProcessSQLReq.param["orderby"] = "update_time";

		l_oSeleteAlarmProcessSQLReq.param["jjsj_end"] = p_strValue;
	}
	else if (p_strCmd == "alarm_id")
	{
		l_oSeleteAlarmProcessSQLReq.sql_id = "select_icc_t_pjdb";
		l_oSeleteAlarmProcessSQLReq.param["alarm_id"] = p_strValue;

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_strValue);
		if (strTime != "")
		{
			l_oSeleteAlarmProcessSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSeleteAlarmProcessSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	else if (p_strCmd == "id")
	{
		l_oSeleteAlarmProcessSQLReq.sql_id = "select_icc_t_pjdb";
		l_oSeleteAlarmProcessSQLReq.param["id"] = p_strValue;

		std::string strTime = m_pDateTime->GetDispatchIdTime(p_strValue);
		if (strTime != "")
		{
			l_oSeleteAlarmProcessSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
			l_oSeleteAlarmProcessSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmProcessSQLReq);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		Process l_oProcess;
		l_oProcess.m_ProcessData.m_strDispatchDeptDistrictCode = l_pResult->GetValue("dispatch_dept_district_code");
		l_oProcess.m_ProcessData.m_strID = l_pResult->GetValue("id");
		l_oProcess.m_ProcessData.m_strAlarmID = l_pResult->GetValue("alarm_id");
		l_oProcess.m_ProcessData.m_strDispatchDeptCode = l_pResult->GetValue("dispatch_dept_code");
		l_oProcess.m_ProcessData.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
		l_oProcess.m_ProcessData.m_strDispatchName = l_pResult->GetValue("dispatch_name");
		l_oProcess.m_ProcessData.m_strRecordID = l_pResult->GetValue("record_id");
		l_oProcess.m_ProcessData.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		l_oProcess.m_ProcessData.m_strProcessDeptCode = l_pResult->GetValue("process_dept_code");
		l_oProcess.m_ProcessData.m_strTimeSubmit = l_pResult->GetValue("time_submit");
		l_oProcess.m_ProcessData.m_strTimeArrived = l_pResult->GetValue("time_arrived");
		l_oProcess.m_ProcessData.m_strTimeSigned = l_pResult->GetValue("time_signed");
		l_oProcess.m_ProcessData.m_strProcessName = l_pResult->GetValue("process_name");
		l_oProcess.m_ProcessData.m_strProcessCode = l_pResult->GetValue("process_code");
		l_oProcess.m_ProcessData.m_strDispatchPersonnel = l_pResult->GetValue("dispatch_personnel");
		l_oProcess.m_ProcessData.m_strDispatchVehicles = l_pResult->GetValue("dispatch_vehicles");
		l_oProcess.m_ProcessData.m_strDispatchBoats = l_pResult->GetValue("dispatch_boats");
		l_oProcess.m_ProcessData.m_strState = l_pResult->GetValue("state");
		l_oProcess.m_ProcessData.m_strCreateTime = l_pResult->GetValue("create_time");
		l_oProcess.m_ProcessData.m_strUpdateTime = l_pResult->GetValue("update_time");
		l_oProcess.m_ProcessData.m_strDispatchDeptName = l_pResult->GetValue("dispatch_dept_name");
		l_oProcess.m_ProcessData.m_strDispatchDeptOrgCode = l_pResult->GetValue("dispatch_dept_org_code");
		l_oProcess.m_ProcessData.m_strProcessDeptName = l_pResult->GetValue("process_dept_name");
		l_oProcess.m_ProcessData.m_strProcessDeptOrgCode = l_pResult->GetValue("process_dept_org_code");
		l_oProcess.m_ProcessData.m_strProcessObjectType = l_pResult->GetValue("process_object_type");
		l_oProcess.m_ProcessData.m_strProcessObjectName = l_pResult->GetValue("process_object_name");
		l_oProcess.m_ProcessData.m_strProcessObjectCode = l_pResult->GetValue("process_object_code");
		l_oProcess.m_ProcessData.m_strBusinessStatus = l_pResult->GetValue("business_status");
		l_oProcess.m_ProcessData.m_strSeatCode = l_pResult->GetValue("seat_code");
		l_oProcess.m_ProcessData.m_strCancelTime = l_pResult->GetValue("cancel_time");
		l_oProcess.m_ProcessData.m_strCancelReason = l_pResult->GetValue("cancel_reason");
		l_oProcess.m_ProcessData.m_strIsAutoAssignJob = l_pResult->GetValue("is_auto_assign_job","0");
		l_oProcess.m_ProcessData.m_strCreateUser = l_pResult->GetValue("create_user");
		l_oProcess.m_ProcessData.m_strUpdateUser = l_pResult->GetValue("update_user");
		l_oProcess.m_ProcessData.m_strOvertimeState = l_pResult->GetValue("overtime_state");

		l_oProcess.m_ProcessData.m_strProcessObjectID = l_pResult->GetValue("process_object_id");
		l_oProcess.m_ProcessData.m_strTransfDeptOrjCode = l_pResult->GetValue("transfers_dept_org_code");
		l_oProcess.m_ProcessData.m_strIsOver = l_pResult->GetValue("is_over","0");

		l_oProcess.m_ProcessData.m_strOverRemark = l_pResult->GetValue("over_remark");
		l_oProcess.m_ProcessData.m_strParentID = l_pResult->GetValue("parent_id");
		l_oProcess.m_ProcessData.m_strGZLDM = l_pResult->GetValue("flow_code");

		l_oProcess.m_ProcessData.m_strCentreProcessDeptCode = l_pResult->GetValue("centre_process_dept_code");
		l_oProcess.m_ProcessData.m_strCentreAlarmDeptCode = l_pResult->GetValue("centre_alarm_dept_code");
		l_oProcess.m_ProcessData.m_strDispatchDeptDistrictName = l_pResult->GetValue("dispatch_dept_district_name");
		l_oProcess.m_ProcessData.m_strLinkedDispatchCode = l_pResult->GetValue("linked_dispatch_code");
		l_oProcess.m_ProcessData.m_strOverTime = l_pResult->GetValue("over_time");
		l_oProcess.m_ProcessData.m_strFinishedTimeoutState = l_pResult->GetValue("finished_timeout_state");
		l_oProcess.m_ProcessData.m_strPoliceType = l_pResult->GetValue("police_type");
		l_oProcess.m_ProcessData.m_strProcessDeptShortName = l_pResult->GetValue("process_dept_short_name");
		l_oProcess.m_ProcessData.m_strDispatchDeptShortName = l_pResult->GetValue("dispatch_dept_short_name");
		l_oProcess.m_ProcessData.m_strCreateTeminal = l_pResult->GetValue("createteminal");
		l_oProcess.m_ProcessData.m_strUpdateTeminal = l_pResult->GetValue("updateteminal");
		l_oProcess.m_ProcessData.m_strReceivedTime = l_pResult->GetValue("received_time");
		p_vecProcessInfo.push_back(l_oProcess);
	}
	if (p_strCmd == "alarm_id")
	{
		return true;
	}

	if (p_vecProcessInfo.size() != 0)
	{
		return true;
	}
	return false;
}

void CBusinessImpl::OnRecvVcsBllStatusSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnRecvVcsBllStatusSync From VCS: [%s]", p_strMsg.c_str());
	PROTOCOL::CVcsBillStatusSync  l_CCVcsBillStatusSync;
	if (!l_CCVcsBillStatusSync.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	PROTOCOL::CVcsBllStatusSynDataRespond::CData l_tmpBllStatusData;
	l_tmpBllStatusData.m_strID = l_CCVcsBillStatusSync.m_oBody.m_strId;
	l_tmpBllStatusData.m_strAlarmID = l_CCVcsBillStatusSync.m_oBody.m_strAlarmID;
	l_tmpBllStatusData.m_strProcessID = l_CCVcsBillStatusSync.m_oBody.m_strProcessID;
	l_tmpBllStatusData.m_strOwner = l_CCVcsBillStatusSync.m_oBody.m_strOwner;
	l_tmpBllStatusData.m_strOwnerType = l_CCVcsBillStatusSync.m_oBody.m_strOwnerType;
	l_tmpBllStatusData.m_strStatus = l_CCVcsBillStatusSync.m_oBody.m_strStatus;
	l_tmpBllStatusData.m_strCreateTime = l_CCVcsBillStatusSync.m_oBody.m_strCreateTime;
	l_tmpBllStatusData.m_strUpdateTime = l_CCVcsBillStatusSync.m_oBody.m_strUpdateTime;

	//enum ESyncType
		//{
		//	ADD = 1,
		//	SUBMIT,
		//	DEL,   //服务端不适用，同步状态禁止使用此状态  [1/29/2019 w16314]
		//	EDIT
		//};
	int p_iSyncType = -1;
	PROTOCOL::CVcsBllStatusSynDataRespond::CData l_Data;
	int l_nQueryRes = _QueryDBBllStatusInfo(l_CCVcsBillStatusSync.m_oBody.m_strId, l_Data);
	if (l_nQueryRes == -1)
	{
		//查询时数据库执行错误直接返回
		return;
	}

	if (l_nQueryRes == 1)
	{
		if (_UpdateDBBllStatusInfo(l_tmpBllStatusData))
		{
			p_iSyncType = 4;
		}
	}
	else
	{
		if (_InsertDBBllStatusInfo(l_tmpBllStatusData))
		{
			p_iSyncType = 1;
		}
	}

	if (p_iSyncType > 0 && !l_tmpBllStatusData.m_strProcessID.empty())
	{
		PROTOCOL::CAlarmProcessSync l_oAlarmProcessSync;
		l_oAlarmProcessSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oAlarmProcessSync.m_oHeader.m_strSubsystemID = "Alarm-Synthetical";
		l_oAlarmProcessSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oAlarmProcessSync.m_oHeader.m_strRelatedID = "";
		l_oAlarmProcessSync.m_oHeader.m_strCmd = "alarm_process_sync";
		l_oAlarmProcessSync.m_oHeader.m_strRequest = "topic_alarm_sync";
		l_oAlarmProcessSync.m_oHeader.m_strRequestType = "1";//主题
		l_oAlarmProcessSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAlarmProcessSync.m_oBody.m_strMsgSource = "vcs";

		//l_oAlarmProcessSync.m_oBody.m_strBllStatus = l_CCVcsBillStatusSync.m_oBody.m_strStatus;
		l_oAlarmProcessSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);
		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_pDisposalData;
		if (!QueryAlarmProcessByIDEx(l_tmpBllStatusData.m_strProcessID, l_oAlarmProcessSync.m_oBody.m_ProcessData))
		{
			ICC_LOG_ERROR(m_pLog, "query id fail id = [%s]", l_tmpBllStatusData.m_strProcessID.c_str());
		}

		//同步消息
		std::string l_strMsg(l_oAlarmProcessSync.ToString(ICCGetIJsonFactory()->CreateJson()));
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "not need process sync :[%s]", l_CCVcsBillStatusSync.m_oBody.m_strId.c_str());
	}
}


void CBusinessImpl::RequestVcsSyncFunc()
{
	m_threadSynData.reset(new boost::thread(boost::bind(&CBusinessImpl::_VcsSynData, this)));
}

void CBusinessImpl::_VcsSynData()
{
	if (m_bIsVcsSyning)
	{
		ICC_LOG_WARNING(m_pLog, "is syning!!!");
		return;
	}

	{
		boost::lock_guard<boost::mutex> lock(m_mutexSyning);
		if (m_bIsVcsSyning)
		{
			ICC_LOG_WARNING(m_pLog, "is syning!!!");
			return;
		}
		m_bIsVcsSyning = true;
	}

	ICC_LOG_DEBUG(m_pLog, "syn data begin!!!!!!!");


	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();

	int iRs = _ProcVcsSynData(m_strVcsSyncStartTime);
	if (iRs == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "syn data success!!!!!!!");
		m_bVcsSynSuccess = true;
		m_strVcsSyncStartTime = "";
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn data failed!!!!!!!");
		m_bVcsSynSuccess = false;
	}

	m_bIsVcsSyning = false;

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

	ICC_LOG_DEBUG(m_pLog, "syn data end!!!!!!! time[%lld]", ullEndTime - ullBeginTime);
}

void CBusinessImpl::OnRecvSyncVcsDataRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnRecvSyncVcsDataRequest: [%s]", p_strMsg.c_str());
	PROTOCOL::CVcsSyncDataRequset l_CVcsSyncDataRequset;
	if (!l_CVcsSyncDataRequset.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	int l_iVcsSyncUpdateFlag = m_nVcsSyncUpdateFlag;
	if (!l_CVcsSyncDataRequset.m_oBody.m_strSyncUpdateFlag.empty())
	{
		m_nVcsSyncUpdateFlag = std::stoi(l_CVcsSyncDataRequset.m_oBody.m_strSyncUpdateFlag);
	}

	int l_iPackageSize = m_iPackageSize;
	if (!l_CVcsSyncDataRequset.m_oBody.m_strPackageSize.empty())
	{
		m_iPackageSize = std::stoi(l_CVcsSyncDataRequset.m_oBody.m_strPackageSize);
	}
	
	
	_ProcVcsSynData(l_CVcsSyncDataRequset.m_oBody.m_strStartTime);

	m_nVcsSyncUpdateFlag = l_iVcsSyncUpdateFlag;
	m_iPackageSize = l_iPackageSize;

	PROTOCOL::CVcsSyncDataRespond l_CAdminDeptRespond;
	l_CAdminDeptRespond.m_oHeader.m_strMsgId = l_CVcsSyncDataRequset.m_oHeader.m_strMsgId;
	l_CAdminDeptRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMessage = l_CAdminDeptRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	if (p_pRequest)
	{
		ICC_LOG_DEBUG(m_pLog, "CVcsSyncDataRespond: [%s]", l_strRespondMessage.c_str());
		p_pRequest->Response(l_strRespondMessage);
	}
}

std::string CBusinessImpl::_GetCaseSyncTime(int nSyncType)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_vcs_sync_time";
	if (nSyncType == 1)
	{
		l_SqlRequest.param["sync_type"] = "1";
		//直接从redis中获取
		std::string l_strMQUpdateTime;
		if (m_pRedisClient->HGet("vcs_sync_updatetime", "MQ", l_strMQUpdateTime))
		{
			ICC_LOG_DEBUG(m_pLog, "_GetCaseSyncTime by redis: [%s]", l_strMQUpdateTime.c_str());
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "_GetCaseSyncTime by redis Failed");
		}
		return l_strMQUpdateTime;
	}
	else
	{
		l_SqlRequest.param["sync_type"] = "2";
	}
	

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	std::string l_strUpdateTime = "";
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
		l_strUpdateTime = "error";
	}
	else
	{
		while (l_pResult->Next())
		{
			l_strUpdateTime = l_pResult->GetValue("sync_time");
			break;
		}
	}
	return l_strUpdateTime;
}

void CBusinessImpl::_UpdateCaseSyncTimeByMQ(const std::string& p_strMsgID)
{
	std::string l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	if (m_pRedisClient->HSet("vcs_sync_updatetime", "MQ", l_strUpdateTime))
	{
		ICC_LOG_DEBUG(m_pLog, "vcs_sync_updatetime_MQ : [%s]", l_strUpdateTime.c_str());
	}
}

void CBusinessImpl::_UpdateCaseSyncTime(const std::string& p_strMsgID, int nSyncType)
{
	if (!m_bEnbleVcsSyncFlag)
	{
		return;
	}

	std::string p_strGUID = p_strMsgID;
	if (p_strMsgID.empty())
	{
		p_strGUID = m_pString->CreateGuid();
	}
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_vcs_sync_time";
	if (nSyncType == 1)  //MQ同步
	{
		//l_SqlRequest.param["sync_type"] = "1";
		_UpdateCaseSyncTimeByMQ(p_strMsgID);
		return;
	}
	else //数据补偿同步
	{
		l_SqlRequest.param["sync_type"] = "2";
	}
	

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	std::string l_strUpdateTime;

	bool bFindFlag = false;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		while (l_pResult->Next())
		{
			l_strUpdateTime = l_pResult->GetValue("sync_time");
			if (l_strUpdateTime.empty())
			{
				l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			}
			bFindFlag = true;
			break;
		}
	}

	if(!bFindFlag)
	{
		l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
		l_SqlRequest.sql_id = "insert_vcs_sync_time";
		l_SqlRequest.param["sync_time"] = l_strUpdateTime;
		l_SqlRequest.param["guid"] = p_strGUID;
	}
	else
	{
		l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
		l_SqlRequest.sql_id = "update_vcs_sync_time";
		l_SqlRequest.set["sync_time"] = l_strUpdateTime;
		//l_SqlRequest.set["guid"] = p_strGUID;
	}
	l_pResult = m_pDBConn->Exec(l_SqlRequest, false);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s],%s", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
	}
}


bool CBusinessImpl::_QueryVcsService()
{

	std::string strNacosIp;
	std::string strNacosPort;
	std::string strNameSpace;
	std::string strGroupName;

	_GetNacosParams(strNacosIp, strNacosPort, strNameSpace, strGroupName);

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), strNameSpace.c_str(),
		strGroupName.c_str(), m_strVcsServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->GetEx(strNacosIp, strNacosPort, strTarget, mapHeaders, strContent, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", strNacosIp.c_str(), strNacosPort.c_str(), strTarget.c_str());
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
			m_strVcsServerIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			m_strVcsPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy vcs service : [%s:%s]. ", m_strVcsServerIp.c_str(), m_strVcsPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy vcs service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());

	return false;
}

bool CBusinessImpl::_LoginVcsService(std::string& p_strToken)
{
	return true;
}

void CBusinessImpl::_LogoutVcs(std::string& p_strToken)
{
	return;
}

bool CBusinessImpl::_SetSqlInsertProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, DataBase::SQLRequest& p_tSQLInsert)
{
	p_tSQLInsert.sql_id = "insert_icc_t_pjdb";
	p_tSQLInsert.param["alarm_id"] = p_oProcessInfo.m_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_oProcessInfo.m_strAlarmID);
	if (strTime != "")
	{
		p_tSQLInsert.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		p_tSQLInsert.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	p_tSQLInsert.param["jjsj"] = p_oProcessInfo.m_strReceivedTime;
	p_tSQLInsert.param["state"] = p_oProcessInfo.m_strState;
	p_tSQLInsert.param["dispatch_dept_district_code"] = p_oProcessInfo.m_strDispatchDeptDistrictCode;
	p_tSQLInsert.param["id"] = p_oProcessInfo.m_strID;
	p_tSQLInsert.param["dispatch_dept_code"] = p_oProcessInfo.m_strDispatchDeptCode;
	p_tSQLInsert.param["dispatch_code"] = p_oProcessInfo.m_strDispatchCode;
	p_tSQLInsert.param["dispatch_name"] = p_oProcessInfo.m_strDispatchName;
	p_tSQLInsert.param["record_id"] = p_oProcessInfo.m_strRecordID;
	p_tSQLInsert.param["dispatch_suggestion"] = p_oProcessInfo.m_strDispatchSuggestion;
	p_tSQLInsert.param["process_dept_code"] = p_oProcessInfo.m_strProcessDeptCode;
	p_tSQLInsert.param["time_submit"] = p_oProcessInfo.m_strTimeSubmit;
	p_tSQLInsert.param["time_arrived"] = p_oProcessInfo.m_strTimeArrived;
	p_tSQLInsert.param["time_signed"] = p_oProcessInfo.m_strTimeSigned;
	p_tSQLInsert.param["process_name"] = p_oProcessInfo.m_strProcessName;
	p_tSQLInsert.param["process_code"] = p_oProcessInfo.m_strProcessCode;
	p_tSQLInsert.param["dispatch_personnel"] = p_oProcessInfo.m_strDispatchPersonnel;
	p_tSQLInsert.param["dispatch_vehicles"] = p_oProcessInfo.m_strDispatchVehicles;
	p_tSQLInsert.param["dispatch_boats"] = p_oProcessInfo.m_strDispatchBoats;
	p_tSQLInsert.param["dispatch_dept_name"] = p_oProcessInfo.m_strDispatchDeptName;
	p_tSQLInsert.param["dispatch_dept_org_code"] = p_oProcessInfo.m_strDispatchDeptOrgCode;
	p_tSQLInsert.param["process_dept_name"] = p_oProcessInfo.m_strProcessDeptName;
	p_tSQLInsert.param["process_dept_org_code"] = p_oProcessInfo.m_strProcessDeptOrgCode;
	p_tSQLInsert.param["process_object_type"] = p_oProcessInfo.m_strProcessObjectType;
	p_tSQLInsert.param["process_object_name"] = p_oProcessInfo.m_strProcessObjectName;
	p_tSQLInsert.param["process_object_code"] = p_oProcessInfo.m_strProcessObjectCode;
	p_tSQLInsert.param["business_status"] = p_oProcessInfo.m_strBusinessStatus;
	p_tSQLInsert.param["seat_code"] = p_oProcessInfo.m_strSeatCode;
	p_tSQLInsert.param["cancel_time"] = p_oProcessInfo.m_strCancelTime;
	p_tSQLInsert.param["cancel_reason"] = p_oProcessInfo.m_strCancelReason;
	p_tSQLInsert.param["is_auto_assign_job"] = p_oProcessInfo.m_strIsAutoAssignJob;
	if (p_oProcessInfo.m_strIsAutoAssignJob.empty())
	{
		p_tSQLInsert.param["is_auto_assign_job"] = "0";
	}
	p_tSQLInsert.param["overtime_state"] = p_oProcessInfo.m_strOvertimeState;

	p_tSQLInsert.param["create_user"] = p_oProcessInfo.m_strCreateUser;
	p_tSQLInsert.param["create_time"] = p_oProcessInfo.m_strCreateTime;
	p_tSQLInsert.param["update_user"] = p_oProcessInfo.m_strUpdateUser;
	p_tSQLInsert.param["update_time"] = p_oProcessInfo.m_strUpdateTime;

	p_tSQLInsert.param["process_object_id"] = p_oProcessInfo.m_strProcessObjectID;
	p_tSQLInsert.param["transfers_dept_org_code"] = p_oProcessInfo.m_strTransfDeptOrjCode;
	std::string l_strIsOver = "0";
	if (!p_oProcessInfo.m_strIsOver.empty())
	{
		l_strIsOver = p_oProcessInfo.m_strIsOver;
	}
	p_tSQLInsert.param["is_over"] = l_strIsOver;
	p_tSQLInsert.param["over_remark"] = p_oProcessInfo.m_strOverRemark;
	p_tSQLInsert.param["parent_id"] = p_oProcessInfo.m_strParentID;
	p_tSQLInsert.param["flow_code"] = p_oProcessInfo.m_strGZLDM;

	p_tSQLInsert.param["centre_process_dept_code"] = p_oProcessInfo.m_strCentreProcessDeptCode;
	p_tSQLInsert.param["centre_alarm_dept_code"] = p_oProcessInfo.m_strCentreAlarmDeptCode;
	p_tSQLInsert.param["dispatch_dept_district_name"] = p_oProcessInfo.m_strDispatchDeptDistrictName;
	p_tSQLInsert.param["linked_dispatch_code"] = p_oProcessInfo.m_strLinkedDispatchCode;
	p_tSQLInsert.param["over_time"] = p_oProcessInfo.m_strOverTime;
	p_tSQLInsert.param["finished_timeout_state"] = p_oProcessInfo.m_strFinishedTimeoutState;
	p_tSQLInsert.param["police_type"] = p_oProcessInfo.m_strPoliceType;
	p_tSQLInsert.param["process_dept_short_name"] = p_oProcessInfo.m_strProcessDeptShortName;
	p_tSQLInsert.param["dispatch_dept_short_name"] = p_oProcessInfo.m_strDispatchDeptShortName;
	p_tSQLInsert.param["createTeminal"] = p_oProcessInfo.m_strCreateTeminal;
	p_tSQLInsert.param["updateTeminal"] = p_oProcessInfo.m_strUpdateTeminal;
	return true;
}


bool CBusinessImpl::_InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	// VCS过来的使用VCS派警单的状
	DataBase::SQLRequest l_tSQLReqInsertProcess;
	if (_SetSqlInsertProcess(p_pProcessInfo,l_tSQLReqInsertProcess))
	{
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertProcess);
		ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return false;
		}
		return true;
	}
	return false;
}

bool CBusinessImpl::_UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_pjdb";
	l_tSQLRequest.param["id"] = p_pProcessInfo.m_strID;
	std::string strTime = m_pDateTime->GetDispatchIdTime(p_pProcessInfo.m_strID);
	if (strTime != "")
	{
		l_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLRequest.set["dispatch_dept_district_code"] = p_pProcessInfo.m_strDispatchDeptDistrictCode;
	l_tSQLRequest.set["alarm_id"] = p_pProcessInfo.m_strAlarmID;
	l_tSQLRequest.set["dispatch_dept_code"] = p_pProcessInfo.m_strDispatchDeptCode;
	l_tSQLRequest.set["dispatch_code"] = p_pProcessInfo.m_strDispatchCode;
	l_tSQLRequest.set["dispatch_name"] = p_pProcessInfo.m_strDispatchName;
	l_tSQLRequest.set["record_id"] = p_pProcessInfo.m_strRecordID;
	l_tSQLRequest.set["dispatch_suggestion"] = p_pProcessInfo.m_strDispatchSuggestion;
	l_tSQLRequest.set["process_dept_code"] = p_pProcessInfo.m_strProcessDeptCode;
	l_tSQLRequest.set["time_submit"] = p_pProcessInfo.m_strTimeSubmit;
	l_tSQLRequest.set["time_arrived"] = p_pProcessInfo.m_strTimeArrived;
	l_tSQLRequest.set["time_signed"] = p_pProcessInfo.m_strTimeSigned;
	l_tSQLRequest.set["process_name"] = p_pProcessInfo.m_strProcessName;
	l_tSQLRequest.set["process_code"] = p_pProcessInfo.m_strProcessCode;
	l_tSQLRequest.set["dispatch_personnel"] = p_pProcessInfo.m_strDispatchPersonnel;
	l_tSQLRequest.set["dispatch_vehicles"] = p_pProcessInfo.m_strDispatchVehicles;
	l_tSQLRequest.set["dispatch_boats"] = p_pProcessInfo.m_strDispatchBoats;
	l_tSQLRequest.set["state"] = p_pProcessInfo.m_strState;
	l_tSQLRequest.set["create_time"] = p_pProcessInfo.m_strCreateTime;
	l_tSQLRequest.set["update_time"] = p_pProcessInfo.m_strUpdateTime;
	l_tSQLRequest.set["dispatch_dept_name"] = p_pProcessInfo.m_strDispatchDeptName;
	l_tSQLRequest.set["dispatch_dept_org_code"] = p_pProcessInfo.m_strDispatchDeptOrgCode;
	l_tSQLRequest.set["process_dept_name"] = p_pProcessInfo.m_strProcessDeptName;
	l_tSQLRequest.set["process_dept_org_code"] = p_pProcessInfo.m_strProcessDeptOrgCode;
	l_tSQLRequest.set["process_object_type"] = p_pProcessInfo.m_strProcessObjectType;
	l_tSQLRequest.set["process_object_name"] = p_pProcessInfo.m_strProcessObjectName;
	l_tSQLRequest.set["process_object_code"] = p_pProcessInfo.m_strProcessObjectCode;
	l_tSQLRequest.set["business_status"] = p_pProcessInfo.m_strBusinessStatus;
	l_tSQLRequest.set["seat_code"] = p_pProcessInfo.m_strSeatCode;
	l_tSQLRequest.set["cancel_time"] = p_pProcessInfo.m_strCancelTime;
	l_tSQLRequest.set["cancel_reason"] = p_pProcessInfo.m_strCancelReason;
	if (!p_pProcessInfo.m_strIsAutoAssignJob.empty())
	{
		l_tSQLRequest.set["is_auto_assign_job"] = p_pProcessInfo.m_strIsAutoAssignJob;
	}

	l_tSQLRequest.set["create_user"] = p_pProcessInfo.m_strCreateUser;
	l_tSQLRequest.set["update_user"] = p_pProcessInfo.m_strUpdateUser;
	l_tSQLRequest.set["overtime_state"] = p_pProcessInfo.m_strOvertimeState;

	if (!p_pProcessInfo.m_strTransfDeptOrjCode.empty())
	{
		l_tSQLRequest.set["transfers_dept_org_code"] = p_pProcessInfo.m_strTransfDeptOrjCode;
	}
	if (!p_pProcessInfo.m_strProcessObjectID.empty())
	{
		l_tSQLRequest.set["process_object_id"] = p_pProcessInfo.m_strProcessObjectID;
	}
	if (!p_pProcessInfo.m_strIsOver.empty())
	{
		l_tSQLRequest.set["is_over"] = p_pProcessInfo.m_strIsOver;
	}
	if (!p_pProcessInfo.m_strOverRemark.empty())
	{
		l_tSQLRequest.set["over_remark"] = p_pProcessInfo.m_strOverRemark;
	}
	if (!p_pProcessInfo.m_strParentID.empty())
	{
		l_tSQLRequest.set["parent_id"] = p_pProcessInfo.m_strParentID;
	}
	if (!p_pProcessInfo.m_strGZLDM.empty())
	{
		l_tSQLRequest.set["flow_code"] = p_pProcessInfo.m_strGZLDM;
	}

	if (!p_pProcessInfo.m_strCentreProcessDeptCode.empty())
	{
		l_tSQLRequest.set["centre_process_dept_code"] = p_pProcessInfo.m_strCentreProcessDeptCode;
	}

	if (!p_pProcessInfo.m_strCentreAlarmDeptCode.empty())
	{
		l_tSQLRequest.set["centre_alarm_dept_code"] = p_pProcessInfo.m_strCentreAlarmDeptCode;
	}

	if (!p_pProcessInfo.m_strDispatchDeptDistrictName.empty())
	{
		l_tSQLRequest.set["dispatch_dept_district_name"] = p_pProcessInfo.m_strDispatchDeptDistrictName;
	}

	if (!p_pProcessInfo.m_strLinkedDispatchCode.empty())
	{
		l_tSQLRequest.set["linked_dispatch_code"] = p_pProcessInfo.m_strLinkedDispatchCode;
	}

	if (!p_pProcessInfo.m_strOverTime.empty())
	{
		l_tSQLRequest.set["over_time"] = p_pProcessInfo.m_strOverTime;
	}

	if (!p_pProcessInfo.m_strFinishedTimeoutState.empty())
	{
		l_tSQLRequest.set["finished_timeout_state"] = p_pProcessInfo.m_strFinishedTimeoutState;
	}

	if (!p_pProcessInfo.m_strPoliceType.empty())
	{
		l_tSQLRequest.set["police_type"] = p_pProcessInfo.m_strPoliceType;
	}

	if (!p_pProcessInfo.m_strProcessDeptShortName.empty())
	{
		l_tSQLRequest.set["process_dept_short_name"] = p_pProcessInfo.m_strProcessDeptShortName;
	}

	if (!p_pProcessInfo.m_strDispatchDeptShortName.empty())
	{
		l_tSQLRequest.set["dispatch_dept_short_name"] = p_pProcessInfo.m_strDispatchDeptShortName;
	}

	if (!p_pProcessInfo.m_strCreateTeminal.empty())
	{
		l_tSQLRequest.set["createTeminal"] = p_pProcessInfo.m_strCreateTeminal;
	}
	if (!p_pProcessInfo.m_strUpdateTeminal.empty())
	{
		l_tSQLRequest.set["updateTeminal"] = p_pProcessInfo.m_strUpdateTeminal;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}


bool CBusinessImpl::_QueryAlarmLogByID(const std::string& p_strLogID, std::string p_strAlarmId)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_log";
	l_SqlRequest.param["id"] = p_strLogID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmId);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false);
	std::string l_strTmpLogID;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		while (l_pResult->Next())
		{
			l_strTmpLogID = l_pResult->GetValue("id");
			break;
		}
	}

	if (l_strTmpLogID.empty())
	{
		return false;
	}
	return true;
}


int CBusinessImpl::_ProcVcsSynData(const std::string& p_strStartTime)
{
	ICC_LOG_DEBUG(m_pLog, "nacos server address: %s:%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	ICC_LOG_DEBUG(m_pLog, "vcp service info: servicename:%s, healthyflag:%s", m_strVcsServiceName.c_str(), m_strVcsServiceHealthyFlag.c_str());
	if (!_QueryVcsService())
	{
		return -1;
	}
	ICC_LOG_DEBUG(m_pLog, "will syn data, vcs service info [%s:%s],StartTime=%s", m_strVcsServerIp.c_str(), m_strVcsPort.c_str(), p_strStartTime.c_str());

	std::string l_strStartTime;
	if (p_strStartTime.empty())
	{
		//获取上次补偿的时间
		l_strStartTime = _GetCaseSyncTime(2);
		if (l_strStartTime == "error")
		{
			ICC_LOG_ERROR(m_pLog, "get case sync time failed!!!!");
			return -1;
		}
		if (l_strStartTime.empty())
		{
			//首次做补偿,通过配置获取补偿前多少天的数据
			l_strStartTime = m_pDateTime->ToString(m_pDateTime->AddDays(m_pDateTime->CurrentDateTime(), m_nFirstSyncDaysAgo*(-1))); //默认补偿前1天数据
		}
		else
		{
			//非首次补偿，获取最后一次MQ同步的时间作为补偿起始时间
			l_strStartTime = _GetCaseSyncTime(1); //上次MQ同步的时间
			if (l_strStartTime == "error")
			{
				ICC_LOG_ERROR(m_pLog, "get case sync time failed!!!!");
				return -1;
			}
		}
	}
	else
	{
		l_strStartTime = p_strStartTime;
	}

	std::string l_strToken;
	if (!_LoginVcsService(l_strToken))
	{
		ICC_LOG_ERROR(m_pLog, "login smp failed!!!!");
		return -1;
	}

	ICC_LOG_DEBUG(m_pLog, "_ProcVcsSynData enter,startTime=%s,m_iPackageSize=%d!!!!", l_strStartTime.c_str(), m_iPackageSize);

	//_ProcSynBllStatusData(l_strStartTime, l_strToken);
	_ProcSynLogData(l_strStartTime, l_strToken);
	_ProcSynProcessData(l_strStartTime, l_strToken);
	_ProcSynAlarmData(l_strStartTime, l_strToken);
	_ProcSynCallerInfoData(l_strStartTime, l_strToken);
	_LogoutVcs(l_strToken);

	if (p_strStartTime.empty())
	{
		_UpdateCaseSyncTime("",2);
	}
	return 0;
}

int CBusinessImpl::_QueryDBBllStatusInfo(const std::string &strID,PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data)
{
	if (strID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "Query ID is empty");
		return -1;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_bll_status";
	l_SqlRequest.param["id"] = strID;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	std::string l_strBillStatusID;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
		return -1;
	}
	else
	{
		while (l_pResult->Next())
		{
			l_strBillStatusID = l_pResult->GetValue("id");
			p_data.m_strID = l_strBillStatusID;
			p_data.m_strAlarmID = l_pResult->GetValue("alarm_id");
			p_data.m_strProcessID = l_pResult->GetValue("process_id");
			p_data.m_strOwner= l_pResult->GetValue("owner");
			p_data.m_strOwnerType = l_pResult->GetValue("owner_type");
			p_data.m_strStatus = l_pResult->GetValue("bll_status");
			p_data.m_strUpdateTime = l_pResult->GetValue("update_time");
			p_data.m_strCreateTime = l_pResult->GetValue("create_time");
			break;
		}
	}

	if (l_strBillStatusID.empty())
	{
		return 0;
	}
	return 1;
}

bool CBusinessImpl::_InsertDBBllStatusInfo(const PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "insert_bll_status";
	l_SqlRequest.param["id"] = p_data.m_strID;
	l_SqlRequest.param["alarm_id"] = p_data.m_strAlarmID;
	l_SqlRequest.param["process_id"] = p_data.m_strProcessID;
	l_SqlRequest.param["owner"] = p_data.m_strOwner;
	l_SqlRequest.param["owner_type"] = p_data.m_strOwnerType;
	l_SqlRequest.param["bll_status"] = p_data.m_strStatus;
	l_SqlRequest.param["update_time"] = p_data.m_strUpdateTime;
	l_SqlRequest.param["create_time"] = p_data.m_strCreateTime;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql[%s] success[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetSQL().c_str());
	}
	return true;
}

bool CBusinessImpl::_UpdateDBBllStatusInfo(const PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "update_bll_status";
	l_SqlRequest.param["id"] = p_data.m_strID;

	l_SqlRequest.set["alarm_id"] = p_data.m_strAlarmID;
	l_SqlRequest.set["process_id"] = p_data.m_strProcessID;
	l_SqlRequest.set["owner"] = p_data.m_strOwner;
	l_SqlRequest.set["owner_type"] = p_data.m_strOwnerType;
	l_SqlRequest.set["bll_status"] = p_data.m_strStatus;
	l_SqlRequest.set["update_time"] = p_data.m_strUpdateTime;
	l_SqlRequest.set["create_time"] = p_data.m_strCreateTime;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql[%s] success[%s]", l_SqlRequest.sql_id.c_str(), l_pResult->GetSQL().c_str());
	}
	return true;
}

//同步业务状态
int CBusinessImpl::_ProcSynBllStatusData(const std::string& p_strStartTime, const std::string& p_strToken)
{
	std::string strErrorMessage;
	std::string strReceive;
	std::map<std::string, std::string> tmp_mapHeaders;

	tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	tmp_mapHeaders.insert(std::make_pair("Authorization", p_strToken));
	std::string l_strLimit = m_pString->Format("%d", m_iPackageSize);
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	std::string tmp_strTarget("/incident/sync-icc-service/history/bllstatus");

	std::string l_strQueryStartTime = p_strStartTime;
	int l_nIndex = 0;
	do
	{
		JsonParser::IJsonPtr tmp_spJson = ICCGetIJsonFactory()->CreateJson();
		if (NULL == tmp_spJson.get())
		{
			return -1;
		}
		tmp_spJson->SetNodeValue("/startTime", l_strQueryStartTime);
		tmp_spJson->SetNodeValue("/limit", l_strLimit);
		std::string tmp_strContent(tmp_spJson->ToString());

		strReceive = m_pHttpClient->PostWithTimeout(m_strVcsServerIp, m_strVcsPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage, m_nTimeOut);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!! m_strVcsServerIp=%s,err[%s]", m_strVcsServerIp.c_str(), strErrorMessage.c_str());
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Recv VcsSynBllstatus HttpMsg:[%s]", strReceive.c_str());
		}

		PROTOCOL::CVcsBllStatusSynDataRespond synDatas;
		if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", tmp_strTarget.c_str());
			return -1;
		}
		else
		{
			l_nIndex++;
		}

		if (synDatas.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
			break;
		}
		std::string l_strRecordEndTime;
		for (int i = 0; i < synDatas.m_oBody.m_vecData.size(); i++)
		{
			l_strRecordEndTime = synDatas.m_oBody.m_vecData[i].m_strUpdateTime;

			std::string l_strBllStatusID = synDatas.m_oBody.m_vecData[i].m_strID;
			PROTOCOL::CVcsBllStatusSynDataRespond::CData l_Data;
			if (!_QueryDBBllStatusInfo(l_strBllStatusID, l_Data))
			{
				//数据库没有对应的记录,数据直接写库,
				_InsertDBBllStatusInfo(synDatas.m_oBody.m_vecData[i]);
				ICC_LOG_DEBUG(m_pLog, "VCS Sync bllstatus,numId=%d,ID=%s,AlarmID=%s,createTime=%s", i, l_strBllStatusID.c_str(), synDatas.m_oBody.m_vecData[i].m_strAlarmID.c_str(), synDatas.m_oBody.m_vecData[i].m_strCreateTime.c_str());
			}
			else
			{
				//数据库有记录，比较更新时间 ICC_LOG_LOWDEBUG
				DateTime::CDateTime l_vcsPJUpdateTime = m_pDateTime->FromString(l_Data.m_strUpdateTime);
				DateTime::CDateTime l_iccPJUpdateTime = m_pDateTime->FromString(synDatas.m_oBody.m_vecData[i].m_strUpdateTime);
				bool bSyncUpdateFlag = false;
				if (l_vcsPJUpdateTime > l_iccPJUpdateTime)
				{
					bSyncUpdateFlag = true;
					_UpdateDBBllStatusInfo(synDatas.m_oBody.m_vecData[i]);
					ICC_LOG_DEBUG(m_pLog, "%d_VCS Sync bllstatus,numId=%d,ID=%s,AlarmID=%s,vcs_updateTime=%s,icc_updateTime=%s", bSyncUpdateFlag, i, l_strBllStatusID.c_str(), l_Data.m_strAlarmID.c_str(), synDatas.m_oBody.m_vecData[i].m_strUpdateTime.c_str(), l_Data.m_strUpdateTime.c_str());
				}
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Query bllstatus VCS Sync,index=%d,dataSize=%d,PackageSize=%d,QueryStartTime=%s,endTime=%s", l_nIndex, synDatas.m_oBody.m_vecData.size(), m_iPackageSize, l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
		if (synDatas.m_oBody.m_vecData.size() < m_iPackageSize)
		{
			break;
		}
		else
		{
			DateTime::CDateTime l_start = m_pDateTime->FromString(l_strQueryStartTime);
			DateTime::CDateTime l_end = m_pDateTime->FromString(l_strRecordEndTime);
			if (l_end < l_start)
			{
				ICC_LOG_ERROR(m_pLog, "query sync Time error,StartTime=%s,RecordEndTime=%s", l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
				break;
			}
			l_strQueryStartTime = l_strRecordEndTime;
		}

	} while (1);

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();
	ICC_LOG_DEBUG(m_pLog, "%s vcs bllstatus sync complete, time[%d]", tmp_strTarget.c_str(), ullEndTime - ullBeginTime);

	return 0;
}

//同步流水
int CBusinessImpl::_ProcSynLogData(const std::string& p_strStartTime, const std::string& p_strToken)
{
	std::string strErrorMessage;
	std::string strReceive;
	std::map<std::string, std::string> tmp_mapHeaders;

	tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	tmp_mapHeaders.insert(std::make_pair("Authorization", p_strToken));
	std::string l_strLimit = m_pString->Format("%d", m_iPackageSize);
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	std::string tmp_strTarget("/incident/sync-icc-service/history/sjz");

	std::string l_strQueryStartTime = p_strStartTime;
	int l_nIndex = 0;
	do
	{
		JsonParser::IJsonPtr tmp_spJson = ICCGetIJsonFactory()->CreateJson();
		if (NULL == tmp_spJson.get())
		{
			return -1;
		}
		tmp_spJson->SetNodeValue("/startTime", l_strQueryStartTime);
		tmp_spJson->SetNodeValue("/limit", l_strLimit);
		std::string tmp_strContent(tmp_spJson->ToString());

		strReceive = m_pHttpClient->PostWithTimeout(m_strVcsServerIp, m_strVcsPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage, m_nTimeOut);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!!err[%s]", strErrorMessage.c_str());
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Recv HttpMsg:[%s]", strReceive.c_str());
		}

		PROTOCOL::CVcsLogSynDataRespond synDatas;
		if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", tmp_strTarget.c_str());
			return -1;
		}
		else
		{
			l_nIndex++;
		}

		if (synDatas.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
			break;
		}
		std::string l_strRecordEndTime;
		for (int i = 0; i < synDatas.m_vecData.size(); i++)
		{
			l_strRecordEndTime = synDatas.m_vecData[i].m_strCreateTime;
			std::string l_strLogID = synDatas.m_vecData[i].m_strID;
			Process l_oProcess;
			int nSyncFlag = 0;
			if (!_QueryAlarmLogByID(l_strLogID, synDatas.m_vecData[i].m_strAlarmID))
			{
				nSyncFlag = -1;
				//数据库没有对应的接警记录,log数据直接写库,
				Alarm l_tAlarmInfo;
				if (!QueryAlarm(synDatas.m_vecData[i].m_strAlarmID, l_tAlarmInfo))
				{
					nSyncFlag = 1;
					InsertDBAlarmLogInfo(synDatas.m_vecData[i]);
					ICC_LOG_DEBUG(m_pLog, "%d_VCS Sync log_%d,ID=%s,AlarmID=%s,createTime=%s", nSyncFlag, i, l_strLogID.c_str(), synDatas.m_vecData[i].m_strAlarmID.c_str(), synDatas.m_vecData[i].m_strCreateTime.c_str());
				}
				else
				{
					if (m_nVcsSyncUpdateFlag > 0)
					{
						nSyncFlag = 0;
						InsertDBAlarmLogInfo(synDatas.m_vecData[i]);
						ICC_LOG_DEBUG(m_pLog, "%d_VCS Sync log_%d,ID=%s,AlarmID=%s,createTime=%s", nSyncFlag, i, l_strLogID.c_str(), synDatas.m_vecData[i].m_strAlarmID.c_str(), synDatas.m_vecData[i].m_strCreateTime.c_str());
					}
				}
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Query log VCS Sync,index=%d,dataSize=%d,PackageSize=%d,QueryStartTime=%s,endTime=%s",l_nIndex, synDatas.m_vecData.size(), m_iPackageSize, l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
		if (synDatas.m_vecData.size() < m_iPackageSize)
		{
			break;
		}
		else
		{
			DateTime::CDateTime l_start = m_pDateTime->FromString(l_strQueryStartTime);
			DateTime::CDateTime l_end = m_pDateTime->FromString(l_strRecordEndTime);
			if (l_end < l_start)
			{
				ICC_LOG_ERROR(m_pLog, "query sync Time error,StartTime=%s,RecordEndTime=%s", p_strStartTime.c_str(), l_strRecordEndTime.c_str());
				break;
			}
			l_strQueryStartTime = l_strRecordEndTime;
		}

	} while (1);

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();
	ICC_LOG_DEBUG(m_pLog, "%s vcs log sync complete, time[%d]", tmp_strTarget.c_str(), ullEndTime - ullBeginTime);

	return 0;
}

//同步派警单数据
int CBusinessImpl::_ProcSynProcessData(const std::string & p_strStartTime,const std::string &p_strToken)
{
	std::string strErrorMessage;
	std::string strReceive;
	std::map<std::string, std::string> tmp_mapHeaders;

	tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	tmp_mapHeaders.insert(std::make_pair("Authorization", p_strToken));
	std::string l_strLimit = m_pString->Format("%d", m_iPackageSize);
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	std::string tmp_strTarget("/incident/sync-icc-service/history/pjdb");

	std::string l_strQueryStartTime = p_strStartTime;
	int l_nIndex = 0;
	do
	{
		JsonParser::IJsonPtr tmp_spJson = ICCGetIJsonFactory()->CreateJson();
		if (NULL == tmp_spJson.get())
		{
			return -1;
		}
		tmp_spJson->SetNodeValue("/startTime", l_strQueryStartTime);
		tmp_spJson->SetNodeValue("/limit", l_strLimit);
		std::string tmp_strContent(tmp_spJson->ToString());

		strReceive = m_pHttpClient->PostWithTimeout(m_strVcsServerIp, m_strVcsPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage, m_nTimeOut);;
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!! m_strVcsServerIp=%s,err[%s]", m_strVcsServerIp.c_str(), strErrorMessage.c_str());
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Recv VcsSynProcess HttpMsg:[%s]", strReceive.c_str());
		}

		PROTOCOL::CVcsProcessSynDataRespond synDatas;
		if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", tmp_strTarget.c_str());
			return -1;
		}
		else
		{
			l_nIndex++;
		}

		if (synDatas.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
			break;
		}
		std::string l_strRecordEndTime;
		for (int i = 0; i < synDatas.m_vecData.size(); i++)
		{
			l_strRecordEndTime = synDatas.m_vecData[i].m_strUpdateTime;

			std::string l_strProcessID = synDatas.m_vecData[i].m_strID;
			Process l_oProcess;
			if (!QueryAlarmProcessByID(l_strProcessID, l_oProcess))
			{
				if (synDatas.m_vecData[i].m_strProcessObjectType == "Staff")
				{
					//调派给个人的不需要同步到ICC
					continue;
				}
				//数据库没有对应的记录,派警单数据直接写库,
				_InsertDBProcessInfo(synDatas.m_vecData[i]);
				ICC_LOG_DEBUG(m_pLog, "VCS Sync process_%d,add,ID=%s,AlarmID=%s,createTime=%s",i, l_strProcessID.c_str(), synDatas.m_vecData[i].m_strAlarmID.c_str(), synDatas.m_vecData[i].m_strCreateTime.c_str());
			}
			else
			{
				int nUpdateFlag = 0;
				DateTime::CDateTime l_vcsPJUpdateTime = m_pDateTime->FromString(synDatas.m_vecData[i].m_strUpdateTime);
				DateTime::CDateTime l_iccPJUpdateTime = m_pDateTime->FromString(l_oProcess.m_ProcessData.m_strUpdateTime);
				uint64 l_uInterval = 0;
				if (m_nVcsSyncUpdateFlag > 0)
				{
					nUpdateFlag = -2;
					if (l_vcsPJUpdateTime > l_iccPJUpdateTime && m_pDateTime->SecondsDifference(l_vcsPJUpdateTime, l_iccPJUpdateTime) > 1)
					{
						
						nUpdateFlag = 1;
						_UpdateDBProcessInfo(synDatas.m_vecData[i]);

						l_uInterval = l_vcsPJUpdateTime - l_iccPJUpdateTime;
					}
				}
				else
				{
					if (l_vcsPJUpdateTime > l_iccPJUpdateTime)
					{
						nUpdateFlag = -1;
						l_uInterval = l_vcsPJUpdateTime - l_iccPJUpdateTime;
					}
				}
				
				
				//数据库有记录，比较更新时间 ICC_LOG_LOWDEBUG
				ICC_LOG_DEBUG(m_pLog, "%d_VCS Sync process_%d,update,ID=%s,AlarmID=%s,vcs_updateTime=%s,icc_updateTime=%s,Interval = %llu", nUpdateFlag, i, l_strProcessID.c_str(),
					synDatas.m_vecData[i].m_strAlarmID.c_str(), synDatas.m_vecData[i].m_strUpdateTime.c_str(), l_oProcess.m_ProcessData.m_strUpdateTime.c_str(), l_uInterval);
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Query process VCS Sync,index=%d,dataSize=%d,PackageSize=%d,QueryStartTime=%s,endTime=%s", l_nIndex,synDatas.m_vecData.size(), m_iPackageSize, l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
		if (synDatas.m_vecData.size() < m_iPackageSize)
		{
			break;
		}
		else
		{
			DateTime::CDateTime l_start = m_pDateTime->FromString(l_strQueryStartTime);
			DateTime::CDateTime l_end = m_pDateTime->FromString(l_strRecordEndTime);
			if (l_end < l_start)
			{
				ICC_LOG_ERROR(m_pLog, "query sync Time error,StartTime=%s,RecordEndTime=%s", l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
				break;
			}
			l_strQueryStartTime = l_strRecordEndTime;
		}

	} while (1);

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();
	ICC_LOG_DEBUG(m_pLog, "%s vcs process sync complete, time[%d]", tmp_strTarget.c_str(), ullEndTime - ullBeginTime);

	return 0;
}

int CBusinessImpl::InsertDBCallerInfo(PROTOCOL::CVcsAlarmSynDataSar::CData synCallerData)
{
	DataBase::SQLRequest l_tSQLReqInsertCaller;
	l_tSQLReqInsertCaller.sql_id = "insert_icc_t_involved_party";
	l_tSQLReqInsertCaller.param["id"] = synCallerData.m_str_id;
	l_tSQLReqInsertCaller.param["alarm_id"] = synCallerData.m_str_alarm_id;
	l_tSQLReqInsertCaller.param["process_id"] = synCallerData.m_str_process_id;
	l_tSQLReqInsertCaller.param["feedback_id"] = synCallerData.m_str_feedback_id;
	l_tSQLReqInsertCaller.param["centre_process_dept_code"] = synCallerData.m_str_centre_process_dept_code;
	l_tSQLReqInsertCaller.param["centre_alarm_dept_code"] = synCallerData.m_str_centre_alarm_dept_code;
	l_tSQLReqInsertCaller.param["centre_process_id"] = synCallerData.m_str_centre_process_id;
	l_tSQLReqInsertCaller.param["centre_feedback_id"] = synCallerData.m_str_centre_feedback_id;
	l_tSQLReqInsertCaller.param["district"] = synCallerData.m_str_district;
	l_tSQLReqInsertCaller.param["district_name"] = synCallerData.m_str_district_name;
	l_tSQLReqInsertCaller.param["name"] = synCallerData.m_str_name;
	l_tSQLReqInsertCaller.param["sex"] = synCallerData.m_str_sex;
	l_tSQLReqInsertCaller.param["identification_type"] = synCallerData.m_str_identification_type;
	l_tSQLReqInsertCaller.param["identification_id"] = synCallerData.m_str_identification_id;
	l_tSQLReqInsertCaller.param["identity"] = synCallerData.m_str_identity;
	l_tSQLReqInsertCaller.param["is_focus_pesron"] = synCallerData.m_str_is_focus_pesron;
	l_tSQLReqInsertCaller.param["focus_pesron_info"] = synCallerData.m_str_focus_pesron_info;
	l_tSQLReqInsertCaller.param["registered_address_division"] = synCallerData.m_str_registered_address_division;
	l_tSQLReqInsertCaller.param["registered_address"] = synCallerData.m_str_registered_address;
	l_tSQLReqInsertCaller.param["current_address_division"] = synCallerData.m_str_current_address_division;
	l_tSQLReqInsertCaller.param["current_address"] = synCallerData.m_str_current_address;
	l_tSQLReqInsertCaller.param["work_unit"] = synCallerData.m_str_work_unit;
	l_tSQLReqInsertCaller.param["occupation"] = synCallerData.m_str_occupation;
	l_tSQLReqInsertCaller.param["contact_number"] = synCallerData.m_str_contact_number;
	l_tSQLReqInsertCaller.param["other_certificates"] = synCallerData.m_str_other_certificates;
	l_tSQLReqInsertCaller.param["lost_item_information"] = synCallerData.m_str_lost_item_information;
	l_tSQLReqInsertCaller.param["create_time"] = synCallerData.m_str_create_time;
	l_tSQLReqInsertCaller.param["update_time"] = synCallerData.m_str_update_time;
	l_tSQLReqInsertCaller.param["date_of_birth"] = synCallerData.m_str_date_of_birth;
	l_tSQLReqInsertCaller.param["digital_signature"] = synCallerData.m_str_digital_signature;
	l_tSQLReqInsertCaller.param["is_delete"] = synCallerData.m_str_is_delete;
	l_tSQLReqInsertCaller.param["createTeminal"] = synCallerData.m_str_createTeminal;
	l_tSQLReqInsertCaller.param["updateTeminal"] = synCallerData.m_str_updateTeminal;
	l_tSQLReqInsertCaller.param["nationality"] = synCallerData.m_str_nationality;
	l_tSQLReqInsertCaller.param["nation"] = synCallerData.m_str_nation;
	l_tSQLReqInsertCaller.param["educationLevel"] = synCallerData.m_str_educationLevel;
	l_tSQLReqInsertCaller.param["marriageStatus"] = synCallerData.m_str_marriageStatus;
	l_tSQLReqInsertCaller.param["politicalOutlook"] = synCallerData.m_str_politicalOutlook;
	l_tSQLReqInsertCaller.param["is_NPC_deputy"] = synCallerData.m_str_is_NPC_deputy;
	l_tSQLReqInsertCaller.param["is_national_staff"] = synCallerData.m_str_is_national_staff;
	l_tSQLReqInsertCaller.param["createUserId"] = synCallerData.m_str_createUserId;
	l_tSQLReqInsertCaller.param["createUserName"] = synCallerData.m_str_createUserName;
	l_tSQLReqInsertCaller.param["createUserOrgCode"] = synCallerData.m_str_createUserOrgCode;
	l_tSQLReqInsertCaller.param["createUserOrgName"] = synCallerData.m_str_createUserOrgName;
	l_tSQLReqInsertCaller.param["updateUserId"] = synCallerData.m_str_updateUserId;
	l_tSQLReqInsertCaller.param["updateUserName"] = synCallerData.m_str_updateUserName;
	l_tSQLReqInsertCaller.param["updateUserOrgCode"] = synCallerData.m_str_updateUserOrgCode;
	l_tSQLReqInsertCaller.param["updateUserOrgName"] = synCallerData.m_str_updateUserOrgName;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertCaller);
	// ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "RecvCallerData insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "RecvCallerData,%s Exec sql success! sql:[%s]", l_tSQLReqInsertCaller.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}
	return true;
}

// 同步报警人背景信息数据
int CBusinessImpl::_ProcSynCallerInfoData(const std::string& p_strStartTime, const std::string& p_strToken)
{
	std::string strErrorMessage;
	std::string strReceive;
	std::map<std::string, std::string> tmp_mapHeaders;

	tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	tmp_mapHeaders.insert(std::make_pair("Authorization", p_strToken));

	std::string l_strLimit = m_pString->Format("%d", m_iPackageSize);
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	//std::string tmp_strTarget("/incident/sync-icc-service/history/jjdb");
	std::string tmp_strTarget("/incident/sync-icc-service/history/sar");
	std::string l_strQueryStartTime = p_strStartTime;
	ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!! [%s-%s]", tmp_strTarget.c_str(), l_strQueryStartTime.c_str());
	int l_nIndex = 0;
	do
	{
		JsonParser::IJsonPtr tmp_spJson = ICCGetIJsonFactory()->CreateJson();
		if (NULL == tmp_spJson.get())
		{
			return -1;
		}
		tmp_spJson->SetNodeValue("/startTime", l_strQueryStartTime);
		tmp_spJson->SetNodeValue("/limit", l_strLimit);
		std::string tmp_strContent(tmp_spJson->ToString());

		strReceive = m_pHttpClient->PostWithTimeout(m_strVcsServerIp, m_strVcsPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage, m_nTimeOut);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!!err[%s]", strErrorMessage.c_str());
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Recv HttpMsg:[%s]", strReceive.c_str());
		}
		
		PROTOCOL::CVcsAlarmSynDataSar synDatas;
		if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", tmp_strTarget.c_str());
			return -1;
		}
		else
		{
			l_nIndex++;
		}

		if (synDatas.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
			break;
		}
		/*std::string strmsg = synDatas.ToString(m_pJsonFty->CreateJson());
		ICC_LOG_ERROR(m_pLog, "parse %s  size %d!!!!", strmsg.c_str(), synDatas.m_vecData.size());*/
		std::string l_strRecordEndTime;
		int size = synDatas.m_vecData.size();
		int nSyncFlag = 0;
		for (int i = 0; i < size; i++)
		{
			l_strRecordEndTime = synDatas.m_vecData[i].m_str_create_time;
			nSyncFlag = -1;
			std::string l_strCallerID = synDatas.m_vecData[i].m_str_id;
			// 查询 此ID 是否已经存在
			if (!QueryCallerData(l_strCallerID))
			{
				nSyncFlag = 1;
				ICC_LOG_DEBUG(m_pLog, "VCS Sync alarm_%d,m_strCaller ID=%s,CreateTime=%s,", i, l_strCallerID.c_str(), synDatas.m_vecData[i].m_str_create_time.c_str());
				InsertDBCallerInfo(synDatas.m_vecData[i]);
			}
			else
			{
				// 要不要更新呢？
				UpdateCallerDataInfo(synDatas.m_vecData[i]);
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Query log VCS Sync,index=%d,dataSize=%d,PackageSize=%d,QueryStartTime=%s,endTime=%s", l_nIndex, synDatas.m_vecData.size(), m_iPackageSize, l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
		if (synDatas.m_vecData.size() < m_iPackageSize)
		{
			break;
		}
		else
		{
			DateTime::CDateTime l_start = m_pDateTime->FromString(l_strQueryStartTime);
			DateTime::CDateTime l_end = m_pDateTime->FromString(l_strRecordEndTime);
			if (l_end < l_start)
			{
				ICC_LOG_ERROR(m_pLog, "query sync Time error,StartTime=%s,RecordEndTime=%s", p_strStartTime.c_str(), l_strRecordEndTime.c_str());
				break;
			}
			l_strQueryStartTime = l_strRecordEndTime;
		}
	} while (1);

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

	ICC_LOG_DEBUG(m_pLog, "%s vcs alarm sync complete, time[%d]", tmp_strTarget.c_str(), ullEndTime - ullBeginTime);

	return 0;
}

int CBusinessImpl::QueryCallerData(const std::string& l_strCallerID)
{
	if (l_strCallerID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "AlarmID is Null");
		return false;
	}

	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_involved_party";
	l_oSeleteAlarmSQLReq.param["id"] = l_strCallerID;
	
	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		return true;
	}
	return false;
}


// 更新涉案人员数据
bool CBusinessImpl::UpdateCallerDataInfo(PROTOCOL::CVcsAlarmSynDataSar::CData synCallerData)
{
	DataBase::SQLRequest l_tSQLReqInsertCaller;
	l_tSQLReqInsertCaller.sql_id = "update_icc_t_involved_party";
	l_tSQLReqInsertCaller.param["id"] = synCallerData.m_str_id;
	if (!synCallerData.m_str_alarm_id.empty())
	{
		l_tSQLReqInsertCaller.set["alarm_id"] = synCallerData.m_str_alarm_id;
	}
	if (!synCallerData.m_str_process_id.empty())
	{
		l_tSQLReqInsertCaller.set["process_id"] = synCallerData.m_str_process_id;
	}
	if (!synCallerData.m_str_feedback_id.empty())
	{
		l_tSQLReqInsertCaller.set["feedback_id"] = synCallerData.m_str_feedback_id;
	}
	if (!synCallerData.m_str_centre_process_dept_code.empty())
	{
		l_tSQLReqInsertCaller.set["centre_process_dept_code"] = synCallerData.m_str_centre_process_dept_code;
	}
	if (!synCallerData.m_str_centre_alarm_dept_code.empty())
	{
		l_tSQLReqInsertCaller.set["centre_alarm_dept_code"] = synCallerData.m_str_centre_alarm_dept_code;
	}
	if (!synCallerData.m_str_centre_process_id.empty())
	{
		l_tSQLReqInsertCaller.set["centre_process_id"] = synCallerData.m_str_centre_process_id;
	}
	if (!synCallerData.m_str_centre_feedback_id.empty())
	{
		l_tSQLReqInsertCaller.set["centre_feedback_id"] = synCallerData.m_str_centre_feedback_id;
	}
	if (!synCallerData.m_str_district.empty())
	{
		l_tSQLReqInsertCaller.set["district"] = synCallerData.m_str_district;
	}
	if (!synCallerData.m_str_district_name.empty())
	{
		l_tSQLReqInsertCaller.set["district_name"] = synCallerData.m_str_district_name;
	}
	if (!synCallerData.m_str_name.empty())
	{
		l_tSQLReqInsertCaller.set["name"] = synCallerData.m_str_name;
	}
	if (!synCallerData.m_str_sex.empty())
	{
		l_tSQLReqInsertCaller.set["sex"] = synCallerData.m_str_sex;
	}
	if (!synCallerData.m_str_identification_type.empty())
	{
		l_tSQLReqInsertCaller.set["identification_type"] = synCallerData.m_str_identification_type;
	}
	if (!synCallerData.m_str_identification_id.empty())
	{
		l_tSQLReqInsertCaller.set["identification_id"] = synCallerData.m_str_identification_id;
	}
	if (!synCallerData.m_str_identity.empty())
	{
		l_tSQLReqInsertCaller.set["identity"] = synCallerData.m_str_identity;
	}
	if (!synCallerData.m_str_is_focus_pesron.empty())
	{
		l_tSQLReqInsertCaller.set["is_focus_pesron"] = synCallerData.m_str_is_focus_pesron;
	}
	if (!synCallerData.m_str_focus_pesron_info.empty())
	{
		l_tSQLReqInsertCaller.set["focus_pesron_info"] = synCallerData.m_str_focus_pesron_info;
	}
	if (!synCallerData.m_str_registered_address_division.empty())
	{
		l_tSQLReqInsertCaller.set["registered_address_division"] = synCallerData.m_str_registered_address_division;
	}
	if (!synCallerData.m_str_registered_address.empty())
	{
		l_tSQLReqInsertCaller.set["registered_address"] = synCallerData.m_str_registered_address;
	}
	if (!synCallerData.m_str_current_address_division.empty())
	{
		l_tSQLReqInsertCaller.set["current_address_division"] = synCallerData.m_str_current_address_division;
	}
	if (!synCallerData.m_str_current_address.empty())
	{
		l_tSQLReqInsertCaller.set["current_address"] = synCallerData.m_str_current_address;
	}
	if (!synCallerData.m_str_work_unit.empty())
	{
		l_tSQLReqInsertCaller.set["work_unit"] = synCallerData.m_str_work_unit;
	}
	if (!synCallerData.m_str_occupation.empty())
	{
		l_tSQLReqInsertCaller.set["occupation"] = synCallerData.m_str_occupation;
	}
	if (!synCallerData.m_str_contact_number.empty())
	{
		l_tSQLReqInsertCaller.set["contact_number"] = synCallerData.m_str_contact_number;
	}
	if (!synCallerData.m_str_other_certificates.empty())
	{
		l_tSQLReqInsertCaller.set["other_certificates"] = synCallerData.m_str_other_certificates;
	}
	if (!synCallerData.m_str_lost_item_information.empty())
	{
		l_tSQLReqInsertCaller.set["lost_item_information"] = synCallerData.m_str_lost_item_information;
	}
	if (!synCallerData.m_str_create_time.empty())
	{
		l_tSQLReqInsertCaller.set["create_time"] = synCallerData.m_str_create_time;
	}
	else {
		l_tSQLReqInsertCaller.set["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	if (!synCallerData.m_str_update_time.empty())
	{
		l_tSQLReqInsertCaller.set["update_time"] = synCallerData.m_str_update_time;
	}
	if (!synCallerData.m_str_date_of_birth.empty())
	{
		l_tSQLReqInsertCaller.set["date_of_birth"] = synCallerData.m_str_date_of_birth;
	}
	if (!synCallerData.m_str_digital_signature.empty())
	{
		l_tSQLReqInsertCaller.set["digital_signature"] = synCallerData.m_str_digital_signature;
	}
	if (!synCallerData.m_str_is_delete.empty())
	{
		l_tSQLReqInsertCaller.set["is_delete"] = synCallerData.m_str_is_delete;
	}
	else {
		l_tSQLReqInsertCaller.set["is_delete"] = "0";
	}
	if (!synCallerData.m_str_createTeminal.empty())
	{
		l_tSQLReqInsertCaller.set["createTeminal"] = synCallerData.m_str_createTeminal;
	}
	if (!synCallerData.m_str_updateTeminal.empty())
	{
		l_tSQLReqInsertCaller.set["updateTeminal"] = synCallerData.m_str_updateTeminal;
	}
	if (!synCallerData.m_str_nationality.empty())
	{
		l_tSQLReqInsertCaller.set["nationality"] = synCallerData.m_str_nationality;
	}
	if (!synCallerData.m_str_nation.empty())
	{
		l_tSQLReqInsertCaller.set["nation"] = synCallerData.m_str_nation;
	}
	if (!synCallerData.m_str_educationLevel.empty())
	{
		l_tSQLReqInsertCaller.set["educationLevel"] = synCallerData.m_str_educationLevel;
	}
	if (!synCallerData.m_str_marriageStatus.empty())
	{
		l_tSQLReqInsertCaller.set["marriageStatus"] = synCallerData.m_str_marriageStatus;
	}
	if (!synCallerData.m_str_politicalOutlook.empty())
	{
		l_tSQLReqInsertCaller.set["politicalOutlook"] = synCallerData.m_str_politicalOutlook;
	}
	if (!synCallerData.m_str_is_NPC_deputy.empty())
	{
		l_tSQLReqInsertCaller.set["is_NPC_deputy"] = synCallerData.m_str_is_NPC_deputy;
	}
	if (!synCallerData.m_str_is_national_staff.empty())
	{
		l_tSQLReqInsertCaller.set["is_national_staff"] = synCallerData.m_str_is_national_staff;
	}
	if (!synCallerData.m_str_createUserId.empty())
	{
		l_tSQLReqInsertCaller.set["createUserId"] = synCallerData.m_str_createUserId;
	}
	if (!synCallerData.m_str_createUserName.empty())
	{
		l_tSQLReqInsertCaller.set["createUserName"] = synCallerData.m_str_createUserName;
	}
	if (!synCallerData.m_str_createUserOrgCode.empty())
	{
		l_tSQLReqInsertCaller.set["createUserOrgCode"] = synCallerData.m_str_createUserOrgCode;
	}
	if (!synCallerData.m_str_createUserOrgName.empty())
	{
		l_tSQLReqInsertCaller.set["createUserOrgName"] = synCallerData.m_str_createUserOrgName;
	}
	if (!synCallerData.m_str_updateUserId.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserId"] = synCallerData.m_str_updateUserId;
	}
	if (!synCallerData.m_str_updateUserName.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserName"] = synCallerData.m_str_updateUserName;
	}
	if (!synCallerData.m_str_updateUserOrgCode.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserOrgCode"] = synCallerData.m_str_updateUserOrgCode;
	}
	if (!synCallerData.m_str_updateUserOrgName.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserOrgName"] = synCallerData.m_str_updateUserOrgName;
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertCaller);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "RecvCallerData insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "RecvCallerData,%s Exec sql success! sql:[%s]", l_tSQLReqInsertCaller.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}
	return true;
}

//同步接警单数据
int CBusinessImpl::_ProcSynAlarmData(const std::string& p_strStartTime, const std::string& p_strToken)
{
	std::string strErrorMessage;
	std::string strReceive;
	std::map<std::string, std::string> tmp_mapHeaders;

	tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	tmp_mapHeaders.insert(std::make_pair("Authorization", p_strToken));
	
	std::string l_strLimit = m_pString->Format("%d", m_iPackageSize);
	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
	//std::string tmp_strTarget("/incident/sync-icc-service/history/jjdb");
	std::string tmp_strTarget("/incident/sync-icc-service/history/case_all");

	std::string l_strQueryStartTime = p_strStartTime;
	int l_nIndex = 0;
	do
	{
		JsonParser::IJsonPtr tmp_spJson = ICCGetIJsonFactory()->CreateJson();
		if (NULL == tmp_spJson.get())
		{
			return -1;
		}
		tmp_spJson->SetNodeValue("/startTime", l_strQueryStartTime);
		tmp_spJson->SetNodeValue("/limit", l_strLimit);
		std::string tmp_strContent(tmp_spJson->ToString());

		strReceive = m_pHttpClient->PostWithTimeout(m_strVcsServerIp, m_strVcsPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage, m_nTimeOut);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "/sync/vcs/ receive nothing!!!!err[%s]", strErrorMessage.c_str());
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Recv HttpMsg:[%s]", strReceive.c_str());
		}

		PROTOCOL::CVcsAlarmSynDataRespond synDatas;
		if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", tmp_strTarget.c_str());
			return -1;
		}
		else
		{
			l_nIndex++;
		}

		if (synDatas.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", tmp_strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
			break;
		}

		std::string l_strRecordEndTime;
		for (int i = 0; i < synDatas.m_vecData.size(); i++)
		{
			l_strRecordEndTime = synDatas.m_vecData[i].m_oAlarm.m_strUpdateTime;

			std::string l_strAlarmID = synDatas.m_vecData[i].m_oAlarm.m_strID;
			Alarm l_tAlarmInfo;
			if (!QueryAlarm(l_strAlarmID, l_tAlarmInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "VCS Sync alarm_%d,m_strAlarmID=%s,CreateTime=%s,ProcessDataSize=%d", i, l_strAlarmID.c_str(), synDatas.m_vecData[i].m_oAlarm.m_strCreateTime.c_str(), synDatas.m_vecData[i].m_vecProcessData.size());

				DataBase::SQLRequest l_tSQLReqInsertAlarm;
				l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_jjdb";
				l_tSQLReqInsertAlarm.param["id"] = l_strAlarmID;
				l_tSQLReqInsertAlarm.param["receiving_time"] = synDatas.m_vecData[i].m_oAlarm.m_strTime;// m_pDateTime->CurrentDateTimeStr();
				l_tSQLReqInsertAlarm.param["create_time"] = synDatas.m_vecData[i].m_oAlarm.m_strCreateTime;
				l_tSQLReqInsertAlarm.param["receipt_code"] = synDatas.m_vecData[i].m_oAlarm.m_strReceiptCode;
				l_tSQLReqInsertAlarm.param["received_time"] = synDatas.m_vecData[i].m_oAlarm.m_strReceivedTime;
				l_tSQLReqInsertAlarm.param["is_invalid"] = "0";
				l_tSQLReqInsertAlarm.param["is_merge"] = "0";
				l_tSQLReqInsertAlarm.param["is_visitor"] = "0";
				l_tSQLReqInsertAlarm.param["is_feedback"] = "0";
				l_tSQLReqInsertAlarm.param["is_delete"] = "0";
				l_tSQLReqInsertAlarm.param["had_push"] = "0";
				l_tSQLReqInsertAlarm.param["is_over"] = "0";

				DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
				ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
				if (!l_pRSet->IsValid())
				{
					ICC_LOG_ERROR(m_pLog, "RecvUpdateCase1 insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
					continue;
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "RecvUpdateCase1,%s Exec sql success! sql:[%s]", l_tSQLReqInsertAlarm.sql_id.c_str(), l_pRSet->GetSQL().c_str());
				}
				
				std::vector<Process> l_vecProcess;
				l_vecProcess.clear();
				PROTOCOL::CHeader l_pRespHeader;
				SendAlarmUpdateRequest(l_pRespHeader, synDatas.m_vecData[i].m_oAlarm, l_vecProcess,"1");
			}
			else
			{
				int nUpdateFlag = 0;
				DateTime::CDateTime l_vcsJJUpdateTime = m_pDateTime->FromString(synDatas.m_vecData[i].m_oAlarm.m_strUpdateTime);
				DateTime::CDateTime l_iccJJUpdateTime = m_pDateTime->FromString(l_tAlarmInfo.m_strUpdateTime);
				if (m_nVcsSyncUpdateFlag > 0)  //需要同步更新数据
				{
					nUpdateFlag = -2;
					//比较更新时间
					if (l_vcsJJUpdateTime > l_iccJJUpdateTime && m_pDateTime->SecondsDifference(l_vcsJJUpdateTime, l_iccJJUpdateTime) > 1)
					{
						nUpdateFlag = 1;
						std::vector<Process> l_vecProcess;
						for (int j = 0; j < synDatas.m_vecData[i].m_vecProcessData.size(); j++)
						{
							Process l_oProcess;
							l_oProcess.m_ProcessData = synDatas.m_vecData[i].m_vecProcessData[j];
							if (l_oProcess.m_ProcessData.m_strProcessObjectType == "Staff")
							{
								//调派给个人的不需要同步到ICC
								continue;
							}
							l_oProcess.m_strMsgSource = "vcs_relocated";
							//先查询数据库是否有派警数据，数据没有先写库，有，则根据更新时间比较，如果是新数据则发给icc server做更新处理
							l_oProcess.m_ProcessData.m_strCreateTime;
							l_oProcess.m_ProcessData.m_strID;
							l_vecProcess.push_back(l_oProcess);
						}
						PROTOCOL::CHeader l_pRespHeader;
						SendAlarmUpdateRequest(l_pRespHeader, synDatas.m_vecData[i].m_oAlarm, l_vecProcess, "2");
					}
				}
				else
				{
					if (l_vcsJJUpdateTime > l_iccJJUpdateTime)
					{
						nUpdateFlag = -1;
					}
				}
				ICC_LOG_DEBUG(m_pLog, "%d_VCS Sync alarm_%d,m_strAlarmID=%s,vcs_UpdateTime=%s,icc_UpdateTime=%s,ProcessDataSize=%d", nUpdateFlag, i, l_strAlarmID.c_str(),
					synDatas.m_vecData[i].m_oAlarm.m_strUpdateTime.c_str(), l_tAlarmInfo.m_strUpdateTime.c_str(), synDatas.m_vecData[i].m_vecProcessData.size());
			}

			for (int k = 0; k < synDatas.m_vecData[i].m_vecProcessData.size(); k++)
			{
				ICC_LOG_DEBUG(m_pLog, "process VCS_Sync alarm_%d,m_strAlarmID=%s,ProcessID=%s", i, l_strAlarmID.c_str(), synDatas.m_vecData[i].m_vecProcessData[k].m_strID.c_str());
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Query Alarm VCS Sync,index=%d,dataSize=%d,PackageSize=%d,QueryStartTime=%s,endTime=%s", l_nIndex, synDatas.m_vecData.size(), m_iPackageSize, l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
		if (synDatas.m_vecData.size() < m_iPackageSize)
		{
			break;
		}
		else
		{
			DateTime::CDateTime l_start = m_pDateTime->FromString(l_strQueryStartTime);
			DateTime::CDateTime l_end = m_pDateTime->FromString(l_strRecordEndTime);
			if (l_end < l_start)
			{
				ICC_LOG_ERROR(m_pLog, "query sync Time error,StartTime=%s,RecordEndTime=%s", l_strQueryStartTime.c_str(), l_strRecordEndTime.c_str());
				break;
			}
			l_strQueryStartTime = l_strRecordEndTime;
		}
		
	} while (1);

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

	ICC_LOG_DEBUG(m_pLog, "%s vcs alarm sync complete, time[%d]", tmp_strTarget.c_str(), ullEndTime - ullBeginTime);

	return 0;
}

void CBusinessImpl::BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value)
{
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, l_manualAcceptType, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
		return;
	}
	l_value = l_ParamInfo.m_strValue;
}

void CBusinessImpl::SendMessageToMsgCenterOnManualAccept(PROTOCOL::CAlarmLogSync l_oProcessData, std::string l_curMsg)
{
	DataBase::IResultSetPtr l_sqlResult;

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "alarminfo";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	if (l_oProcessData.m_oBody.m_strSourceName != "vcs")
	{
		return;
	}

	if (l_oProcessData.m_oBody.m_strOperate == "DispatchOrg_ManualAccept" || l_oProcessData.m_oBody.m_strOperate == "DispatchOrg_AutoAccept")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("ManualAcceptOrg", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectOrgName);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Dispatch_AutoAccept" || l_oProcessData.m_oBody.m_strOperate == "Dispatch_ManualAccept")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("ManualAccept", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strFromObjectName);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_SiteDone")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_SiteDone", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectName);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_CaseDone")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_CaseDone", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Feedback_Text")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Feedback_Text", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "DispatchOrg_AcceptTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("DispatchOrg_AcceptTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "DispatchOrg_SendTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("DispatchOrg_SendTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_Arrive")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_Arrive", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectName);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_ArriveTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_ArriveTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_CaseDoneTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_CaseDoneTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Handle_SiteDoneTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Handle_SiteDoneTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectName);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Dispatch_AcceptTimeOut")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Dispatch_AcceptTimeOut", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}
	else if (l_oProcessData.m_oBody.m_strOperate == "Dispatch_Send")
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("Dispatch_Send", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectOrgName);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oProcessData.m_oBody.m_strToObjectName);
	}
	else
	{
		std::string l_curMsg;
		BuildManualAcceptConfig("VcsMsg", l_curMsg);

		l_CDTOData.title = m_pString->ReplaceFirst(l_curMsg, "$", l_oProcessData.m_oBody.m_strAlarmID);
	}


	l_CDTOData.moduleCode = "alarminfo";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_oProcessData.m_oBody.m_strAlarmID;

	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_user_all";
	l_strDeptSql.param["alarm_id"] = l_oProcessData.m_oBody.m_strAlarmID;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);
	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}

	MsgCenter::ReceiveObject l_recObj;
	l_recObj.type = "user";

	while (l_sqlResult->Next())
	{
		l_recObj.noticeList.push_back(l_sqlResult->GetValue("guid"));
		l_recObj.syncList.push_back(l_sqlResult->GetValue("guid"));
	}

	l_CDTOData.receiveObjects.push_back(l_recObj);

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return;
	}

	m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);

	std::string strNacosIp;
	std::string strNacosPort;
	std::string strNameSpace;
	std::string strGroupName;

	_GetNacosParams(strNacosIp, strNacosPort, strNameSpace, strGroupName);

	ICC_LOG_DEBUG(m_pLog, "add msg to msg center : [%s]", l_curSendMsg.c_str());
	m_pMsgCenter->Send(l_curSendMsg, strNacosIp, strNacosPort, strNameSpace, strGroupName);
}


bool CBusinessImpl::_QueryFeedBack(const std::string& p_strID, bool isUpdateFeedBackFlag)
{
	if (p_strID.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_fkdb";
	l_tSQLReq.param["id"] = p_strID;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "_QueryFeedBack  sql  :[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_fdkb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	else
	{
		while (l_pRSet->Next())
		{
			ICC_LOG_DEBUG(m_pLog, "select_icc_t_fdkb ok! sql:[%s]", l_pRSet->GetSQL().c_str());
			isUpdateFeedBackFlag = true;
			return true;
		}
	}
	return true;
}

bool CBusinessImpl::_InsertFeedBackData(const PROTOCOL::CFeekBackInfo& p_oFeekBackInfo)
{
	// return true;
	DataBase::SQLRequest l_tSQLInsertFeedback;
	l_tSQLInsertFeedback.sql_id = "insert_icc_t_fkdb";

	l_tSQLInsertFeedback.param["id"] = p_oFeekBackInfo.m_strID;
	l_tSQLInsertFeedback.param["destrict"] = p_oFeekBackInfo.m_strReceiptDeptDistrictCode;
	l_tSQLInsertFeedback.param["caseId"] = p_oFeekBackInfo.m_strAlarmID;
	l_tSQLInsertFeedback.param["dispatchId"] = p_oFeekBackInfo.m_strProcessID;
	l_tSQLInsertFeedback.param["record"] = p_oFeekBackInfo.m_strRecordID;
	l_tSQLInsertFeedback.param["feedbackOrgCode"] = p_oFeekBackInfo.m_strFeedbackOrgCode;
	l_tSQLInsertFeedback.param["feedbackCode"] = p_oFeekBackInfo.m_strFeedbackCode;
	l_tSQLInsertFeedback.param["feedbackName"] = p_oFeekBackInfo.m_strFeedbackName;
	l_tSQLInsertFeedback.param["feedbackTime"] = p_oFeekBackInfo.m_strFeedbackTime;
	l_tSQLInsertFeedback.param["forceAcceptTime"] = p_oFeekBackInfo.m_strForceAcceptTime;
	l_tSQLInsertFeedback.param["arriveTime"] = p_oFeekBackInfo.m_strArriveTime;
	l_tSQLInsertFeedback.param["siteDoneTime"] = p_oFeekBackInfo.m_strSiteDoneTime;
	l_tSQLInsertFeedback.param["caseType"] = p_oFeekBackInfo.m_strCaseType;
	l_tSQLInsertFeedback.param["case2ndType"] = p_oFeekBackInfo.m_strAlarmSecondType;
	l_tSQLInsertFeedback.param["case3rdType"] = p_oFeekBackInfo.m_strAlarmThirdType;
	l_tSQLInsertFeedback.param["happenTime"] = p_oFeekBackInfo.m_strHappenTime;
	l_tSQLInsertFeedback.param["longitude"] = p_oFeekBackInfo.m_strAlarmLongitude;
	l_tSQLInsertFeedback.param["latitude"] = p_oFeekBackInfo.m_strAlarmLatitude;
	l_tSQLInsertFeedback.param["disposalDec"] = p_oFeekBackInfo.m_strDisposalDec;
	l_tSQLInsertFeedback.param["dispatcheVehicles"] = p_oFeekBackInfo.m_strDispatcheVehicles;
	l_tSQLInsertFeedback.param["dispatchePeople"] = p_oFeekBackInfo.m_strDispatchePeople;
	l_tSQLInsertFeedback.param["rescuers"] = p_oFeekBackInfo.m_strRescuers;
	l_tSQLInsertFeedback.param["rescuersDes"] = p_oFeekBackInfo.m_strRescuersDes;
	l_tSQLInsertFeedback.param["injured"] = p_oFeekBackInfo.m_strIinjured;
	l_tSQLInsertFeedback.param["injuredDes"] = p_oFeekBackInfo.m_strInjuredDes;
	l_tSQLInsertFeedback.param["dead"] = p_oFeekBackInfo.m_strDead;
	l_tSQLInsertFeedback.param["deadDes"] = p_oFeekBackInfo.m_strDeadDes;
	l_tSQLInsertFeedback.param["resultCode"] = p_oFeekBackInfo.m_strResultCode;
	l_tSQLInsertFeedback.param["resultDes"] = p_oFeekBackInfo.m_strResultDes;
	l_tSQLInsertFeedback.param["weatherCode"] = p_oFeekBackInfo.m_strWeatherCode;
	l_tSQLInsertFeedback.param["lossDes"] = p_oFeekBackInfo.m_strLossDes;
	l_tSQLInsertFeedback.param["caught"] = p_oFeekBackInfo.m_strCaught;
	l_tSQLInsertFeedback.param["involved"] = p_oFeekBackInfo.m_strInvolved;
	l_tSQLInsertFeedback.param["escapees"] = p_oFeekBackInfo.m_strEscapees;
	l_tSQLInsertFeedback.param["trafficAccidentCode"] = p_oFeekBackInfo.m_strTrafficAccidentCode;
	l_tSQLInsertFeedback.param["isCarryDangerousGoods"] = p_oFeekBackInfo.m_strIsCarryDangerousGoods;
	l_tSQLInsertFeedback.param["trafficAccidentCauseCode"] = p_oFeekBackInfo.m_strTrafficAccidentCauseCode;
	l_tSQLInsertFeedback.param["visibilityCode"] = p_oFeekBackInfo.m_strVisibilityCode;
	l_tSQLInsertFeedback.param["roadConditionCode"] = p_oFeekBackInfo.m_strRoadConditionCode;
	l_tSQLInsertFeedback.param["damagedVehicles"] = p_oFeekBackInfo.m_strDamagedVehicles;
	l_tSQLInsertFeedback.param["damagedNonMotorVehicles"] = p_oFeekBackInfo.m_strDamagedNonMotorVehicles;
	l_tSQLInsertFeedback.param["roadTypeCode"] = p_oFeekBackInfo.m_strRoadTypeCode;
	l_tSQLInsertFeedback.param["status"] = p_oFeekBackInfo.m_strStatus;
	l_tSQLInsertFeedback.param["createTime"] = p_oFeekBackInfo.m_strCreateTime.empty() ? m_pDateTime->CurrentDateTimeStr(): p_oFeekBackInfo.m_strCreateTime;
	l_tSQLInsertFeedback.param["updateTime"] = p_oFeekBackInfo.m_strUpdateTime.empty() ? m_pDateTime->CurrentDateTimeStr(): p_oFeekBackInfo.m_strUpdateTime;
	l_tSQLInsertFeedback.param["townShip"] = p_oFeekBackInfo.m_strTownShip;
	l_tSQLInsertFeedback.param["territorialVillage"] = p_oFeekBackInfo.m_strTerritorialVillage;
	l_tSQLInsertFeedback.param["isEvidence"] = p_oFeekBackInfo.m_strIsEvidence;
	l_tSQLInsertFeedback.param["transferOrgCode"] = p_oFeekBackInfo.m_strTransferOrgCode;
	l_tSQLInsertFeedback.param["transferOrgContacts"] = p_oFeekBackInfo.m_strTransferOrgContacts;
	l_tSQLInsertFeedback.param["transferOrgFeedbackDesCode"] = p_oFeekBackInfo.m_strTransferOrgFeedbackDesCode;
	l_tSQLInsertFeedback.param["transferOrgFeedbackDes"] = p_oFeekBackInfo.m_strTransferOrgFeedbackDes;
	l_tSQLInsertFeedback.param["caseNumber"] = p_oFeekBackInfo.m_strCaseNumber;
	l_tSQLInsertFeedback.param["lable"] = p_oFeekBackInfo.m_strLable;
	l_tSQLInsertFeedback.param["occurredArea"] = p_oFeekBackInfo.m_strOccurredArea;
	l_tSQLInsertFeedback.param["locationCode"] = p_oFeekBackInfo.m_strLocationCode;
	// l_tSQLInsertFeedback.param["forceId"] = p_oFeekBackInfo.m_strForceId;
	l_tSQLInsertFeedback.param["policeType"] = p_oFeekBackInfo.m_strPoliceType;
	// l_tSQLInsertFeedback.param["version"] = p_oFeekBackInfo.m_strVersion;
	l_tSQLInsertFeedback.param["bllStatus"] = p_oFeekBackInfo.m_strBllStatus;
	l_tSQLInsertFeedback.param["feedbackOrgIdentifier"] = p_oFeekBackInfo.m_strFeedbackOrgIdentifier;
	// l_tSQLInsertFeedback.param["feedbackOrgId"] = p_oFeekBackInfo.m_strFeedbackOrgId;
	l_tSQLInsertFeedback.param["feedbackOrgName"] = p_oFeekBackInfo.m_strFeedbackOrgName;
	l_tSQLInsertFeedback.param["liveStatus"] = p_oFeekBackInfo.m_strLiveStatus;
	l_tSQLInsertFeedback.param["case4thType"] = p_oFeekBackInfo.m_strCase4thType;
	l_tSQLInsertFeedback.param["updateOrgIdentifier"] = p_oFeekBackInfo.m_strUpdateOrgIdentifier;
	// l_tSQLInsertFeedback.param["timeOutStatus"] = p_oFeekBackInfo.m_strTimeOutStatus;
	l_tSQLInsertFeedback.param["launchBoat"] = p_oFeekBackInfo.m_strLaunchBoat;
	l_tSQLInsertFeedback.param["alarmAddress"] = p_oFeekBackInfo.m_strAlarmAddress;
	l_tSQLInsertFeedback.param["address"] = p_oFeekBackInfo.m_strAddress;
	l_tSQLInsertFeedback.param["isInvaild"] = p_oFeekBackInfo.m_strIsInvaild;
	l_tSQLInsertFeedback.param["transferOrgName"] = p_oFeekBackInfo.m_strTransferOrgName;
	l_tSQLInsertFeedback.param["transferOrgIdentifier"] = p_oFeekBackInfo.m_strTransferOrgIdentifier;
	// l_tSQLInsertFeedback.param["transferOrgContactsId"] = p_oFeekBackInfo.m_strTransferOrgContactsId;
	l_tSQLInsertFeedback.param["modifyAreaOrgId"] = p_oFeekBackInfo.m_strModifyAreaOrgId;
	l_tSQLInsertFeedback.param["modifyAreaOrgName"] = p_oFeekBackInfo.m_strModifyAreaOrgName;
	l_tSQLInsertFeedback.param["modifyAreaOrgIdentifier"] = p_oFeekBackInfo.m_strModifyAreaOrgIdentifier;
	l_tSQLInsertFeedback.param["district_name"] = p_oFeekBackInfo.m_strDistrictName;
	// l_tSQLInsertFeedback.param["transferOrgId"] = p_oFeekBackInfo.m_strTransferOrgId;
	std::string strTime = "";
	if (GetAlarmReceiveByAlarmId(p_oFeekBackInfo.m_strAlarmID, strTime) && !strTime.empty())
	{
		l_tSQLInsertFeedback.param["received_time"] = strTime;
	}
	else if (strTime.empty() && !p_oFeekBackInfo.m_strReceivedTime.empty()) {
		l_tSQLInsertFeedback.param["received_time"] = p_oFeekBackInfo.m_strReceivedTime;
	}
	else {

	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLInsertFeedback);
	ICC_LOG_DEBUG(m_pLog, "_QueryFeedBack  sql  :[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert feedback info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::GetAlarmReceiveByAlarmId(const std::string& p_strAlarmID, std::string& p_strReceiceTime)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["id"] = p_strAlarmID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "GetAlarmReceiveByAlarmId sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		if (l_pResult->Next())
		{
			p_strReceiceTime = l_pResult->GetValue("received_time");
		}
	}
	return true;
}

bool CBusinessImpl::_UpdateFeedBackData(const PROTOCOL::CFeekBackInfo& p_oFeekBackInfo)
{
	DataBase::SQLRequest l_tSQLUpdateFeedback;
	l_tSQLUpdateFeedback.sql_id = "update_icc_t_fkdb";

	l_tSQLUpdateFeedback.param["id"] = p_oFeekBackInfo.m_strID;

	l_tSQLUpdateFeedback.set["destrict"] = p_oFeekBackInfo.m_strReceiptDeptDistrictCode;
	if (!p_oFeekBackInfo.m_strAlarmID.empty())
	{
		l_tSQLUpdateFeedback.set["caseId"] = p_oFeekBackInfo.m_strAlarmID;
	}
	if (!p_oFeekBackInfo.m_strProcessID.empty())
	{
		l_tSQLUpdateFeedback.set["dispatchId"] = p_oFeekBackInfo.m_strProcessID;
	}
	l_tSQLUpdateFeedback.set["record"] = p_oFeekBackInfo.m_strRecordID;
	l_tSQLUpdateFeedback.set["feedbackOrgCode"] = p_oFeekBackInfo.m_strFeedbackOrgCode;
	l_tSQLUpdateFeedback.set["feedbackCode"] = p_oFeekBackInfo.m_strFeedbackCode;
	l_tSQLUpdateFeedback.set["feedbackName"] = p_oFeekBackInfo.m_strFeedbackName;
	if (!p_oFeekBackInfo.m_strFeedbackTime.empty())
	{
		l_tSQLUpdateFeedback.set["feedbackTime"] = p_oFeekBackInfo.m_strFeedbackTime;
	}

	l_tSQLUpdateFeedback.set["forceAcceptTime"] = p_oFeekBackInfo.m_strForceAcceptTime;
	l_tSQLUpdateFeedback.set["arriveTime"] = p_oFeekBackInfo.m_strArriveTime;
	l_tSQLUpdateFeedback.set["siteDoneTime"] = p_oFeekBackInfo.m_strSiteDoneTime;
	l_tSQLUpdateFeedback.set["caseType"] = p_oFeekBackInfo.m_strCaseType;
	l_tSQLUpdateFeedback.set["case2ndType"] = p_oFeekBackInfo.m_strAlarmSecondType;
	l_tSQLUpdateFeedback.set["case3rdType"] = p_oFeekBackInfo.m_strAlarmThirdType;
	l_tSQLUpdateFeedback.set["happenTime"] = p_oFeekBackInfo.m_strHappenTime;
	if (!p_oFeekBackInfo.m_strAlarmLongitude.empty())
	{
		l_tSQLUpdateFeedback.set["longitude"] = p_oFeekBackInfo.m_strAlarmLongitude;
	}

	if (!p_oFeekBackInfo.m_strAlarmLatitude.empty())
	{
		l_tSQLUpdateFeedback.set["latitude"] = p_oFeekBackInfo.m_strAlarmLatitude;
	}

	l_tSQLUpdateFeedback.set["disposalDec"] = p_oFeekBackInfo.m_strDisposalDec;
	l_tSQLUpdateFeedback.set["dispatcheVehicles"] = p_oFeekBackInfo.m_strDispatcheVehicles;
	l_tSQLUpdateFeedback.set["dispatchePeople"] = p_oFeekBackInfo.m_strDispatchePeople;
	l_tSQLUpdateFeedback.set["rescuers"] = p_oFeekBackInfo.m_strRescuers;
	l_tSQLUpdateFeedback.set["rescuersDes"] = p_oFeekBackInfo.m_strRescuersDes;
	l_tSQLUpdateFeedback.set["injured"] = p_oFeekBackInfo.m_strIinjured;
	l_tSQLUpdateFeedback.set["injuredDes"] = p_oFeekBackInfo.m_strInjuredDes;
	l_tSQLUpdateFeedback.set["dead"] = p_oFeekBackInfo.m_strDead;
	l_tSQLUpdateFeedback.set["deadDes"] = p_oFeekBackInfo.m_strDeadDes;
	l_tSQLUpdateFeedback.set["resultCode"] = p_oFeekBackInfo.m_strResultCode;
	l_tSQLUpdateFeedback.set["resultDes"] = p_oFeekBackInfo.m_strResultDes;
	l_tSQLUpdateFeedback.set["weatherCode"] = p_oFeekBackInfo.m_strWeatherCode;
	l_tSQLUpdateFeedback.set["lossDes"] = p_oFeekBackInfo.m_strLossDes;
	l_tSQLUpdateFeedback.set["caught"] = p_oFeekBackInfo.m_strCaught;
	l_tSQLUpdateFeedback.set["involved"] = p_oFeekBackInfo.m_strInvolved;
	l_tSQLUpdateFeedback.set["escapees"] = p_oFeekBackInfo.m_strEscapees;
	l_tSQLUpdateFeedback.set["trafficAccidentCode"] = p_oFeekBackInfo.m_strTrafficAccidentCode;
	l_tSQLUpdateFeedback.set["isCarryDangerousGoods"] = p_oFeekBackInfo.m_strIsCarryDangerousGoods;
	l_tSQLUpdateFeedback.set["trafficAccidentCauseCode"] = p_oFeekBackInfo.m_strTrafficAccidentCauseCode;
	l_tSQLUpdateFeedback.set["visibilityCode"] = p_oFeekBackInfo.m_strVisibilityCode;
	l_tSQLUpdateFeedback.set["roadConditionCode"] = p_oFeekBackInfo.m_strRoadConditionCode;
	l_tSQLUpdateFeedback.set["damagedVehicles"] = p_oFeekBackInfo.m_strDamagedVehicles;
	l_tSQLUpdateFeedback.set["damagedNonMotorVehicles"] = p_oFeekBackInfo.m_strDamagedNonMotorVehicles;
	l_tSQLUpdateFeedback.set["roadTypeCode"] = p_oFeekBackInfo.m_strRoadTypeCode;
	if (!p_oFeekBackInfo.m_strStatus.empty())
	{
		l_tSQLUpdateFeedback.set["status"] = p_oFeekBackInfo.m_strStatus;
	}
	if (!p_oFeekBackInfo.m_strUpdateTime.empty())
	{
		l_tSQLUpdateFeedback.set["updateTime"] = p_oFeekBackInfo.m_strUpdateTime;
	}
	if (!p_oFeekBackInfo.m_strTownShip.empty())
	{
		l_tSQLUpdateFeedback.set["townShip"] = p_oFeekBackInfo.m_strTownShip;
	}
	if (!p_oFeekBackInfo.m_strTerritorialVillage.empty())
	{
		l_tSQLUpdateFeedback.set["territorialVillage"] = p_oFeekBackInfo.m_strTerritorialVillage;
	}
	if (!p_oFeekBackInfo.m_strIsEvidence.empty())
	{
		l_tSQLUpdateFeedback.set["isEvidence"] = p_oFeekBackInfo.m_strIsEvidence;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgCode.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgCode"] = p_oFeekBackInfo.m_strTransferOrgCode;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgContacts.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgContacts"] = p_oFeekBackInfo.m_strTransferOrgContacts;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgFeedbackDesCode.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgFeedbackDesCode"] = p_oFeekBackInfo.m_strTransferOrgFeedbackDesCode;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgFeedbackDes.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgFeedbackDes"] = p_oFeekBackInfo.m_strTransferOrgFeedbackDes;
	}
	if (!p_oFeekBackInfo.m_strCaseNumber.empty())
	{
		l_tSQLUpdateFeedback.set["caseNumber"] = p_oFeekBackInfo.m_strCaseNumber;
	}
	if (!p_oFeekBackInfo.m_strLable.empty())
	{
		l_tSQLUpdateFeedback.set["lable"] = p_oFeekBackInfo.m_strLable;
	}
	if (!p_oFeekBackInfo.m_strOccurredArea.empty())
	{
		l_tSQLUpdateFeedback.set["occurredArea"] = p_oFeekBackInfo.m_strOccurredArea;
	}
	if (!p_oFeekBackInfo.m_strLocationCode.empty())
	{
		l_tSQLUpdateFeedback.set["locationCode"] = p_oFeekBackInfo.m_strLocationCode;
	}
	//if (!p_oFeekBackInfo.m_strForceId.empty())
	//{
	//	l_tSQLUpdateFeedback.set["forceId"] = p_oFeekBackInfo.m_strForceId;
	//}
	if (!p_oFeekBackInfo.m_strPoliceType.empty())
	{
		l_tSQLUpdateFeedback.set["policeType"] = p_oFeekBackInfo.m_strPoliceType;
	}
	//if (!p_oFeekBackInfo.m_strVersion.empty())
	//{
	//	l_tSQLUpdateFeedback.set["version"] = p_oFeekBackInfo.m_strVersion;
	//}
	if (!p_oFeekBackInfo.m_strBllStatus.empty())
	{
		l_tSQLUpdateFeedback.set["bllStatus"] = p_oFeekBackInfo.m_strBllStatus;
	}
	if (!p_oFeekBackInfo.m_strFeedbackOrgIdentifier.empty())
	{
		l_tSQLUpdateFeedback.set["feedbackOrgIdentifier"] = p_oFeekBackInfo.m_strFeedbackOrgIdentifier;
	}
	//if (!p_oFeekBackInfo.m_strFeedbackOrgId.empty())
	//{
	//	l_tSQLUpdateFeedback.set["feedbackOrgId"] = p_oFeekBackInfo.m_strFeedbackOrgId;
	//}
	if (!p_oFeekBackInfo.m_strFeedbackOrgName.empty())
	{
		l_tSQLUpdateFeedback.set["feedbackOrgName"] = p_oFeekBackInfo.m_strFeedbackOrgName;
	}
	if (!p_oFeekBackInfo.m_strLiveStatus.empty())
	{
		l_tSQLUpdateFeedback.set["liveStatus"] = p_oFeekBackInfo.m_strLiveStatus;
	}
	if (!p_oFeekBackInfo.m_strCase4thType.empty())
	{
		l_tSQLUpdateFeedback.set["case4thType"] = p_oFeekBackInfo.m_strCase4thType;
	}
	if (!p_oFeekBackInfo.m_strUpdateOrgIdentifier.empty())
	{
		l_tSQLUpdateFeedback.set["updateOrgIdentifier"] = p_oFeekBackInfo.m_strUpdateOrgIdentifier;
	}
	//if (!p_oFeekBackInfo.m_strTimeOutStatus.empty())
	//{
	//	l_tSQLUpdateFeedback.set["timeOutStatus"] = p_oFeekBackInfo.m_strTimeOutStatus;
	//}
	if (!p_oFeekBackInfo.m_strLaunchBoat.empty())
	{
		l_tSQLUpdateFeedback.set["launchBoat"] = p_oFeekBackInfo.m_strLaunchBoat;
	}
	if (!p_oFeekBackInfo.m_strAlarmAddress.empty())
	{
		l_tSQLUpdateFeedback.set["alarmAddress"] = p_oFeekBackInfo.m_strAlarmAddress;
	}
	if (!p_oFeekBackInfo.m_strAddress.empty())
	{
		l_tSQLUpdateFeedback.set["address"] = p_oFeekBackInfo.m_strAddress;
	}
	if (!p_oFeekBackInfo.m_strIsInvaild.empty())
	{
		l_tSQLUpdateFeedback.set["isInvaild"] = p_oFeekBackInfo.m_strIsInvaild;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgName.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgName"] = p_oFeekBackInfo.m_strTransferOrgName;
	}
	if (!p_oFeekBackInfo.m_strTransferOrgIdentifier.empty())
	{
		l_tSQLUpdateFeedback.set["transferOrgIdentifier"] = p_oFeekBackInfo.m_strTransferOrgIdentifier;
	}
	//if (!p_oFeekBackInfo.m_strTransferOrgContactsId.empty())
	//{
	//	l_tSQLUpdateFeedback.set["transferOrgContactsId"] = p_oFeekBackInfo.m_strTransferOrgContactsId;
	//}
	if (!p_oFeekBackInfo.m_strModifyAreaOrgId.empty())
	{
		l_tSQLUpdateFeedback.set["modifyAreaOrgId"] = p_oFeekBackInfo.m_strModifyAreaOrgId;
	}
	if (!p_oFeekBackInfo.m_strModifyAreaOrgName.empty())
	{
		l_tSQLUpdateFeedback.set["modifyAreaOrgName"] = p_oFeekBackInfo.m_strModifyAreaOrgName;
	}
	if (!p_oFeekBackInfo.m_strModifyAreaOrgIdentifier.empty())
	{
		l_tSQLUpdateFeedback.set["modifyAreaOrgIdentifier"] = p_oFeekBackInfo.m_strModifyAreaOrgIdentifier;
	}
	if (!p_oFeekBackInfo.m_strDistrictName.empty())
	{
		l_tSQLUpdateFeedback.set["district_name"] = p_oFeekBackInfo.m_strDistrictName;
	}
	//if (!p_oFeekBackInfo.m_strTransferOrgId.empty())
	//{
	//	l_tSQLUpdateFeedback.set["transferOrgId"] = p_oFeekBackInfo.m_strTransferOrgId;
	//}

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_oFeekBackInfo.m_strAlarmID);
	if (strTime != "")
	{
		l_tSQLUpdateFeedback.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLUpdateFeedback.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}


	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLUpdateFeedback);
	ICC_LOG_DEBUG(m_pLog, "_QueryFeedBack  sql  :[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update feedback info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

void CBusinessImpl::OnNotifiAlarmAttachSyn(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiSynAlarmAttachRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::SynAlarmAttachRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	PROTOCOL::SynAlarmAttachRequest l_oResponse;

	if (!l_oRequest.m_oBody.m_strBeginTime.empty())
	{
		SearchAlarmAttach(l_oRequest, l_oResponse);
	}
	else
	{
		l_oResponse.m_oBody.m_strcode = "400";
		l_oResponse.m_oBody.m_strmessage = "begin_time empty";
		ICC_LOG_ERROR(m_pLog, "failed begin_time empty ");
	}

	std::string strmsg = l_oResponse.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "syn_alarm_attach_request send message:[%s]", strmsg.c_str());

}

void CBusinessImpl::OnVcsAlarmAttachSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnVcsAlarmAttachSync: [%s]", p_strMsg.c_str());
	PROTOCOL::CNotifiAddAlarmAttachRequest l_oSyncRequest;
	//回复
	PROTOCOL::CNotifiAlarmAttachResponse l_oRespond;
	do 
	{
		if (!l_oSyncRequest.ParseStringSync(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse msg failed.");
			l_oRespond.m_oBody.m_strcode = "301";
			l_oRespond.m_oBody.m_strmessage = "data[] empty";
			break;
		}

		if (l_oSyncRequest.m_oBody.m_vecdata.empty())
		{
			l_oRespond.m_oBody.m_strcode = "400";
			l_oRespond.m_oBody.m_strmessage = "data[] empty";
			break;
		}
		// 转发给icc
		if (l_oSyncRequest.m_oHeader.m_strRequest == "queue_alarm_to_icc")
		{
			l_oSyncRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
			l_oSyncRequest.m_oHeader.m_strCmd = "add_alarm_attach_request";
			l_oSyncRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oSyncRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
			l_oSyncRequest.m_oHeader.m_strRequestType = "0";
			l_oSyncRequest.m_oHeader.m_strRequestFlag = "MQ";
			l_oSyncRequest.m_oHeader.m_strResponse = "VCS";
			std::string l_strMessage = l_oSyncRequest.ToStringSync(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}

	} while (false);


	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pRequest->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnVcsAlarmAttachSync send message:[%s]", strmsg.c_str());
}

void CBusinessImpl::SearchAlarmAttach(const PROTOCOL::SynAlarmAttachRequest& p_pRequest, PROTOCOL::SynAlarmAttachRequest& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	std::string l_strcount;
	DataBase::IResultSetPtr l_pRSetCount;
	DataBase::IResultSetPtr l_Result;


	// 获取总数
	if (!SearchAttach(p_pRequest, "search_icc_t_attach_syn_count", sqlReqeust, l_pRSetCount))
	{
		p_pResponse.m_oBody.m_strcode = "401";
		p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_attach_syn_count failed";
		return;
	}

	// 每页个数
	if (!SearchAttach(p_pRequest, "search_icc_t_attach_syn", sqlReqeust, l_Result))
	{
		p_pResponse.m_oBody.m_strcode = "402";
		p_pResponse.m_oBody.m_strmessage = "Search search_icc_t_attach_syn failed";
		return;
	}

	if (l_pRSetCount->Next())
	{
		// 总数
		l_strcount = l_pRSetCount->GetValue("count");
	}

	int l_iSize = m_pString->ToInt(p_pRequest.m_oBody.m_strSize);
	int l_iCurrent = m_pString->ToInt(p_pRequest.m_oBody.m_strCurrent);
	int l_iCount = m_pString->ToInt(l_strcount);

	if (l_iSize <= 0)
	{
		l_iSize = 10;
	}

	if (l_iCurrent < 1)
	{
		l_iCurrent = 1;
	}

	if (l_Result->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_Result->RecordSize();
		p_pResponse.m_oBody.m_strSize = std::to_string(l_iSize);
		p_pResponse.m_oBody.m_strCurrent = std::to_string(l_iCurrent);

		int l_iPages = 0;
		if (l_iCount % l_iSize != 0)
		{
			l_iPages = 1;
		}

		p_pResponse.m_oBody.m_strPages = std::to_string((l_iCount / l_iSize) + l_iPages);
		p_pResponse.m_oBody.m_strCount = l_strcount;

		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_data.m_strId = l_Result->GetValue(index, "id");
			p_pResponse.m_oBody.m_data.m_stralarmId = l_Result->GetValue(index, "alarm_id");
			p_pResponse.m_oBody.m_data.m_strattachId = l_Result->GetValue(index, "attach_id");

			p_pResponse.m_oBody.m_data.m_strattachName = l_Result->GetValue(index, "attach_name");
			p_pResponse.m_oBody.m_data.m_strattachType = l_Result->GetValue(index, "attach_type");
			p_pResponse.m_oBody.m_data.m_strattachPath = l_Result->GetValue(index, "attach_path");
			p_pResponse.m_oBody.m_data.m_strattachSource = l_Result->GetValue(index, "attach_source");
			p_pResponse.m_oBody.m_data.m_strcreateUser = l_Result->GetValue(index, "create_user");
			p_pResponse.m_oBody.m_data.m_strcreateTime = l_Result->GetValue(index, "create_time");
			p_pResponse.m_oBody.m_vecdata.push_back(p_pResponse.m_oBody.m_data);
		}
	}

	p_pResponse.m_oBody.m_strcode = "200";
	p_pResponse.m_oBody.m_strmessage = "success";
}

bool CBusinessImpl::SearchAttach(const PROTOCOL::SynAlarmAttachRequest& p_pRequest, std::string p_pSqlid, DataBase::SQLRequest sqlReqeust, DataBase::IResultSetPtr& p_pRSet)
{
	sqlReqeust.sql_id = p_pSqlid;
	sqlReqeust.param["begin_time"] = p_pRequest.m_oBody.m_strBeginTime;
	sqlReqeust.param["end_time"] = p_pRequest.m_oBody.m_strEndTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_oBody.m_strEndTime;

	if (0 == p_pSqlid.compare("search_icc_t_attach_syn"))
	{
		int l_isize = m_pString->ToInt(p_pRequest.m_oBody.m_strSize);
		int l_icurrent = m_pString->ToInt(p_pRequest.m_oBody.m_strCurrent);

		if (l_isize <= 0)
		{
			l_isize = 10;
		}

		if (l_icurrent < 1)
		{
			l_icurrent = 1;
		}

		sqlReqeust.param["limit"] = std::to_string(l_isize);
		sqlReqeust.param["offset"] = std::to_string(l_icurrent - 1);

	}

	p_pRSet = m_pDBConn->Exec(sqlReqeust, true);
	ICC_LOG_INFO(m_pLog, "search sql:[%s]", p_pRSet->GetSQL().c_str());

	if (!p_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "search attach,error msg[\n%s\n]", p_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}


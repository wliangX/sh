#include "Boost.h"

#include "BusinessImpl.h"
#include "SwitchEventCallback.h"
#include "BlackListManager.h"
#include "TransferCallManager.h"
#include "ConferenceManager.h"
#include "HytProtocol.h"
#define ACD_INFO "acd_dept"
//////////////////////////////////////////////////////////////////////////
CBusinessImpl::CBusinessImpl()
{
	//
}
CBusinessImpl::~CBusinessImpl()
{
	//
}


void CBusinessImpl::OnInit()
{
	m_nTransferRingTimeout = DEFAULT_RING_TIMEOUT;
	m_nConferenceRingTimeout = DEFAULT_RING_TIMEOUT;
	m_bLoadPlugins = true;
	m_bStartService = false;

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);

	
	
	m_StringUtilPtr = ICCGetIStringFactory()->CreateString();
	m_DateTimePtr = ICCGetIDateTimeFactory()->CreateDateTime();
//	m_TimerManagerPtr = ICCGetITimerFactory()->CreateTimerManager();
	m_JsonFactoryPtr = ICCGetIJsonFactory();
	m_ConfigPtr = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pDBConnPtr = ICCGetIDBConnFactory()->CreateDBConn(DataBase::DBType::PostgreSQL);
	m_LogPtr = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
//	m_dbProcessMutex = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);

	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	std::string l_strSwitchApiType = m_ConfigPtr->GetValue("ICC/Plugin/CTI/SwitchApiType", "0");
	if (l_strSwitchApiType == "3")
	{
		m_SwitchClientPtr = ICCGetIHCPClientFactory()->CreateSwitchClient(ClientType_HCP);
		CHytProtocol::Instance()->SetFSAesMode(true);
	}
	else
	{
		m_SwitchClientPtr = ICCGetISwitchClientFactory()->CreateSwitchClient(ClientType_TSAPI);
	}

	ICC_LOG_DEBUG(m_LogPtr, "CTI Plugin Init Success,SwitchApiType:[%s]", l_strSwitchApiType.c_str());
}

void CBusinessImpl::OnStart()
{
	m_strServiceID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "ICC.Server.CTI");
	std::string l_strSwitchApiType = m_ConfigPtr->GetValue("ICC/Plugin/CTI/SwitchApiType", "0");
	
	if (l_strSwitchApiType.compare(SWITCH_TYPE_TAPI) == 0)
	{
		m_bLoadPlugins = false;
		ICC_LOG_DEBUG(m_LogPtr, "There is no need to load the plugin");

		return;
	}

	m_strMinOnlineAgentNum = m_ConfigPtr->GetValue("ICC/Plugin/CTI/MinOnlineAgentNum", "1");

	_LoadMergeHuntGrpConver();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_LoadAllBlackListRespond, OnNotifiLoadAllBlackListRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_LoadAllBlackListSync, OnNotifiLoadAllBlackListSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ClientRegisterSync, OnNotifiClientRegisterSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DoubleServerSync, OnNotifiDoubleServerSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_AgentLoginRequest, OnNotifiAgentLoginRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_AgentLogoutRequest, OnNotifiAgentLogoutRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetAgentStateRequest, OnNotifiSetAgentStateRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_MakeCallRequest, OnNotifiMakeCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_HangupRequest, OnNotifiHangupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_AnswerCallRequest, OnNotifiAnswerCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RefuseAnswerRequest, OnNotifiRefuseAnswerRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ClearCallRequest, OnNotifiClearCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ListenCallRequest, OnNotifiListenCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_PickupCallRequest, OnNotifiPickupCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BargeInCallRequest, OnNotifiBargeInCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ForcePopCallRequest, OnNotifiForcePopCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ConsultationCallRequest, OnNotifiConsultationCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TransferCallRequest, OnNotifiTransferCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TransferCallExRequest, OnNotifiTransferCallExRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CancelTransferCallExRequest, OnNotifiCancelTransferCallExRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeflectCallRequest, OnNotifiDeflectCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_HoldCallRequest, OnNotifiHoldCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RetrieveCallRequest, OnNotifiRetrieveCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ReconnectCallRequest, OnNotifiReconnectCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_MakeConferenceRequest, OnMakeConferenceRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_AddConferencePartyRequest, OnAddConferencePartyRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetConferencePartyRequest, OnGetConferencePartyRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_conference_by_callid", OnQueryConferenceByCallidRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "disenable_conference_party_request", OnDisenableConferencePartyRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "make_conference_ex_request", OnMakeConferenceExRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteConferencePartyRequest, OnDeleteConferencePartyRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TakeOverCallRequest, OnTakeOverCallRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCTIConnStateRequest, OnNotifiGetCTIConnStateRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetDeviceListRequest, OnNotifiGetDeviceListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetACDListRequest, OnNotifiGetACDListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAgentListRequest, OnNotifiGetAgentListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_free_agent_request", OnNotifiGetFreeAgentRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCallListRequest, OnNotifiGetCallListRequest);
//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallEventQueryRequest, OnNotifiQueryCallEventRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetReadyAgentRequest, OnNotifiGetReadyAgentRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetBlackListRequest, OnNotifiSetBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteBlackListRequest, OnNotifiDeleteBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteAllBlackListRequest, OnNotifiDeleteAllBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAllBlackListRequest, OnNotifiGetAllBlackListRequest);
//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BlackCallQueryRequest, OnNotifiQueryBlackCallRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCurrentTimeRequest, OnNotifiGetCurrentTimeRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "agent_timeout_sync", OnAgentHeartBeatTimeoutSync);

	

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_min_online_agent_request", OnNotifiGetMinOnlineAgentRequest);
//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RemoveReleaseCallRequest, OnNotifiRemoveReleaseCallRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_history_callList", OnGetHistoryCallListRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "record_file_up_sync", OnReceiveRecordFileUpRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_over_sync", OnReceiveRecordFileUpRequest);

	//	m_TimerManagerPtr->AddTimer(BLACK_LIST_TIMEOUT, 0, boost::make_shared<CHandleTimer>(this));

	this->LoadParams();

	CBlackListManager::Instance()->SetLogPtr(m_LogPtr);
	CBlackListManager::Instance()->SetDateTime(m_DateTimePtr);
	CBlackListManager::Instance()->SetSwitchClientPtr(m_SwitchClientPtr);

	CHytProtocol::Instance()->SetObserverCenterPtr(m_pObserverCenter);
	CHytProtocol::Instance()->SetSwitchClientPtr(m_SwitchClientPtr);
	CHytProtocol::Instance()->SetStringUtilPtr(m_StringUtilPtr);
	CHytProtocol::Instance()->SetJsonFactoryPtr(m_JsonFactoryPtr);
	CHytProtocol::Instance()->SetDateTimePtr(m_DateTimePtr);
	CHytProtocol::Instance()->SetLogPtr(m_LogPtr);
	CHytProtocol::Instance()->SetRedisClientPtr(m_pRedisClient);
	CHytProtocol::Instance()->SetDBConnPtr(m_pDBConnPtr);
	CHytProtocol::Instance()->SetHelpToolPtr(m_pHelpTool);
	CHytProtocol::Instance()->SetConfiglPtr(m_pStaticConfig);


	CTransferCallManager::Instance()->SetLogPtr(m_LogPtr);
	CTransferCallManager::Instance()->SetDateTimePtr(m_DateTimePtr);
	CTransferCallManager::Instance()->SetSwitchClientPtr(m_SwitchClientPtr);
	CTransferCallManager::Instance()->SetRingTimeout(m_nTransferRingTimeout);

	CConferenceManager::Instance()->SetLogPtr(m_LogPtr);
	CConferenceManager::Instance()->SetDateTimePtr(m_DateTimePtr);
	CConferenceManager::Instance()->SetStringUtilPtr(m_StringUtilPtr);
	CConferenceManager::Instance()->SetSwitchClientPtr(m_SwitchClientPtr);
	CConferenceManager::Instance()->SetRingTimeout(m_nConferenceRingTimeout);

	if (l_strSwitchApiType == "3")
	{
		CTransferCallManager::Instance()->SetFsAesMode(true);
	}
	this->InitSwitch();

	ICC_LOG_DEBUG(m_LogPtr, "CTI Plugin Start Success,MinOnlineAgentNum:%s", m_strMinOnlineAgentNum.c_str());
}

void CBusinessImpl::OnStop()
{
	if (m_bLoadPlugins)
	{
		this->StopServeice();

		CTransferCallManager::Instance()->ExitInstance();
		CConferenceManager::Instance()->ExitInstance();
		CBlackListManager::Instance()->ExitInstance();
	}

//	this->DestroySwitch();

	ICC_LOG_DEBUG(m_LogPtr, "CTI Plugin Stop Succsess");
}

void CBusinessImpl::OnDestroy()
{
	if (m_bLoadPlugins)
	{
		this->DestroySwitch();
	}

	ICC_LOG_DEBUG(m_LogPtr, "CTI Plugin Destroy Succsess");
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadParams()
{
	std::string l_strTransferRingTimeout = m_ConfigPtr->GetValue("ICC/Plugin/CTI/TransferRingTimeout", "25");
	m_nTransferRingTimeout = atoi(l_strTransferRingTimeout.c_str());

	std::string l_strConferencerRingTimeout = m_ConfigPtr->GetValue("ICC/Plugin/CTI/ConferenceRingTimeout", "25");
	m_nConferenceRingTimeout = atoi(l_strConferencerRingTimeout.c_str());

	int l_nCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/ACDGrpConvers", "ACDGrpConver");
	for (int i = 0; i < l_nCount; i++)
	{
		std::string l_strPath = m_StringUtilPtr->Format("ICC/Plugin/CTI/ACDGrpConvers/ACDGrpConver[%d]", i);

		std::string l_strACD = m_ConfigPtr->GetValue(l_strPath + "/ACD", "");
		std::string l_strAlias = m_ConfigPtr->GetValue(l_strPath + "/Alias", "");
		std::string l_strDept = m_ConfigPtr->GetValue(l_strPath + "/Dept", "");

		CACDAlias l_objACDGrp;
		l_objACDGrp.m_strACD = l_strACD;
		l_objACDGrp.m_strAlias = l_strAlias;
		l_objACDGrp.m_strDept = l_strDept;
		m_mapACDGrpConversion[l_strACD] = l_objACDGrp;
	}
}
void CBusinessImpl::LoadBlackList()
{
	CHytProtocol::Instance()->LoadAllBlackList();
}

void CBusinessImpl::InitSwitch()
{
	if (m_SwitchClientPtr)
	{
		m_SwitchClientPtr->InitSwitch(boost::make_shared<CSwitchEventCallback>(this));
	}
}
void CBusinessImpl::StartSwitch()
{
	if (m_SwitchClientPtr)
	{
		m_SwitchClientPtr->StartSwitch();
	}
}
void CBusinessImpl::StopSwitch()
{
	if (m_SwitchClientPtr)
	{
		m_SwitchClientPtr->StopSwitch();
	}
}
void CBusinessImpl::DestroySwitch()
{
	if (m_SwitchClientPtr)
	{
		m_SwitchClientPtr->DestroySwitch();
	}
}

void CBusinessImpl::StartService()
{
	if (!m_bStartService)
	{
		m_pRedisClient->Del("WaitingCallList");

		m_bStartService = true;

		this->StartSwitch();

		this->LoadBlackList();
		CBlackListManager::Instance()->StartThread();

		CTransferCallManager::Instance()->StartThread();
		CConferenceManager::Instance()->StartThread();
	}
}

void CBusinessImpl::StopServeice()
{
	if (m_bStartService)
	{
		m_bStartService = false;

		CConferenceManager::Instance()->StopThread();
		CTransferCallManager::Instance()->StopThread();
		CBlackListManager::Instance()->StopThread();

		this->StopSwitch();
	}
}


//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::GetACDGrpConver(std::string& p_strDestAlias, std::string& p_strDestDept, const std::string& p_strSrcACDGrp)
{
	bool l_bFind = false;
	std::string strDeptCode;
	ICC_LOG_DEBUG(m_LogPtr, "HGet ACD INFO success,ACD:[%s]!!!", p_strSrcACDGrp.c_str());
	if (m_pRedisClient->HGet(ACD_INFO, p_strSrcACDGrp, strDeptCode))
	{
		PROTOCOL::CAcdInfo l_AcdInfo;
		if (!l_AcdInfo.Parse(strDeptCode, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Parse ACD INFO failed!!!");
		}
		ICC_LOG_DEBUG(m_LogPtr, "HGet ACD INFO success, ACD:[%s],DeptCode:[%s]!!!", strDeptCode.c_str(), l_AcdInfo.m_dept_code.c_str());
		p_strDestDept = l_AcdInfo.m_dept_code;
	}
	auto it = m_mapACDGrpConversion.find(p_strSrcACDGrp);
	if (it != m_mapACDGrpConversion.end())
	{
		l_bFind = true;
		p_strDestAlias = it->second.m_strAlias;
		// p_strDestDept = it->second.m_strDept;
	}

	return l_bFind;
}

void CBusinessImpl::Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->Event_CTIConnState(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->Event_AgentState(p_pSwitchNotif);
		//CHytProtocol::Instance()->Sync_AgentState(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_CallState(ICallStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		//	ACD 组名称转换，如 11001 -> 110
		/*std::string l_strACDAlias = "";
		std::string l_strDept = "";
		std::string l_strACDGrp = p_pSwitchNotif->GetACDGrp();
		if (this->GetACDGrpConver(l_strACDAlias, l_strDept, l_strACDGrp))
		{
			p_pSwitchNotif->SetACDGrp(l_strACDAlias);
			p_pSwitchNotif->SetACDAlias(l_strACDGrp);
			p_pSwitchNotif->SetACDDept(l_strDept);
		}*/

		_AssignAcdToCallNotify(p_pSwitchNotif);


		CHytProtocol::Instance()->Event_CallState(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_CallOver(ICallOverNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		std::string l_strOriginalCTICallRefId = "";
		std::string l_strConferenceCompere = "";
		std::string l_strConferenceId = CConferenceManager::Instance()->FindConferenceIdByCallId(l_strCTICallRefId, l_strConferenceCompere, l_strOriginalCTICallRefId);

		bool l_bConsultationCallOver = false;
		if (!l_strConferenceId.empty() && l_strCTICallRefId != l_strOriginalCTICallRefId)
		{
			//会议咨询结束
			l_bConsultationCallOver = true;
		}

		if (!l_bConsultationCallOver)
		{
			CConferenceManager::Instance()->DeleteConference(l_strConferenceId,l_strCTICallRefId);
			_AssignAcdToCallNotify(p_pSwitchNotif);

			CHytProtocol::Instance()->Event_CallOver(p_pSwitchNotif);
		}
		else
		{
			ICC_LOG_DEBUG(m_LogPtr, "conference consultation call over:CTICallRefId:[%s],conferenceId:[%s],OriginalCTICallRefId:[%s]",
				l_strCTICallRefId.c_str(), l_strConferenceId.c_str(), l_strOriginalCTICallRefId.c_str());
		}
	}
}
void CBusinessImpl::Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strDeviceNum = p_pSwitchNotif->GetDeviceNum();
	//	std::string l_strRelateDeviceNum = p_pSwitchNotif->GetRelateDeviceNum();
		std::string l_strCallerId = p_pSwitchNotif->GetCallerId();
		std::string l_strCalledId = p_pSwitchNotif->GetCalledId();
		std::string l_strOriginalCallerId = p_pSwitchNotif->GetOriginalCallerId();
		std::string l_strOriginalCalledId = p_pSwitchNotif->GetOriginalCalledId();
		std::string l_strDeviceState = p_pSwitchNotif->GetDeviceState();
		std::string l_strStateTime = p_pSwitchNotif->GetStateTime();
		std::string l_strCallDirection = p_pSwitchNotif->GetCallDirection();
		long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		std::string l_strAcdGrp = p_pSwitchNotif->GetACDGrp();
		std::string l_strOriginalCalled = p_pSwitchNotif->GetOriginalCalledId();
		if (!l_strAcdGrp.empty())
		{
			if (m_strEnableSkillMerge == "1")
			{
				std::string l_strACDMerge;
				if (GetMergeHuntGrpConver(l_strACDMerge, l_strOriginalCalled))
				{
					p_pSwitchNotif->SetACDGrp(l_strACDMerge);
					ICC_LOG_DEBUG(m_LogPtr, "MergeHuntGrpConver,OriginalCalled:[%s],AcdGrp:[%s],ACDMergeNum:[%s]", l_strOriginalCalled.c_str(), l_strAcdGrp.c_str(), l_strACDMerge.c_str());
				}
			}
		}

		std::string l_strOriginalCTICallRefId = "";
		std::string l_strConferenceCompere = "";
		std::string l_strConferenceId = CConferenceManager::Instance()->FindConferenceIdByCallId(l_strCTICallRefId, l_strConferenceCompere, l_strOriginalCTICallRefId);
		bool bBargeinCreateFlag = false;
		std::string l_strConfBargeinSponsor = "";
		if (!l_strConferenceId.empty())
		{
			CConferenceManager::Instance()->ProcessDeviceStateNotif(l_strDeviceNum, l_strCalledId, l_strDeviceState, l_strCTICallRefId, l_strConferenceId);

			//	拨号、振铃为新话务 ID ，需在此修改为原话务 ID 后再发给客户端
			p_pSwitchNotif->SetCTICallRefId(l_strOriginalCTICallRefId);

			ICC_LOG_DEBUG(m_LogPtr, "Conference device event,ConferenceID=%s,DeviceNum=%s,CTICallRefId=%s,OriginalCTICallRefId:%s,deviceState=%s,bBargeinCreate=%d",
				l_strConferenceId.c_str(),
				l_strDeviceNum.c_str(),
				l_strCTICallRefId.c_str(),
				l_strOriginalCTICallRefId.c_str(),
				l_strDeviceState.c_str(), bBargeinCreateFlag);
		}

		if (CTransferCallManager::Instance()->FindTransferCallByActiveCallRefId(l_strCTICallRefId) ||
			CTransferCallManager::Instance()->FindTransferCallByHeldCallRefId(l_strCTICallRefId))
		{
			if (l_strDeviceState.compare(CallStateString[STATE_TALK]) == 0 && !l_strOriginalCallerId.empty())
			{
				//	转移，修改为原始主叫
				p_pSwitchNotif->SetCallerId(l_strOriginalCallerId);
			}
			CTransferCallManager::Instance()->DeviceStateNotif(l_strDeviceNum, l_strCTICallRefId, l_strDeviceState, l_strStateTime);
		}

		CHytProtocol::Instance()->Event_DeviceState(p_pSwitchNotif, l_strConferenceId, l_strConferenceCompere, l_strConfBargeinSponsor);
	}
}

void CBusinessImpl::Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strDeviceNum = p_pSwitchNotif->GetDeviceNum();
		std::string l_strDeviceState = p_pSwitchNotif->GetDeviceState();
		std::string l_strStateTime = p_pSwitchNotif->GetStateTime();
		long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		std::string l_strOriginalCTICallRefId = "";
		std::string l_strConferenceCompere = "";
		std::string  l_strConferenceId = CConferenceManager::Instance()->FindConferenceIdByCallId(l_strCTICallRefId, l_strConferenceCompere, l_strOriginalCTICallRefId);
		if (!l_strConferenceId.empty())
		{
			CConferenceManager::Instance()->ProcessConferenceHangup(l_strDeviceNum, l_strCTICallRefId, l_strConferenceId);
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "Not Find CTICallRefId l_strCTICallRefId:[%s],l_strDeviceNum:%s", l_strCTICallRefId.c_str(), l_strDeviceNum.c_str());
		}
	}
}
void CBusinessImpl::Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->Event_RefuseCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_Failed(IFailedEventNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strDeviceNum = p_pSwitchNotif->GetFailedDeviceNum();
		std::string l_strCalledId = p_pSwitchNotif->GetCalledId();
		long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();;

		ICC_LOG_DEBUG(m_LogPtr, "event failed,CTICallRefId:[%s],Device:[%s],calledId:[%s],lCSTACallRefId:[%d]", 
			l_strCTICallRefId.c_str(), l_strDeviceNum.c_str(), l_strCalledId.c_str(), l_lCSTACallRefId);

		//	呼叫失败，会议、转移发起的咨询可能还没返回结果，因此不能通过 CTICallRefId 来判断是否属于会议
		CConferenceManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCalledId, l_strCTICallRefId);
		CTransferCallManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCalledId, l_strCTICallRefId);

		CHytProtocol::Instance()->Event_Failed(p_pSwitchNotif);
	}
}

void CBusinessImpl::Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->Event_BlackTransfer(p_pSwitchNotif);
	}
}

//////////////////////////////////////////////////////////////////////////
// 具体命令执行结果反馈( CR-- Command Result)
void CBusinessImpl::CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_AgentLogin(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_AgentLogout(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_SetAgentState(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		bool l_bResult = p_pSwitchNotif->GetResult();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		CHytProtocol::Instance()->CR_MakeCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		bool l_bResult = p_pSwitchNotif->GetResult();


		CHytProtocol::Instance()->CR_AnswerCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_RefuseAnswer(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_Hangup(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_ClearCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_ListenCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_PickupCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_BargeInCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_ForcePopCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_ConsultationCall(IConsultationCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		bool l_bResult = p_pSwitchNotif->GetResult();
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		int l_nErrorCode = p_pSwitchNotif->GetErrorCode();
		std::string l_strResult = l_bResult ? Result_Success : Result_Failed;
		std::string l_strSponsor = p_pSwitchNotif->GetSponsor();
		std::string l_strTargetDevice = p_pSwitchNotif->GetTargetDevice();
		std::string l_strActiveCTICallRefId = p_pSwitchNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = p_pSwitchNotif->GetHeldCTICallRefId();
		std::string l_strOriginalCallerId = p_pSwitchNotif->GetOriginalCallerId();
		std::string l_strOriginalCalledId = p_pSwitchNotif->GetOriginalCalledId();


		ICC_LOG_DEBUG(m_LogPtr, "consultationCall result!!! result:%d,sponsor:%s,targetDevice:%s,ActiveCTICallRefId:%s,HeldCTICallRefId:%s",
			l_bResult, l_strSponsor.c_str(), l_strTargetDevice.c_str(), l_strActiveCTICallRefId.c_str(), l_strHeldCTICallRefId.c_str());

		if (CTransferCallManager::Instance()->FindTransferCallByConsultationCallTaskId(l_lRequestId))
		{
			//	转警
			CTransferCallManager::Instance()->ProcessConsultationCallResult(l_lRequestId, l_strHeldCTICallRefId, l_strActiveCTICallRefId, l_bResult, l_nErrorCode);
		}
		else if (CConferenceManager::Instance()->FindConferenceByConsultationCallTaskId(l_lRequestId))
		{
			// 会议,只保留一个话务
			CConferenceManager::Instance()->ProcessConsultationCallResult(l_lRequestId, l_strHeldCTICallRefId, l_strActiveCTICallRefId, l_strTargetDevice, l_bResult);
		}
		else
		{
			// 普通咨询
			CHytProtocol::Instance()->CR_ConsultationCall(p_pSwitchNotif);
		}
	}
}
void CBusinessImpl::CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		bool l_bResult = p_pSwitchNotif->GetResult();
		std::string l_strSponsor = p_pSwitchNotif->GetSponsor();
		std::string l_strTargrt = p_pSwitchNotif->GetTargetDevice();
		std::string l_strActiveCallRefId = p_pSwitchNotif->GetActiveCTICallRefId();
		std::string l_strHeldCallRefId = p_pSwitchNotif->GetHeldCTICallRefId();

		if (CTransferCallManager::Instance()->FindTransferCallByTransferCallTaskId(l_lRequestId))
		{
			// CTI 实现咨询及转移操作
			CTransferCallManager::Instance()->ProcessTransferCallResult(l_lRequestId, l_strHeldCallRefId, l_strActiveCallRefId, l_bResult);
		}
		else
		{
			CHytProtocol::Instance()->CR_TransferCall(p_pSwitchNotif);
		}
	}
}
void CBusinessImpl::CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_DeflectCall(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_HoldCall(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_RetrieveCall(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_ReconnectCall(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		bool l_bResult = p_pSwitchNotif->GetResult();
		long l_lRequestId = p_pSwitchNotif->GetRequestId();

		if (CConferenceManager::Instance()->FindConferenceByConferenceCallTaskId(l_lRequestId))
		{
			CConferenceManager::Instance()->ProcessConferenceCallResult(l_lRequestId, l_bResult);
		}
		else
		{
			CHytProtocol::Instance()->CR_ConferenceCall(p_pSwitchNotif);
		}
	}
}
void CBusinessImpl::CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_TakeOverCall(p_pSwitchNotif);
	}
}

/*
void CBusinessImpl::CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif)
{
	//CHytProtocol::Instance()->CR_AddConferenceParty(p_pSwitchNotif);
}

void CBusinessImpl::CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		ICC_LOG_LOWDEBUG(m_LogPtr, "Receive DeleteConferenceParty Result, RequestId: [%u]", p_pSwitchNotif->GetRequestId());

		CHytProtocol::Instance()->CR_DeleteConferenceParty(p_pSwitchNotif);
	}
}*/

void CBusinessImpl::CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_GetCTIConnState(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		auto iter = p_pSwitchNotif->m_DeviceList.begin();
		while (iter != p_pSwitchNotif->m_DeviceList.end())
		{
			boost::shared_ptr<CDeviceStateNotif> l_pNotifObj = *iter;
			if (l_pNotifObj)
			{
				std::string l_strAcdGrp = l_pNotifObj->GetACDGrp();
				std::string l_strOriginalCalled = l_pNotifObj->GetOriginalCalledId();
				if (!l_strAcdGrp.empty())
				{
					if (m_strEnableSkillMerge == "1")
					{
						std::string l_strACDMerge;
						if (GetMergeHuntGrpConver(l_strACDMerge, l_strOriginalCalled))
						{
							l_pNotifObj->SetACDGrp(l_strACDMerge);
							ICC_LOG_DEBUG(m_LogPtr, "MergeHuntGrpConver,OriginalCalled:[%s],AcdGrp:[%s],ACDMergeNum:[%s]", l_strOriginalCalled.c_str(), l_strAcdGrp.c_str(), l_strACDMerge.c_str());
						}
					}
				}
				
			}
			iter++;
		}
		CHytProtocol::Instance()->CR_GetDeviceList(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_GetACDList(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_GetAgentList(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_GetFreeAgentList(p_pSwitchNotif);
	}
}

void CBusinessImpl::CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		CHytProtocol::Instance()->CR_GetCallList(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lTaskId = p_pSwitchNotif->GetRequestId();
		if (CTransferCallManager::Instance()->FindTransferCallByGetReadyAgentTaskId(l_lTaskId))
		{
			bool l_bResult = p_pSwitchNotif->GetResult();
			std::string l_strReadyAgent = p_pSwitchNotif->GetReadyAgent();
			CTransferCallManager::Instance()->ProcessGetReadyAgentResult(l_lTaskId, l_strReadyAgent, l_bResult);
		}
		else
		{
			CHytProtocol::Instance()->CR_GetReadyAgent(p_pSwitchNotif);
		}
	}
}


void CBusinessImpl::CR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_pSwitchNotif)
{
	ICC_LOG_DEBUG(m_LogPtr, "CR_FSAesEventEx cmd: %s", p_strCmdName.c_str());
	IFSAesExEventNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CFSAesExEventNotif>(p_pSwitchNotif);
	if (l_pResultNotif)
	{
		if (p_strCmdName == "get_history_callList")
		{
			CHytProtocol::Instance()->CR_GetHistoryCallList(l_pResultNotif);
		}
		else if (p_strCmdName == "conference_sync")
		{

			CHytProtocol::Instance()->Sync_MakeConference(l_pResultNotif->GetEventData());
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	/*{
		"header": {
		"cmd": "sync_server_lock"
		},
		"body" : {
		"service_id": "ICC.Server.CTI"
		"service_state": "master"
		}
		}*/

	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_ERROR(m_LogPtr, "Invalid BargeInCall request protocol: %s", l_strRequestMsg.c_str());

			return;
		}
		
		if (l_oSyncObj.m_oBody.m_strServiceID != m_strServiceID)
		{
			return;
		}

		ICC_LOG_DEBUG(m_LogPtr, "Server Role Sync,Config ServerFalg:%s, clientid:%s", m_ConfigPtr->ServerFlag().c_str(), l_oSyncObj.m_oBody.m_strClientID.c_str());
		std::string l_strServiceState = l_oSyncObj.m_oBody.m_strServiceState;
		if (l_strServiceState.compare(DoubleServerRole_Active) == 0)
		{
			if (l_oSyncObj.m_oBody.m_strClientID == m_ConfigPtr->ServerFlag())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Server Role Sync: loacal Active, StartService,ServerFlag:%s", m_ConfigPtr->ServerFlag().c_str());
				this->StartService();
			}
			else
			{
				ICC_LOG_DEBUG(m_LogPtr, "Server Role Sync: remote Active, StopService,ServerFlag:%s", m_ConfigPtr->ServerFlag().c_str());
				this->StopServeice();
			}

		}
		else
		{
			if (l_oSyncObj.m_oBody.m_strClientID == m_ConfigPtr->ServerFlag())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Server Role Sync: loacal Standby, StopService,ServerFlag:%s", m_ConfigPtr->ServerFlag().c_str());
				this->StopServeice();
			}

		}
	}
}

void CBusinessImpl::OnNotifiClientRegisterSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ClientRegister Sync: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ClientRegisterSync(l_strRequestMsg);
	}
}

void CBusinessImpl::OnNotifiLoadAllBlackListRespond(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRespondMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive LoadAllBlackList Respond: %s", l_strRespondMsg.c_str());

		PROTOCOL::CLoadAllBlackListRespond l_oGetAllBlackListRespond;
		if (!l_oGetAllBlackListRespond.ParseString(l_strRespondMsg, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_ERROR(m_LogPtr, "Invalid LoadAllBlackList respond protocol: [%s]", l_strRespondMsg.c_str());

			return;
		}

		std::map<std::string, boost::shared_ptr<CLimitNum>>	l_mapBlackList;
		std::string l_strCount = l_oGetAllBlackListRespond.m_oBody.m_strCount;

		int l_nCount = atoi(l_strCount.c_str());
		for (int i = 0; i < l_nCount; ++i)
		{
			boost::shared_ptr<CLimitNum> l_pLimitNumObj = boost::make_shared<CLimitNum>();

			std::string l_strLimitNum = l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitNum;
			l_pLimitNumObj->SetLimitNum(l_strLimitNum);
			l_pLimitNumObj->SetLimitMin(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitMin);
			l_pLimitNumObj->SetLimitReason(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitReason);
			l_pLimitNumObj->SetStaffCode(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strStaffCode);
			l_pLimitNumObj->SetStaffName(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strStaffName);
			l_pLimitNumObj->SetBeginTime(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strBeginTime);
			l_pLimitNumObj->SetEndTime(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strEndTime);
			l_pLimitNumObj->SetApprover(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strApprover);
			l_pLimitNumObj->SetApprovedInfo(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strApprovedInfo);
			l_pLimitNumObj->SetReceiptDeptName(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strReceiptDeptName);
			l_pLimitNumObj->SetReceiptDeptDistrictCode(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strReceiptDeptDistrictCode);

			l_mapBlackList[l_strLimitNum] = l_pLimitNumObj;

			ICC_LOG_DEBUG(m_LogPtr, "Load blacklist limit_num: [%s], limit_min: [%s], begin_time: [%s], end_time: [%s]",
				l_pLimitNumObj->GetLimitNum().c_str(),
				l_pLimitNumObj->GetLimitMin().c_str(),
				l_pLimitNumObj->GetBeginTime().c_str(),
				l_pLimitNumObj->GetEndTime().c_str());
		}

		CBlackListManager::Instance()->InitBlackList(l_mapBlackList);
		CBlackListManager::Instance()->WriteBlackListToSwitch();
	}
}

void CBusinessImpl::OnNotifiLoadAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRespondMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive LoadAllBlackList Sync: %s", l_strRespondMsg.c_str());

		PROTOCOL::CGetAllBlackListRespond l_oGetAllBlackListRespond;
		if (!l_oGetAllBlackListRespond.ParseString(l_strRespondMsg, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_ERROR(m_LogPtr, "Invalid LoadAllBlackList sync protocol: [%s]", l_strRespondMsg.c_str());

			return;
		}

		std::map<std::string, boost::shared_ptr<CLimitNum>>	l_mapBlackList;
		std::string l_strCount = l_oGetAllBlackListRespond.m_oBody.m_strCount;

		int l_nCount = atoi(l_strCount.c_str());
		for (int i = 0; i < l_nCount; ++i)
		{
			boost::shared_ptr<CLimitNum> l_pLimitNumObj = boost::make_shared<CLimitNum>();

			std::string l_strLimitNum = l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitNum;
			l_pLimitNumObj->SetLimitNum(l_strLimitNum);
			l_pLimitNumObj->SetLimitMin(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitMin);
			l_pLimitNumObj->SetLimitReason(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strLimitReason);
			l_pLimitNumObj->SetStaffCode(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strStaffCode);
			l_pLimitNumObj->SetStaffName(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strStaffName);
			l_pLimitNumObj->SetBeginTime(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strBeginTime);
			l_pLimitNumObj->SetEndTime(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strEndTime);
			l_pLimitNumObj->SetApprover(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strApprover);
			l_pLimitNumObj->SetApprovedInfo(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strApprovedInfo);
			l_pLimitNumObj->SetReceiptDeptName(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strReceiptDeptName);
			l_pLimitNumObj->SetReceiptDeptDistrictCode(l_oGetAllBlackListRespond.m_oBody.m_vecData[i].m_strReceiptDeptDistrictCode);

			l_mapBlackList[l_strLimitNum] = l_pLimitNumObj;

			ICC_LOG_DEBUG(m_LogPtr, "Sync blacklist limit_num: [%s], limit_min: [%s], begin_time: [%s], end_time: [%s]",
				l_pLimitNumObj->GetLimitNum().c_str(),
				l_pLimitNumObj->GetLimitMin().c_str(),
				l_pLimitNumObj->GetBeginTime().c_str(),
				l_pLimitNumObj->GetEndTime().c_str());
		}

		CBlackListManager::Instance()->InitBlackList(l_mapBlackList);
		CBlackListManager::Instance()->WriteBlackListToSwitch();
	}
}

void CBusinessImpl::OnNotifiAgentLoginRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive AgentLogin Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentLogin(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiAgentLogoutRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive AgentLogout Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentLogout(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiSetAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive SetAgentState Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_SetAgentState(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiMakeCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive MakeCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_MakeCall(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiHangupRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive Hangup Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_Hangup(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiAnswerCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive AnswerCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AnswerCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiRefuseAnswerRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive RefuseAnswer Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RefuseAnswer(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiClearCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ClearCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ClearCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiListenCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ListenCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ListenCall(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiPickupCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive PickupCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_PickupCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiBargeInCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive BargeInCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_BargeInCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiForcePopCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ForcePopCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ForcePopCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiConsultationCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ConsultationCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ConsultationCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiTransferCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive TransferCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_TransferCall(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive TransferCallEx Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_TransferCallEx(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiCancelTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive CancelTransferCallEx Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_CancelTransferCallEx(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiDeflectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive DeflectCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeflectCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiHoldCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive HoldCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_HoldCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiRetrieveCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive RetrieveCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RetrieveCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiReconnectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive ReconnectCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ReconnectCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnMakeConferenceRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive MakeConference Request:%s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_MakeConference(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnMakeConferenceExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive MakeConferenceEx Request:%s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_MakeConferenceEx(l_strRequestMsg, p_pNotifiRequest);
	}
}


void CBusinessImpl::OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive AddConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AddConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryConferenceByCallid(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}


void CBusinessImpl::OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive DeleteConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive DisenableConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DisenableConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnTakeOverCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive TakeOverCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_TakeOverCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCTIConnStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetCTIConnState Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCTIConnState(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetDeviceListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetDeviceList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetDeviceList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetACDListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetACDList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetACDList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetFreeAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetAgentList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetFreeAgentList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetAgentListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetAgentList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetAgentList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetCallList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCallList(l_strRequestMsg, p_pNotifiRequest);
	}
}

/*
void CBusinessImpl::OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive QueryCallEvent Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryCallEvent(l_strRequestMsg, p_pNotifiRequest);
	}
}*/
void CBusinessImpl::OnNotifiGetReadyAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetReadyAgent Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetReadyAgent(l_strRequestMsg, p_pNotifiRequest);
	}
}

/*
void CBusinessImpl::OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive QueryBlackCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryBlackCall(l_strRequestMsg, p_pNotifiRequest);
	}
}*/

void CBusinessImpl::OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive SetBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_SetBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive DeleteBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive DeleteAllBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteAllBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetAllBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetAllBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		//ICC_LOG_DEBUG(m_LogPtr, "Receive GetCurrentTime Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCurrentTime(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetMinOnlineAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRespondMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive GetMinOnlineAgentRequest: %s", l_strRespondMsg.c_str());

		PROTOCOL::CGetMinOnlineAgentRespond l_oGetMinOnlineAgentRespond;

		l_oGetMinOnlineAgentRespond.m_oBody.m_strMinOnlineNum = m_strMinOnlineAgentNum;
		std::string l_strMessage = l_oGetMinOnlineAgentRespond.ToString(m_JsonFactoryPtr->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_LogPtr, "Send MinOnlineAgent Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}

	}
}

void CBusinessImpl::OnAgentHeartBeatTimeoutSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive Agent heartbeat Timeout Sync: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentHeartBeatTimeout(l_strRequestMsg, p_pNotifiRequest);
	}
}
/*
void CBusinessImpl::OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_LogPtr, "Receive RemoveReleaseCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RemoveReleaseCall(l_strRequestMsg, p_pNotifiRequest);
	}
}*/

/*void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
std::string l_strLimitNum;
if (CBlackListManager::Instance()->BlackListTimeout(l_strLimitNum))
{
CBlackListManager::Instance()->DeleteBlackList(l_strLimitNum);

CDBProcess::Instance()->DeleteBlackList(l_strLimitNum);

if (m_SwitchClientPtr)
{
m_SwitchClientPtr->DeleteBlackList(l_strLimitNum);
}

CHytProtocol::Instance()->Sync_DeleteBlackList(l_strLimitNum);

ICC_LOG_DEBUG(m_LogPtr, "Remove BlackList: %s. Reason: BlackListTimeout", l_strLimitNum.c_str());
}
}*/


void CBusinessImpl::_LoadMergeHuntGrpConver()
{
	m_strEnableSkillMerge = m_ConfigPtr->GetValue("ICC/Plugin/CTI/EnableSkillMerge", "1");

	int l_nCount = m_ConfigPtr->GetNodeCount("ICC/Plugin/CTI/GrpMergeConvers", "GrpMergeConver");
	for (int i = 0; i < l_nCount; i++)
	{
		std::string l_strPath = m_StringUtilPtr->Format("ICC/Plugin/CTI/GrpMergeConvers/GrpMergeConver[%d]", i);

		std::string l_strNumber = m_ConfigPtr->GetValue(l_strPath + "/Number", "");
		std::string l_strConverACD = m_ConfigPtr->GetValue(l_strPath + "/ConverACD", "");
		if (!l_strConverACD.empty() && !l_strNumber.empty())
		{
			ICC_LOG_DEBUG(m_LogPtr, "Number:[%s]-->ConverACD:[%s]", l_strNumber.c_str(), l_strConverACD.c_str());
			m_mapMergeHuntGrpConvers[l_strNumber] = l_strConverACD;
		}
	}
}

bool CBusinessImpl::GetMergeHuntGrpConver(std::string& p_strDestAcd, const std::string& p_strSrcNum)
{
	bool l_bFind = false;
	auto it = m_mapMergeHuntGrpConvers.find(p_strSrcNum);
	if (it != m_mapMergeHuntGrpConvers.end())
	{
		l_bFind = true;
		p_strDestAcd = it->second;
	}
	return l_bFind;
}

void CBusinessImpl::_AssignAcdToCallNotify(ICallStateNotifPtr p_pSwitchNotif)
{
	std::string l_strACDAlias = "";
	std::string l_strDept = "";
	std::string l_strACDGrp = p_pSwitchNotif->GetACDGrp();
	std::string l_strOrgCalled = p_pSwitchNotif->GetOriginalCalledId();
	std::string l_strCallredId = p_pSwitchNotif->GetCTICallRefId();

	ICC_LOG_DEBUG(m_LogPtr, " call id: %s, skill merage flag = %s, org acd : %s, org called : %s", l_strCallredId.c_str(),
		m_strEnableSkillMerge.c_str(), l_strACDGrp.c_str(), l_strOrgCalled.c_str());

	if (l_strACDGrp.empty())
	{
		ICC_LOG_DEBUG(m_LogPtr, " acd is empty, not need set acd : call id :%s ", l_strCallredId.c_str());
		return;
	}

	if (m_strEnableSkillMerge == "1")
	{
		if (GetMergeHuntGrpConver(l_strACDAlias, l_strOrgCalled))
		{
			p_pSwitchNotif->SetACDGrp(l_strACDAlias);
		}

		p_pSwitchNotif->SetACDAlias(l_strACDGrp);

		std::string strTmpAcdAlias;
		if (this->GetACDGrpConver(strTmpAcdAlias, l_strDept, l_strACDGrp))
		{
			p_pSwitchNotif->SetACDDept(l_strDept);
		}
	}
	else
	{
		if (this->GetACDGrpConver(l_strACDAlias, l_strDept, l_strACDGrp))
		{
			p_pSwitchNotif->SetACDGrp(l_strACDAlias);
			p_pSwitchNotif->SetACDAlias(l_strACDGrp);
			p_pSwitchNotif->SetACDDept(l_strDept);
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, " call %s convert result, org acd : %s, org called : %s, acd alias = %s, ", l_strCallredId.c_str(),
		l_strACDGrp.c_str(), l_strOrgCalled.c_str(), l_strACDAlias.c_str());

}

void CBusinessImpl::OnGetHistoryCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_LogPtr, "Receive Get History CallList Request: %s", l_strRequestMsg.c_str());

	CHytProtocol::Instance()->CC_GetHistoryCallList(l_strRequestMsg, p_pNotifiRequest);
}

void CBusinessImpl::OnReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_LogPtr, "Receive RecordFileUp Request: %s", l_strRequestMsg.c_str());

	CHytProtocol::Instance()->CC_RecordFileUp(l_strRequestMsg, p_pNotifiRequest);

}

void CBusinessImpl::OnReceiveCallOverRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_LogPtr, "Receive CallOver Request: %s", l_strRequestMsg.c_str());

	CHytProtocol::Instance()->CC_RecordFileUp(l_strRequestMsg, p_pNotifiRequest);

}


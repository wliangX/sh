#include "Boost.h"

#include "BusinessImpl.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include "AgentManager.h"
#include "DeviceManager.h"
#include "CallManager.h"
#include "AvayaSwitchManager.h"
#include "BlackListManager.h"
#include "ConferenceManager.h"
#include "TransferCallManager.h"
#include "HytProtocol.h"

#include "AvayaEvent.h"



bool g_bCTITestStart = false;
bool g_bCTITestStartOkFlag = false;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCTITestUdpHelper::CCTITestUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool, Udp::IUdpPtr pUdpServer, CBusinessImpl *pBusiness)
{
	m_pHelpTool = pHelpTool;
	m_pLog = pLogPtr;
	m_pUdp = pUdpServer;
	m_pBusiness = pBusiness;

	m_nIntCallRefId = 50;
}

CCTITestUdpHelper::~CCTITestUdpHelper()
{

}

void CCTITestUdpHelper::SetTestCallInfo(const string& strCaller, const string& strCalled, const string & strAgentNo,long nCallRefId)
{
	CTestCallInfo tmpCallInfo;
	tmpCallInfo.m_lCSTACallRefId = nCallRefId;
	tmpCallInfo.m_strCaller = strCaller;
	tmpCallInfo.m_strCalled = strCalled;
	tmpCallInfo.m_strAgentNo = strAgentNo;
	m_mapCallInfo[strCaller] = tmpCallInfo;
}

bool CCTITestUdpHelper::GetTestCallInfoByRefId(long nCallRefId, CTestCallInfo &pCallInfo)
{
	for each (auto l_pCallObj in m_mapCallInfo)
	{
		if (l_pCallObj.second.m_lCSTACallRefId == nCallRefId)
		{
			pCallInfo.m_lCSTACallRefId = nCallRefId;
			pCallInfo.m_strCalled = l_pCallObj.second.m_strCalled;
			pCallInfo.m_strCaller = l_pCallObj.second.m_strCaller;
			return true;
		}
	}
	return false;
}

bool CCTITestUdpHelper::GetTestCallInfoByCaller(const string& strCaller, CTestCallInfo& pCallInfo)
{
	std::map<std::string, CTestCallInfo>::iterator it;
	it = m_mapCallInfo.find(strCaller);
	if (it != m_mapCallInfo.end())
	{
		std::string l_strCTICallRefId = "";
		if (CCallManager::Instance()->GetCallState(it->second.m_lCSTACallRefId) != CallStateString[STATE_HANGUP])
		{
			if (CCallManager::Instance()->GetCallByCSTACallRefId(it->second.m_lCSTACallRefId, l_strCTICallRefId))
			{
				pCallInfo = it->second;
				pCallInfo.m_strCTICallRefId = l_strCTICallRefId;
				return true;
			}
		}

		//挂机或者没找到
		m_mapCallInfo.erase(it);
	}
	return false;
}

bool CCTITestUdpHelper::CTITestCallOut(const string& strCaller, const string& strCalled, string& strCallRefId)
{
	bool bRes = false;
	std::string l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(m_nIntCallRefId);
	if (!l_strCTICallRefId.empty())
	{
		CAvayaEvent::Instance()->DeliveredEvent(atoi(strCaller.c_str()), 0, LINECALLSTATE_RINGBACK, m_nIntCallRefId, 0, false, strCaller, strCalled);

		CAvayaEvent::Instance()->EstablishedEvent(atoi(strCaller.c_str()),0, m_nIntCallRefId, 0, strCaller, strCalled);
		SetTestCallInfo(strCaller, strCalled, strCaller, m_nIntCallRefId);

		bRes = true;

		
	}
	m_nIntCallRefId++;

	return bRes;
	
}

void CCTITestUdpHelper::OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	char l_pBuffer[UDP_RECV_BUF_SIZE] = {0};
	memset(l_pBuffer, 0, UDP_RECV_BUF_SIZE);
	memcpy(l_pBuffer, p_pData, p_iLength);
	printf("======>Receive Test Cmd:[%s]\n", l_pBuffer);
	ICC_LOG_DEBUG(m_pLog, "======>Receive Test Cmd:[%s]", l_pBuffer);
	std::string l_strParam(l_pBuffer);
	std::vector<std::string> l_vecParam;
	m_pStringUtilPtr->Split(l_strParam, " ", l_vecParam, true);

	string strReturnRes = "";
	strReturnRes = m_pStringUtilPtr->Format("======>cmd error:[%s]", l_pBuffer);
	int nSize = l_vecParam.size();
	do
	{
		std::string l_strCMD = l_vecParam[0];
		if (l_strCMD == "starttest")
		{
			if (!g_bCTITestStart)
			{
				g_bCTITestStart = true;
				m_pBusiness->StartSwitchService();
				strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]", l_pBuffer);
				printf("======>cmd execute success:[%s]\n", l_pBuffer);
			}
			else
			{
				if (!g_bCTITestStartOkFlag)
				{
					strReturnRes = m_pStringUtilPtr->Format("======>CTI test is starting ,please wait!");
				}
				else
				{
					strReturnRes = m_pStringUtilPtr->Format("======>CTI test started!");
				}
			}
			break;
		}

		if (!g_bCTITestStart)
		{
			strReturnRes = m_pStringUtilPtr->Format("======>failed,CTI test not started!");
			break;
		}

		if (!g_bCTITestStartOkFlag)
		{
			strReturnRes = m_pStringUtilPtr->Format("======>CTI test is starting ,please wait!");
			break;
		}

		if (l_strCMD == "makecall")
		{
			if (nSize == 4)
			{
				//模拟一条呼入
				std::string strCaller = l_vecParam[1];  //主叫
				std::string strCalled = l_vecParam[2];  //被叫
				std::string strAgentNum = l_vecParam[3];
				CTestCallInfo l_CallInfo;
				if (GetTestCallInfoByCaller(strCaller, l_CallInfo))
				{
					strReturnRes = m_pStringUtilPtr->Format("======>The current caller is calling,cmd execute failed:[%s] ,RefId:%d,CallRefId:[%s]", l_pBuffer, l_CallInfo.m_lCSTACallRefId, l_CallInfo.m_strCTICallRefId.c_str());
					break;
				}

				if (CAvayaEvent::Instance()->DeliveredEvent(atoi(strAgentNum.c_str()), 0, 2, m_nIntCallRefId, 0, false, strCaller, strCalled))
				{
					strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]", l_pBuffer);
					SetTestCallInfo(strCaller, strCalled, strAgentNum, m_nIntCallRefId);
				}
				else
				{
					strReturnRes = m_pStringUtilPtr->Format("======>DeliveredEvent() return false,cmd execute failed:[%s]", l_pBuffer);
				}
				m_nIntCallRefId++;
			}

		}
		else if (l_strCMD == "hangupcall")
		{
			if (nSize == 2)
			{
				std::string strCaller = l_vecParam[1];  //主叫
				CTestCallInfo l_CallInfo;
				if (GetTestCallInfoByCaller(strCaller, l_CallInfo))
				{
					CAvayaEvent::Instance()->DeviceHangup(l_CallInfo.m_lCSTACallRefId, l_CallInfo.m_strAgentNo, l_CallInfo.m_strAgentNo);

					CAvayaEvent::Instance()->DeviceFree(l_CallInfo.m_lCSTACallRefId, l_CallInfo.m_strAgentNo);
					strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]\n", l_pBuffer);
				}
				else
				{
					strReturnRes = m_pStringUtilPtr->Format("======>not find call,cmd execute failed:[%s]", l_pBuffer);
				}
			}
			
		}
		else if (l_strCMD == "setagentstate")
		{
			if (nSize == 4)
			{
				if (l_vecParam[3] == ReadyState_Idle)
				{
					CAgentManager::Instance()->ReadyStateSync(l_vecParam[1], l_vecParam[2], "ready", "");  
				}
				else
				{
					CAgentManager::Instance()->ReadyStateSync(l_vecParam[1], l_vecParam[2], "notready", "");
				}
				strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]\n", l_pBuffer);
				
			}
		}
	} while (0);

	printf("%s\n", strReturnRes.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strReturnRes.c_str());

	m_pUdp->Send(strReturnRes, p_strRemoteIP, p_iPort);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
}

void CCTITestUdpHelper::OnStart()
{

}

void CCTITestUdpHelper::OnStop()
{

}
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
	m_bLoadPlugins = true;
	m_bStartService = false;

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);
	m_pStringUtil = ICCGetIStringFactory()->CreateString();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pTimerManager = ICCGetITimerFactory()->CreateTimerManager();
	m_pJsonFactory = ICCGetIJsonFactory();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);

	m_pDBConnPtr = ICCGetIDBConnFactory()->CreateDBConn(DataBase::DBType::PostgreSQL);

	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pUdp = ICCGetIUdpFactory()->CreateUdp();
	m_UdpHelper = boost::make_shared<CCTITestUdpHelper>(m_pLog, m_pHelpTool, m_pUdp, this);
	if (m_UdpHelper)
	{
		m_UdpHelper->SetStringUtilPtr(m_pStringUtil);
		int l_UDPPort = 44402;
		m_pUdp->Bind(l_UDPPort, m_UdpHelper);
	}

	ICC_LOG_DEBUG(m_pLog, "CTI Plugin Init Success");
}

void CBusinessImpl::OnStart()
{
	m_strServiceID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "ICC.Server.CTI");
	std::string l_strSwitchApiType = m_pConfig->GetValue("ICC/Plugin/CTI/SwitchApiType", "0");
	if (l_strSwitchApiType.compare(SWITCH_TYPE_TAPI) != 0)
	{
		m_bLoadPlugins = false;
		ICC_LOG_WARNING(m_pLog, "There is no need to load the plugin");

		return;
	}

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
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ConsultationCallExRequest, OnNotifiConsultationCallExRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TransferCallRequest, OnNotifiTransferCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TransferCallExRequest, OnNotifiTransferCallExRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CancelTransferCallExRequest, OnNotifiCancelTransferCallExRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeflectCallRequest, OnNotifiDeflectCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_HoldCallRequest, OnNotifiHoldCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RetrieveCallRequest, OnNotifiRetrieveCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ReconnectCallRequest, OnNotifiReconnectCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ConferenceCallRequest, OnConferenceCallRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_MakeConferenceRequest, OnMakeConferenceRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_AddConferencePartyRequest, OnAddConferencePartyRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_conference_party_request", OnGetConferencePartyRequest); 
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteConferencePartyRequest, OnDeleteConferencePartyRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_conference_by_callid", OnQueryConferenceByCallidRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "disenable_conference_party_request", OnDisenableConferencePartyRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCTIConnStateRequest, OnNotifiGetCTIConnStateRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetDeviceListRequest, OnNotifiGetDeviceListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetACDListRequest, OnNotifiGetACDListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAgentListRequest, OnNotifiGetAgentListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCallListRequest, OnNotifiGetCallListRequest);
//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallEventQueryRequest, OnNotifiQueryCallEventRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetReadyAgentRequest, OnNotifiGetReadyAgentRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetBlackListRequest, OnNotifiSetBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteBlackListRequest, OnNotifiDeleteBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteAllBlackListRequest, OnNotifiDeleteAllBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAllBlackListRequest, OnNotifiGetAllBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetCurrentTimeRequest, OnNotifiGetCurrentTimeRequest);
	
//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BlackCallQueryRequest, OnNotifiQueryBlackCallRequest);

//	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RemoveReleaseCallRequest, OnNotifiRemoveReleaseCallRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_InitOverTimer, OnInitOverTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BlackListManagerTimer, OnBlackListManagerTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallManagerTimer, OnCallManagerTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ConferenceManagerTimer, OnConferenceManagerTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TransferCallManagerTimer, OnTransferCAllManagerTimer);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "agent_timeout_sync", OnAgentHeartBeatTimeoutSync);

	this->SetComponentsPtr();

	CSysConfig::Instance()->LoadParamter();
	CDeviceManager::Instance()->LoadAllDevice();
	CAgentManager::Instance()->LoadAllAgent();
	CAvayaSwitchManager::Instance()->OnInit();

	std::string l_strTimer = m_pTimerManager->AddTimer(Cmd_InitOverTimer, 3, 0);
	ICC_LOG_DEBUG(m_pLog, "InitOver Timer %s Start", l_strTimer.c_str());

	l_strTimer = m_pTimerManager->AddTimer(Cmd_BlackListManagerTimer, 1, 0);
	ICC_LOG_DEBUG(m_pLog, "BlackListManager Timer %s Start", l_strTimer.c_str());

	l_strTimer = m_pTimerManager->AddTimer(Cmd_CallManagerTimer, 3, 0);
	ICC_LOG_DEBUG(m_pLog, "CallManager Timer %s Start", l_strTimer.c_str());

	l_strTimer = m_pTimerManager->AddTimer(Cmd_ConferenceManagerTimer, 1, 0);
	ICC_LOG_DEBUG(m_pLog, "ConferenceManager Timer %s Start", l_strTimer.c_str());

	l_strTimer = m_pTimerManager->AddTimer(Cmd_TransferCallManagerTimer, 1, 0);
	ICC_LOG_DEBUG(m_pLog, "TransferCallManager Timer %s Start", l_strTimer.c_str());

	ICC_LOG_DEBUG(m_pLog, "CTI Plugin Start Success");
}

void CBusinessImpl::OnStop()
{
	if (m_bLoadPlugins)
	{
		this->StopSwitchServeice();
	}

	ICC_LOG_DEBUG(m_pLog, "CTI Plugin Stop Succsess");
}

void CBusinessImpl::OnDestroy()
{
	if (m_bLoadPlugins)
	{
		CAvayaSwitchManager::Instance()->ExitInstance();
		CBlackListManager::Instance()->ExitInstance();
		CDeviceManager::Instance()->ExitInstance();
		CAgentManager::Instance()->ExitInstance();
		CSysConfig::Instance()->ExitInstance();
	}

	ICC_LOG_DEBUG(m_pLog, "CTI Plugin Destroy Succsess");
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::SetComponentsPtr()
{
	CAgentManager::Instance()->SetLogPtr(m_pLog);
	CAgentManager::Instance()->SetDateTimePtr(m_pDateTime);
	CAgentManager::Instance()->SetStringUtilPtr(m_pStringUtil);

	CAvayaSwitchManager::Instance()->SetLogPtr(m_pLog);
	CAvayaSwitchManager::Instance()->SetDateTimePtr(m_pDateTime);
	CAvayaSwitchManager::Instance()->SetStringUtilPtr(m_pStringUtil);

	CBlackListManager::Instance()->SetLogPtr(m_pLog);
	CBlackListManager::Instance()->SetDateTime(m_pDateTime);

	CCallManager::Instance()->SetLogPtr(m_pLog);
	CCallManager::Instance()->SetDateTimePtr(m_pDateTime);
	CCallManager::Instance()->SetStringUtilPtr(m_pStringUtil);

	CConferenceManager::Instance()->SetLogPtr(m_pLog);
	CConferenceManager::Instance()->SetDateTimePtr(m_pDateTime);
	CConferenceManager::Instance()->SetStringUtilPtr(m_pStringUtil);

	CDeviceManager::Instance()->SetLogPtr(m_pLog);
	CDeviceManager::Instance()->SetDateTimePtr(m_pDateTime);
	CDeviceManager::Instance()->SetStringUtilPtr(m_pStringUtil);
	CDeviceManager::Instance()->SetRedisClientPtr(m_pRedisClient);
	CDeviceManager::Instance()->SetJsonFactoryPtr(m_pJsonFactory);

	CHytProtocol::Instance()->SetObserverCenterPtr(m_pObserverCenter);
	CHytProtocol::Instance()->SetStringUtilPtr(m_pStringUtil);
	CHytProtocol::Instance()->SetJsonFactoryPtr(m_pJsonFactory);
	CHytProtocol::Instance()->SetDateTimePtr(m_pDateTime);
	CHytProtocol::Instance()->SetLogPtr(m_pLog);
	CHytProtocol::Instance()->SetRedisClientPtr(m_pRedisClient);
	CHytProtocol::Instance()->SetDBConnPtr(m_pDBConnPtr);
	

	CSysConfig::Instance()->SetLogPtr(m_pLog);
	CSysConfig::Instance()->SetConfigPtr(m_pConfig);
	CSysConfig::Instance()->SetRedisClientPtr(m_pRedisClient);
	CSysConfig::Instance()->SetStringUtilPtr(m_pStringUtil);

	CTaskManager::Instance()->SetLogPtr(m_pLog);

	CTransferCallManager::Instance()->SetLogPtr(m_pLog);
	CTransferCallManager::Instance()->SetDateTimePtr(m_pDateTime);
}
void CBusinessImpl::LoadBlackList()
{
	CHytProtocol::Instance()->LoadAllBlackList();
}

void CBusinessImpl::StartSwitchService()
{
	if (!m_bStartService)
	{
		m_bStartService = true;

		this->LoadBlackList();

		CAvayaSwitchManager::Instance()->OnStart();
	}
}
void CBusinessImpl::StopSwitchServeice()
{
	if (m_bStartService)
	{
		m_bStartService = false;

		CAvayaSwitchManager::Instance()->OnStop();

		CTransferCallManager::Instance()->ClearTransferCallList();
		CConferenceManager::Instance()->ClearConferenceList();
		CBlackListManager::Instance()->ClearBlackList();
		CCallManager::Instance()->ClearCallList();
		CTaskManager::Instance()->ClearCmdTaskQueue();
		CTaskManager::Instance()->ClearExcutedTaskQueue();
		CTaskManager::Instance()->ClearSwitchEventTaskQueue();;
		CAgentManager::Instance()->SetInitOver(false);
	}
}

//////////////////////////////////////////////////////////////////////////
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
		CHytProtocol::Instance()->Sync_AgentState(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_CallState(ICallStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		//	ACD 组名称转换，如 11001 -> 110
		std::string l_strACDGrpConver = "";
		if (CSysConfig::Instance()->GetACDGrpConver(l_strACDGrpConver, p_pSwitchNotif->GetACDGrp()))
		{
			p_pSwitchNotif->SetACDGrp(l_strACDGrpConver);
		}

		CHytProtocol::Instance()->Event_CallState(p_pSwitchNotif);
	}
}
void CBusinessImpl::Event_CallOverEx(ICallOverNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();
		if (!CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			//	会议成员咨询失败，不保存话务
			CConferenceManager::Instance()->DeleteConference(l_strCTICallRefId);

			//	ACD 组名称转换，如 11001 -> 110
			std::string l_strACDGrpConver = "";
			if (CSysConfig::Instance()->GetACDGrpConver(l_strACDGrpConver, p_pSwitchNotif->GetACDGrp()))
			{
				p_pSwitchNotif->SetACDGrp(l_strACDGrpConver);
			}

			CHytProtocol::Instance()->Event_CallOver(p_pSwitchNotif);
		}
	}
}
void CBusinessImpl::Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		std::string l_strDeviceNum = p_pSwitchNotif->GetDeviceNum();
		std::string l_strCallerId = p_pSwitchNotif->GetCallerId();
		std::string l_strCalledId = p_pSwitchNotif->GetCalledId();
		std::string l_strOriginalCallerId = p_pSwitchNotif->GetOriginalCallerId();
		std::string l_strOriginalCalledId = p_pSwitchNotif->GetOriginalCalledId();
		std::string l_strDeviceState = p_pSwitchNotif->GetDeviceState();
		std::string l_strStateTime = p_pSwitchNotif->GetStateTime();
		std::string l_strCallDirection = p_pSwitchNotif->GetCallDirection();
		long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		if (CConferenceManager::Instance()->FindConferenceByCTICallRefId(l_strCTICallRefId) ||
			CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			CConferenceManager::Instance()->DeviceStateNotif(l_strDeviceNum, l_strDeviceState, l_strCTICallRefId);

			std::string l_strOriginalCTICallRefId = "";
			if (CConferenceManager::Instance()->GetCTICallRefIdByActiveCallRefId(l_strCTICallRefId, l_strOriginalCTICallRefId))
			{
				//	拨号、振铃为新话务 ID ，需在此修改为原话务 ID 后再发给客户端
				p_pSwitchNotif->SetCTICallRefId(l_strOriginalCTICallRefId);
			}

			//	会议成员接通，状态修改为 会议
			if (l_strDeviceState.compare(CallStateString[STATE_TALK]) == 0)
			{
				p_pSwitchNotif->SetDeviceState(CallStateString[STATE_CONFERENCE]);
			}
		}
		if (CTransferCallManager::Instance()->FindTransferCallByActiveCallRefId(l_strCTICallRefId) ||
			CTransferCallManager::Instance()->FindTransferCallByHeldCallRefId(l_strCTICallRefId))
		{
			if (l_strDeviceState.compare(CallStateString[STATE_TALK]) == 0 && !l_strOriginalCallerId.empty())
			{
				//	转移，修改为原始主叫
				p_pSwitchNotif->SetCallerId(l_strOriginalCallerId);
			}

			CTransferCallManager::Instance()->DeviceStateNotif(l_strDeviceNum, l_strCTICallRefId, l_strDeviceState);
		}

		CHytProtocol::Instance()->Event_DeviceState(p_pSwitchNotif);
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

		if (CConferenceManager::Instance()->FindConferenceByCTICallRefId(l_strCTICallRefId) ||
			CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			CConferenceManager::Instance()->ProcessConferenceHangup(l_strDeviceNum, l_strCTICallRefId);
		}
	}
}
void CBusinessImpl::Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive CSTA RefuseCall Event");

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

		/*if (CConferenceManager::Instance()->FindConferenceByActiveCallRefId(l_strCTICallRefId))
		{
			CConferenceManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCalledId, l_strCTICallRefId);
		}
		else if (CTransferCallManager::Instance()->FindTransferCallByActiveCallRefId(l_strCTICallRefId))
		{
			CTransferCallManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCTICallRefId);
		}
		else
		{*/
			//	呼叫失败，会议、转移发起的咨询可能还没返回结果，因此不能通过 CTICallRefId 来判断是否属于会议
			CConferenceManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCalledId, l_strCTICallRefId);
			CTransferCallManager::Instance()->ProcessFailedEvent(l_strDeviceNum, l_strCalledId,l_strCTICallRefId);

			CHytProtocol::Instance()->Event_Failed(p_pSwitchNotif);
	//	}
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

		if (CConferenceManager::Instance()->FindConferenceByMakeCallTaskId(l_lRequestId))
		{
			CConferenceManager::Instance()->ProcessMakeCallResult(l_lRequestId, l_strCTICallRefId, l_bResult);
		}
		else
		{
			CHytProtocol::Instance()->CR_MakeCall(p_pSwitchNotif);
		}
	}
}
void CBusinessImpl::CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		bool l_bResult = p_pSwitchNotif->GetResult();

		if (CConferenceManager::Instance()->FindConferenceByAnswerCallTaskId(l_lRequestId))
		{
			CConferenceManager::Instance()->ProcessAnswerCallResult(l_lRequestId, "", l_bResult);
		}
		else
		{
			CHytProtocol::Instance()->CR_AnswerCall(p_pSwitchNotif);
		}
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

		if (CTransferCallManager::Instance()->FindTransferCallByConsultationCallTaskId(l_lRequestId))
		{
			//	转警
			CTransferCallManager::Instance()->ProcessConsultationCallResult(l_lRequestId, l_strHeldCTICallRefId, l_strActiveCTICallRefId, l_bResult, l_nErrorCode);
		}
		else if (CConferenceManager::Instance()->FindConferenceByConsultationCallTaskId(l_lRequestId))
		{
			// 会议,只保留一个话务
			CConferenceManager::Instance()->ProcessConsultationCallResult(l_lRequestId, l_strHeldCTICallRefId, l_strActiveCTICallRefId, l_bResult);
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

/*
void CBusinessImpl::CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif)
{
	//CHytProtocol::Instance()->CR_AddConferenceParty(p_pSwitchNotif);
}

void CBusinessImpl::CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteConferenceParty Result, RequestId: [%u]", p_pSwitchNotif->GetRequestId());

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
		ICC_LOG_DEBUG(m_pLog, "Receive GetDeviceList Result, RequestId: [%u]", p_pSwitchNotif->GetRequestId());

		CHytProtocol::Instance()->CR_GetDeviceList(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetAgentList Result, RequestId: [%u]", p_pSwitchNotif->GetRequestId());

		CHytProtocol::Instance()->CR_GetAgentList(p_pSwitchNotif);
	}
}
void CBusinessImpl::CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetCallList Result, RequestId: [%u]", p_pSwitchNotif->GetRequestId());

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
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFactory->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid BargeInCall request protocol : %s", l_strRequestMsg.c_str());

			return;
		}

		if (l_oSyncObj.m_oBody.m_strServiceID == m_strServiceID)
		{
			std::string l_strServiceState = l_oSyncObj.m_oBody.m_strServiceState;

			if (l_strServiceState.compare(DoubleServerRole_Active) == 0)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Active, StartService");
				this->StartSwitchService();
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Standby, StopService");
				this->StopSwitchServeice();
			}
		}		
	}
}

void CBusinessImpl::OnNotifiClientRegisterSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ClientRegister Sync: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ClientRegisterSync(l_strRequestMsg);
	}
}

void CBusinessImpl::OnNotifiLoadAllBlackListRespond(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRespondMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive LoadAllBlackList Respond: %s", l_strRespondMsg.c_str());

		PROTOCOL::CGetAllBlackListRespond l_oGetAllBlackListRespond;
		if (!l_oGetAllBlackListRespond.ParseString(l_strRespondMsg, m_pJsonFactory->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid LoadAllBlackList respond protocol: [%s]", l_strRespondMsg.c_str());

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

			l_mapBlackList[l_strLimitNum] = l_pLimitNumObj;

			ICC_LOG_DEBUG(m_pLog, "Load blacklist limit_num: [%s], limit_min: [%s], begin_time: [%s], end_time: [%s]",
				l_pLimitNumObj->GetLimitNum().c_str(),
				l_pLimitNumObj->GetLimitMin().c_str(),
				l_pLimitNumObj->GetBeginTime().c_str(),
				l_pLimitNumObj->GetEndTime().c_str());
		}

		CBlackListManager::Instance()->InitBlackList(l_mapBlackList);
	}
}

void CBusinessImpl::OnNotifiLoadAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRespondMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive LoadAllBlackList Sync: %s", l_strRespondMsg.c_str());

		PROTOCOL::CGetAllBlackListRespond l_oGetAllBlackListRespond;
		if (!l_oGetAllBlackListRespond.ParseString(l_strRespondMsg, m_pJsonFactory->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid LoadAllBlackList sync protocol: [%s]", l_strRespondMsg.c_str());

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

			l_mapBlackList[l_strLimitNum] = l_pLimitNumObj;

			ICC_LOG_DEBUG(m_pLog, "Sync blacklist limit_num: [%s], limit_min: [%s], begin_time: [%s], end_time: [%s]",
				l_pLimitNumObj->GetLimitNum().c_str(),
				l_pLimitNumObj->GetLimitMin().c_str(),
				l_pLimitNumObj->GetBeginTime().c_str(),
				l_pLimitNumObj->GetEndTime().c_str());
		}

		CBlackListManager::Instance()->InitBlackList(l_mapBlackList);
	}
}

void CBusinessImpl::OnNotifiAgentLoginRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive AgentLogin Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentLogin(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiAgentLogoutRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive AgentLogout Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentLogout(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiSetAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive SetAgentState Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_SetAgentState(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiMakeCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive MakeCall Request: %s", l_strRequestMsg.c_str());
		if (g_bCTITestStartOkFlag)
		{
			PROTOCOL::CMakeCallRequest l_oRequestObj;
			if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFactory->CreateJson()))
			{
				return;
			}

			PROTOCOL::CMakeCallRespond l_oRespondObj;
			l_oRespondObj.m_oBody.m_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
			l_oRespondObj.m_oBody.m_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
			l_oRespondObj.m_oBody.m_strCaseId = l_oRequestObj.m_oBody.m_strCaseId;
			l_oRespondObj.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;

			std::string l_strCallRefId = "";
			if (m_UdpHelper->CTITestCallOut(l_oRequestObj.m_oBody.m_strCallerId, l_oRequestObj.m_oBody.m_strCalledId, l_strCallRefId))
			{
				l_oRespondObj.m_oBody.m_strCallRefId = l_strCallRefId;
				l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			}
			else
			{
				l_oRespondObj.m_oHeader.m_strResult = Result_Failed;
			}
			std::string l_strRespondMsg = l_oRespondObj.ToString(m_pJsonFactory->CreateJson());
			p_pNotifiRequest->Response(l_strRespondMsg);
			
		}
		else
		{
			CHytProtocol::Instance()->CC_MakeCall(l_strRequestMsg, p_pNotifiRequest);
		}
		
	}
}
void CBusinessImpl::OnNotifiHangupRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive Hangup Request: %s", l_strRequestMsg.c_str());
		if (g_bCTITestStartOkFlag)
		{
			PROTOCOL::CHangupRequest l_oRequestObj;
			if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFactory->CreateJson()))
			{
				return;
			}

			PROTOCOL::CHangupRespond l_oRespondObj;
			l_oRespondObj.m_oBody.m_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
			l_oRespondObj.m_oBody.m_strDevice = l_oRequestObj.m_oBody.m_strDevice;
			l_oRespondObj.m_oBody.m_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;

			l_oRespondObj.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
			l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			std::string l_strRespondMsg = l_oRespondObj.ToString(m_pJsonFactory->CreateJson());
			p_pNotifiRequest->Response(l_strRespondMsg);

			long l_lRelatedCSTACallRefId = 0;
			int nRes = 0;
			if (CCallManager::Instance()->GetCSTACallRefId(l_oRespondObj.m_oBody.m_strCallRefId, l_lRelatedCSTACallRefId))
			{
				nRes = 1;
				CTestCallInfo  pCallInfo;
				if (m_UdpHelper->GetTestCallInfoByRefId(l_lRelatedCSTACallRefId, pCallInfo))
				{
					CAvayaEvent::Instance()->DeviceHangup(l_lRelatedCSTACallRefId, l_oRespondObj.m_oBody.m_strDevice, l_oRespondObj.m_oBody.m_strDevice);
					CAvayaEvent::Instance()->DeviceFree(l_lRelatedCSTACallRefId, l_oRespondObj.m_oBody.m_strDevice);
					nRes = 2;
				}
			}
			ICC_LOG_DEBUG(m_pLog, "HangupCall Reporse: MsgID:%s,flag=%d,CallRefId: [%d]", l_oRespondObj.m_oHeader.m_strMsgId.c_str(), nRes, l_lRelatedCSTACallRefId);
		}
		else
		{
			CHytProtocol::Instance()->CC_Hangup(l_strRequestMsg, p_pNotifiRequest);
		}
		
	}
}
void CBusinessImpl::OnNotifiAnswerCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive AnswerCall Request: %s", l_strRequestMsg.c_str());
		if (g_bCTITestStartOkFlag)
		{
			//++++ cti test
			PROTOCOL::CAnswerCallRequest l_oRequestObj;
			if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFactory->CreateJson()))
			{
				//ICC_LOG_ERROR(m_LogPtr, "Invalid AnswerCall request protocol: [%s]", p_strNetMsg.c_str());
				return;
			}

			PROTOCOL::CAnswerCallRespond l_oRespondObj;
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_SetAgentStateRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
			l_oRespondObj.m_oBody.m_strDevice = l_oRequestObj.m_oBody.m_strDevice;
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
			l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			std::string l_strRespondMsg = l_oRespondObj.ToString(m_pJsonFactory->CreateJson());
			p_pNotifiRequest->Response(l_strRespondMsg);

			long l_lRelatedCSTACallRefId = 0;
			int nRes = 0;
			if (CCallManager::Instance()->GetCSTACallRefId(l_oRespondObj.m_oBody.m_strCallRefId, l_lRelatedCSTACallRefId))
			{
				nRes = 1;
				CTestCallInfo  pCallInfo;
				if (m_UdpHelper->GetTestCallInfoByRefId(l_lRelatedCSTACallRefId, pCallInfo))
				{
					CAvayaEvent::Instance()->CallTalking(l_lRelatedCSTACallRefId, pCallInfo.m_strCaller, pCallInfo.m_strCalled, l_oRespondObj.m_oBody.m_strDevice);
					nRes = 2;
				}
				
			}
			ICC_LOG_DEBUG(m_pLog, "AnswerCall Reporse: MsgID:%s,flag=%d,CallRefId: [%d]", l_oRespondObj.m_oHeader.m_strMsgId.c_str(), nRes, l_lRelatedCSTACallRefId);
			
		}
		else
		{
			CHytProtocol::Instance()->CC_AnswerCall(l_strRequestMsg, p_pNotifiRequest);
		}
		
	}
}

void CBusinessImpl::OnNotifiRefuseAnswerRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive RefuseAnswer Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RefuseAnswer(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiClearCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ClearCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ClearCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiListenCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ListenCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ListenCall(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiPickupCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive PickupCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_PickupCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiBargeInCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive BargeInCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_BargeInCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiForcePopCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ForcePopCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ForcePopCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiConsultationCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ConsultationCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ConsultationCall(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiConsultationCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ConsultationCallEx Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ConsultationCallEx(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiTransferCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive TransferCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_TransferCall(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive TransferCallEx Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_TransferCallEx(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiCancelTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive CancelTransferCallEx Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_CancelTransferCallEx(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiDeflectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive DeflectCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeflectCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiHoldCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive HoldCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_HoldCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiRetrieveCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive RetrieveCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RetrieveCall(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiReconnectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ReconnectCall Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ReconnectCall(strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnConferenceCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive ConferenceCall Request:%s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_ConferenceCall(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive AddConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AddConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryConferenceByCallid(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive DisenableConferenceParty Request:%s", strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DisenableConferenceParty(strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCTIConnStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetCTIConnState Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCTIConnState(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetDeviceListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetDeviceList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetDeviceList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetACDListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetACDList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetACDList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetAgentListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetAgentList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetAgentList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetCallList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCallList(l_strRequestMsg, p_pNotifiRequest);
	}
}

/*
void CBusinessImpl::OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive QueryCallEvent Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryCallEvent(l_strRequestMsg, p_pNotifiRequest);
	}
}*/
void CBusinessImpl::OnNotifiGetReadyAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetReadyAgent Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetReadyAgent(l_strRequestMsg, p_pNotifiRequest);
	}
}

/*
void CBusinessImpl::OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive QueryBlackCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_QueryBlackCall(l_strRequestMsg, p_pNotifiRequest);
	}
}*/

void CBusinessImpl::OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive SetBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_SetBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteAllBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_DeleteAllBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}
void CBusinessImpl::OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetAllBlackList Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetAllBlackList(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		//ICC_LOG_DEBUG(m_pLog, "Receive GetCurrentTime Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_GetCurrentTime(l_strRequestMsg, p_pNotifiRequest);
	}
}

void CBusinessImpl::OnAgentHeartBeatTimeoutSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive Agent heartbeat Timeout Sync: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_AgentHeartBeatTimeout(l_strRequestMsg, p_pNotifiRequest);
	}
}

/*
void CBusinessImpl::OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive RemoveReleaseCall Request: %s", l_strRequestMsg.c_str());

		CHytProtocol::Instance()->CC_RemoveReleaseCall(l_strRequestMsg, p_pNotifiRequest);
	}
}*/

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::OnInitOverTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "OnInitOverTimer");
	if (CAvayaSwitchManager::Instance()->GetSwitchConnect() || g_bCTITestStart)
	{
		if (!CAgentManager::Instance()->IsInitOver())
		{
			Sleep(1000);
			CAgentManager::Instance()->SetInitOver(true);
			ICC_LOG_DEBUG(m_pLog, "Agent Manager Init Over !");
		}
	}
}
void CBusinessImpl::OnBlackListManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strLimitNum = "";

	if (CBlackListManager::Instance()->BlackListIsTimeout(l_strLimitNum))
	{
		CBlackListManager::Instance()->ProcessBlackListTimeout(l_strLimitNum);
	}
}
// 话务管理定时器
void CBusinessImpl::OnCallManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	long l_lCSTACallRefId = DEFAULT_CALLREFID;

	while (CCallManager::Instance()->FindTimeoutCall(l_lCSTACallRefId))
	{
		CCallManager::Instance()->ProcessTimeoutCall(l_lCSTACallRefId);
	}
}
// 会议管理定时器
void CBusinessImpl::OnConferenceManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	bool l_bIsMakeCall = false;
	std::string l_strConferenceId = "";

	while (CConferenceManager::Instance()->FindDialTimeout(l_strConferenceId, l_bIsMakeCall))
	{
		CConferenceManager::Instance()->ProcessDialTimeout(l_strConferenceId, l_bIsMakeCall);
	}

	while (CConferenceManager::Instance()->FindRingTimeout(l_strConferenceId, l_bIsMakeCall))
	{
		CConferenceManager::Instance()->ProcessRingTimeout(l_strConferenceId, l_bIsMakeCall);
	}
}
// 转警管理定时器
void CBusinessImpl::OnTransferCAllManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strHeldCallRefId = "";

	while (CTransferCallManager::Instance()->FindDialTimeout(l_strHeldCallRefId))
	{
		CTransferCallManager::Instance()->ProcessDialTimeout(l_strHeldCallRefId);
	}

	while (CTransferCallManager::Instance()->FindRingTimeout(l_strHeldCallRefId))
	{
		CTransferCallManager::Instance()->ProcessRingTimeout(l_strHeldCallRefId);
	}
}
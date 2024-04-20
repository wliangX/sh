
#include "Boost.h"

#include "AvayaSwitchManager.h"
#include "SysConfig.h"
#include "SwitchClientImpl.h"
#include "DeviceManager.h"
#include "AgentManager.h"
#include "BlackListManager.h"
#include "AvayaSwitch.h"
#include "Task.h"
#include "TaskManager.h"
#include "CallManager.h"

#include "AvayaEvent.h"

#define MAX_HEARTBEAT		3		//	心跳检测次数
#define CHECK_TIMESPAN		1		//	单位：秒
#define SLEEP_TIMESPAN		30		//	单位：毫秒
using namespace std;

//////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//avaya 8300 模拟
bool g_bCTITestStart = false;

#if 1
CCTITestUdpHelper::CCTITestUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool, Udp::IUdpPtr pUdpServer)
{
	m_pHelpTool = pHelpTool;
	m_pLog = pLogPtr;
	m_pUdp = pUdpServer;

	m_nIntCallRefId = 50;
}

CCTITestUdpHelper::~CCTITestUdpHelper()
{

}

void CCTITestUdpHelper::SetTestCallInfo(const string& strCaller, const string& strCalled, const string& strAgentNo, long nCallRefId)
{
	CTestCallInfo tmpCallInfo;
	tmpCallInfo.m_lCSTACallRefId = nCallRefId;
	tmpCallInfo.m_strCaller = strCaller;
	tmpCallInfo.m_strCalled = strCalled;
	tmpCallInfo.m_strAgentNo = strAgentNo;
	m_mapCallInfo[strCaller] = tmpCallInfo;
}

bool CCTITestUdpHelper::GetTestCallInfoByRefId(long nCallRefId, CTestCallInfo& pCallInfo)
{
	for (auto l_pCallObj : m_mapCallInfo)
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

bool CCTITestUdpHelper::CTITestCallOut(long p_lRequestId,const string& strCaller, const string& strCalled, string& strCallRefId)
{
	bool bRes = false;
	std::string l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(m_nIntCallRefId);
	if (!l_strCTICallRefId.empty())
	{
		long lMonitorId = atoi(strCaller.c_str());
		/////////////////////////////////////////////////////
		CSTAMakeCallConfEvent_t  l_makeCall;
		memset(&l_makeCall, 0, sizeof(CSTAMakeCallConfEvent_t));
		l_makeCall.newCall.callID = m_nIntCallRefId;
		strcpy(l_makeCall.newCall.deviceID, strCaller.c_str());
		l_makeCall.newCall.devIDType = STATIC_ID;
		CAvayaEvent::Instance()->MakeCallConf(p_lRequestId, l_makeCall);

		CSTAServiceInitiatedEvent_t l_serviceInitiated;
		memset(&l_serviceInitiated, 0, sizeof(CSTAServiceInitiatedEvent_t));
		l_serviceInitiated.cause = EC_NONE;
		l_serviceInitiated.localConnectionInfo = CS_INITIATE;
		l_serviceInitiated.initiatedConnection.callID = m_nIntCallRefId;
		strcpy(l_serviceInitiated.initiatedConnection.deviceID, strCaller.c_str());
		CAvayaEvent::Instance()->ServiceInitiatedEvent(lMonitorId, l_serviceInitiated);

		//CSTAOriginatedEvent_t originated;
		//memset(&originated, 0, sizeof(CSTAOriginatedEvent_t));

		//CAvayaEvent::Instance()->OriginatedEvent(lMonitorId, originated);
		// 
		////////////////////////////////////////////////////////////////
		CSTADeliveredEvent_t l_EvtDelivered;
		memset(&l_EvtDelivered, 0, sizeof(CSTADeliveredEvent_t));
		strcpy(l_EvtDelivered.calledDevice.deviceID, strCalled.c_str());
		l_EvtDelivered.calledDevice.deviceIDStatus = ID_PROVIDED;
		l_EvtDelivered.calledDevice.deviceIDType; //DEVICE_IDENTIFIER

		strcpy(l_EvtDelivered.connection.deviceID, strCalled.c_str());
		strcpy(l_EvtDelivered.alertingDevice.deviceID, strCalled.c_str());

		strcpy(l_EvtDelivered.callingDevice.deviceID, strCaller.c_str()); 
		l_EvtDelivered.calledDevice.deviceIDStatus = ID_PROVIDED;
		l_EvtDelivered.calledDevice.deviceIDType; //DEVICE_IDENTIFIER

		l_EvtDelivered.connection.callID = m_nIntCallRefId; //交换机话务ID

		//模拟回铃状态
		l_EvtDelivered.localConnectionInfo = CS_ALERTING;
		CAvayaEvent::Instance()->DeliveredEvent(lMonitorId, l_EvtDelivered);

		//模拟呼叫连接
		l_EvtDelivered.localConnectionInfo = CS_CONNECT;
		CAvayaEvent::Instance()->DeliveredEvent(lMonitorId, l_EvtDelivered);

		
		//
		CSTAEstablishedEvent_t l_established;
		memset(&l_established, 0, sizeof(CSTAEstablishedEvent_t));
		l_established.establishedConnection.callID = m_nIntCallRefId;
		strcpy(l_established.establishedConnection.deviceID, strCalled.c_str());//strDn
		strcpy(l_established.answeringDevice.deviceID, strCalled.c_str());//strAnswerId
		strcpy(l_established.callingDevice.deviceID, strCaller.c_str());//strCallerId
		strcpy(l_established.calledDevice.deviceID, strCalled.c_str());//strCalledId
		l_established.localConnectionInfo = CS_CONNECT;
		l_established.cause = EC_NEW_CALL;
		CAvayaEvent::Instance()->EstablishedEvent(lMonitorId, l_established);
		SetTestCallInfo(strCaller, strCalled, strCaller, m_nIntCallRefId);

		bRes = true;
	}
	m_nIntCallRefId++;

	return bRes;

}

void CCTITestUdpHelper::OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort)
{
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	char l_pBuffer[UDP_RECV_BUF_SIZE] = { 0 };
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
			if (!CAvayaSwitchManager::Instance()->isCTITestMode())
			{
				strReturnRes = m_pStringUtilPtr->Format("======>startup failed,Please configure as test mode!");
				break;
			}

			if (!g_bCTITestStart)
			{
				g_bCTITestStart = true;
				CAvayaSwitchManager::Instance()->StartTestSwitchService();
				strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]", l_pBuffer);
			}
			else
			{
				strReturnRes = m_pStringUtilPtr->Format("======>CTI test started!");
			}
			break;
		}

		if (!g_bCTITestStart)
		{
			strReturnRes = m_pStringUtilPtr->Format("======>failed,CTI test not started!");
			break;
		}

		if (l_strCMD == "makecall")  //makecall 8006 110 8001  ,表示模拟主叫号码8006叫110 ，呼叫分配给8001坐席
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

				/*
				long lCSTACallRefId = delivered.connection.callID;					//	交换机话务ID
				std::string strDn(delivered.connection.deviceID);					//  话务成员
				std::string strAlertingDn(delivered.alertingDevice.deviceID);		//	振铃的分机号码
				std::string strCallerId(delivered.callingDevice.deviceID);			//	主叫号码
				std::string strCalledId(delivered.calledDevice.deviceID);			//	被叫号码(110，119)
				std::string strRedirDn(delivered.lastRedirectionDevice.deviceID);	//	重定向号码
				std::string strState = CAvayaResult::Instance()->GetConnectionStateString(delivered.localConnectionInfo);
				std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(delivered.cause);
				*/

				CSTADeliveredEvent_t l_EvtDelivered;
				strcpy(l_EvtDelivered.calledDevice.deviceID, strCalled.c_str());//被叫号码(110，119)
				l_EvtDelivered.calledDevice.deviceIDStatus = ID_PROVIDED;
				l_EvtDelivered.calledDevice.deviceIDType; //DEVICE_IDENTIFIER

				strcpy(l_EvtDelivered.connection.deviceID, strAgentNum.c_str());
				strcpy(l_EvtDelivered.alertingDevice.deviceID, strAgentNum.c_str());

				strcpy(l_EvtDelivered.callingDevice.deviceID, strCaller.c_str()); //主叫号码
				l_EvtDelivered.calledDevice.deviceIDStatus = ID_PROVIDED;
				l_EvtDelivered.calledDevice.deviceIDType; //DEVICE_IDENTIFIER

				l_EvtDelivered.connection.callID = m_nIntCallRefId; //交换机话务ID

				//模拟话务分配状态
				long lMonitorId = atoi(strAgentNum.c_str());
				/*
				l_EvtDelivered.localConnectionInfo = CS_NONE;
				l_EvtDelivered.cause == EC_NEW_CALL;
				CAvayaEvent::Instance()->DeliveredEvent(lMonitorId, l_EvtDelivered);

				//模拟呼入状态
				l_EvtDelivered.cause == EC_REDIRECTED;
				CAvayaEvent::Instance()->DeliveredEvent(lMonitorId, l_EvtDelivered);
				*/
				l_EvtDelivered.localConnectionInfo = CS_ALERTING;
				l_EvtDelivered.cause = EC_NEW_CALL;
				CAvayaEvent::Instance()->DeliveredEvent(lMonitorId, l_EvtDelivered);
				{
					strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]", l_pBuffer);
					SetTestCallInfo(strCaller, strCalled, strAgentNum, m_nIntCallRefId);
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

					//CAvayaEvent::Instance()->DeviceFree(l_CallInfo.m_lCSTACallRefId, l_CallInfo.m_strAgentNo);

					strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]\n", l_pBuffer);
				}
				else
				{
					strReturnRes = m_pStringUtilPtr->Format("======>not find call,cmd execute failed:[%s]", l_pBuffer);
				}
			}

		}
		else if (l_strCMD == "answercall")
		{
			if (nSize == 2)
			{
				std::string strCaller = l_vecParam[1];  //主叫
				CTestCallInfo l_CallInfo;
				if (GetTestCallInfoByCaller(strCaller, l_CallInfo))
				{
					CAvayaEvent::Instance()->CallTalking(l_CallInfo.m_lCSTACallRefId, l_CallInfo.m_strCaller, l_CallInfo.m_strAgentNo, l_CallInfo.m_strAgentNo);
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
					CAgentManager::Instance()->ReadyStateSync(l_vecParam[1], l_vecParam[2], "ready");
				}
				else
				{
					CAgentManager::Instance()->ReadyStateSync(l_vecParam[1], l_vecParam[2], "notready");
				}
				strReturnRes = m_pStringUtilPtr->Format("======>cmd execute success:[%s]\n", l_pBuffer);

			}
		}
	} while (0);

	printf("%s\n", strReturnRes.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strReturnRes.c_str());

	m_pUdp->Send(strReturnRes, p_strRemoteIP, p_iPort);

	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
}

void CCTITestUdpHelper::OnStart()
{

}

void CCTITestUdpHelper::OnStop()
{

}

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
bool CAvayaSwitchManager::StartTestSwitchService()
{
	if (!m_bCTITestModeFlag)
	{
		return false;
	}
	m_bSwitchConnect = true;
	if (m_bSwitchConnect)
	{
		CDeviceManager::Instance()->SetCTITestData();
		CAgentManager::Instance()->SetCTITestData();
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
int CAvayaSwitchManager::m_nHeartBeatCount = 0;
boost::shared_ptr<CAvayaSwitchManager> CAvayaSwitchManager::m_pInstance = nullptr;
CAvayaSwitchManager::CAvayaSwitchManager()
{
	m_bCmdTaskThreadAlive = false;
	m_bCheckExcusedTaskThreadAlive = false;
	m_bSwitchEventTaskThreadAlive = false;
	m_bWindowMsgThreadAlive = false;

	m_bSwitchConnect = false;
//	m_bExitSys = false;

	m_nHeartBeatCount = 0;

	m_pWindowsMsgThread = nullptr;
	m_pCmdTaskThread = nullptr;
	m_pSwitchEventTaskThread = nullptr;
	m_pCheckExcusedTaskThread = nullptr;

	m_oLastCheckExcusedTime = 0;

	m_bCTITestModeFlag = false;
	m_bOpenStreamThreadAlive = false;
}
CAvayaSwitchManager::~CAvayaSwitchManager()
{
//	m_bExitSys = true;
}

boost::shared_ptr<CAvayaSwitchManager> CAvayaSwitchManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CAvayaSwitchManager>();
	}

	return m_pInstance;
}
void CAvayaSwitchManager::ExitInstance()
{
	CAvayaSwitch::Instance()->ExitInstance();

	m_mapCmdNameToSwitch.clear();
	m_mapSwitchEvent.clear();

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

void CAvayaSwitchManager::OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback)
{	
	m_pSwitchEventCallback = p_pCallback;

	CAvayaSwitch::Instance()->OnInit(p_pResourceManager);
	m_pResourceManager = p_pResourceManager;
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();

//	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ReOpenStream, &CAvayaSwitchManager::ReOpenStream));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_QueryDeviceInfo, &CAvayaSwitchManager::QueryDeviceInfo));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MonitorDevice, &CAvayaSwitchManager::MonitorDevice));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MonitorCallVirDevice, &CAvayaSwitchManager::MonitorCallsViaDevice));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RouteRegisterReq, &CAvayaSwitchManager::RouteRegisterReq));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RouteRequest, &CAvayaSwitchManager::RouteRequest));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AgentLogin, &CAvayaSwitchManager::AgentLogin));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AgentLogout, &CAvayaSwitchManager::AgentLogout));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_SetAgentState, &CAvayaSwitchManager::SetAgentState));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_QueryAgentState, &CAvayaSwitchManager::QueryAgentState));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_MakeCall, &CAvayaSwitchManager::MakeCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_Hangup, &CAvayaSwitchManager::Hangup));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AnswerCall, &CAvayaSwitchManager::AnswerCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RefuseAnswer, &CAvayaSwitchManager::RefuseAnswer));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RefuseBlackCall, &CAvayaSwitchManager::RefuseBlackCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ClearCall, &CAvayaSwitchManager::ClearCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ListenCall, &CAvayaSwitchManager::ListenCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_PickupCall, &CAvayaSwitchManager::PickupCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_BargeInCall, &CAvayaSwitchManager::BargeInCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ForcePopCall, &CAvayaSwitchManager::ForcePopCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ConsultationCall, &CAvayaSwitchManager::ConsultationCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_TransferCall, &CAvayaSwitchManager::TransferCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeflectCall, &CAvayaSwitchManager::DeflectCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_HoldCall, &CAvayaSwitchManager::HoldCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_RetrieveCall, &CAvayaSwitchManager::RetrieveCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ReconnectCall, &CAvayaSwitchManager::ReconnectCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_ConferenceCall, &CAvayaSwitchManager::ConferenceCall));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_TakeOverCall, &CAvayaSwitchManager::TakeOverCall));
	//	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_AddConferenceParty, &CAvayaSwitchManager::AddConferenceParty));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetCTIConnState, &CAvayaSwitchManager::GetAESConnState));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetDeviceList, &CAvayaSwitchManager::GetDeviceList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetACDList, &CAvayaSwitchManager::GetACDList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetAgentList, &CAvayaSwitchManager::GetAgentList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetCallList, &CAvayaSwitchManager::GetCallList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetReadyAgent, &CAvayaSwitchManager::GetReadyAgent));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_GetFreeAgentList, &CAvayaSwitchManager::GetFreeAgent));

	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_SetBlackList, &CAvayaSwitchManager::SetBlackList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeleteBlackList, &CAvayaSwitchManager::DeleteBlackList));
	m_mapCmdNameToSwitch.insert(std::pair<int, PCmdSwitch>(Task_DeleteAllBlackList, &CAvayaSwitchManager::DeleteAllBlackList));

	//////////////////////////////////////////////////////////////////////////
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_UniversalFailRet, &CAvayaSwitchManager::UniversalFailRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConnectAesEvent, &CAvayaSwitchManager::OpenStreamConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_FailedEvent, &CAvayaSwitchManager::FailedEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AgentStateEvent, &CAvayaSwitchManager::AgentStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeviceStateEvent, &CAvayaSwitchManager::DeviceStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConferenceHangupEvent, &CAvayaSwitchManager::ConferenceHangupEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_CallStateEvent, &CAvayaSwitchManager::CallStateEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_CallOverEvent, &CAvayaSwitchManager::CallOverEvent));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RefuseCallEvent, &CAvayaSwitchManager::RefuseCallEvent));

	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_QueryDeviceInfoRet, &CAvayaSwitchManager::QueryDeviceInfoRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_MonitorDeviceConf, &CAvayaSwitchManager::MonitorDeviceConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RouteRegisterConf, &CAvayaSwitchManager::RouteRegisterConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_QueryAgentStateRet, &CAvayaSwitchManager::QueryAgentStateRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_SetAgentStateConf, &CAvayaSwitchManager::SetAgentStateConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_MakeCallConf, &CAvayaSwitchManager::MakeCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AnswerCallConf, &CAvayaSwitchManager::AnswerCallConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RefuseAnswerConf, &CAvayaSwitchManager::RefuseAnswerConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ClearConnectionConf, &CAvayaSwitchManager::ClearConnectionConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_SingleStepConferenceConf, &CAvayaSwitchManager::SingleStepConferenceConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_PickupCallConf, &CAvayaSwitchManager::PickupCallConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_BargeInCallConf, &CAvayaSwitchManager::BargeInCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ClearCallConf, &CAvayaSwitchManager::ClearCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConsultationCallConf, &CAvayaSwitchManager::ConsultationCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_TransferCallConf, &CAvayaSwitchManager::TransferCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeflectCallConf, &CAvayaSwitchManager::DeflectCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_HoldCallConf, &CAvayaSwitchManager::HoldCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_RetrieveCallConf, &CAvayaSwitchManager::RetrieveCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ReconnectCallConf, &CAvayaSwitchManager::ReconnectCallConf));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_ConferenceCallConf, &CAvayaSwitchManager::ConferenceCallConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_AddConferenceParty, &CAvayaSwitchManager::AddConferencePartyConf));
	//	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_DeleteConferenceParty, &CAvayaSwitchManager::DeleteConferencePartyConf));

	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetCTIConnStateRet, &CAvayaSwitchManager::GetAESConnStateRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetDeviceListRet, &CAvayaSwitchManager::GetDeviceListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetACDListRet, &CAvayaSwitchManager::GetACDListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetAgentListRet, &CAvayaSwitchManager::GetAgentListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetCallListRet, &CAvayaSwitchManager::GetCallListRet));
	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetReadyAgentRet, &CAvayaSwitchManager::GetReadyAgentRet));

	m_mapSwitchEvent.insert(std::pair<int, PSwitchEvent>(Task_GetFreeAgentListRet, &CAvayaSwitchManager::GetFreeAgentListRet));
	
	m_bCTITestModeFlag = CSysConfig::Instance()->GetCTITestFlag();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pUdp = ICCGetIUdpFactory()->CreateUdp();
	int l_UDPPort = CSysConfig::Instance()->GetCTITestPort();
	if (l_UDPPort > 0)
	{
		m_CTITestUdpHelper = boost::make_shared<CCTITestUdpHelper>(m_pLog, m_pHelpTool, m_pUdp);
		if (m_CTITestUdpHelper)
		{
			m_CTITestUdpHelper->SetStringUtilPtr(m_pStrUtil);
			bool bFlag = true;
			try {
				m_pUdp->Bind(l_UDPPort, m_CTITestUdpHelper);
			}
			catch (...)
			{
				bFlag = false;
			}

			if (!bFlag)
			{
				l_UDPPort = 21004;
				m_pUdp->Bind(l_UDPPort, m_CTITestUdpHelper);
			}
			
		}
	}
	ICC_LOG_DEBUG(m_pLog, "CTI Plugin Init Success,CTI TestFlag=%d,testUDPPort=%d", m_bCTITestModeFlag, l_UDPPort);
}

void CAvayaSwitchManager::OnStart()
{
	if (m_bCTITestModeFlag)
	{
		CAvayaSwitch::Instance()->OnStart();
		StartWindowsMsgThread();
		StartCmdTaskThread();
		StartSwitchEventTaskThread();
		StartCheckExcusedTaskThread();

		return;
	}
	CAvayaSwitch::Instance()->OnStart();
	StartWindowsMsgThread();

	//打开流
	OpenStreamProcess(false);

	StartCmdTaskThread();
	StartSwitchEventTaskThread();
	StartCheckExcusedTaskThread();
}


void CAvayaSwitchManager::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "=====================CAvayaSwitchManager stop!! =====================");
	StopCheckExcusedTaskThread();
	StopSwitchEventTaskThread();
	StopCmdTaskThread();

	m_bOpenStreamThreadAlive = false;
	if (m_bSwitchConnect)
	{
		m_bSwitchConnect = false;
		CAvayaSwitch::Instance()->OnCloseStreamEvent();

	}
	CAvayaSwitch::Instance()->OnStop();
	StopWindowsMsgThread();
}

//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::DoWindowsMsg()
{	
	ICC_LOG_DEBUG(m_pLog, "===================== Create My Windows Msg Thread Begin!! =====================");

#if defined(WIN32) || defined(WIN64)
	if (CAvayaSwitch::Instance()->CreateMyWindows())
	{
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		CAvayaSwitch::Instance()->DestroyMyWindows();
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "CreateMyWindows Failed !!");
	}

#else
	while (m_bWindowMsgThreadAlive)
	{
		if (CAvayaSwitch::Instance()->EventIsPending())
		{
			CAvayaSwitch::Instance()->OnTSAPIEvent();
			CAvayaSwitch::Instance()->ResetHandleEvent();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}
#endif

	ICC_LOG_DEBUG(m_pLog, "-------------------- Create My Windows Msg Thread Exit!! ---------------------");
}
void CAvayaSwitchManager::StartWindowsMsgThread()
{
	if (!m_bWindowMsgThreadAlive)
	{
		m_bWindowMsgThreadAlive = true;

		m_pWindowsMsgThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoWindowsMsg, this));
	}
}
void  CAvayaSwitchManager::StopWindowsMsgThread()
{
	if (m_bWindowMsgThreadAlive)
	{
		m_bWindowMsgThreadAlive = false;
#if defined(WIN32) || defined(WIN64)
		CAvayaSwitch::Instance()->CloseMyWindows();
#endif

		if (m_pWindowsMsgThread)
		{
			m_pWindowsMsgThread->join();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::ExeCommand(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask)
{
	int l_nResult = ERROR_UNKNOWN;

	std::string l_strTaskName = TaskNameString[p_nTaskName];
	//long l_lTaskId = p_pTask->GetTaskId();

	if (m_mapCmdNameToSwitch.find(p_nTaskName) != m_mapCmdNameToSwitch.end())
	{
		if (p_nTaskName != Task_QueryAgentState)
		{
			ICC_LOG_LOWDEBUG(m_pLog, "Begin Cmd Task, TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}

		PCmdSwitch pFunc = m_mapCmdNameToSwitch[p_nTaskName];
		l_nResult = (this->*pFunc)(p_pTask);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "TaskName: [%s] TaskId: [%u] is not supply",
			l_strTaskName.c_str(), p_lTaskId);
	}

	if (l_nResult != RESULT_SUCCESS)
	{
		// 调用命令失败,通知应用层
		ProcessFailedCmd(p_nTaskName, p_lTaskId, l_nResult);

		ICC_LOG_ERROR(m_pLog, "End Cmd Task, Cmd Invoke failed. TaskName: [%s] TaskId: [%u] ErrorCode: [%d]",
			l_strTaskName.c_str(), p_lTaskId, l_nResult);
	}
	else
	{
		if (p_nTaskName != Task_QueryAgentState /*&& p_nTaskName != Task_QueryDeviceInfo*/)
		{
			ICC_LOG_LOWDEBUG(m_pLog, "End Cmd Task, Cmd Invoke success. TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}
	}
}
void CAvayaSwitchManager::DoCmdTask()
{
	ICC_LOG_DEBUG(m_pLog, "===================== Cmd Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bCmdTaskThreadAlive)
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetCmdTaskHeader();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			E_TASK_NAME l_nTaskName = l_pTask->GetTaskName();

			// 添加到执行队列
			CTaskManager::Instance()->AddExcutedTask(l_pTask);

			// 执行处理流程
			CAvayaSwitchManager::Instance()->ExeCommand(l_nTaskName, l_lTaskId, l_pTask);
			
			CTaskManager::Instance()->DeleteCmdTask(l_lTaskId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ Cmd Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartCmdTaskThread()
{
	if (!m_bCmdTaskThreadAlive)
	{
		m_bCmdTaskThreadAlive = true;

		m_pCmdTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoCmdTask, this));
	}
}
void CAvayaSwitchManager::StopCmdTaskThread()
{
	if (m_bCmdTaskThreadAlive)
	{
		m_bCmdTaskThreadAlive = false;

		if (m_pCmdTaskThread)
		{
			m_pCmdTaskThread->join();
		}
	}
}


void CAvayaSwitchManager::ExeSwitchEvent(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask)
{
	std::string l_strTaskName = TaskNameString[p_nTaskName];
//	long l_lTaskId = p_pTask->GetTaskId();

	if (m_mapSwitchEvent.find(p_nTaskName) != m_mapSwitchEvent.end())
	{
		ICC_LOG_LOWDEBUG(m_pLog, "Begin ExeSwitchEvent, TaskName[%s] TaskId[%u]",
			l_strTaskName.c_str(), p_lTaskId);

		PSwitchEvent l_pFunc = m_mapSwitchEvent[p_nTaskName];
		(this->*l_pFunc)(p_pTask);

		if (p_nTaskName != Task_QueryAgentStateRet/* && p_nTaskName != Task_QueryDeviceInfoRet*/)
		{
			ICC_LOG_LOWDEBUG(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}
		else
		{
			ICC_LOG_LOWDEBUG(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u]",
				l_strTaskName.c_str(), p_lTaskId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "End ExeSwitchEvent, TaskName: [%s] TaskId: [%u] is not supply",
			l_strTaskName.c_str(), p_lTaskId);
	}
}
void CAvayaSwitchManager::DoSwitchEventTask()
{
	ICC_LOG_DEBUG(m_pLog, "=============== DoSwitchEvent Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bSwitchEventTaskThreadAlive)
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetSwitchEventTaskHeader();
		if (l_pTask)
		{
			// 执行处理流程
			long l_lTaskId = l_pTask->GetTaskId();
			E_TASK_NAME l_nTaskName = l_pTask->GetTaskName();

			CAvayaSwitchManager::Instance()->ExeSwitchEvent(l_nTaskName, l_lTaskId, l_pTask);

			CTaskManager::Instance()->DeleteSwitchEventTask(l_lTaskId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ DoSwitchEvent Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartSwitchEventTaskThread()
{
	if (!m_bSwitchEventTaskThreadAlive)
	{
		m_bSwitchEventTaskThreadAlive = true;

		m_pSwitchEventTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoSwitchEventTask, this));
	}
}
void CAvayaSwitchManager::StopSwitchEventTaskThread()
{
	if (m_bSwitchEventTaskThreadAlive)
	{
		m_bSwitchEventTaskThreadAlive = false;

		if (m_pSwitchEventTaskThread)
		{
			m_pSwitchEventTaskThread->join();
		}
	}
}

void CAvayaSwitchManager::ResetCheckExcusedTime()
{
	m_oLastCheckExcusedTime = m_pDateTimePtr->CurrentDateTime();
}
bool CAvayaSwitchManager::CheckExcusedIsTimeout()
{
	DateTime::CDateTime l_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	DateTime::CDateTime l_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastCheckExcusedTime, CHECK_TIMESPAN);

	if (l_oCurrentTime > l_oEndTime)
	{
		return true;
	}

	return false;
}
void CAvayaSwitchManager::DoCheckExcusedTask()
{
	ICC_LOG_DEBUG(m_pLog, "=============== DoCheckExcusedTask Task Thread Start!! =====================");

	while (CAvayaSwitchManager::Instance()->m_bCheckExcusedTaskThreadAlive)
	{
	//	if (CAvayaSwitchManager::Instance()->CheckExcusedIsTimeout())
	//	{
			long l_lTaskId = DEFAULT_TASKID;
			E_TASK_NAME l_nTaskName = Task_NULL;

			if (CTaskManager::Instance()->ExcuseTaskTimeout(l_nTaskName, l_lTaskId))
			{
				// CMD 执行超时
				CAvayaSwitchManager::Instance()->ProcessFailedCmd(l_nTaskName, l_lTaskId, ERROR_CMD_TIMEOUT);
			}

	/*		CAvayaSwitchManager::Instance()->ResetCheckExcusedTime();
		} 
		else
		{*/
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 2));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN * 2));
	//	}		
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ DoCheckExcusedTask Task Thread Exit!! ------------------");
}
void CAvayaSwitchManager::StartCheckExcusedTaskThread()
{
	if (!m_bCheckExcusedTaskThreadAlive)
	{
		m_bCheckExcusedTaskThreadAlive = true;
		m_oLastCheckExcusedTime = m_pDateTimePtr->CurrentDateTime();

		m_pCheckExcusedTaskThread = boost::make_shared<boost::thread>(boost::bind(&CAvayaSwitchManager::DoCheckExcusedTask, this));
	}
}
void CAvayaSwitchManager::StopCheckExcusedTaskThread()
{
	if (m_bCheckExcusedTaskThreadAlive)
	{
		m_bCheckExcusedTaskThreadAlive = false;

		if (m_pCheckExcusedTaskThread)
		{
			m_pCheckExcusedTaskThread->join();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::FailedEvent(ITaskPtr p_pTask)
{
	if (m_pSwitchEventCallback)
	{
		//	话务结束处理

		IFailedEventNotifPtr l_pFailedEventNotif = boost::dynamic_pointer_cast<CFailedEventNotif>(p_pTask->GetSwitchNotif());
		if (l_pFailedEventNotif)
		{
			m_pSwitchEventCallback->Event_Failed(l_pFailedEventNotif);
		}
	}
}

void CAvayaSwitchManager::AgentStateEvent(ITaskPtr p_pTask)
{
	if (m_pSwitchEventCallback)
	{
		IAgentStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CAgentStateNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			std::string l_strOnlineCount = m_pStrUtil->Format("%d", CAgentManager::Instance()->GetReadyAgentCount(l_pNotif->GetACDGrp()));
			l_pNotif->SetCurrOnlineNum(l_strOnlineCount);

			std::string l_strMinOnlineNum = m_pStrUtil->Format("%d", CSysConfig::Instance()->GetMinOnlineAgentNum());
			l_pNotif->SetMinOnlineNum(l_strMinOnlineNum);

			m_pSwitchEventCallback->Event_AgentState(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::DeviceStateEvent(ITaskPtr p_pTask)
{
	//	收到设备状态事件，重设心跳时间
	CDeviceManager::Instance()->ResetHeartBeatTime();

	if (m_pSwitchEventCallback)
	{
		IDeviceStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CDeviceStateNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			m_pSwitchEventCallback->Event_DeviceState(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::ConferenceHangupEvent(ITaskPtr p_pTask)
{
	if (m_pSwitchEventCallback)
	{
		IDeviceStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CDeviceStateNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			m_pSwitchEventCallback->Event_ConferenceHangup(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::CallStateEvent(ITaskPtr p_pTask)
{
	//	收到话务事件，重设心跳时间
	CDeviceManager::Instance()->ResetHeartBeatTime();

	if (m_pSwitchEventCallback)
	{
		ICallStateNotifPtr l_pNotif = boost::dynamic_pointer_cast<CCallStateNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			m_pSwitchEventCallback->Event_CallState(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::CallOverEvent(ITaskPtr p_pTask)
{
	if (m_pSwitchEventCallback)
	{
		ICallOverNotifPtr l_pNotif = boost::dynamic_pointer_cast<CCallOverNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			m_pSwitchEventCallback->Event_CallOver(l_pNotif);
		}
	}
}
void CAvayaSwitchManager::RefuseCallEvent(ITaskPtr p_pTask)
{
	if (m_pSwitchEventCallback)
	{
		IRefuseCallNotifPtr l_pNotif = boost::dynamic_pointer_cast<CRefuseCallNotif>(p_pTask->GetSwitchNotif());
		if (l_pNotif)
		{
			m_pSwitchEventCallback->Event_RefuseCall(l_pNotif);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::ProcessFailedCmd(E_TASK_NAME p_nTaskName, long p_lRequestId, int p_nErrorCode)
{
	switch (p_nTaskName)
	{
	case Task_QueryDeviceInfo:
		QueryDeviceInfoFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_AgentLogin:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_AgentLogout:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_SetAgentState:
		SetAgentStateFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_MakeCall:
		MakeCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_Hangup:
		ClearConnectionFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_RefuseAnswer:
		ClearCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ClearCall:
		ClearCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ForcePopCall:
		ClearConnectionFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_AnswerCall:
		AnswerCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_BargeInCall:
		SingleStepConferenceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ListenCall:
		SingleStepConferenceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_PickupCall:
		PickupCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ConsultationCall:
		ConsultationCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_TransferCall:
		TransferCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_DeflectCall:
		DeflectCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_HoldCall:
		HoldCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_RetrieveCall:
		RetrieveCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ReconnectCall:
		ReconnectCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_ConferenceCall:
		ConferenceCallFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_MonitorCallVirDevice:
		MonitorDeviceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_MonitorDevice:
		MonitorDeviceFailed(p_lRequestId, p_nErrorCode);
		break;
	case Task_TakeOverCall:
		TakeOverCallFailed(p_lRequestId, p_nErrorCode);
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "Delete Failed ExcusedTask, TaskName: [%s] TaskId: [%u]",
			TaskNameString[p_nTaskName].c_str(), p_lRequestId);
		CTaskManager::Instance()->DeleteExcusedTask(p_lRequestId);
		break;
	}
}
void CAvayaSwitchManager::QueryDeviceInfoFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "QueryDeviceInfoFailed, RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	IQueryDeviceInfoResultNotifPtr l_pResultNotif = boost::make_shared<CQueryDeviceInfoResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);
		if (p_nErrorCode == ERROR_INVALID_CSTA_DEVICE_IDENTIFIER)
		{
			//	无效设备，但连接正常
			l_pResultNotif->SetResult(true);
		}
		else
		{
			l_pResultNotif->SetResult(false);
		}

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_QueryDeviceInfoRet);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->QueryDeviceInfoRet(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask QueryDeviceInfoRet, RequestId: [%u], TaskId: [%u]",
				l_lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create QueryDeviceInfoRet Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create QueryDeviceInfoRet Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::SetAgentStateFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "SetAgentStateFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISetAgentStateResultNotifPtr l_pResultNotif = boost::make_shared<CSetAgentStateResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_SetAgentStateConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->SetAgentStateConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask SetAgentStateConf, RequestId: [%u], TaskId: [%u]",
				l_lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create SetAgentStateConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create SetAgentStateConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::MakeCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "MakeCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	IMakeCallResultNotifPtr l_pResultNotif = boost::make_shared<CMakeCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_MakeCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->MakeCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask MakeCallConf, RequestId: [%u], TaskId: [%u]",
				l_lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create MakeCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create MakeCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ClearConnectionFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ClearConnectionFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ClearConnectionConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ClearConnectionConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ClearConnectionConf, RequestId: [%u], TaskId: [%u]",
				l_lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ClearConnectionConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ClearConnectionConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ClearCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ClearCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ClearCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ClearCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ClearCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ClearCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ClearCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::AnswerCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "AnswerCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_AnswerCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->AnswerCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask AnswerCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create AnswerCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create AnswerCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::SingleStepConferenceFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "SingleStepConferenceFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_SingleStepConferenceConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->SingleStepConferenceConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask SingleStepConferenceConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create SingleStepConferenceConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create SingleStepConferenceConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::PickupCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "PickupCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_PickupCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->PickupCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask PickupCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create PickupCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create PickupCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ConsultationCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ConsultationCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	IConsultationCallResultNotifPtr l_pResultNotif = boost::make_shared<CConsultationCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ConsultationCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ConsultationCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConsultationCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ConsultationCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ConsultationCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::TransferCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "TransferCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ITransferCallResultNotifPtr l_pResultNotif = boost::make_shared<CTransferCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_TransferCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->TransferCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask TransferCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create TransferCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create TransferCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::DeflectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "DeflectCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_DeflectCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->DeflectCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask DeflectCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create DeflectCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create DeflectCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::HoldCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "HoldCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_HoldCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->HoldCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask HoldCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create HoldCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create HoldCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::RetrieveCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "RetrieveCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_RetrieveCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->RetrieveCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask RetrieveCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create RetrieveCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create RetrieveCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ReconnectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ReconnectCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ReconnectCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ReconnectCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ReconnectCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ReconnectCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ReconnectCallConf Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::ConferenceCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "ConferenceCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_ConferenceCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->ConferenceCallConf(l_pTask);
			/*CTaskManager::Instance()->AddSwitchEventTask(l_pTask);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConferenceCallConf, RequestId: [%u], TaskId: [%u]",
				lRequestId, l_lTaskId);*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
void CAvayaSwitchManager::MonitorDeviceFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "MonitorDeviceFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	std::string l_strDeviceNum = "";
	ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(p_lRequestId);
	if (l_pExcutedTask)
	{
		IMonitorDeviceNotifPtr l_pMonitorNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(l_pExcutedTask->GetSwitchNotif());
		if (l_pMonitorNotif)
		{
			l_strDeviceNum = l_pMonitorNotif->GetDeviceNum();
			ICC_LOG_WARNING(m_pLog, "Monitor Device: [%s] Failed !!!!", l_strDeviceNum.c_str());
		}

		CTaskManager::Instance()->DeleteExcusedTask(p_lRequestId);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "MonitorDeviceConf, ExcutedTask TaskId: [%u] Not Exist !!!!", p_lRequestId);
	}
}
void CAvayaSwitchManager::TakeOverCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ICC_LOG_WARNING(m_pLog, "TakeOverCallFailed,  RequestId: [%u], ErrorCode: [%d], ErrorMsg: [%s]",
		p_lRequestId, p_nErrorCode, p_strErrorMsg.c_str());

	ISwitchResultNotifPtr l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(false);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			l_pTask->SetTaskName(Task_TakeOverCallConf);
			l_pTask->SetSwitchNotif(l_pResultNotif);

			this->TakeOverCallConf(l_pTask);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Task Object Failed, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Create ConferenceCallConf Notif Object Failed, RequestId: [%u]", p_lRequestId);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAvayaSwitchManager::UniversalFailRet(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pSwitchResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pSwitchResultNotif)
	{
		int l_nErrorCode = l_pSwitchResultNotif->GetErrorCode();
		std::string l_strErrorMsg = l_pSwitchResultNotif->GetErrorMsg();
		long l_lRequestId = l_pSwitchResultNotif->GetRequestId();
		long l_lCSTACallRefId = l_pSwitchResultNotif->GetCSTACallRefId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lRequestId);
		if (l_pExcutedTask)
		{
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			ICC_LOG_WARNING(m_pLog, "fail ret.TaskName:[%d],CSTACallRefId:[%d],RequestId:[%d],ErrorCode:[%d]", l_nTaskName, l_lCSTACallRefId, l_lRequestId, l_nErrorCode);
			switch (l_nTaskName)
			{
			case Task_QueryDeviceInfo:
				QueryDeviceInfoFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_AgentLogin:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_AgentLogout:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_SetAgentState:
				SetAgentStateFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_MakeCall:
				MakeCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_Hangup:
				if (l_nErrorCode == NO_CONNECTION_TO_CLEAR)
				{
					CDeviceManager::Instance()->DeleteAllLogicalCallState(l_lCSTACallRefId);
					CCallManager::Instance()->DeleteCall(l_lCSTACallRefId);
				}
				ClearConnectionFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_RefuseAnswer:
				ClearCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ForcePopCall:
				ClearCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_AnswerCall:
				AnswerCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_BargeInCall:
				SingleStepConferenceFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ListenCall:
				SingleStepConferenceFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_PickupCall:
				PickupCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ConsultationCall:
				ConsultationCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_TransferCall:
				TransferCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_DeflectCall:
				DeflectCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_HoldCall:
				HoldCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_RetrieveCall:
				RetrieveCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ReconnectCall:
				ReconnectCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			case Task_ConferenceCall:
				ConferenceCallFailed(l_lRequestId, l_nErrorCode, l_strErrorMsg);
				break;
			default:
				ICC_LOG_WARNING(m_pLog, "Delete Failed ExcusedTask, TaskName: [%s] TaskId: [%u]",
					TaskNameString[l_nTaskName].c_str(), l_lRequestId);
				CTaskManager::Instance()->DeleteExcusedTask(l_lRequestId);
				break;
			}
		}
	}
}
void CAvayaSwitchManager::OpenStreamConf(ITaskPtr p_pTask)
{
	ICTIConnStateNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CCTIConnStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		m_bSwitchConnect = l_pResultNotif->GetStreamOpen();
		ICC_LOG_DEBUG(m_pLog, "OpenStreamConf,m_bSwitchConnect=%d!!!!", m_bSwitchConnect);

		if (m_pSwitchEventCallback)
		{
			m_pSwitchEventCallback->Event_CTIConnState(l_pResultNotif);
		}

		CDeviceManager::Instance()->SetSwitchConnect(m_bSwitchConnect);
		CAgentManager::Instance()->SetSwitchConnect(m_bSwitchConnect);
		if (!m_bSwitchConnect)
		{
			ICC_LOG_WARNING(m_pLog, "StreamException,=%d !!!!", l_pResultNotif->GetStreamException());
			OpenStreamProcess(true);
		}
	}
}
void CAvayaSwitchManager::QueryDeviceInfoRet(ITaskPtr p_pTask)
{
	bool l_bReOpenStream = false;

	IQueryDeviceInfoResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CQueryDeviceInfoResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		std::string l_strDeviceNum = l_pResultNotif->GetDeviceNum();
		if (!l_bResult)
		{
			m_nHeartBeatCount++;
			ICC_LOG_WARNING(m_pLog, "QueryDeviceInfo Failed, HeartBeat Count: [%d]", m_nHeartBeatCount);

			if (m_nHeartBeatCount >= MAX_HEARTBEAT)
			{
				l_bReOpenStream = true;
				m_nHeartBeatCount = 0;
			}
		}
		else
		{
			m_nHeartBeatCount = 0;
		}

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
	}

	if (l_bReOpenStream)
	{
		ICC_LOG_WARNING(m_pLog, "QueryDeviceInfo, Max HeartBeat Count, ReOpenStream");
		//重新打开流exit(0)
		OpenStreamProcess(true);
	}
}

void CAvayaSwitchManager::OpenStreamProcess(bool p_bIsReOpen)
{
	m_bSwitchConnect = false;
	if (p_bIsReOpen)
	{
		CDeviceManager::Instance()->SetSwitchConnect(false);
		CAgentManager::Instance()->SetSwitchConnect(false);

		CCallManager::Instance()->ClearAll();
		CTaskManager::Instance()->ClearTask();
		CAvayaSwitch::Instance()->PostCloseStreamMessage();
	}
	//

	if (!m_bOpenStreamThreadAlive)
	{
		m_bOpenStreamThreadAlive = true;
		m_threadOpenStream.reset(new boost::thread(boost::bind(&CAvayaSwitchManager::_OpenStreamThreadFun, this)));
	}
}

void CAvayaSwitchManager::_OpenStreamThreadFun()
{
	ICC_LOG_DEBUG(m_pLog, "=============== OpenStreamThread Start!! ===============");
	while (CAvayaSwitchManager::Instance()->m_bOpenStreamThreadAlive)
	{
		bool bIsSlaveCfgEnable = true;
		std::string l_strAvayaSlaveAESIP = CSysConfig::Instance()->GetSlaveAEServerIP();
		std::string l_strAvayaMasterAESIP = CSysConfig::Instance()->GetAEServerIP();
		if (l_strAvayaSlaveAESIP.empty() || l_strAvayaSlaveAESIP == l_strAvayaMasterAESIP)
		{
			bIsSlaveCfgEnable = false;
		}

		//if (!l_strAvayaSlaveAESIP.empty())
		if(bIsSlaveCfgEnable)
		{
			std::string l_strCurrAvayaAESIP;
			std::string l_strLastConnAvayaIP = CSysConfig::Instance()->GetLastConnectAvayaIP();
			if (l_strLastConnAvayaIP == l_strAvayaSlaveAESIP || l_strLastConnAvayaIP.empty())
			{
				//上一次连的从，当前用主
				CAvayaSwitch::Instance()->SetConnectSlaveAvayaFlag(false);
				l_strCurrAvayaAESIP = CSysConfig::Instance()->GetAEServerIP();
			}
			else
			{
				CAvayaSwitch::Instance()->SetConnectSlaveAvayaFlag(true);
				l_strCurrAvayaAESIP = l_strAvayaSlaveAESIP;
			}
			CSysConfig::Instance()->SetCurrConnectAvayaIP(l_strCurrAvayaAESIP);
			ICC_LOG_DEBUG(m_pLog, "=====================OpenStreamThread!! LastConnAvayaAESIP:%s,CurrConnAvayaAESIP:%s", l_strLastConnAvayaIP.c_str(), l_strCurrAvayaAESIP.c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "=====================OpenStreamThread Master!! =====================");
			CAvayaSwitch::Instance()->SetConnectSlaveAvayaFlag(false);
		}

		//打开流
		if (CAvayaSwitch::Instance()->OnOpenStreamEvent() > 0)
		{
			//
			CAvayaSwitchManager::Instance()->m_bOpenStreamThreadAlive = false;
			break;
		}
		else
		{
			if (bIsSlaveCfgEnable)
			{
				//如果配置了主被功能，只能退出进程来切换
				ICC_LOG_ERROR(m_pLog, "Switch Initialize failed, exit() !!!");
				exit(0);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	};

	CAvayaSwitchManager::Instance()->m_bOpenStreamThreadAlive = false;
	ICC_LOG_DEBUG(m_pLog, "===============OpenStreamThread Exit!! ===============");

}

void CAvayaSwitchManager::MonitorDeviceConf(ITaskPtr p_pTask)
{
	//根据任务号，匹配所监视设备的 DeviceNum 和 MonitorId 
	IMonitorDeviceResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CMonitorDeviceResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		long l_lMonitorId = l_pResultNotif->GetMonitorId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IMonitorDeviceNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
				boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
				if (l_pDevice)
				{
					l_pDevice->SetMonitorId(l_lMonitorId);
					l_pDevice->SetIsMonistor(true);

					std::string l_strShowMsg = m_pStrUtil->Format("MonitorDeviceConf, Set Device:%s MonitorId: %d",
						l_strDeviceNum.c_str(), l_lMonitorId);
					ICC_LOG_DEBUG(m_pLog, "%s", l_strShowMsg.c_str());
					ShowMsg(l_strShowMsg);

					if (l_pDevice->GetDeviceType() == CTC_TYPE_ACDGROUP)
					{
						IRouteRegisterReqNotifPtr l_pRouteRegisterReqNotif = boost::make_shared<CRouteRegisterReqNotif>();
						if (l_pRouteRegisterReqNotif)
						{
							l_pRouteRegisterReqNotif->SetDeviceNum(l_strDeviceNum);

							long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_RouteRegisterReq, l_pRouteRegisterReqNotif);
							std::string l_strShowMsg = m_pStrUtil->Format("RouteRegisterReq  Device: %s, RequestId: %d ",
								l_strDeviceNum.c_str(), l_lTaskId);
							ICC_LOG_DEBUG(m_pLog, "%s", l_strShowMsg.c_str());
						}
					}
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::RouteRegisterConf(ITaskPtr p_pTask)
{
	IRouteRegisterResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CRouteRegisterResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		long l_lRouteRegId = l_pResultNotif->GetRouteRegId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IRouteRegisterReqNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRouteRegisterReqNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
				boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
				if (l_pDevice)
				{
					l_pDevice->SetRouteRegId(l_lRouteRegId);

					std::string l_strShowMsg = m_pStrUtil->Format("RouteRegisterConf, Set Device:%s lRouteRegId: %d",
						l_strDeviceNum.c_str(), l_lRouteRegId);
					ICC_LOG_DEBUG(m_pLog, "%s", l_strShowMsg.c_str());
					ShowMsg(l_strShowMsg);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::SetAgentStateConf(ITaskPtr p_pTask)
{
	ISetAgentStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSetAgentStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				bool l_bPreLogin = l_pRequestNotif->GetPreLogin();
				std::string l_strAgent = l_pRequestNotif->GetAgentId();
				std::string l_strACD = l_pRequestNotif->GetACDGrp();
				std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();
				std::string l_strReadyState = l_pRequestNotif->GetReadyState();

				l_pResultNotif->SetAgentId(l_strAgent);
				l_pResultNotif->SetACDGrp(l_strACD);
				l_pResultNotif->SetLoginMode(l_strLoginMode);
				l_pResultNotif->SetReadyState(l_strReadyState);

				E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
				switch (l_nTaskName)
				{
				case Task_AgentLogin:
					if (l_bResult)
					{
						CAgentManager::Instance()->LoginModeSync(l_strAgent, l_strACD, AGENT_MODE_LOGIN);
					}
					if (m_pSwitchEventCallback)
					{
						m_pSwitchEventCallback->CR_AgentLogin(l_pResultNotif);
					}
					break;
				case Task_AgentLogout:
					if (l_bResult)
					{
						CAgentManager::Instance()->LoginModeSync(l_strAgent, l_strACD, AGENT_MODE_LOGOUT);
					}
					m_pSwitchEventCallback->CR_AgentLogout(l_pResultNotif);
					break;
				case Task_SetAgentState:
					if (l_bResult)
					{
						if (l_bPreLogin)
						{
							CAgentManager::Instance()->LoginModeSync(l_strAgent, l_strACD, AGENT_MODE_LOGIN);

							//	先执行 Login，再执行 ready/notready
							l_pRequestNotif->SetPreLogin(false);
							SetAgentState(l_pExcutedTask);

							return;
						}

						CAgentManager::Instance()->ReadyStateSync(l_strAgent, l_strACD, l_strReadyState);
					}
					if (m_pSwitchEventCallback)
					{
						m_pSwitchEventCallback->CR_SetAgentState(l_pResultNotif);
					}
					break;
				default:
					break;
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "SetAgentStateConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::QueryAgentStateRet(ITaskPtr p_pTask)
{
	IQueryAgentStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CQueryAgentStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		std::string l_strAgentState = l_pResultNotif->GetAgentState();
		bool l_bResult = l_pResultNotif->GetResult();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IQueryAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryAgentStateNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();

				CAgentManager::Instance()->OnAgentState(l_strDeviceNum, l_strAgentState);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "QueryAgentStateRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::MakeCallConf(ITaskPtr p_pTask)
{
	IMakeCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CMakeCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IMakeCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMakeCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				/*long l_lCSTACallRefId = l_pResultNotif->GetCSTACallRefId();*/
				std::string l_strCallerId = l_pRequestNotif->GetCallerId();
				std::string l_strCalledId = l_pRequestNotif->GetCalledId();
				std::string l_strCaseId = l_pRequestNotif->GetCaseId();

				l_pResultNotif->SetCallerId(l_strCallerId);
				l_pResultNotif->SetCalledId(l_strCalledId);
				l_pResultNotif->SetCaseId(l_strCaseId);

				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_MakeCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "MakeCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ClearConnectionConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
//			std::string l_strEventType = l_pExcutedTask->GetEventType();
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_Hangup:
				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_Hangup(l_pResultNotif);
				}
				break;
			case Task_ForcePopCall:		//	强拆
				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_ForcePopCall(l_pResultNotif);
				}
				break;
			case Task_TakeOverCall:		//	接管话务
				this->TakeOverCallConf(p_pTask);
				break;
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ClearConnectionConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ClearCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			long l_lCSTACallRefId = DEFAULT_CALLREFID;
			IForcePopCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CForcePopCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
				CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId);
			}
			
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			/*case Task_ForcePopCall:		//	强拆
				if (m_pSwitchEventCallback)
				{
					if (CCallManager::Instance()->GetConfMemCount(l_lCSTACallRefId) > 0)
					{
						CCallManager::Instance()->DeleteCall(l_lCSTACallRefId);
					}

					m_pSwitchEventCallback->CR_ForcePopCall(l_pResultNotif);
				}
				break;*/
			case Task_ClearCall:		//	强拆整个话务
				if (m_pSwitchEventCallback)
				{
					if (CCallManager::Instance()->GetConfMemCount(l_lCSTACallRefId) > 0)
					{
						CCallManager::Instance()->DeleteCall(l_lCSTACallRefId);
					}

					m_pSwitchEventCallback->CR_ClearCall(l_pResultNotif);
				}
				break;
			case Task_RefuseAnswer:		//正常拒接
				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_RefuseAnswer(l_pResultNotif);
				}
				break;
			case Task_RefuseBlackCall:	//黑名单拦截
				break;
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ClearCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::AnswerCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			long l_lCSTACallRefId = l_pRequestNotif->GetCSTACallRefId();
			std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

			l_pResultNotif->SetCSTACallRefId(l_lCSTACallRefId);
			l_pResultNotif->SetDeviceNum(l_strDevice);}*/
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_AnswerCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "AnswerCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

/*
void CAvayaSwitchManager::RefuseAnswerConf(ITaskPtr l_pTask)
{
ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(pTask->GetSwitchNotif());
if (l_pResultNotif)
{
long l_lTaskId = pResultNotif->GetRequestId();

ITaskPtr pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
if (pExcutedTask)
{
std::string strEventType = pExcutedTask->GetEventType();
E_TASK_NAME nTaskName = pExcutedTask->GetTaskName();
switch (nTaskName)
{
case Task_Hangup:
m_pSwitchEventCallback->CR_Hangup(pResultNotif);
break;
case Task_ForcePopCall:
m_pSwitchEventCallback->CR_ForcePopCall(pResultNotif);
break;
case Task_RefuseAnswer:
if (strEventType.compare(EventType_BlackListCall) == 0)
{
//黑名单拦截
}
else
{
//正常拒接
m_pSwitchEventCallback->CR_RefuseAnswer(pResultNotif);
}
break;
default:
break;
}

CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
}
}
}*/

void CAvayaSwitchManager::SingleStepConferenceConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		bool l_bResult = l_pResultNotif->GetResult();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			E_TASK_NAME l_nTaskName = l_pExcutedTask->GetTaskName();
			switch (l_nTaskName)
			{
			case Task_ListenCall:
				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_ListenCall(l_pResultNotif/*boost::dynamic_pointer_cast<CListenCallNotif>(l_pRequestNotif)*/);
				}
				break;
			case Task_BargeInCall:
				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_BargeInCall(l_pResultNotif/*boost::dynamic_pointer_cast<CBargeInCallNotif>(l_pRequestNotif)*/);
				}
				break;
			case Task_TakeOverCall:		//	接管话务
				this->TakeOverCallConf(p_pTask);
			default:
				break;
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "SingleStepConferenceConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::PickupCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IPickupCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CPickupCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
			l_pResultNotif->SetTarget(l_pRequestNotif->GetTarget());
			}*/
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_PickupCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "PickupCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

/*
void CAvayaSwitchManager::BargeInCallConf(ITaskPtr pTask)
{
ISwitchResultNotifPtr pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(pTask->GetSwitchNotif());
if (pResultNotif)
{
m_pSwitchEventCallback->CR_BargeInCall(pResultNotif);

long l_lTaskId = pResultNotif->GetRequestId();
ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
if (l_pExcutedTask)
{
/ *IBargeInCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CBargeInCallNotif>(l_pExcutedTask->GetSwitchNotif());
if (l_pRequestNotif)
{
pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
pResultNotif->SetTarget(l_pRequestNotif->GetTarget());

m_pSwitchEventCallback->CR_BargeInCall(pResultNotif);
}* /

CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
}
}
}*/

void CAvayaSwitchManager::ConsultationCallConf(ITaskPtr p_pTask)
{
	IConsultationCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CConsultationCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			IConsultationCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConsultationCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				long l_lActiveCSTACallRefId = l_pResultNotif->GetActiveCSTACallRefId();
				bool l_bIsTransferCall = l_pRequestNotif->GetIsTransferCall();
				std::string l_strHeldCTICallRefId = l_pRequestNotif->GetCTICallRefId();
				std::string l_strSponsor = l_pRequestNotif->GetSponsor();
				std::string l_strTargetACDGrp = l_pRequestNotif->GetTargetACDGrp();
				std::string l_strTargetDevice = l_pRequestNotif->GetTargetDevice();
				std::string l_strOriginalCallerId = "";
				std::string l_strOriginalCalledId = "";
				if (CCallManager::Instance()->GetOriginalCallByCTICallRefId(l_strHeldCTICallRefId, l_strOriginalCallerId, l_strOriginalCalledId))
				{
					CCallManager::Instance()->SetOriginalCallerId(l_lActiveCSTACallRefId, l_strOriginalCallerId);
					CCallManager::Instance()->SetOriginalCalledId(l_lActiveCSTACallRefId, l_strOriginalCalledId);
				}
				CCallManager::Instance()->SetIsTransferCall(l_lActiveCSTACallRefId, l_bIsTransferCall);

				if (!l_bIsTransferCall)
				{
					//	会议产生的咨询，保存会议主持人
					CCallManager::Instance()->SetConfCompere(l_lActiveCSTACallRefId, l_strSponsor);
				}

				l_pResultNotif->SetHeldCTICallRefId(l_strHeldCTICallRefId);
				l_pResultNotif->SetOriginalCallerId(l_strOriginalCallerId);
				l_pResultNotif->SetOriginalCalledId(l_strOriginalCalledId);
				l_pResultNotif->SetSponsor(l_strSponsor);
				
				std::string l_strTargetCheck = CSysConfig::Instance()->GetCheckNum(l_strTargetDevice);
				//l_pRequestNotif->SetTargetDevice(l_strTargetCheck);
				l_pResultNotif->SetTargetDevice(l_strTargetCheck);

				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_ConsultationCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ConsultationCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::TransferCallConf(ITaskPtr p_pTask)
{
	ITransferCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CTransferCallResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			ITransferCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTransferCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				l_pResultNotif->SetHeldCTICallRefId(l_pRequestNotif->GetHeldCTICallRefId());
				l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
				l_pResultNotif->SetTargetDevice(l_pRequestNotif->GetTarget());

				if (m_pSwitchEventCallback)
				{
					m_pSwitchEventCallback->CR_TransferCall(l_pResultNotif);
				}
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "TransferCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::DeflectCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			/*IDeflectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeflectCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
			l_pResultNotif->SetSponsor(l_pRequestNotif->GetSponsor());
			l_pResultNotif->SetTarget(l_pRequestNotif->GetTarget());

			m_pSwitchEventCallback->CR_DeflectCall(l_pResultNotif);
			}*/
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_DeflectCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "DeflectCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::HoldCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_HoldCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "HoldCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::RetrieveCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_RetrieveCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "RetrieveCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::ReconnectCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_ReconnectCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ReconnectCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::ConferenceCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_ConferenceCall(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "ConferenceCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::TakeOverCallConf(ITaskPtr p_pTask)
{
	ISwitchResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSwitchResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		bool l_bResult = l_pResultNotif->GetResult();
		long l_lTaskId = l_pResultNotif->GetRequestId();
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			ITakeOverCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTakeOverCallNotif>(l_pExcutedTask->GetSwitchNotif());
			if (l_pRequestNotif)
			{
				bool l_bDoNext = false;
				std::string l_strNextStep = Step_StopListenCall;
				std::string l_strCurrentStep = l_pRequestNotif->GetStep();
				std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
				std::string l_strSponsor = l_pRequestNotif->GetSponsor();
				std::string l_strTarget = l_pRequestNotif->GetTarget();
				long l_lOriginalRequestId = l_pRequestNotif->GetOriginalRequestId();

				//1、挂断监听者；2、监听者强插到通话；3、挂断被监听的坐席
				//StopListenCall, BargeInCall, Hangup
				if (l_strCurrentStep.compare(Step_StopListenCall) == 0)
				{
					l_bDoNext = true;
					l_strNextStep = Step_BargeInCall;
					l_lOriginalRequestId = l_lTaskId;
					ICC_LOG_DEBUG(m_pLog, "TakeOverCallConf, ExcutedTask TaskId: [%u], OriginalRequestId: [%u] Step StopListenCall Success, Next Step: BargeInCall", 
						l_lTaskId, l_lOriginalRequestId);
				}
				else if (l_strCurrentStep.compare(Step_BargeInCall) == 0)
				{
					l_bDoNext = true;
					l_strNextStep = Step_Hangup;
					ICC_LOG_DEBUG(m_pLog, "TakeOverCallConf, ExcutedTask TaskId: [%u], OriginalRequestId: [%u] Step BargeInCall Success, Next Step: Hangup", 
						l_lTaskId, l_lOriginalRequestId);
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "TakeOverCallConf, ExcutedTask TaskId: [%u] Success, OriginalRequestId: [%u] Step Over", 
						l_lTaskId, l_lOriginalRequestId);
				}

				if (l_bResult && l_bDoNext)
				{
					//挂断监听者成功，继续执行 强插 或 挂断被监听坐席
					ITakeOverCallNotifPtr l_pNextStepRequestNotif = boost::make_shared<CTakeOverCallNotif>();
					if (l_pNextStepRequestNotif)
					{
						l_pNextStepRequestNotif->SetCTICallRefId(l_strCTICallRefId);
						l_pNextStepRequestNotif->SetSponsor(l_strSponsor);
						l_pNextStepRequestNotif->SetTarget(l_strTarget);
						l_pNextStepRequestNotif->SetStep(l_strNextStep);
						l_pNextStepRequestNotif->SetOriginalRequestId(l_lOriginalRequestId);

						long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_TakeOverCall, l_pNextStepRequestNotif);
						ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], TakeOverCall Step [%s], CallRefId [%s], Sponsor [%s], Device [%s]",
							l_lNewTaskId, l_strNextStep.c_str(), l_strCTICallRefId.c_str(), l_strSponsor.c_str(), l_strTarget.c_str());
					}
					else
					{
						ICC_LOG_FATAL(m_pLog, "Create CTakeOverCallNotif Object Failed !!!");
					}
				} 
				else
				{
					if (m_pSwitchEventCallback)
					{
						// 接管分三步执行，各步的 RequestId 不同，最后的结果需返回原 RequestId
						ITakeOverCallResultNotifPtr pTempNotif = boost::make_shared<CTakeOverCallResultNotif>();
						if (pTempNotif)
						{
							pTempNotif->SetRequestId(l_lOriginalRequestId);
							pTempNotif->SetCTICallRefId(l_strCTICallRefId);
							pTempNotif->SetSponsor(l_strSponsor);
							pTempNotif->SetTarget(l_strTarget);
							pTempNotif->SetResult(l_bResult);
							pTempNotif->SetErrorCode(l_pResultNotif->GetErrorCode());
							pTempNotif->SetErrorMsg(l_pResultNotif->GetErrorMsg());
							
							m_pSwitchEventCallback->CR_TakeOverCall(pTempNotif);
						}
						else
						{
							ICC_LOG_FATAL(m_pLog, "Create CSwitchResultNotif Object Failed !!!");
						}
					}
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "TakeOverCallConf, ExcutedTask TaskId: [%u], RequestNotif Not Exist !!!!", l_lTaskId);
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "TakeOverCallConf, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

/*
void CAvayaSwitchManager::AddConferencePartyConf(ITaskPtr pTask)
{
IAddConferencePartyNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CAddConferencePartyNotif>(pTask->GetSwitchNotif());
if (pResultNotif)
{
long l_lTaskId = pResultNotif->GetRequestId();

ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
if (l_pExcutedTask)
{
IAddConferencePartyNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAddConferencePartyNotif>(l_pExcutedTask->GetSwitchNotif());
if (l_pRequestNotif)
{
pResultNotif->SetCompere(l_pRequestNotif->GetCompere());
pResultNotif->SetTarget(l_pRequestNotif->GetTarget());
pResultNotif->SetTargetDeviceType(l_pRequestNotif->GetTargetDeviceType());

m_pSwitchEventCallback->CR_AddConferenceParty(pResultNotif);
}

CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
}
}
}

void CAvayaSwitchManager::DeleteConferencePartyConf(ITaskPtr pTask)
{
IDeleteConferencePartyNotifPtr pResultNotif = boost::dynamic_pointer_cast<CDeleteConferencePartyNotif>(pTask->GetSwitchNotif());
if (pResultNotif)
{
long l_lTaskId = pResultNotif->GetRequestId();

ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
if (l_pExcutedTask)
{
IDeleteConferencePartyNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeleteConferencePartyNotif>(l_pExcutedTask->GetSwitchNotif());
if (l_pRequestNotif)
{
pResultNotif->SetCompere(l_pRequestNotif->GetCompere());
pResultNotif->SetTarget(l_pRequestNotif->GetTarget());

m_pSwitchEventCallback->CR_DeleteConferenceParty(pResultNotif);
}

CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
}
}
}*/

void CAvayaSwitchManager::GetAESConnStateRet(ITaskPtr p_pTask)
{
	IGetCTIConnStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetCTIConnStateResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetCTIConnState(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetAESConnStateRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetDeviceListRet(ITaskPtr p_pTask)
{
	IGetDeviceListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetDeviceListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetDeviceList(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetDeviceListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetACDListRet(ITaskPtr p_pTask)
{
	IGetACDListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetACDListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetACDList(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetACDListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetAgentListRet(ITaskPtr p_pTask)
{
	IGetAgentListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetAgentListResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetAgentList(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetAgentListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::GetFreeAgentListRet(ITaskPtr p_pTask)
{
	IGetFreeAgentResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetFreeAgentResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetFreeAgentList(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetFreeAgentListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

void CAvayaSwitchManager::GetCallListRet(ITaskPtr pTask)
{
	IGetCallListResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetCallListResultNotif>(pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetCallList(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetCallListRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}
void CAvayaSwitchManager::GetReadyAgentRet(ITaskPtr p_pTask)
{
	IGetReadyAgentResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetReadyAgentResultNotif>(p_pTask->GetSwitchNotif());
	if (l_pResultNotif)
	{
		long l_lTaskId = l_pResultNotif->GetRequestId();

		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExcutedTask(l_lTaskId);
		if (l_pExcutedTask)
		{
			if (m_pSwitchEventCallback)
			{
				m_pSwitchEventCallback->CR_GetReadyAgent(l_pResultNotif);
			}

			CTaskManager::Instance()->DeleteExcusedTask(l_lTaskId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetReadyAgentRet, ExcutedTask TaskId: [%u] Not Exist !!!!", l_lTaskId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

int CAvayaSwitchManager::QueryDeviceInfo(ITaskPtr p_pTask)
{
	IQueryDeviceInfoNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryDeviceInfoNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lTaskId = p_pTask->GetTaskId();

			bool l_bRet = CAvayaSwitch::Instance()->QueryDeviceInfo(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Query Device [%s] Info Failed!!",
					l_lTaskId, l_strDeviceNum.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Query Device [%s] Info Success!!",
					l_lTaskId, l_strDeviceNum.c_str());
			}
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::MonitorDevice(ITaskPtr p_pTask)
{
	IMonitorDeviceNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice && !l_pDevice->GetIsMonistor())
		{
			long l_lTaskId = p_pTask->GetTaskId();

			bool l_bRet = CAvayaSwitch::Instance()->MonitorDevice(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] Monitor Failed!!",
					l_lTaskId, l_strDeviceNum.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] Monitor Success!!",
					l_lTaskId, l_strDeviceNum.c_str());
			}
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::MonitorCallsViaDevice(ITaskPtr p_pTask)
{
	IMonitorDeviceNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMonitorDeviceNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			long l_lTaskId = p_pTask->GetTaskId();

			bool l_bRet = CAvayaSwitch::Instance()->MonitorCallsViaDevice(l_strDeviceNum, l_lTaskId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device[ %s] MonitorCallsViaDevice Failed!!",
					l_lTaskId, l_strDeviceNum.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] MonitorCallsViaDevice Success!!",
					l_lTaskId, l_strDeviceNum.c_str());
			}
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::RouteRegisterReq(ITaskPtr p_pTask)
{
	IRouteRegisterReqNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRouteRegisterReqNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strDeviceNum = l_pRequestNotif->GetDeviceNum();

		bool l_bRet = CAvayaSwitch::Instance()->RouteRegisterReq(l_strDeviceNum, l_lRequestId);
		if (!l_bRet)
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], RouteRegisterReq  Device [%s] Failed !",
				l_lRequestId, l_strDeviceNum.c_str());

			return RESULT_FAILED;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], RouteRegisterReq  Device [%s] Success !",
				l_lRequestId, l_strDeviceNum.c_str());
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::RouteRequest(ITaskPtr p_pTask)
{
	IRouteCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRouteCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCallerId = l_pRequestNotif->GetCallerId();
		std::string l_strRouteDest = l_pRequestNotif->GetRouteDest();
		long l_lCSTACallRefId = l_pRequestNotif->GetCSTACallRefId();
		long l_lRegisterReqId = l_pRequestNotif->GetRegisterReqId();
		long l_lCrossRefId = l_pRequestNotif->GetCrossRefId();
		bool l_bIsBlackCall = l_pRequestNotif->GetIsBlackCall();

		if (l_bIsBlackCall)
		{
			// 路由目标
			bool l_bRet = CAvayaSwitch::Instance()->RouteSelectInv(l_lRegisterReqId, l_lCrossRefId, "108", l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "RequestId [%u], RouteSelectInv RegisterReqId [%d] CrossRefId [%d] CallerId [%s] RouteSelected [%s] Failed !",
					l_lRequestId, l_lRegisterReqId, l_lCrossRefId, l_strCallerId.c_str(), l_strRouteDest.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "RequestId [%u], RouteSelectInv RegisterReqId [%d] CrossRefId [%d] CallerId [%s] RouteSelected [%s] Success !",
					l_lRequestId, l_lRegisterReqId, l_lCrossRefId, l_strCallerId.c_str(), l_strRouteDest.c_str());
			}
		}
		else
		{
			bool l_bRet = CAvayaSwitch::Instance()->RouteEndInv(l_lRegisterReqId, l_lCrossRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "RequestId [%u], RouteEndInv CallerId [%s] Failed !",
					l_lRequestId, l_strCallerId.c_str());
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "RequestId [%u], RouteEndInv CallerId [%s] Success !",
					l_lRequestId, l_strCallerId.c_str());
			}
		}
	}

	return RESULT_SUCCESS;
}


int CAvayaSwitchManager::AgentLogin(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();

		/*boost::shared_ptr<CAgent> l_pAgent = CAgentManager::Instance()->GetAgentByDn(l_strACD, l_strDeviceNum);
		if (l_pAgent && l_pAgent->IsLogin())*/
		if (CAgentManager::Instance()->AgentIsLogin(l_strDeviceNum, l_strACD))
		{
			ISetAgentStateResultNotifPtr l_pResultnotif = boost::make_shared<CSetAgentStateResultNotif>();
			if (l_pResultnotif)
			{
				l_pResultnotif->SetRequestId(l_lRequestId);
				l_pResultnotif->SetAgentId(l_strDeviceNum);
				l_pResultnotif->SetACDGrp(l_strACD);
				l_pResultnotif->SetLoginMode(l_strLoginMode);
				l_pResultnotif->SetResult(true);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, l_pResultnotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, SetAgentStateConf, TaskId [%u], RequestId [%u], Agent [%s] Was Login!!",
					l_lTaskId, l_lRequestId, l_strDeviceNum.c_str());
			}

			return RESULT_SUCCESS;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			std::string l_strAgent = l_pDevice->GetAgentId();
			std::string l_strPsw = l_pDevice->GetAgentPsw();

			bool l_bRet = CAvayaSwitch::Instance()->SetAgentState(l_strDeviceNum, l_strACD, l_strAgent, l_strPsw, AM_LOG_IN, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Login Failed!!",
					l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Login Success!!",
					l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str());
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::AgentLogout(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strLoginMode = l_pRequestNotif->GetLoginMode();

		/*boost::shared_ptr<CAgent> l_pAgent = CAgentManager::Instance()->GetAgentByDn(l_strACD, l_strDeviceNum);
		if (l_pAgent && !l_pAgent->IsLogin())*/
		if (!CAgentManager::Instance()->AgentIsLogin(l_strDeviceNum, l_strACD))
		{
			ISetAgentStateResultNotifPtr l_pResultnotif = boost::make_shared<CSetAgentStateResultNotif>();
			if (l_pResultnotif)
			{
				l_pResultnotif->SetRequestId(l_lRequestId);
				l_pResultnotif->SetAgentId(l_strDeviceNum);
				l_pResultnotif->SetACDGrp(l_strACD);
				l_pResultnotif->SetLoginMode(l_strLoginMode);
				l_pResultnotif->SetResult(true);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, l_pResultnotif);
				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], RequestId [%u], Agent [%s] Was Logout !",
					l_lTaskId, l_lRequestId, l_strDeviceNum.c_str());
			}

			return RESULT_SUCCESS;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
		if (l_pDevice)
		{
			std::string l_strAgent = l_pDevice->GetAgentId();
			std::string l_strPsw = l_pDevice->GetAgentPsw();

			bool l_bRet = CAvayaSwitch::Instance()->SetAgentState(l_strDeviceNum, l_strACD, l_strAgent, l_strPsw, AM_LOG_OUT, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Logout Failed!!",
					l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str());

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Logout Success!!",
					l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str());
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::SetAgentState(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	ISetAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strACD = l_pRequestNotif->GetACDGrp();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();
		std::string l_strReadyState = l_pRequestNotif->GetReadyState();

		std::string strLoginMode;
		std::string strAgentState;

		CAgentManager::Instance()->GetAgentState(l_strDeviceNum, strLoginMode, strAgentState);
		if (l_strReadyState == strAgentState)
		{
			ISetAgentStateResultNotifPtr l_pResultnotif = boost::make_shared<CSetAgentStateResultNotif>();
			if (l_pResultnotif)
			{
				l_pResultnotif->SetRequestId(l_lRequestId);
				l_pResultnotif->SetAgentId(l_strDeviceNum);
				l_pResultnotif->SetACDGrp(l_strACD);
				l_pResultnotif->SetLoginMode(strLoginMode);			
				l_pResultnotif->SetResult(true);

				long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, l_pResultnotif);
				ICC_LOG_DEBUG(m_pLog, "state is same, not send request to avaya, TaskId [%u], RequestId [%u], Agent [%s] Was Login!!",
					l_lTaskId, l_lRequestId, l_strDeviceNum.c_str());
			}

			return RESULT_SUCCESS;
		}

		if (!CAgentManager::Instance()->AgentIsLogin(l_strDeviceNum, l_strACD) && !m_bCTITestModeFlag)
		{
			// Agent 未 Login，先执行 Login
			l_pRequestNotif->SetPreLogin(true);
			l_pRequestNotif->SetLoginMode(LoginMode_Login);

			return AgentLogin(p_pTask);
		}
		else
		{
			// Agent 已 Login，执行 ready/notready
			int l_nMode = AM_NOT_READY;
			if (l_strReadyState.compare(ReadyState_Ready) == 0)
			{
				l_nMode = AM_READY;
			}

			boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
			if (l_pDevice)
			{
				if (l_nMode == AM_NOT_READY)
				{
					//	最小在席数判断，接警席配置文件中的ACD默认为 110 ，
					//	按分机号重新获取取 ACDGrp
					std::string l_strRealACDGrp = l_strACD;

					CAgentManager::Instance()->GetACDGrpByDn(l_strRealACDGrp, l_strDeviceNum);
					int l_nReadyAgentCount = CAgentManager::Instance()->GetReadyAgentCount(l_strRealACDGrp);
					if (l_nReadyAgentCount <= CSysConfig::Instance()->GetMinOnlineAgentNum())
					{
						ICC_LOG_WARNING(m_pLog, "MinOnlineAgentNum [%d], TaskId [%u], Set Agent [%s] State %s Failed !",
							l_nReadyAgentCount, l_lRequestId, l_strDeviceNum.c_str(), l_strReadyState.c_str());

						return ERROR_MIN_ONLINE_AGENT;
					}
				}

				std::string l_strAgent = l_pDevice->GetAgentId();
				std::string l_strPsw = l_pDevice->GetAgentPsw();

				bool l_bRet = false;
				if (m_bCTITestModeFlag)
				{
					if (!CAgentManager::Instance()->AgentIsLogin(l_strDeviceNum, l_strACD))
					{
						CAgentManager::Instance()->AgentLogin(l_strDeviceNum, l_strACD);
					}
					CAgentManager::Instance()->ReadyStateSync(l_strDeviceNum, l_strACD, l_strReadyState);

					ICC_LOG_DEBUG(m_pLog, "CTITest TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Set State [%s] Success!!",
						l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str(), l_strReadyState.c_str());

					l_bRet = true;
					CSTASetAgentStateConfEvent_t l_setAgentState;
					CAvayaEvent::Instance()->SetAgentStateConf(l_lRequestId, l_setAgentState);
				}
				else
				{
					l_bRet = CAvayaSwitch::Instance()->SetAgentState(l_strDeviceNum, l_strACD, l_strAgent, l_strPsw, l_nMode, l_lRequestId);
				}
				
				if (!l_bRet)
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Set State [%s] Failed!!",
						l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str(), l_strReadyState.c_str());

					return RESULT_FAILED;
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ACD [%s] Agent [%s:%s] Set State [%s] Success!!",
						l_lRequestId, l_strDeviceNum.c_str(), l_strACD.c_str(), l_strAgent.c_str(), l_strPsw.c_str(), l_strReadyState.c_str());
				}
			}
			else
			{
				return ERROR_INVALID_DEVICE;
			}
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::QueryAgentState(ITaskPtr p_pTask)
{
	/*if (!m_bSwitchConnect)
	{
	return RESULT_FAILED;
	}*/

	IQueryAgentStateNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CQueryAgentStateNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strDeviceNum = l_pRequestNotif->GetAgentId();

		bool l_bRet = CAvayaSwitch::Instance()->QueryAgentState(l_strDeviceNum, l_lRequestId);
		if (!l_bRet)
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Get Agent [%s] State Failed !",
				l_lRequestId, l_strDeviceNum.c_str());

			return RESULT_FAILED;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Get Agent [%s] State Success !",
				l_lRequestId, l_strDeviceNum.c_str());
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::MakeCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IMakeCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CMakeCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCallerId = l_pRequestNotif->GetCallerId();
		std::string l_strCalledId = l_pRequestNotif->GetCalledId();
		std::string l_strTargetType = l_pRequestNotif->GetTargetPhoneType();

		if (l_strCallerId.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed , CallerId is Empty!",
				l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

			return ERROR_INVALID_CALLING_DEVICE;
		}
		if (l_strCalledId.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed  , CalledId is Empty!",
				l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

			return ERROR_INVALID_CALLED_DEVICE;
		}

		std::string l_strDestNum = l_strCalledId;
		if (CSysConfig::Instance()->GetOutCallNum(l_strCalledId, l_strDestNum))
		{
			l_strCalledId = l_strDestNum;
		}
		else
		{
			l_strCalledId = CSysConfig::Instance()->GetOutCallNumEx(l_strCalledId);
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strCallerId);
		if (l_pDevice)
		{
			if (m_bCTITestModeFlag)
			{
				if (g_bCTITestStart)
				{
					//需要限制下，不允许坐席呼叫坐席,两个坐席的状态不好模拟
					if (!CDeviceManager::Instance()->FindDeviceByDn(l_strCalledId))
					{
						std::string l_strCallRefId;
						if (m_CTITestUdpHelper->CTITestCallOut(l_lRequestId, l_strCallerId, l_strCalledId, l_strCallRefId))
						{
							ICC_LOG_DEBUG(m_pLog, "CTITest TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Success !",
								l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

							return RESULT_SUCCESS;
						}
					}
					
				}
				ICC_LOG_ERROR(m_pLog, "CTITest TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed,bCTITestStart=%d !",
					l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str(), g_bCTITestStart);

				return RESULT_FAILED;
			}
			else
			{
				bool l_bRet = CAvayaSwitch::Instance()->MakeCall(l_strCallerId, l_strCalledId, l_lRequestId);
				if (!l_bRet)
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Failed !",
						l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());

					return RESULT_FAILED;
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], MakeCall CallerId [%s] CalledId [%s] Success !",
						l_lRequestId, l_strCallerId.c_str(), l_strCalledId.c_str());
				}
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::Hangup(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			if (m_bCTITestModeFlag)
			{
				std::string l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strTalkTime;
				CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strTalkTime);
				//CAvayaEvent::Instance()->DeviceHangup(l_lCSTACallRefId, l_strCalledId, l_strCalledId);
				CAvayaEvent::Instance()->DeviceHangup(l_lCSTACallRefId, l_strDevice, l_strDevice);
				
				CSTAClearCallConfEvent_t l_clearCall;
				CAvayaEvent::Instance()->ClearCallConf(l_lRequestId, l_clearCall);

			
				ICC_LOG_DEBUG(m_pLog, "CTITest TaskId [%u], Sopnsor [%s] Hangup Device [%s] CallRefId [%u] Success !",
					l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_SUCCESS;
			}
			else
			{
				std::string l_strE1RelayIndex = CCallManager::Instance()->GetE1RelayIndexByNumber(l_lCSTACallRefId, l_strDevice);
				bool l_bDynamicId = false;
				if (!l_strE1RelayIndex.empty())
				{
					l_strDevice = l_strE1RelayIndex;
					l_bDynamicId = true;
				}

				bool l_bRet = CAvayaSwitch::Instance()->ClearConnection(l_strDevice, l_lCSTACallRefId, l_lRequestId, l_bDynamicId);
				if (!l_bRet)
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], Sopnsor [%s] Hangup Device [%s] CallRefId [%u] Failed !",
						l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lCSTACallRefId);

					return RESULT_FAILED;
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Sopnsor [%s] Hangup Device [%s],E1_Relay:[%s] CallRefId [%u],m_bDynamicId:%d Success !",
						l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_strE1RelayIndex.c_str(), l_lCSTACallRefId, l_bDynamicId);
				}
			}
			
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::AnswerCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDevice);
		if (l_pDevice)
		{
			if (m_bCTITestModeFlag)
			{
				//AVAYA模拟测试模式
				if (m_pSwitchEventCallback)
				{
					std::string l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strTalkTime;
					CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strTalkTime);
					
					CAvayaEvent::Instance()->CallTalking(l_lCSTACallRefId, l_strCallerId, l_strCalledId, l_strDevice);

					ISwitchResultNotifPtr pSwitchResultNotif = boost::make_shared<CSwitchResultNotif>();
					pSwitchResultNotif->SetRequestId(l_lRequestId);
					pSwitchResultNotif->SetResult(true);
					//pSwitchResultNotif->SetErrorCode(nErrorCode);
					//pSwitchResultNotif->SetErrorMsg(strErrorMsg);
					
					m_pSwitchEventCallback->CR_AnswerCall(pSwitchResultNotif);
					//AnswerCallConf(pSwitchResultNotif);
					CTaskManager::Instance()->DeleteExcusedTask(l_lRequestId);

					ICC_LOG_DEBUG(m_pLog, "CTITest TaskId [%u], AnswerCall Device [%s],callerId[%s],calledId[%s] CallRefId [%u] Success !",l_lRequestId, 
						l_strDevice.c_str(), l_strCallerId.c_str(), l_strCalledId.c_str(), l_lCSTACallRefId);
				}
			}
			else
			{
				bool l_bRet = CAvayaSwitch::Instance()->AnswerCall(l_strDevice, l_lCSTACallRefId, l_lRequestId);
				if (!l_bRet)
				{
					ICC_LOG_ERROR(m_pLog, "TaskId [%u], AnswerCall Device [%s] CallRefId [%u] Failed !",
						l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);

					return RESULT_FAILED;
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "TaskId [%u], AnswerCall Device [%s] CallRefId [%u] Success !",
						l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);
				}
			}
			
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RefuseAnswer(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDevice);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearCall(l_strDevice, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], RefuseCall Device [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], RefuseCall Device [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RefuseBlackCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IAnswerCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CAnswerCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strDevice = l_pRequestNotif->GetDeviceNum();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDevice);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearCall(l_strDevice, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], RefuseBlcakCall Device [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], RefuseBlcakCall Device [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strDevice.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::ClearCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	IClearCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CClearCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			ICC_LOG_ERROR(m_pLog, "not find TaskId [%u], ClearCall Sponsor [%s] Target [%s] CallRefId [%u] !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearCall(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], ClearCall Sponsor [%s] Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], ClearCall Sponsor [%s] Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::ListenCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IListenCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CListenCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ListenCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ListenCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}


		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ListenCall(l_strSponsor, l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ListenCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ListenCall  Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::PickupCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IPickupCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CPickupCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->PickupCall(l_strSponsor, l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] PickupCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::BargeInCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IBargeInCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CBargeInCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->BargeInCall(l_strSponsor, l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] BargeInCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ForcePopCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IForcePopCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CForcePopCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		/*boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ForcePopCall(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ForcePopCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ForcePopCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}*/
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearConnection(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ForcePopCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ForcePopCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::ConsultationCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IConsultationCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConsultationCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTargetACDGrp = l_pRequestNotif->GetTargetACDGrp();
		std::string l_strTargetType = l_pRequestNotif->GetTargetDeviceType();
		std::string l_strTargetDevice = l_pRequestNotif->GetTargetDevice();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTargetDevice.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str());

			return ERROR_INVALID_DEVICE;
		}

		std::string l_strDestNum = l_strTargetDevice;
		if (CSysConfig::Instance()->GetOutCallNum(l_strTargetDevice, l_strDestNum))
		{
			l_strTargetDevice = l_strDestNum;
		}
		else
		{
			l_strTargetDevice = CSysConfig::Instance()->GetOutCallNumEx(l_strTargetDevice);
		}

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{

			bool l_bRet = CAvayaSwitch::Instance()->ConsultationCall(l_strSponsor, l_strTargetDevice, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ConsultationCall TargetACDGrp [%s] TargetDevice [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTargetACDGrp.c_str(), l_strTargetDevice.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::TransferCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	ITransferCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTransferCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

	//	l_strTarget = CSysConfig::Instance()->GetCheckNum(l_strTarget);
		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->TransferCall(l_strSponsor, l_lActiveCSTACallRefId, l_lHeldCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] TransferCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeflectCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IDeflectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CDeflectCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		if (l_strSponsor.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] Failed, Sponsor is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}
		if (l_strTarget.empty())
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] Failed, Target is Empty !!",
				l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str());

			return ERROR_INVALID_DEVICE;
		}

		bool isBlackTarget = false;
		std::string l_strBlackRouteDest = CSysConfig::Instance()->GetBlackRouteDest();
		if (l_strBlackRouteDest != l_strTarget)
		{
			std::string l_strDestNum = l_strTarget;
			if (CSysConfig::Instance()->GetOutCallNum(l_strTarget, l_strDestNum))
			{
				l_strTarget = l_strDestNum;
			}
			else
			{
				l_strTarget = CSysConfig::Instance()->GetOutCallNumEx(l_strTarget);
			}
		}
		else
			isBlackTarget = true;

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->DeflectCall(l_strSponsor, l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] DeflectCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				if (isBlackTarget)
				{
					IBlackTransferEventNotifPtr l_pSendRequestNotif = boost::make_shared<CBlackTransferEventNotif>();
					l_pSendRequestNotif->SetCaller(l_pRequestNotif->GetOrgCaller());
					l_pSendRequestNotif->SetCalled(l_pRequestNotif->GetOrgCalled());

					m_pSwitchEventCallback->Event_BlackTransfer(l_pSendRequestNotif);
				}
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::HoldCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IHoldCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CHoldCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetDevice();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->HoldCall(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] HoldCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] HoldCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::RetrieveCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IRetriveCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CRetriveCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetDevice();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->RetrieveCall(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] RetrieveCall Target [%s] CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] RetrieveCall Target [%s] CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ReconnectCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IReconnectCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CReconnectCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strDevice = l_pRequestNotif->GetDevice();

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			ICC_LOG_ERROR(m_pLog, "not find call,ActiveCTICallRefId:[%s],HeldCTICallRefId:[%s],l_strSponsor:[%s],requestID:[%d]",
				l_strActiveCTICallRefId.c_str(), l_strHeldCTICallRefId.c_str(), l_strSponsor.c_str(), l_lRequestId);
	
			return ERROR_INVALID_CALL;
		}

		boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strSponsor);
		if (l_pDevice)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ReconnectCall(l_strDevice, l_lActiveCSTACallRefId, l_lHeldCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Device [%s] ReconnectCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Device [%s] ReconnectCall Target [%s] ActiveCallRefId %u HeldCallRefId %u Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strDevice.c_str(), l_lActiveCSTACallRefId, l_lHeldCSTACallRefId);
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "not find device,ActiveCTICallRefId:[%s],HeldCTICallRefId:[%s],l_strSponsor:[%s],requestID:[%d]",
				l_strActiveCTICallRefId.c_str(), l_strHeldCTICallRefId.c_str(), l_strSponsor.c_str(), l_lRequestId);

			return ERROR_INVALID_DEVICE;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::ConferenceCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	IConferenceCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CConferenceCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strActiveCTICallRefId = l_pRequestNotif->GetActiveCTICallRefId();
		std::string l_strHeldCTICallRefId = l_pRequestNotif->GetHeldCTICallRefId();
		std::string l_strTarget = l_pRequestNotif->GetTarget();

		long l_lActiveCSTACallRefId = DEFAULT_CALLREFID;
		long l_lHeldCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strActiveCTICallRefId, l_lActiveCSTACallRefId) || !CCallManager::Instance()->GetCSTACallRefId(l_strHeldCTICallRefId, l_lHeldCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		bool l_bRet = CAvayaSwitch::Instance()->ConferenceCall(l_strTarget, l_lHeldCSTACallRefId, l_lActiveCSTACallRefId, l_lRequestId);
		if (!l_bRet)
		{
			ICC_LOG_ERROR(m_pLog, "TaskId [%u], Target [%s]  ConferenceCall, HeldCallRefId [%u], ActiveCallRefId [%u] Failed !!",
				l_lRequestId, l_strTarget.c_str(), l_lHeldCSTACallRefId, l_lActiveCSTACallRefId);

			return RESULT_FAILED;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Target [%s]  ConferenceCall, HeldCallRefId [%u], ActiveCallRefId [%u] Success !!",
				l_lRequestId, l_strTarget.c_str(), l_lHeldCSTACallRefId, l_lActiveCSTACallRefId);
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::TakeOverCall(ITaskPtr p_pTask)
{
	if (!m_bSwitchConnect)
	{
		return ERROR_NET_DISCONNECT;
	}

	if (m_bCTITestModeFlag)
	{
		return ERROR_CMD_FAILED;
	}

	ITakeOverCallNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CTakeOverCallNotif>(p_pTask->GetSwitchNotif());
	if (l_pRequestNotif)
	{
		long l_lRequestId = l_pRequestNotif->GetRequestId();
		std::string l_strCTICallRefId = l_pRequestNotif->GetCTICallRefId();
		std::string l_strSponsor = l_pRequestNotif->GetSponsor();
		std::string l_strTarget = l_pRequestNotif->GetTarget();
		std::string l_strStep = l_pRequestNotif->GetStep();

		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		if (!CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lCSTACallRefId))
		{
			return ERROR_INVALID_CALL;
		}

		//1、挂断监听者；2、监听者强插到通话；3、挂断被监听的坐席
		//StopListenCall, BargeInCall, Hangup
		if (l_strStep.compare(Step_StopListenCall) == 0)
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearConnection(l_strSponsor, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Sponsor [%s] ClearConnection, CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Sponsor [%s] ClearConnection, CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_lCSTACallRefId);
			}
		} 
		else if (l_strStep.compare(Step_BargeInCall) == 0)
		{
			bool l_bRet = CAvayaSwitch::Instance()->BargeInCall(l_strSponsor, l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Sponsor [%s] Target [%s] BargeInCall, CallRefId [%u] Failed !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Sponsor [%s] Target [%s] BargeInCall, CallRefId [%u] Success !!",
					l_lRequestId, l_strSponsor.c_str(), l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
		else
		{
			bool l_bRet = CAvayaSwitch::Instance()->ClearConnection(l_strTarget, l_lCSTACallRefId, l_lRequestId);
			if (!l_bRet)
			{
				ICC_LOG_ERROR(m_pLog, "TaskId [%u], Target [%s] ClearConnection, CallRefId [%u] Failed !!",
					l_lRequestId, l_strTarget.c_str(), l_lCSTACallRefId);

				return RESULT_FAILED;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "TaskId [%u], Target [%s] ClearConnection, CallRefId [%u] Success !!",
					l_lRequestId, l_strTarget.c_str(), l_lCSTACallRefId);
			}
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::GetAESConnState(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetCTIConnStateResultNotifPtr l_pResultNotif = boost::make_shared<CGetCTIConnStateResultNotif>();
		if (l_pResultNotif)
		{
			l_pResultNotif->SetRequestId(l_lRequestId);
			l_pResultNotif->SetStreamOpen(m_bSwitchConnect);
			l_pResultNotif->SetSwitchType(SWITCH_TYPE_TSAPI);

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetCTIConnStateRet, l_pResultNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetAESConnStateRet, RequestId [%u]",
				l_lTaskId, l_lRequestId);

			/*ITaskPtr l_pNewTask = CTaskManager::Instance()->CreateNewTask();
			if (l_pNewTask)
			{
				l_pNewTask->SetTaskName(Task_GetCTIConnStateRet);
				l_pNewTask->SetSwitchNotif(l_pResultNotif);

				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetAESConnStateRet, RequestId [%u]",
					l_pNewTask->GetTaskId(), l_lRequestId);

				CTaskManager::Instance()->AddSwitchEventTask(l_pNewTask);
			}*/
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetDeviceList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetDeviceListResultNotifPtr l_pResultNotif = CDeviceManager::Instance()->GetDeviceList(l_lRequestId);
		if (l_pResultNotif)
		{
			for (auto l_pDeviceStateObj : l_pResultNotif->m_DeviceList)
			{
				if (l_pDeviceStateObj)
				{
					std::string l_strCTICallRefId = "";
					std::string l_strCallerId = "";
					std::string l_strCalledId = "";
					std::string l_strOriginalCallerId = "";
					std::string l_strOriginalCalledId = "";
					std::string l_strCallDirection = "";
					std::string	l_strLoginMode;		//	login/logout 状态
					std::string	l_strReadyState;
					std::string l_strTalkTime = "";

					std::string l_strAcdGrp;
					std::string l_strDeviceNum = l_pDeviceStateObj->GetDeviceNum();
					long l_lCSTACallRefId = l_pDeviceStateObj->GetCSTACallRefId();

					//CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strTalkTime);
					CCallManager::Instance()->GetCallByCSTACallRefIdEx(l_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strAcdGrp, l_strTalkTime);
					l_pDeviceStateObj->SetCTICallRefId(l_strCTICallRefId);
					l_pDeviceStateObj->SetCallerId(l_strCallerId);
					l_pDeviceStateObj->SetCalledId(l_strCalledId);
					l_pDeviceStateObj->SetCallDirection(l_strCallDirection);

					CAgentManager::Instance()->GetAgentState(l_strDeviceNum, l_strLoginMode, l_strReadyState);
					l_pDeviceStateObj->SetLoginMode(l_strLoginMode);
					l_pDeviceStateObj->SetReadyState(l_strReadyState);
					l_pDeviceStateObj->SetDeptCode(CDeviceManager::Instance()->GetDeptCodeByDeviceNum(l_strDeviceNum));
					l_pDeviceStateObj->SetOriginalCalledId(l_strOriginalCalledId);

					l_pDeviceStateObj->SetTalkTime(l_strTalkTime);
					if (!l_strAcdGrp.empty())
					{
						std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(l_strAcdGrp);
						if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0 || strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0)
						{
							l_pDeviceStateObj->SetACDGrp(l_strAcdGrp);
						}
					}
					
				}
			}

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetDeviceListRet, l_pResultNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetDeviceListRet, RequestId [%u]",
				l_lTaskId, l_lRequestId);

			/*ITaskPtr l_pNewTask = CTaskManager::Instance()->CreateNewTask();
			if (l_pNewTask)
			{
				l_pNewTask->SetTaskName(Task_GetDeviceListRet);
				l_pNewTask->SetSwitchNotif(l_pResultNotif);

				ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask, TaskId [%u], GetDeviceListRet, RequestId [%u]",
					l_pNewTask->GetTaskId(), l_lRequestId);

				CTaskManager::Instance()->AddSwitchEventTask(l_pNewTask);
			}*/
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "GetDeviceListRet, GetDeviceListResultNotif Obj  Failed, RequestId [%u]", l_lRequestId);

			return RESULT_FAILED;
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetACDList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		IGetACDListRequestNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetACDListRequestNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			long l_lRequestId = p_pTask->GetTaskId();

			CAgentManager::Instance()->GetACDList(l_lRequestId);
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetAgentList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		IGetAgentListRequestNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetAgentListRequestNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			long l_lRequestId = p_pTask->GetTaskId();
			std::string l_strACDGrp = l_pRequestNotif->GetACDGrp();

			CAgentManager::Instance()->GetAgentList(l_lRequestId, l_strACDGrp);
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::GetCallList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();
		CCallManager::Instance()->GetCallList(l_lRequestId);
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::GetReadyAgent(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		long l_lRequestId = p_pTask->GetTaskId();

		IGetReadyAgentNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetReadyAgentNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			std::string strACDGrp = l_pRequestNotif->GetACDGrp();

			CAgentManager::Instance()->GetReadyAgent(strACDGrp, l_lRequestId);
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::GetFreeAgent(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		IGetAgentListRequestNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CGetAgentListRequestNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			long l_lRequestId = p_pTask->GetTaskId();
			std::string l_strACDGrp = l_pRequestNotif->GetACDGrp();
			std::string l_strDeptCode = l_pRequestNotif->GetDeptCode();
			CAgentManager::Instance()->GetFreeAgentList(l_lRequestId, l_strACDGrp, l_strDeptCode);
		}
	}

	return RESULT_SUCCESS;
}

int CAvayaSwitchManager::SetBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		ISetBlackListNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetBlackListNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			CBlackListManager::Instance()->AddBlackList(l_pRequestNotif->GetLimitNum());
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeleteBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		ISetBlackListNotifPtr l_pRequestNotif = boost::dynamic_pointer_cast<CSetBlackListNotif>(p_pTask->GetSwitchNotif());
		if (l_pRequestNotif)
		{
			CBlackListManager::Instance()->DeleteBlackList(l_pRequestNotif->GetLimitNum());
		}
	}

	return RESULT_SUCCESS;
}
int CAvayaSwitchManager::DeleteAllBlackList(ITaskPtr p_pTask)
{
	if (p_pTask)
	{
		CBlackListManager::Instance()->ClearBlackList();
	}

	return RESULT_SUCCESS;
}

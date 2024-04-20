#include "Boost.h"
#include "AvayaResult.h"
//#include "Common.h"


boost::shared_ptr<CAvayaResult> CAvayaResult::m_pResultInstance = nullptr;
boost::shared_ptr<CAvayaResult> CAvayaResult::Instance()
{
	if (m_pResultInstance == nullptr)
	{
		m_pResultInstance = boost::make_shared<CAvayaResult>();
	}

	return m_pResultInstance;
}
void CAvayaResult::ExitInstance()
{
	if (m_pResultInstance)
	{
		m_pResultInstance.reset();
	}
}

CAvayaResult::CAvayaResult(void)
{
	m_pStrUtil = nullptr;
}
CAvayaResult::~CAvayaResult(void)
{
	//
}

void CAvayaResult::Init(IResourceManagerPtr pResourceManager)
{
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, pResourceManager)->CreateString();
}
//////////////////////////////////////////////////////////////////////////

std::string CAvayaResult::GetEventClassString(EventClass_t eventClass)
{
	std::string strRet = "";

	switch(eventClass)
	{
	case ACSCONFIRMATION: 
		strRet = ("ACSCONFIRMATION"); break;
	case ACSUNSOLICITED: 
		strRet = ("ACSUNSOLICITED"); break;
	case CSTAREQUEST: 
		strRet = ("CSTAREQUEST"); break;
	case CSTACONFIRMATION: 
		strRet = ("CSTACONFIRMATION"); break;
	case CSTAUNSOLICITED: 
		strRet = ("CSTAUNSOLICITED"); break;
	case CSTAEVENTREPORT: 
		strRet = ("CSTAEVENTREPORT"); break;
	default:
		{
		//	strRet = StringFormat("EventClass %d", eventClass);
			strRet = m_pStrUtil->Format("EventClass %d", eventClass);
			break;
		}
	}
	
	return strRet;
}

std::string CAvayaResult::GetConnectionID_tString(ConnectionID_t conn)
{
//	std::string strRet = StringFormat("CallId: %u, DeviceId: %s, DeviceType: %d", conn.callID, conn.deviceID, conn.devIDType);
	std::string strRet = m_pStrUtil->Format("CallId: %u, DeviceId: %s, DeviceType: %d", conn.callID, conn.deviceID, conn.devIDType);

	return strRet;
}
std::string CAvayaResult::GetAgentModeString(AgentMode_t agentmode)
{
	std::string strRet = "";
	switch(agentmode)
	{
	case AM_LOG_IN: 
		strRet = ("AM_LOG_IN"); break;
	case AM_LOG_OUT:
		strRet = ("AM_LOG_OUT"); break;
	case AM_NOT_READY:
		strRet = ("AM_NOT_READY"); break;
	case AM_READY:
		strRet = ("AM_READY"); break;
	case AM_WORK_NOT_READY: 
		strRet = ("AM_WORK_NOT_READY"); break;
	case AM_WORK_READY:
		strRet = ("AM_WORK_READY"); break;
	default:
		{
		//	strRet = StringFormat("AgentMode_t %d", agentmode);
			strRet = m_pStrUtil->Format("AgentMode_t %d", agentmode);
			break;
		}
	}

	return strRet;
}

/*
std::string CAvayaResult::GetActionTypeString(E_ACTION_TYPE actionType)
{
	switch(actionType)
	{
	case AC_StartMonitor: return ("AC_StartMonitor");
	case AC_StopMonitor: return ("AC_StopMonitor");
	case AC_MakeCall: return ("AC_MakeCall");
	case AC_AnswerCall: return ("AC_AnswerCall");
	case AC_HangupCall: return ("AC_HangupCall");
	case AC_HoldCall: return ("AC_HoldCall");
	case AC_RetrieveHeld: return ("AC_RetrieveHeld");
	case AC_ReconnectHeldCall: return ("AC_ReconnectHeldCall");
	case AC_Conference: return ("AC_Conference");
	case AC_TransferCall: return ("AC_TransferCall");
	case AC_CancelCall: return ("AC_CancelCall");
	case AC_ConsultationCall: return ("AC_ConsultationCall");
	case AC_DeflectCall: return ("AC_DeflectCall");
	case AC_SwapWithHeld: return ("AC_SwapWithHeld");
	case AC_PickupCall: return ("AC_PickupCall");
	case AC_RespondToInactiveCall: return ("AC_RespondToInactiveCall");
	case AC_SetAgentMode: return ("AC_SetAgentMode");
	case AC_GetAgentStatus: return ("AC_GetAgentStatus");
	case AC_SingleStepTransfer: return ("AC_SingleStepTransfer");
	case AC_SingleStepConference: return ("AC_SingleStepConference");
	case AC_SetDoNotDisturb: return ("AC_SetDoNotDisturb");
	case AC_Snapshot: return ("AC_Snapshot");
		//TSAPI自己独立实现的函数功能------------------------
	case AC_CallCompletion: return ("AC_CallCompletion");
	case AC_ClearCall: return ("AC_ClearCall");
	case AC_GroupPickupCall: return ("AC_GroupPickupCall");
	case AC_MakePredictiveCall: return ("AC_MakePredictiveCall");
	case AC_SetMsgWaitingInd: return ("AC_SetMsgWaitingInd");
	case AC_SetForwarding: return ("AC_SetForwarding");
	case AC_QueryMsgWaitingInd: return ("AC_QueryMsgWaitingInd");
	case AC_QueryDoNotDisturb: return ("AC_QueryDoNotDisturb");
	case AC_QueryForwarding: return ("AC_QueryForwarding");
	case AC_QueryLastNumber: return ("AC_QueryLastNumber");
	case AC_QueryDeviceInfo: return ("AC_QueryDeviceInfo");
	case AC_MonitorCall: return ("AC_MonitorCall");
	case AC_ChangeMonitorFilter: return ("AC_ChangeMonitorFilter");
	case AC_MonitorStop: return ("AC_MonitorStop");
	case AC_SnapshotCall: return ("AC_SnapshotCall");
	case AC_RouteRegister: return ("AC_RouteRegister");
	case AC_RouteRegisterCancel: return ("AC_RouteRegisterCancel");
	case AC_RouteSelect: return ("AC_RouteSelect");
	case AC_RouteEnd: return ("AC_RouteEnd");
	case AC_RouteSelectInv: return ("AC_RouteSelectInv");
	case AC_RouteEndInv: return ("AC_RouteEndInv");
	case AC_SendPrivateEvent: return ("AC_SendPrivateEvent");
	case AC_cstaSysStatReq: return ("AC_cstaSysStatReq");
	case AC_SysStatStart: return ("AC_SysStatStart");
	case AC_SysStatStop: return ("AC_SysStatStop");
	case AC_cstaGetAPICaps: return ("AC_cstaGetAPICaps");
	case AC_cstaGetDeviceList: return ("AC_cstaGetDeviceList");
	case AC_QueryCallMonitor: return ("AC_QueryCallMonitor");
	}
	return ("");
}*/
std::string CAvayaResult::GetEventTypeString(EventType_t eventType)
{
	switch(eventType)
	{
	case CSTA_ALTERNATE_CALL: return ("CSTA_ALTERNATE_CALL");
	case CSTA_ALTERNATE_CALL_CONF: return ("CSTA_ALTERNATE_CALL_CONF");
	case CSTA_ANSWER_CALL: return ("CSTA_ANSWER_CALL");
	case CSTA_ANSWER_CALL_CONF: return ("CSTA_ANSWER_CALL_CONF");
	case CSTA_CALL_COMPLETION: return ("CSTA_CALL_COMPLETION");
	case CSTA_CALL_COMPLETION_CONF: return ("CSTA_CALL_COMPLETION_CONF");
	case CSTA_CLEAR_CALL: return ("CSTA_CLEAR_CALL");
	case CSTA_CLEAR_CALL_CONF: return ("CSTA_CLEAR_CALL_CONF");
	case CSTA_CLEAR_CONNECTION: return ("CSTA_CLEAR_CONNECTION");
	case CSTA_CLEAR_CONNECTION_CONF: return ("CSTA_CLEAR_CONNECTION_CONF");
	case CSTA_CONFERENCE_CALL: return ("CSTA_CONFERENCE_CALL");
	case CSTA_CONFERENCE_CALL_CONF: return ("CSTA_CONFERENCE_CALL_CONF");
	case CSTA_CONSULTATION_CALL: return ("CSTA_CONSULTATION_CALL");
	case CSTA_CONSULTATION_CALL_CONF: return ("CSTA_CONSULTATION_CALL_CONF");
	case CSTA_DEFLECT_CALL: return ("CSTA_DEFLECT_CALL");
	case CSTA_DEFLECT_CALL_CONF: return ("CSTA_DEFLECT_CALL_CONF");
	case CSTA_PICKUP_CALL: return ("CSTA_PICKUP_CALL");
	case CSTA_PICKUP_CALL_CONF: return ("CSTA_PICKUP_CALL_CONF");
	case CSTA_GROUP_PICKUP_CALL: return ("CSTA_GROUP_PICKUP_CALL");
	case CSTA_GROUP_PICKUP_CALL_CONF: return ("CSTA_GROUP_PICKUP_CALL_CONF");
	case CSTA_HOLD_CALL: return ("CSTA_HOLD_CALL");
	case CSTA_HOLD_CALL_CONF: return ("CSTA_HOLD_CALL_CONF");
	case CSTA_MAKE_CALL: return ("CSTA_MAKE_CALL");
	case CSTA_MAKE_CALL_CONF: return ("CSTA_MAKE_CALL_CONF");
	case CSTA_MAKE_PREDICTIVE_CALL: return ("CSTA_MAKE_PREDICTIVE_CALL"); 
	case CSTA_MAKE_PREDICTIVE_CALL_CONF: return ("CSTA_MAKE_PREDICTIVE_CALL_CONF"); 
	case CSTA_QUERY_MWI: return ("CSTA_QUERY_MWI"); 
	case CSTA_QUERY_MWI_CONF: return ("CSTA_QUERY_MWI_CONF"); 
	case CSTA_QUERY_DND: return ("CSTA_QUERY_DND"); 
	case CSTA_QUERY_DND_CONF: return ("CSTA_QUERY_DND_CONF"); 
	case CSTA_QUERY_FWD: return ("CSTA_QUERY_FWD"); 
	case CSTA_QUERY_FWD_CONF: return ("CSTA_QUERY_FWD_CONF"); 
	case CSTA_QUERY_AGENT_STATE: return ("CSTA_QUERY_AGENT_STATE"); 
	case CSTA_QUERY_AGENT_STATE_CONF: return ("CSTA_QUERY_AGENT_STATE_CONF"); 
	case CSTA_QUERY_LAST_NUMBER: return ("CSTA_QUERY_LAST_NUMBER"); 
	case CSTA_QUERY_LAST_NUMBER_CONF: return ("CSTA_QUERY_LAST_NUMBER_CONF"); 
	case CSTA_QUERY_DEVICE_INFO: return ("CSTA_QUERY_DEVICE_INFO"); 
	case CSTA_QUERY_DEVICE_INFO_CONF: return ("CSTA_QUERY_DEVICE_INFO_CONF"); 
	case CSTA_RECONNECT_CALL: return ("CSTA_RECONNECT_CALL"); 
	case CSTA_RECONNECT_CALL_CONF: return ("CSTA_RECONNECT_CALL_CONF");	
	case CSTA_RETRIEVE_CALL: return ("CSTA_RETRIEVE_CALL"); 
	case CSTA_RETRIEVE_CALL_CONF: return ("CSTA_RETRIEVE_CALL_CONF"); 
	case CSTA_SET_MWI: return ("CSTA_SET_MWI"); 
	case CSTA_SET_MWI_CONF: return ("CSTA_SET_MWI_CONF"); 
	case CSTA_SET_DND: return ("CSTA_SET_DND"); 
	case CSTA_SET_DND_CONF: return ("CSTA_SET_DND_CONF"); 
	case CSTA_SET_FWD: return ("CSTA_SET_FWD"); 
	case CSTA_SET_FWD_CONF: return ("CSTA_SET_FWD_CONF"); 
	case CSTA_SET_AGENT_STATE: return ("CSTA_SET_AGENT_STATE"); 
	case CSTA_SET_AGENT_STATE_CONF: return ("CSTA_SET_AGENT_STATE_CONF"); 
	case CSTA_TRANSFER_CALL: return ("CSTA_TRANSFER_CALL"); 
	case CSTA_TRANSFER_CALL_CONF: return ("CSTA_TRANSFER_CALL_CONF"); 
	case CSTA_UNIVERSAL_FAILURE_CONF: return ("CSTA_UNIVERSAL_FAILURE_CONF"); 
	case CSTA_CALL_CLEARED: return ("CSTA_CALL_CLEARED"); 
	case CSTA_CONFERENCED: return ("CSTA_CONFERENCED"); 
	case CSTA_CONNECTION_CLEARED: return ("CSTA_CONNECTION_CLEARED"); 
	case CSTA_DELIVERED: return ("CSTA_DELIVERED"); 
	case CSTA_DIVERTED: return ("CSTA_DIVERTED"); 
	case CSTA_ESTABLISHED: return ("CSTA_ESTABLISHED"); 
	case CSTA_FAILED: return ("CSTA_FAILED"); 
	case CSTA_HELD: return ("CSTA_HELD"); 
	case CSTA_NETWORK_REACHED: return ("CSTA_NETWORK_REACHED"); 
	case CSTA_ORIGINATED: return ("CSTA_ORIGINATED"); 
	case CSTA_QUEUED: return ("CSTA_QUEUED"); 
	case CSTA_RETRIEVED: return ("CSTA_RETRIEVED"); 
	case CSTA_SERVICE_INITIATED: return ("CSTA_SERVICE_INITIATED"); 
	case CSTA_TRANSFERRED: return ("CSTA_TRANSFERRED"); 
	case CSTA_CALL_INFORMATION: return ("CSTA_CALL_INFORMATION"); 
	case CSTA_DO_NOT_DISTURB: return ("CSTA_DO_NOT_DISTURB"); 
	case CSTA_FORWARDING: return ("CSTA_FORWARDING"); 
	case CSTA_MESSAGE_WAITING: return ("CSTA_MESSAGE_WAITING"); 
	case CSTA_LOGGED_ON: return ("CSTA_LOGGED_ON"); 
	case CSTA_LOGGED_OFF: return ("CSTA_LOGGED_OFF"); 
	case CSTA_NOT_READY: return ("CSTA_NOT_READY"); 
	case CSTA_READY: return ("CSTA_READY"); 
	case CSTA_WORK_NOT_READY: return ("CSTA_WORK_NOT_READY"); 
	case CSTA_WORK_READY: return ("CSTA_WORK_READY"); 
	case CSTA_ROUTE_REGISTER_REQ: return ("CSTA_ROUTE_REGISTER_REQ"); 
	case CSTA_ROUTE_REGISTER_REQ_CONF: return ("CSTA_ROUTE_REGISTER_REQ_CONF"); 
	case CSTA_ROUTE_REGISTER_CANCEL: return ("CSTA_ROUTE_REGISTER_CANCEL"); 
	case CSTA_ROUTE_REGISTER_CANCEL_CONF: return ("CSTA_ROUTE_REGISTER_CANCEL_CONF"); 
	case CSTA_ROUTE_REGISTER_ABORT: return ("CSTA_ROUTE_REGISTER_ABORT"); 
	case CSTA_ROUTE_REQUEST: return ("CSTA_ROUTE_REQUEST"); 
	case CSTA_ROUTE_SELECT_REQUEST: return ("CSTA_ROUTE_SELECT_REQUEST"); 
	case CSTA_RE_ROUTE_REQUEST: return ("CSTA_RE_ROUTE_REQUEST"); 
	case CSTA_ROUTE_USED: return ("CSTA_ROUTE_USED"); 
	case CSTA_ROUTE_END: return ("CSTA_ROUTE_END"); 
	case CSTA_ROUTE_END_REQUEST: return ("CSTA_ROUTE_END_REQUEST"); 
	case CSTA_ESCAPE_SVC: return ("CSTA_ESCAPE_SVC"); 
	case CSTA_ESCAPE_SVC_CONF: return ("CSTA_ESCAPE_SVC_CONF"); 
	case CSTA_ESCAPE_SVC_REQ: return ("CSTA_ESCAPE_SVC_REQ"); 
	case CSTA_ESCAPE_SVC_REQ_CONF: return ("CSTA_ESCAPE_SVC_REQ_CONF");
	case CSTA_PRIVATE: return ("CSTA_PRIVATE");
	case CSTA_PRIVATE_STATUS: return ("CSTA_PRIVATE_STATUS");
	case CSTA_SEND_PRIVATE: return ("CSTA_SEND_PRIVATE");
	case CSTA_BACK_IN_SERVICE: return ("CSTA_BACK_IN_SERVICE");
	case CSTA_OUT_OF_SERVICE: return ("CSTA_OUT_OF_SERVICE");
	case CSTA_REQ_SYS_STAT: return ("CSTA_REQ_SYS_STAT");
	case CSTA_SYS_STAT_REQ_CONF: return ("CSTA_SYS_STAT_REQ_CONF");
	case CSTA_SYS_STAT_START: return ("CSTA_SYS_STAT_START");
	case CSTA_SYS_STAT_START_CONF: return ("CSTA_SYS_STAT_START_CONF");
	case CSTA_SYS_STAT_STOP: return ("CSTA_SYS_STAT_STOP");
	case CSTA_SYS_STAT_STOP_CONF: return ("CSTA_SYS_STAT_STOP_CONF");
	case CSTA_CHANGE_SYS_STAT_FILTER: return ("CSTA_CHANGE_SYS_STAT_FILTER");
	case CSTA_CHANGE_SYS_STAT_FILTER_CONF: return ("CSTA_CHANGE_SYS_STAT_FILTER_CONF");
	case CSTA_SYS_STAT: return ("CSTA_SYS_STAT");
	case CSTA_SYS_STAT_ENDED: return ("CSTA_SYS_STAT_ENDED");
	case CSTA_SYS_STAT_REQ: return ("CSTA_SYS_STAT_REQ"); 
	case CSTA_REQ_SYS_STAT_CONF: return ("CSTA_REQ_SYS_STAT_CONF"); 
	case CSTA_SYS_STAT_EVENT_SEND: return ("CSTA_SYS_STAT_EVENT_SEND"); 
	case CSTA_MONITOR_DEVICE: return ("CSTA_MONITOR_DEVICE"); 
	case CSTA_MONITOR_CALL: return ("CSTA_MONITOR_CALL"); 
	case CSTA_MONITOR_CALLS_VIA_DEVICE: return ("CSTA_MONITOR_CALLS_VIA_DEVICE"); 
	case CSTA_MONITOR_CONF: return ("CSTA_MONITOR_CONF");
	case CSTA_CHANGE_MONITOR_FILTER: return ("CSTA_CHANGE_MONITOR_FILTER"); 
	case CSTA_CHANGE_MONITOR_FILTER_CONF: return ("CSTA_CHANGE_MONITOR_FILTER_CONF"); 
	case CSTA_MONITOR_STOP: return ("CSTA_MONITOR_STOP"); 
	case CSTA_MONITOR_STOP_CONF: return ("CSTA_MONITOR_STOP_CONF"); 
	case CSTA_MONITOR_ENDED: return ("CSTA_MONITOR_ENDED"); 
	case CSTA_SNAPSHOT_CALL: return ("CSTA_SNAPSHOT_CALL"); 
	case CSTA_SNAPSHOT_CALL_CONF: return ("CSTA_SNAPSHOT_CALL_CONF"); 
	case CSTA_SNAPSHOT_DEVICE: return ("CSTA_SNAPSHOT_DEVICE"); 
	case CSTA_SNAPSHOT_DEVICE_CONF: return ("CSTA_SNAPSHOT_DEVICE_CONF"); 
	case CSTA_GETAPI_CAPS: return ("CSTA_GETAPI_CAPS"); 
	case CSTA_GETAPI_CAPS_CONF: return ("CSTA_GETAPI_CAPS_CONF"); 
	case CSTA_GET_DEVICE_LIST: return ("CSTA_GET_DEVICE_LIST"); 
	case CSTA_GET_DEVICE_LIST_CONF: return ("CSTA_GET_DEVICE_LIST_CONF");
	case CSTA_QUERY_CALL_MONITOR: return ("CSTA_QUERY_CALL_MONITOR"); 
	case CSTA_QUERY_CALL_MONITOR_CONF: return ("CSTA_QUERY_CALL_MONITOR_CONF"); 
	case CSTA_ROUTE_REQUEST_EXT: return ("CSTA_ROUTE_REQUEST_EXT"); 
	case CSTA_ROUTE_USED_EXT: return ("CSTA_ROUTE_USED_EXT"); 
	case CSTA_ROUTE_SELECT_INV_REQUEST: return ("CSTA_ROUTE_SELECT_INV_REQUEST"); 
	case CSTA_ROUTE_END_INV_REQUEST: return ("CSTA_ROUTE_END_INV_REQUEST"); 
	}

	return ("");

}

int CAvayaResult::GetCSTAUnsoliEventErrorCode(CSTAUniversalFailure_t error)
{
	return (100 + error);
}
std::string CAvayaResult::GetACSUnsoliEventString(ACSUniversalFailure_t error)
{
	switch(error)
	{
	case TSERVER_STREAM_FAILED:	return  ("TSERVER_STREAM_FAILED");
	case TSERVER_NO_THREAD: return  ("TSERVER_NO_THREAD");
	case TSERVER_BAD_DRIVER_ID: return  ("TSERVER_BAD_DRIVER_ID");
	case TSERVER_DEAD_DRIVER: return  ("TSERVER_DEAD_DRIVER");
	case TSERVER_MESSAGE_HIGH_WATER_MARK: return  ("TSERVER_MESSAGE_HIGH_WATER_MARK");
	case TSERVER_FREE_BUFFER_FAILED: return  ("TSERVER_FREE_BUFFER_FAILED");
	case TSERVER_SEND_TO_DRIVER: return  ("TSERVER_SEND_TO_DRIVER");
	case TSERVER_RECEIVE_FROM_DRIVER: return  ("TSERVER_RECEIVE_FROM_DRIVER");
	case TSERVER_REGISTRATION_FAILED: return  ("TSERVER_REGISTRATION_FAILED");
	case TSERVER_SPX_FAILED: return  ("TSERVER_SPX_FAILED");
	case TSERVER_TRACE: return  ("TSERVER_TRACE");
	case TSERVER_NO_MEMORY: return  ("TSERVER_NO_MEMORY");
	case TSERVER_ENCODE_FAILED: return  ("TSERVER_ENCODE_FAILED");
	case TSERVER_DECODE_FAILED: return  ("TSERVER_DECODE_FAILED");
	case TSERVER_BAD_CONNECTION: return  ("TSERVER_BAD_CONNECTION");
	case TSERVER_BAD_PDU: return  ("TSERVER_BAD_PDU");
	case TSERVER_NO_VERSION: return  ("TSERVER_NO_VERSION");
	case TSERVER_ECB_MAX_EXCEEDED: return  ("TSERVER_ECB_MAX_EXCEEDED");
	case TSERVER_NO_ECBS: return  ("TSERVER_NO_ECBS");
	case TSERVER_NO_SDB: return  ("TSERVER_NO_SDB");
	case TSERVER_NO_SDB_CHECK_NEEDED: return  ("TSERVER_NO_SDB_CHECK_NEEDED");
	case TSERVER_SDB_CHECK_NEEDED: return  ("TSERVER_SDB_CHECK_NEEDED");
	case TSERVER_BAD_SDB_LEVEL: return  ("TSERVER_BAD_SDB_LEVEL");
	case TSERVER_BAD_SERVERID: return  ("TSERVER_BAD_SERVERID");
	case TSERVER_BAD_STREAM_TYPE: return  ("TSERVER_BAD_STREAM_TYPE");
	case TSERVER_BAD_PASSWORD_OR_LOGIN: return  ("TSERVER_BAD_PASSWORD_OR_LOGIN");
	case TSERVER_NO_USER_RECORD: return  ("TSERVER_NO_USER_RECORD");
	case TSERVER_NO_DEVICE_RECORD: return  ("TSERVER_NO_DEVICE_RECORD");
	case TSERVER_DEVICE_NOT_ON_LIST: return  ("TSERVER_DEVICE_NOT_ON_LIST");
	case TSERVER_USERS_RESTRICTED_HOME: return  ("TSERVER_USERS_RESTRICTED_HOME");
	case TSERVER_NO_AWAYPERMISSION: return  ("TSERVER_NO_AWAYPERMISSION");
	case TSERVER_NO_HOMEPERMISSION: return  ("TSERVER_NO_HOMEPERMISSION");
	case TSERVER_NO_AWAY_WORKTOP: return  ("TSERVER_NO_AWAY_WORKTOP");
	case TSERVER_BAD_DEVICE_RECORD: return  ("TSERVER_BAD_DEVICE_RECORD");
	case TSERVER_DEVICE_NOT_SUPPORTED: return  ("TSERVER_DEVICE_NOT_SUPPORTED");
	case TSERVER_INSUFFICIENT_PERMISSION: return  ("TSERVER_INSUFFICIENT_PERMISSION");
	case TSERVER_NO_RESOURCE_TAG: return  ("TSERVER_NO_RESOURCE_TAG");
	case TSERVER_INVALID_MESSAGE: return  ("TSERVER_INVALID_MESSAGE");
	case TSERVER_EXCEPTION_LIST: return  ("TSERVER_EXCEPTION_LIST");
	case TSERVER_NOT_ON_OAM_LIST: return  ("TSERVER_NOT_ON_OAM_LIST");
	case TSERVER_PBX_ID_NOT_IN_SDB: return  ("TSERVER_PBX_ID_NOT_IN_SDB");
	case TSERVER_USER_LICENSES_EXCEEDED: return  ("TSERVER_USER_LICENSES_EXCEEDED");
	case TSERVER_OAM_DROP_CONNECTION: return  ("TSERVER_OAM_DROP_CONNECTION");
	case TSERVER_NO_VERSION_RECORD: return  ("TSERVER_NO_VERSION_RECORD");
	case TSERVER_OLD_VERSION_RECORD: return  ("TSERVER_OLD_VERSION_RECORD");
	case TSERVER_BAD_PACKET: return  ("TSERVER_BAD_PACKET");
	case TSERVER_OPEN_FAILED: return  ("TSERVER_OPEN_FAILED");
	case TSERVER_OAM_IN_USE: return  ("TSERVER_OAM_IN_USE");
	case TSERVER_DEVICE_NOT_ON_HOME_LIST: return  ("TSERVER_DEVICE_NOT_ON_HOME_LIST");
	case TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST: return  ("TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST");
	case TSERVER_DEVICE_NOT_ON_AWAY_LIST: return  ("TSERVER_DEVICE_NOT_ON_AWAY_LIST");
	case TSERVER_DEVICE_NOT_ON_ROUTE_LIST: return  ("TSERVER_DEVICE_NOT_ON_ROUTE_LIST");
	case TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST: return  ("TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST");
	case TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST: return  ("TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST");
	case TSERVER_NO_CALL_CALL_MONITOR_PERMISSION: return  ("TSERVER_NO_CALL_CALL_MONITOR_PERMISSION");
	case TSERVER_HOME_DEVICE_LIST_EMPTY: return  ("TSERVER_HOME_DEVICE_LIST_EMPTY");
	case TSERVER_CALL_CONTROL_LIST_EMPTY: return  ("TSERVER_CALL_CONTROL_LIST_EMPTY");
	case TSERVER_AWAY_LIST_EMPTY: return  ("TSERVER_AWAY_LIST_EMPTY");
	case TSERVER_ROUTE_LIST_EMPTY: return  ("TSERVER_ROUTE_LIST_EMPTY");
	case TSERVER_MONITOR_DEVICE_LIST_EMPTY: return  ("TSERVER_MONITOR_DEVICE_LIST_EMPTY");
	case TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY: return  ("TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY");
	case TSERVER_USER_AT_HOME_WORKTOP: return  ("TSERVER_USER_AT_HOME_WORKTOP");
	case TSERVER_DEVICE_LIST_EMPTY: return  ("TSERVER_DEVICE_LIST_EMPTY");
	case TSERVER_BAD_GET_DEVICE_LEVEL: return  ("TSERVER_BAD_GET_DEVICE_LEVEL");
	case TSERVER_DRIVER_UNREGISTERED: return  ("TSERVER_DRIVER_UNREGISTERED");
	case TSERVER_NO_ACS_STREAM: return  ("TSERVER_NO_ACS_STREAM");
	case TSERVER_DROP_OAM: return  ("TSERVER_DROP_OAM");
	case TSERVER_ECB_TIMEOUT: return  ("TSERVER_ECB_TIMEOUT");
	case TSERVER_BAD_ECB: return  ("TSERVER_BAD_ECB");
	case TSERVER_ADVERTISE_FAILED: return  ("TSERVER_ADVERTISE_FAILED");
	case TSERVER_NETWARE_FAILURE: return  ("TSERVER_NETWARE_FAILURE");
	case TSERVER_TDI_QUEUE_FAULT: return  ("TSERVER_TDI_QUEUE_FAULT");
	case TSERVER_DRIVER_CONGESTION: return  ("TSERVER_DRIVER_CONGESTION");
	case TSERVER_NO_TDI_BUFFERS: return  ("TSERVER_NO_TDI_BUFFERS");
	case TSERVER_OLD_INVOKEID: return  ("TSERVER_OLD_INVOKEID");
	case TSERVER_HWMARK_TO_LARGE: return  ("TSERVER_HWMARK_TO_LARGE");
	case TSERVER_SET_ECB_TO_LOW: return  ("TSERVER_SET_ECB_TO_LOW");
	case TSERVER_NO_RECORD_IN_FILE: return  ("TSERVER_NO_RECORD_IN_FILE");
	case TSERVER_ECB_OVERDUE: return  ("TSERVER_ECB_OVERDUE");
	case TSERVER_BAD_PW_ENCRYPTION: return  ("TSERVER_BAD_PW_ENCRYPTION");
	case TSERVER_BAD_TSERV_PROTOCOL: return  ("TSERVER_BAD_TSERV_PROTOCOL");
	case TSERVER_BAD_DRIVER_PROTOCOL: return  ("TSERVER_BAD_DRIVER_PROTOCOL");
	case TSERVER_BAD_TRANSPORT_TYPE: return  ("TSERVER_BAD_TRANSPORT_TYPE");
	case TSERVER_PDU_VERSION_MISMATCH: return  ("TSERVER_PDU_VERSION_MISMATCH");
	case TSERVER_VERSION_MISMATCH: return  ("TSERVER_VERSION_MISMATCH");
	case TSERVER_LICENSE_MISMATCH: return  ("TSERVER_LICENSE_MISMATCH");
	case TSERVER_BAD_ATTRIBUTE_LIST: return  ("TSERVER_BAD_ATTRIBUTE_LIST");
	case TSERVER_BAD_TLIST_TYPE: return  ("TSERVER_BAD_TLIST_TYPE");
	case TSERVER_BAD_PROTOCOL_FORMAT: return  ("TSERVER_BAD_PROTOCOL_FORMAT");
	case TSERVER_OLD_TSLIB: return  ("TSERVER_OLD_TSLIB");
	case TSERVER_BAD_LICENSE_FILE: return  ("TSERVER_BAD_LICENSE_FILE");
	case TSERVER_NO_PATCHES: return  ("TSERVER_NO_PATCHES");
	case TSERVER_SYSTEM_ERROR: return  ("TSERVER_SYSTEM_ERROR");
	case TSERVER_OAM_LIST_EMPTY: return  ("TSERVER_OAM_LIST_EMPTY");
	case TSERVER_TCP_FAILED: return  ("TSERVER_TCP_FAILED");
	case TSERVER_SPX_DISABLED: return  ("TSERVER_SPX_DISABLED");
	case TSERVER_TCP_DISABLED: return  ("TSERVER_TCP_DISABLED");
	case TSERVER_REQUIRED_MODULES_NOT_LOADED: return  ("TSERVER_REQUIRED_MODULES_NOT_LOADED");
	case TSERVER_TRANSPORT_IN_USE_BY_OAM: return  ("TSERVER_TRANSPORT_IN_USE_BY_OAM");
	case TSERVER_NO_NDS_OAM_PERMISSION: return  ("TSERVER_NO_NDS_OAM_PERMISSION");
	case TSERVER_OPEN_SDB_LOG_FAILED: return  ("TSERVER_OPEN_SDB_LOG_FAILED");
	case TSERVER_INVALID_LOG_SIZE: return  ("TSERVER_INVALID_LOG_SIZE");
	case TSERVER_WRITE_SDB_LOG_FAILED: return  ("TSERVER_WRITE_SDB_LOG_FAILED");
	case TSERVER_NT_FAILURE: return  ("TSERVER_NT_FAILURE");
	case TSERVER_LOAD_LIB_FAILED: return  ("TSERVER_LOAD_LIB_FAILED");
	case TSERVER_INVALID_DRIVER: return  ("TSERVER_INVALID_DRIVER");
	case TSERVER_REGISTRY_ERROR: return  ("TSERVER_REGISTRY_ERROR");
	case TSERVER_DUPLICATE_ENTRY: return  ("TSERVER_DUPLICATE_ENTRY");
	case TSERVER_DRIVER_LOADED: return  ("TSERVER_DRIVER_LOADED");
	case TSERVER_DRIVER_NOT_LOADED: return  ("TSERVER_DRIVER_NOT_LOADED");
	case TSERVER_NO_LOGON_PERMISSION: return  ("TSERVER_NO_LOGON_PERMISSION");
	case TSERVER_ACCOUNT_DISABLED: return  ("TSERVER_ACCOUNT_DISABLED");
	case TSERVER_NO_NETLOGON: return  ("TSERVER_NO_NETLOGON");
	case TSERVER_ACCT_RESTRICTED: return  ("TSERVER_ACCT_RESTRICTED");
	case TSERVER_INVALID_LOGON_TIME: return  ("TSERVER_INVALID_LOGON_TIME");
	case TSERVER_INVALID_WORKSTATION: return  ("TSERVER_INVALID_WORKSTATION");
	case TSERVER_ACCT_LOCKED_OUT: return  ("TSERVER_ACCT_LOCKED_OUT");
	case TSERVER_PASSWORD_EXPIRED: return  ("TSERVER_PASSWORD_EXPIRED");
	case DRIVER_DUPLICATE_ACSHANDLE: return ("DRIVER_DUPLICATE_ACSHANDLE");
	case DRIVER_INVALID_ACS_REQUEST: return ("DRIVER_INVALID_ACS_REQUEST");
	case DRIVER_ACS_HANDLE_REJECTION: return ("DRIVER_ACS_HANDLE_REJECTION");
	case DRIVER_INVALID_CLASS_REJECTION: return ("DRIVER_INVALID_CLASS_REJECTION");
	case DRIVER_GENERIC_REJECTION: return ("DRIVER_GENERIC_REJECTION");
	case DRIVER_RESOURCE_LIMITATION: return ("DRIVER_RESOURCE_LIMITATION");
	case DRIVER_ACSHANDLE_TERMINATION: return ("DRIVER_ACSHANDLE_TERMINATION");
	case DRIVER_LINK_UNAVAILABLE: return ("DRIVER_LINK_UNAVAILABLE");
	case DRIVER_OAM_IN_USE: return ("DRIVER_OAM_IN_USE");
	}
	
	return ("");
}

std::string CAvayaResult::GetCSTAUnsoliEventString(CSTAUniversalFailure_t error)
{
	switch(error)
	{
	case GENERIC_UNSPECIFIED: return ("GENERIC_UNSPECIFIED");
	case GENERIC_OPERATION: return ("GENERIC_OPERATION");
	case REQUEST_INCOMPATIBLE_WITH_OBJECT: return ("REQUEST_INCOMPATIBLE_WITH_OBJECT");
	case VALUE_OUT_OF_RANGE: return ("VALUE_OUT_OF_RANGE");
	case OBJECT_NOT_KNOWN: return ("OBJECT_NOT_KNOWN");
	case INVALID_CALLING_DEVICE: return ("INVALID_CALLING_DEVICE");
	case INVALID_CALLED_DEVICE: return ("INVALID_CALLED_DEVICE");
	case INVALID_FORWARDING_DESTINATION: return ("INVALID_FORWARDING_DESTINATION");
	case PRIVILEGE_VIOLATION_ON_SPECIFIED_DEVICE: return ("PRIVILEGE_VIOLATION_ON_SPECIFIED_DEVICE");
	case PRIVILEGE_VIOLATION_ON_CALLED_DEVICE: return ("PRIVILEGE_VIOLATION_ON_CALLED_DEVICE");
	case PRIVILEGE_VIOLATION_ON_CALLING_DEVICE: return ("PRIVILEGE_VIOLATION_ON_CALLING_DEVICE");
	case INVALID_CSTA_CALL_IDENTIFIER: return ("INVALID_CSTA_CALL_IDENTIFIER");
	case INVALID_CSTA_DEVICE_IDENTIFIER: return ("INVALID_CSTA_DEVICE_IDENTIFIER");
	case INVALID_CSTA_CONNECTION_IDENTIFIER: return ("INVALID_CSTA_CONNECTION_IDENTIFIER");
	case INVALID_DESTINATION: return ("INVALID_DESTINATION");
	case INVALID_FEATURE: return ("INVALID_FEATURE");
	case INVALID_ALLOCATION_STATE: return ("INVALID_ALLOCATION_STATE");
	case INVALID_CROSS_REF_ID: return ("INVALID_CROSS_REF_ID");
	case INVALID_OBJECT_TYPE: return ("INVALID_OBJECT_TYPE");
	case SECURITY_VIOLATION: return ("SECURITY_VIOLATION");
	case GENERIC_STATE_INCOMPATIBILITY: return ("GENERIC_STATE_INCOMPATIBILITY");
	case INVALID_OBJECT_STATE: return ("INVALID_OBJECT_STATE");
	case INVALID_CONNECTION_ID_FOR_ACTIVE_CALL: return ("INVALID_CONNECTION_ID_FOR_ACTIVE_CALL");
	case NO_ACTIVE_CALL: return ("NO_ACTIVE_CALL");
	case NO_HELD_CALL: return ("NO_HELD_CALL");
	case NO_CALL_TO_CLEAR: return ("NO_CALL_TO_CLEAR");
	case NO_CONNECTION_TO_CLEAR: return ("NO_CONNECTION_TO_CLEAR");
	case NO_CALL_TO_ANSWER: return ("NO_CALL_TO_ANSWER");
	case NO_CALL_TO_COMPLETE: return ("NO_CALL_TO_COMPLETE");
	case GENERIC_SYSTEM_RESOURCE_AVAILABILITY: return ("GENERIC_SYSTEM_RESOURCE_AVAILABILITY");
	case SERVICE_BUSY: return ("SERVICE_BUSY");
	case RESOURCE_BUSY: return ("RESOURCE_BUSY");
	case RESOURCE_OUT_OF_SERVICE: return ("RESOURCE_OUT_OF_SERVICE");
	case NETWORK_BUSY: return ("NETWORK_BUSY");
	case NETWORK_OUT_OF_SERVICE: return ("NETWORK_OUT_OF_SERVICE");
	case OVERALL_MONITOR_LIMIT_EXCEEDED: return ("OVERALL_MONITOR_LIMIT_EXCEEDED");
	case CONFERENCE_MEMBER_LIMIT_EXCEEDED: return ("CONFERENCE_MEMBER_LIMIT_EXCEEDED");
	case GENERIC_SUBSCRIBED_RESOURCE_AVAILABILITY: return ("GENERIC_SUBSCRIBED_RESOURCE_AVAILABILITY");
	case OBJECT_MONITOR_LIMIT_EXCEEDED: return ("OBJECT_MONITOR_LIMIT_EXCEEDED");
	case EXTERNAL_TRUNK_LIMIT_EXCEEDED: return ("EXTERNAL_TRUNK_LIMIT_EXCEEDED");
	case OUTSTANDING_REQUEST_LIMIT_EXCEEDED: return ("OUTSTANDING_REQUEST_LIMIT_EXCEEDED");
	case GENERIC_PERFORMANCE_MANAGEMENT: return ("GENERIC_PERFORMANCE_MANAGEMENT");
	case PERFORMANCE_LIMIT_EXCEEDED: return ("PERFORMANCE_LIMIT_EXCEEDED");
	case UNSPECIFIED_SECURITY_ERROR: return ("UNSPECIFIED_SECURITY_ERROR");
	case SEQUENCE_NUMBER_VIOLATED: return ("SEQUENCE_NUMBER_VIOLATED");
	case TIME_STAMP_VIOLATED: return  ("TIME_STAMP_VIOLATED");
	case PAC_VIOLATED: return ("PAC_VIOLATED");
	case SEAL_VIOLATED: return ("SEAL_VIOLATED");
	case GENERIC_UNSPECIFIED_REJECTION: return ("GENERIC_UNSPECIFIED_REJECTION");
	case GENERIC_OPERATION_REJECTION: return ("GENERIC_OPERATION_REJECTION");
	case DUPLICATE_INVOCATION_REJECTION: return ("DUPLICATE_INVOCATION_REJECTION");
	case UNRECOGNIZED_OPERATION_REJECTION: return ("UNRECOGNIZED_OPERATION_REJECTION");
	case MISTYPED_ARGUMENT_REJECTION: return ("MISTYPED_ARGUMENT_REJECTION");
	case RESOURCE_LIMITATION_REJECTION: return ("RESOURCE_LIMITATION_REJECTION");
	case ACS_HANDLE_TERMINATION_REJECTION: return ("ACS_HANDLE_TERMINATION_REJECTION");
	case SERVICE_TERMINATION_REJECTION: return ("SERVICE_TERMINATION_REJECTION");
	case REQUEST_TIMEOUT_REJECTION: return ("REQUEST_TIMEOUT_REJECTION");
	case UNRECOGNIZED_APDU_REJECTION: return ("UNRECOGNIZED_APDU_REJECTION");
	case MISTYPED_APDU_REJECTION: return ("MISTYPED_APDU_REJECTION");
	case BADLY_STRUCTURED_APDU_REJECTION: return ("BADLY_STRUCTURED_APDU_REJECTION");
	case INITIATOR_RELEASING_REJECTION: return ("INITIATOR_RELEASING_REJECTION");
	case UNRECOGNIZED_LINKEDID_REJECTION: return ("UNRECOGNIZED_LINKEDID_REJECTION");
	case LINKED_RESPONSE_UNEXPECTED_REJECTION: return ("LINKED_RESPONSE_UNEXPECTED_REJECTION");
	case UNEXPECTED_CHILD_OPERATION_REJECTION: return ("UNEXPECTED_CHILD_OPERATION_REJECTION");
	case MISTYPED_RESULT_REJECTION: return ("MISTYPED_RESULT_REJECTION");
	case UNRECOGNIZED_ERROR_REJECTION: return ("UNRECOGNIZED_ERROR_REJECTION");
	case UNEXPECTED_ERROR_REJECTION: return ("UNEXPECTED_ERROR_REJECTION");
	case MISTYPED_PARAMETER_REJECTION: return ("MISTYPED_PARAMETER_REJECTION");
	case NON_STANDARD: return ("NON_STANDARD");
	}

	return ("");
}


std::string CAvayaResult::GetConnectionStateString(LocalConnectionState_t state)
{
	switch(state)
	{
	case CS_NONE: return ("CS_NONE");
	case CS_NULL: return ("CS_NULL");
	case CS_INITIATE: return ("CS_INITIATE");
	case CS_ALERTING: return ("CS_ALERTING");
	case CS_CONNECT: return ("CS_CONNECT");
	case CS_HOLD: return ("CS_HOLD");
	case CS_QUEUED: return ("CS_QUEUED");
	case CS_FAIL: return ("CS_FAIL");
	}

	return ("");
}

std::string CAvayaResult::GetCSTAEventCauseString(CSTAEventCause_t eventCause)
{
	switch(eventCause)
	{
	case EC_NONE: return ("EC_NONE");
	case EC_ACTIVE_MONITOR: return ("EC_ACTIVE_MONITOR");
	case EC_ALTERNATE: return ("EC_ALTERNATE");
	case EC_BUSY: return ("EC_BUSY");
	case EC_CALL_BACK: return ("EC_CALL_BACK");
	case EC_CALL_CANCELLED: return ("EC_CALL_CANCELLED");
	case EC_CALL_FORWARD_ALWAYS: return ("EC_CALL_FORWARD_ALWAYS");
	case EC_CALL_FORWARD_BUSY: return ("EC_CALL_FORWARD_BUSY");
	case EC_CALL_FORWARD_NO_ANSWER: return ("EC_CALL_FORWARD_NO_ANSWER");
	case EC_CALL_FORWARD: return ("EC_CALL_FORWARD");
	case EC_CALL_NOT_ANSWERED: return ("EC_CALL_NOT_ANSWERED");
	case EC_CALL_PICKUP: return ("EC_CALL_PICKUP");
	case EC_CAMP_ON: return ("EC_CAMP_ON");
	case EC_DEST_NOT_OBTAINABLE: return ("EC_DEST_NOT_OBTAINABLE");
	case EC_DO_NOT_DISTURB: return ("EC_DO_NOT_DISTURB");
	case EC_INCOMPATIBLE_DESTINATION: return ("EC_INCOMPATIBLE_DESTINATION");
	case EC_INVALID_ACCOUNT_CODE: return ("EC_INVALID_ACCOUNT_CODE");
	case EC_KEY_CONFERENCE: return ("EC_KEY_CONFERENCE");
	case EC_LOCKOUT: return ("EC_LOCKOUT");
	case EC_MAINTENANCE: return ("EC_MAINTENANCE");
	case EC_NETWORK_CONGESTION: return ("EC_NETWORK_CONGESTION");
	case EC_NETWORK_NOT_OBTAINABLE: return ("EC_NETWORK_NOT_OBTAINABLE");
	case EC_NEW_CALL: return ("EC_NEW_CALL");
	case EC_NO_AVAILABLE_AGENTS: return ("EC_NO_AVAILABLE_AGENTS");
	case EC_OVERRIDE: return ("EC_OVERRIDE");
	case EC_PARK: return ("EC_PARK");
	case EC_OVERFLOW: return ("EC_OVERFLOW");
	case EC_RECALL: return ("EC_RECALL");
	case EC_REDIRECTED: return ("EC_REDIRECTED");
	case EC_REORDER_TONE: return ("EC_REORDER_TONE");
	case EC_RESOURCES_NOT_AVAILABLE: return ("EC_RESOURCES_NOT_AVAILABLE");
	case EC_SILENT_MONITOR: return ("EC_SILENT_MONITOR");
	case EC_TRANSFER: return ("EC_TRANSFER");
	case EC_TRUNKS_BUSY: return ("EC_TRUNKS_BUSY");
	case EC_VOICE_UNIT_INITIATOR: return ("EC_VOICE_UNIT_INITIATOR");
	case EC_NETWORKSIGNAL: return ("EC_NETWORKSIGNAL");
	case EC_ALERTTIMEEXPIRED: return ("EC_ALERTTIMEEXPIRED");
	case EC_DESTOUTOFORDER: return ("EC_DESTOUTOFORDER");
	case EC_NOTSUPPORTEDBEARERSERVICE: return ("EC_NOTSUPPORTEDBEARERSERVICE");
	case EC_UNASSIGNED_NUMBER: return ("EC_UNASSIGNED_NUMBER");
	case EC_INCOMPATIBLE_BEARER_SERVICE: return ("EC_INCOMPATIBLE_BEARER_SERVICE");
	}

	return ("");
}

std::string CAvayaResult::GetForwardTypeString(ForwardingType_t forwardType)
{
	switch(forwardType)
	{
	case FWD_IMMEDIATE: return ("FWD_IMMEDIATE");
	case FWD_BUSY: return ("FWD_BUSY");
	case FWD_NO_ANS: return ("FWD_NO_ANS");
	case FWD_BUSY_INT: return ("FWD_BUSY_INT");
	case FWD_BUSY_EXT: return ("FWD_BUSY_EXT");
	case FWD_NO_ANS_INT: return ("FWD_NO_ANS_INT");
	case FWD_NO_ANS_EXT: return ("FWD_NO_ANS_EXT");
	}

	return ("");
}

std::string CAvayaResult::GetACSERRStr(int iErrCode)
{
	switch(iErrCode)
	{
	case ACSPOSITIVE_ACK: return ("ACSPOSITIVE_ACK");
	case ACSERR_APIVERDENIED: return ("ACSERR_APIVERDENIED  This return indicates that the API Version requested is invalid and not supported by the existing API Client Library.创建流:版本无效或client库不支持!");
	case ACSERR_BADPARAMETER: return ("ACSERR_BADPARAMETER  One or more of the parameters is invalid. 创建流:参数无效!");
	case ACSERR_DUPSTREAM: return ("ACSERR_DUPSTREAM  This return indicates that an ACS Stream is already established with the requested Server.");
	case ACSERR_NODRIVER: return ("ACSERR_NODRIVER  This error return value indicates that no API Client Library Driver was found or installed on the system.创建流:没有库!");
	case ACSERR_NOSERVER: return ("ACSERR_NOSERVER  This indicates that the requested Server is not present in the network.创建流:网络中没有服务!");
	case ACSERR_NORESOURCE: return ("ACSERR_NORESOURCE  This return value indicates that there are insufficient resources to open a ACS Stream.创建流:没有有效的资源创建流!");
	case ACSERR_UBUFSMALL: return ("ACSERR_UBUFSMALL  The user buffer size was smaller than the size of the next available event.");
	case ACSERR_NOMESSAGE: return ("ACSERR_NOMESSAGE  There were no messages available to return to the application.");
	case ACSERR_UNKNOWN: return ("ACSERR_UNKNOWN  The ACS Stream has encountered an unspecified error.");
	case ACSERR_BADHDL: return ("ACSERR_BADHDL  The ACS Handle is invalid");
	case ACSERR_STREAM_FAILED: return ("ACSERR_STREAM_FAILED  The ACS Stream has failed due to network problems. No further operations are possible on this stream.");
	case ACSERR_NOBUFFERS: return ("ACSERR_NOBUFFERS  There were not enough buffers available to place an outgoing message on the send queue. No message has been sent.");
	case ACSERR_QUEUE_FULL: return ("ACSERR_QUEUE_FULL The send queue is full. No message has been sent.");
	}

	return ("Unknown Error.");
}

std::string CAvayaResult::GetErrorString(std::string strErrorModule, int iErrCode)
{
	std::string	szError = "";

	std::string strWhy = GetACSERRStr(iErrCode);
//	std::string	szError = StringFormat("Module: %s, Error: %s", strErrorModule.c_str(), strWhy.c_str());
	szError = m_pStrUtil->Format("Module: %s, Error: %s", strErrorModule.c_str(), strWhy.c_str());

	return szError;
}

std::string CAvayaResult::GetAgentStateString(CSTAQueryAgentStateConfEvent_t queryAgentState)
{
	switch(queryAgentState.agentState)
	{
	case AG_NOT_READY: return ("AG_NOT_READY");
	case AG_NULL: return ("AG_NULL");
	case AG_READY: return ("AG_READY");
	case AG_WORK_NOT_READY: return ("AG_WORK_NOT_READY");
	case AG_WORK_READY: return ("AG_WORK_READY");
	}

	return ("");
}

/*
std::string CAvayaResult::GetAgentModeString(E_AGENT_MODE_TYPE eAgentMode)
{
	return AgentModeTypeString[eAgentMode];
}
std::string CAvayaResult::GetAgentReadyString(E_AGENT_READY_TYPE eAgentReady)
{
	return AgentReadyTypeString[eAgentReady];
}

std::string CAvayaResult::GetCallDirectionString(E_CALL_DIRECTION eCallDirection)
{
	return CallDirectionString[eCallDirection];
}
std::string CAvayaResult::GetCallModeString(E_CALL_MODE eCallMode)
{
	return CallModeString[eCallMode];
}

std::string CAvayaResult::GetCallStateString(E_CALL_STATE eCallState)
{
	return CallStateString[eCallState];
}
std::string CAvayaResult::GetCallFailedTypeString(E_FAILED_TYPE eFailedType)
{
	return FailedTypeString[eFailedType];
}

std::string CAvayaResult::GetHangupTypeString(E_HANGUP_TYPE eHangupType)
{
	return HangupTypeString[eHangupType];
}*/